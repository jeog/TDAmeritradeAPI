/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#include <iostream>
#include <map>
#include <ctime>
#include <functional>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

#include "../../include/util.h"
#include "../../include/_tdma_api.h"
#include "../../include/websocket_connect.h"


/*
 * StreamingSession basic flow:
 *
 *                                      _______________________________
 *                                     |                               |
 *                                     |                               |
 * StreamingSession:  connect  ----> login  ----> subscribe      listen thread
 *                       |          /     \          \/                /\
 * WebSocketClient:   connect     send    recv      send               /\
 *                       |          \      /         \/                /\
 * TDAmeritrade:      connect         auth            -> process/serv ->
 *
 *
 */
namespace {
    void
    D(std::string msg)
    { util::debug_out("StreamingSession", msg); }
}

namespace tdma {

using namespace std;


const string StreamingSession::VERSION("1.0");

const chrono::milliseconds StreamingSession::MIN_TIMEOUT(1000);
const chrono::milliseconds StreamingSession::MIN_LISTENING_TIMEOUT(10000);
const chrono::milliseconds StreamingSession::LOGOUT_TIMEOUT(1000);
const chrono::milliseconds StreamingSession::DEF_CONNECT_TIMEOUT(3000);
const chrono::milliseconds StreamingSession::DEF_LISTENING_TIMEOUT(30000);
const chrono::milliseconds StreamingSession::DEF_SUBSCRIBE_TIMEOUT(1500);

const string StreamingSession::ListenerThreadTarget::RESPONSE_TO_REQUEST("response");
const string StreamingSession::ListenerThreadTarget::RESPONSE_NOTIFY("notify");
const string StreamingSession::ListenerThreadTarget::RESPONSE_SNAPSHOT("snapshot");
const string StreamingSession::ListenerThreadTarget::RESPONSE_DATA("data");

set<string> StreamingSession::active_primary_accounts;
set<string> StreamingSession::active_accounts;


StreamingSession*
StreamingSession::Create( Credentials& creds,
                              const string& account_id,
                              streaming_cb_ty callback,
                              chrono::milliseconds connect_timeout,
                              chrono::milliseconds listening_timeout,
                              chrono::milliseconds subscribe_timeout,
                              bool request_response_to_cout )
{
    /*
     * we can only have 1 session per user so we first we check against the
     * account_id arg to avoid the streamer info call if already active, then
     * we check for a unique primary acct from returned info.
     *
     * should we check against user id (not account id) or something else ??
     */
    if( active_accounts.count(account_id) )
        throw StreamingException("can not create StreamingSession; "
            "there is an active session for this account: " + account_id);    

    StreamerInfo si = get_streamer_info(creds);
    if( active_primary_accounts.count(si.primary_acct_id) )
        throw StreamingException("can not create StreamingSession; "
            "there is an active session for this primary account: "
            + si.primary_acct_id);    

    StreamingSession* ss = new StreamingSession( si, account_id, callback,
                                                 connect_timeout,
                                                 listening_timeout,
                                                 subscribe_timeout,
                                                 request_response_to_cout );

    active_accounts.insert( ss->_account_id );
    active_primary_accounts.insert( si.primary_acct_id );
    return ss;
}


void
StreamingSession::Destroy( StreamingSession* session )
{
    if( session ){
        active_primary_accounts.erase( session->_streamer_info.primary_acct_id );
        active_accounts.erase( session->_account_id );
        delete session;
    }
}


StreamingSession::StreamingSession( const StreamerInfo& streamer_info,
                                        const string& account_id,
                                        streaming_cb_ty callback,
                                        chrono::milliseconds connect_timeout,
                                        chrono::milliseconds listening_timeout,
                                        chrono::milliseconds subscribe_timeout,
                                        bool request_response_to_cout)
    :
        _streamer_info( streamer_info ),
        _account_id( account_id ),
        _client(nullptr),
        _callback( callback ),
        _connect_timeout( max(connect_timeout, MIN_TIMEOUT) ),
        _listening_timeout( max(listening_timeout, MIN_LISTENING_TIMEOUT) ),
        _subscribe_timeout( max(listening_timeout, MIN_TIMEOUT) ),
        _listener_thread(),
        _server_id(),
        _next_request_id(0),        
        _logged_in(false),
        _listening(false),
        _qos( QOSType::fast ),
        _last_heartbeat(0),
        _responses_pending(),
        _request_response_to_cout( request_response_to_cout )
    {
        D("construct");
    }


StreamingSession::~StreamingSession()
{
    D("destruct");
    stop();
}


void
StreamingSession::ListenerThreadTarget::operator()()
{
    _ss->_listening = true;

    D("call back (listening_start)");
    _ss->_callback( StreamingCallbackType::listening_start, 0, 0, json() );

    StreamingCallbackType cb_t = StreamingCallbackType::listening_stop;
    json cb_j;

    try{
        exec();

    }catch( Timeout& e ){
        /*
         * if timed out assume there's an issue w/ the connection
         * (MIN_LISTENING_TIMEOUT assures a heartbeat from server)
         * and just shut it down rather than trying to logout etc.
         */
        D("listening thread TIMEOUT");
        _ss->_reset();
        cb_t = StreamingCallbackType::timeout;

    }catch( StreamingException& e ){
        /*
         * any type of StreamingException inside exec() close/reset connection
         */
        D(string("listening thread STREAMING EXCEPTION: ") + e.what());
        _ss->_reset();
        cb_t = StreamingCallbackType::error;
        cb_j = { {"error:", e.what()} };

    }catch(...){
        /*
         * trouble regardless, do our best to close the connection
         */
        D("listening thread EXCETION");
        _ss->_reset();
        throw;
    }

    _ss->_listening = false;

    D("call back (" + to_string(cb_t) + ")");
    /* callback should be last thing we do */
    _ss->_callback(cb_t, 0, 0, cb_j);

}

void
StreamingSession::ListenerThreadTarget::exec()
{
    D("begin listening loop");
    while( _ss->_listening ){

        /* _client should *always* be connected while listening */
        if( !_ss->_client->is_connected() )
            throw StreamingException("client connection ended unexpectedly");

        /* block for _listening_timeout msec until we get at least 1 message */
        auto results = _ss->_client->recv_atleast_n_or_wait_for( 1,
                           _ss->_listening_timeout
                        );

        if( results.empty() ) /* TIMED OUT */
            throw Timeout();

        /* each message can have mutliple results */
        for(string& res : results){

            /* empty message is the signal to stop listening */
            if( res.empty() ){
                D("stop-listening message");
                _ss->_listening = false;
                break;
            }

            /*
             * parse handles the return message logic:
             *
             *      response to request: check against _responses_pending
             *                           and callback to calling function on
             *                           main thread which sends to stdout
             *                           if necessary
             *
             *      notify: record heartbeat, if not heartbeat call back
             *
             *      data: call back to client w/ json object returned
             *
             *      snapshot: NOT IMPLEMENTED
             */
            try{
                parse(res);
            }catch( json::exception& e ){
                cerr << "Error Parsing Json: " << endl
                     << '\t' << e.what() << endl
                     << '\t' << res << endl;
            }
        }
    }
    D("end listening loop");
}


void
StreamingSession::ListenerThreadTarget::parse(const string& responses)
{
    auto resp = json::parse(responses);
    auto r = resp.begin();
    if( r == resp.end() )
        throw StreamingException("invalid response JSON");

    string resp_ty = r.key();
    auto resp_array = r.value();

    if(resp_ty == RESPONSE_TO_REQUEST){
        for(auto& resp : resp_array){
            parse_response_to_request(resp);
        }
    }else if(resp_ty == RESPONSE_NOTIFY){
         for(auto& resp : resp_array){
            parse_response_notify(resp);
         }
    }else if(resp_ty == RESPONSE_SNAPSHOT){
         for(auto& resp : resp_array){
            parse_response_snapshot(resp);
         }
    }else if(resp_ty == RESPONSE_DATA){
         for(auto& resp : resp_array){
            parse_response_data(resp);
         }
    }else{
        throw StreamingException("invalid response type");
    }
}

void
StreamingSession::ListenerThreadTarget::parse_response_to_request(
    const json& response
    )
{
    string service = response["service"];
    string command = response["command"];
    string req_id = response["requestid"];

    PendingResponse pr;
    bool pr_exists;
    tie(pr, pr_exists) = _ss->_responses_pending.get_and_remove_safe(stoi(req_id));
    if( !pr_exists ){
        cerr<< " received duplicate or unexpected response: " + response.dump();
        return;
    }

    if( service != pr.service || command != pr.command )
    {
        throw StreamingException("received invalid respose: " + response.dump()
                                  + ", (" + pr.service + "," + pr.command + ")");
    }

    auto content = response["content"];
    if( pr.callback )
        pr.callback( stoi(req_id), service, command, response["timestamp"],
                     content["code"], content["msg"] );
}


void
StreamingSession::ListenerThreadTarget::parse_response_notify(
    const json& response
    )
{
    auto r = response.find("heartbeat");
    if( r != response.end() ){
        string hb_str = r.value();
        _ss->_last_heartbeat = stoull(hb_str);
    }else{
#ifdef DEBUG
       stringstream ss;
       ss << response;
       D("notify: " + ss.str());
#endif
        _ss->_callback( StreamingCallbackType::notify, 0, 0, response );
    }
}


void
StreamingSession::ListenerThreadTarget::parse_response_snapshot(
    const json& response
    )
{
    /*
     * TODO implement if we implement any of the following Streamer Services:
     *      CHART_HISTORY_FUTUES
     *      NEWS_HEADLINE_LIST
     */
    cout<< "SNAPSHOT - " << response << endl;
}


void
StreamingSession::ListenerThreadTarget::parse_response_data(
    const json& response
    )
{
    try{
        string service = response.at("service");
        _ss->_callback( StreamingCallbackType::data, StreamerService(service),
                        response.at("timestamp"), response.at("content") );
    }catch(exception& e){
        throw StreamingException("invalid 'data' response: " + string(e.what()));
    }
}



bool
StreamingSession::_login()
{
    D("login");

    map<string, string> params{
        {"token", _streamer_info.credentials.token},
        {"version", VERSION},
        {"credential", _streamer_info.credentials_encoded}
    };

    int req_id = _next_request_id++;
    StreamingRequests requests(
        { AdminSubscription(AdminCommandType::LOGIN, params) },
        _account_id, 
        _streamer_info.credentials.app_id, 
        {req_id}
    );

     _client->send( requests.to_json().dump() );

    string rmessage = _client->recv_or_wait_for(_listening_timeout);
    if( rmessage.empty() ){
        cerr<< "timed out waiting for login response" << endl;
        return false;
    }

    auto response = json::parse(rmessage);
    auto r = response.begin();
    if( r == response.end() )
        throw StreamingException("invalid response JSON");

    string response_ty = r.key();
    if( response_ty != "response" )
        throw StreamingException("invalid login response: " + response_ty);

    auto info = r.value()[0];
    string service = info["service"];
    string command = info["command"];
    string response_req_id = info["requestid"];

    if( service != to_string(StreamerService::type::ADMIN) ||
        command != to_string(AdminCommandType::LOGIN ) ||
        response_req_id != to_string(req_id) )
    {
        /*
         *  since login is first we can assume no other responses will be on 
         *  the line; if any of the fields don't match exactly treat as error
         */
        cerr << "invalid login response" << endl
             << "\t service: " << service << endl
             << "\t command: " << command << endl
             << "\t requestid: " << response_req_id << endl;
        return false;
    }

    auto content = info["content"];
    int code = content["code"];
    string msg = content["msg"];

    if( _request_response_to_cout )
        PendingResponse::CALLBACK_TO_COUT( req_id, service, command,
                                           info["timestamp"], code, msg);
    
    if( code ){
        cerr<< "logout error"
            << "\t code: " << code << endl
            << "\t message: " << msg << endl;
        return false;
    }                    

    _server_id = msg;
    D("login success");
    return true;
}


bool
StreamingSession::_logout()
{
    using namespace chrono;

    D("logout");

    /*
     * need to be sure the listening thread is already dead so it can't
     * steal our confirm
     */
    assert( !_listening );
    assert( !_listener_thread.joinable() );

    int req_id = _next_request_id++;
    StreamingRequests requests(
            { AdminSubscription(AdminCommandType::LOGOUT) },
            _account_id, 
            _streamer_info.credentials.app_id, 
           {req_id}
    );

    _client->send( requests.to_json().dump() );

    auto t_beg = steady_clock::now();
    auto t_remaining = LOGOUT_TIMEOUT;
    while( t_remaining.count() >= 0 ){ //timeout outside recv
        /*
         * use a different wait/block mechanism in here so we can process each
         * message individually within LOGOUT_TIMEOUT_MSEC
         */
        string rmessage = _client->recv_or_wait_for(t_remaining);
        if( rmessage.empty() ){
            cerr<< "timed out waiting for logout response" << endl;
            return false; // timeout inside recv
        }

        auto response = json::parse(rmessage);
        auto r = response.begin();
        if( r == response.end() )
            throw StreamingException("invalid logout response JSON");

        string response_ty = r.key();
        if( response_ty == "response" ){
            auto info = r.value()[0];
            string service = info["service"];
            string command = info["command"];
            string response_req_id = info["requestid"];

            if( service == to_string(StreamerService::type::ADMIN) &&
                command == to_string(AdminCommandType::LOGOUT ) &&
                response_req_id == to_string(req_id) )
            {
                auto content = info["content"];
                int code = content["code"];
                string msg = content["msg"];
                if( _request_response_to_cout )
                    PendingResponse::CALLBACK_TO_COUT(req_id, service, command,
                                                      info["timestamp"], code, msg);
                
                if( code ){
                    cerr<< "logout error"
                        << "\t code: " << code << endl
                        << "\t message: " << msg << endl;
                    break;
                }
                D("logout success");
                return true;
            }
        }

        auto t_elapsed = duration_cast<milliseconds>(steady_clock::now() - t_beg);
        t_remaining = LOGOUT_TIMEOUT - t_elapsed;
    }

    cerr<< "logout failed for lack of response" << endl;
    return false;
}


bool
StreamingSession::set_qos(const QOSType& qos)
{
    if( _client ){
        assert( _client->is_connected() );
    }else
        throw StreamingException("can not set QOS on a stopped session");    

    map<string, string> params{
        {"qoslevel", to_string(static_cast<unsigned int>(qos))}
    };
    AdminSubscription sub(AdminCommandType::QOS, params);

    condition_variable c;
    mutex m;
    bool f, success;
    string message;

    PendingResponse::response_cb_ty cb =
        [&](int id, string serv, string cmd, unsigned long long ts,
            int code, string msg)
        {
            if( _request_response_to_cout ){
                PendingResponse::CALLBACK_TO_COUT(id, serv, cmd, ts, code, msg);
            }
            success = (code == 0);
            message = msg;
            {
                lock_guard<mutex> _(m);
                f = true;
            }
            c.notify_all();
        };

    _subscribe( {sub}, cb );

    unique_lock<mutex> l(m);
    if( !c.wait_for(l, _subscribe_timeout, [&](){ return f; } ) )
    {
        cerr<< "timed out trying to set QOS(may still happen)" << endl;
    }

    if( !success ){
        cerr<< "set QOS to " << qos << "(" << static_cast<unsigned int>(qos)
            << ") failed: " << message << endl;
        return false;
    }

    _qos = qos;
    return true;
}



void
StreamingSession::_subscribe( const vector<StreamingSubscription>& subscriptions,
                              PendingResponse::response_cb_ty callback )
{    
    if( !_responses_pending.empty() ){
        _responses_pending.access(
            [](const std::unordered_map<int, PendingResponse>& m){
                cerr<< "(" << m.size() << ") RESPONSES STILL PENDING" << endl;
                for( auto& p : m ){
                    cerr<< "\t request_id: " << p.second.request_id << endl
                        << "\t service: " << p.second.service << endl
                        << "\t command: " << p.second.command << endl << endl;
                }
            }
        );
    }

    vector<int> req_ids;
    for(auto& s : subscriptions){
        req_ids.push_back( _next_request_id++ );
    }
    StreamingRequests requests( subscriptions, _account_id,
                                _streamer_info.credentials.app_id, req_ids );

    auto msg = requests.to_json().dump();
    _client->send( msg );

    for( size_t i = 0; i < subscriptions.size(); ++i ){
        _responses_pending.insert(
            req_ids[i],
            PendingResponse(req_ids[i],
                            to_string(subscriptions[i].get_service()),
                            subscriptions[i].get_command(),
                            callback)
            );
    }
}


deque<bool>
StreamingSession::add_subscriptions(
        const vector<StreamingSubscription>& subscriptions
        )
{
    if( _client ){
        assert( _client->is_connected() );
    }else
        throw StreamingException("can not add subscription to a stopped session");

    condition_variable c;
    mutex m;
    int ntarget = subscriptions.size();
    int ndone = 0;
    deque<bool> successes(ntarget, false);

    PendingResponse::response_cb_ty cb =
        [&](int id, string serv, string cmd, unsigned long long ts,
            int code, string msg)
        {
            if( _request_response_to_cout ){
                PendingResponse::CALLBACK_TO_COUT(id, serv, cmd, ts, code, msg);
            }
            successes[ndone] = ( code == 0 );
            {
                lock_guard<mutex> _(m);
                if( ++ndone < ntarget )
                    return;
            }
            c.notify_all();
        };

    _subscribe(subscriptions, cb);

    unique_lock<mutex> l(m);
    if( !c.wait_for( l, _subscribe_timeout, [&](){return ndone >= ntarget;} ) )
    {
        cerr<< "timed out setting subscriptions (may have succeeded)" << endl;
    }

    return successes;
}


deque<bool>
StreamingSession::start(const vector<StreamingSubscription>& subscriptions)
{
    D("start");

    if( _client ){
        throw StreamingException("session has already started");
    }

    D("_client->reset");
    _client.reset( new conn::WebSocketClient(_streamer_info.url) );

    D("_client->connect");
    _client->connect( _connect_timeout );
    if( !_client->is_connected() ){
        _client.reset();
        throw StreamingException("streaming session failed to connect");
    }

    _logged_in = _login();
    _start_listener_thread();
    return add_subscriptions(subscriptions);
}


void
StreamingSession::stop()
{
    D("stop");

    _stop_listener_thread();
    try{
        if( _client && _client->is_connected() ){
            if( _logged_in )
                _logged_in = !_logout();
            _client->close();
        }
    }catch(...){
        _reset();
        throw;
    }

    _reset();
}


void
StreamingSession::_reset()
{
    D("_reset");
    _client.reset();
    _responses_pending.clear();
    _server_id.clear();
}


void
StreamingSession::_start_listener_thread()
{
    D("start listener thread");
    assert( !_listening );

    D("join listener thread");
    if( _listener_thread.joinable() )
        _listener_thread.join();

    D("move new listener thread");
    _listener_thread = move( thread(ListenerThreadTarget(this)) );
}


void
StreamingSession::_stop_listener_thread()
{
    D("stop listening thread");
    /*
     * force listeners thread out of a wait, but allow it to consume messages
     * up to *this* point first by setting _listening to false in loop
     */
    if( _listening && _client )
        _client->push_empty_message();

    D("join listener thread");
    if( _listener_thread.joinable() )
        _listener_thread.join();
}



StreamingSession::PendingResponse::PendingResponse(
        int request_id,
        const std::string& service,
        const std::string& command,
        response_cb_ty callback )
    :
        request_id( request_id ),
        service( service ),
        command( command ),
        callback( callback )
    {
    }


StreamingSession::PendingResponse::PendingResponse()
    :
        request_id(-1),
        service(),
        command(),
        callback()
    {
    }


const StreamingSession::PendingResponse::response_cb_ty
StreamingSession::PendingResponse::CALLBACK_TO_COUT =
    [](int id, string serv, string cmd, unsigned long long ts, int c, string msg)
    {
        cout<< "RESPONSE CALLBACK" << endl
            << "\t request_id: " << id << endl
            << "\t service: " << serv << endl
            << "\t command: " << cmd << endl
            << "\t timestamp: " << ts << endl
            << "\t code: " << c << endl
            << "\t message: " << msg << endl
            << endl;
    };


StreamingSession::StreamingRequest::StreamingRequest(
        StreamerService::type service,
        const string& command,
        const string& account_id,
        const string& source_id,
        int request_id,
        const map<string, string>& paramaters )
    :
        _service(service),
        _command(command),
        _account_id(account_id),
        _source_id(source_id),
        _request_id(request_id),
        _parameters(paramaters)
    {
    }


StreamingSession::StreamingRequest::StreamingRequest(
        const StreamingSubscription& subscription,
        const string& account_id,
        const string& source_id,
        int request_id )
    :
        _service( subscription.get_service() ),
        _command( subscription.get_command() ),
        _account_id( account_id ),
        _source_id( source_id ),
        _request_id( request_id ),
        _parameters( subscription.get_parameters() )
    {
    }


json
StreamingSession::StreamingRequest::to_json() const
{
    return {
       {"service", to_string(_service)},
       {"requestid", to_string(_request_id)},
       {"command", _command},
       {"account", _account_id},
       {"source", _source_id},
       {"parameters", _parameters}
    };
}


json
StreamingSession::StreamingRequests::to_json() const
{
    json v = json::array();
    for( size_t i = 0; i < _requests.size(); ++i ){
        v[i]  = _requests[i].to_json();
    }
    return {{"requests", v}};
}


StreamingSession::StreamingRequests::StreamingRequests(
        const vector<StreamingSubscription>& subscriptions,
        const string& account_id,
        const string& source_id,
        const vector<int>& request_ids )
{
    assert( subscriptions.size() == request_ids.size() );
    for( size_t i = 0; i < subscriptions.size(); ++i ){
        _requests.emplace_back( subscriptions[i], account_id, source_id,
                                request_ids[i] );
    }
}


SharedSession::SharedSession( Credentials& creds,
                                  const std::string& account_id,
                                  streaming_cb_ty callback,
                                  std::chrono::milliseconds connect_timeout,
                                  std::chrono::milliseconds listening_timeout,
                                  std::chrono::milliseconds subscribe_timeout,
                                  bool request_response_to_cout )
    :
        _s( StreamingSession::Create( creds, account_id, callback,
                 connect_timeout, listening_timeout, subscribe_timeout,
                 request_response_to_cout ),
            Deleter() )
    {
    }

StreamingSession*
SharedSession::operator->()
{
    if( !_s )
        throw StreamingException("null session");
    return _s.get();
}

} /*tdma*/
