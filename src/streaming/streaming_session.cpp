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

#include "../../include/_streaming.h"
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
    { util::debug_out("StreamingSessionImpl", msg); }


    std::set<std::string> active_primary_accounts;
    std::set<std::string> active_accounts;
}

namespace tdma {

using namespace std;

class AdminSubscriptionImpl
        : public StreamingSubscriptionImpl {
public:
    AdminSubscriptionImpl( AdminCommandType command,
                       const std::map<std::string, std::string>& params = {})
        :
            StreamingSubscriptionImpl( StreamerServiceType::ADMIN,
                                   to_string(command), params )
    {}
};

struct PendingResponse{
    typedef std::function<void(int, std::string, std::string,
                               unsigned long long, int, std::string)>
    response_cb_ty;

    static const response_cb_ty CALLBACK_TO_COUT;

    int request_id;
    std::string service;
    std::string command;
    response_cb_ty callback;

    PendingResponse( int request_id,
                     const std::string& service,
                     const std::string& command,
                     response_cb_ty callback = nullptr )
        :
            request_id( request_id ),
            service( service ),
            command( command ),
            callback( callback )
        {
        }

    PendingResponse()
        :
            request_id(-1),
            service(),
            command(),
            callback()
        {
        }

};

const PendingResponse::response_cb_ty
PendingResponse::CALLBACK_TO_COUT =
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


class StreamingRequest{
    StreamerServiceType _service;
    std::string _command;
    std::string _account_id;
    std::string _source_id;
    int _request_id;
    std::map<std::string, std::string> _parameters;

public:
    StreamingRequest( StreamerServiceType service,
                      const std::string& command,
                      const std::string& account_id,
                      const std::string& source_id,
                      int request_id,
                      const std::map<std::string, std::string>& paramaters )
        :
            _service(service),
            _command(command),
            _account_id(account_id),
            _source_id(source_id),
            _request_id(request_id),
            _parameters(paramaters)
        {
        }

    StreamingRequest( const StreamingSubscriptionImpl& subscription,
                      const std::string& account_id,
                      const std::string& source_id,
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
    to_json() const
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
};


class StreamingRequests{
    std::vector<StreamingRequest> _requests;

public:
    StreamingRequests( std::initializer_list<StreamingRequest> requests )
        : _requests( requests )
    {}

    StreamingRequests( const std::vector<StreamingSubscriptionImpl>& subscriptions,
                       const std::string& account_id,
                       const std::string& source_id,
                       const std::vector<int>& request_ids )
        {
            assert( subscriptions.size() == request_ids.size() );
            for( size_t i = 0; i < subscriptions.size(); ++i ){
                _requests.emplace_back( subscriptions[i], account_id, source_id,
                                        request_ids[i] );
            }
        }

    json
    to_json() const
    {
        json v = json::array();
        for( size_t i = 0; i < _requests.size(); ++i ){
            v[i]  = _requests[i].to_json();
        }
        return {{"requests", v}};
    }

};

// for the proxy object
const string StreamingSession::VERSION(STREAMING_VERSION);
const chrono::milliseconds StreamingSession::MIN_TIMEOUT(STREAMING_MIN_TIMEOUT);
const chrono::milliseconds StreamingSession::MIN_LISTENING_TIMEOUT(
    STREAMING_MIN_LISTENING_TIMEOUT);
const chrono::milliseconds StreamingSession::LOGOUT_TIMEOUT(
    STREAMING_LOGOUT_TIMEOUT);
const chrono::milliseconds StreamingSession::DEF_CONNECT_TIMEOUT(
    STREAMING_DEF_CONNECT_TIMEOUT);
const chrono::milliseconds StreamingSession::DEF_LISTENING_TIMEOUT(
    STREAMING_DEF_LISTENING_TIMEOUT);
const chrono::milliseconds StreamingSession::DEF_SUBSCRIBE_TIMEOUT(
    STREAMING_DEF_SUBSCRIBE_TIMEOUT);


/*
 * CALLBACK passed to StreamingSession:
 *
 * callback(StreamingCallbackType, service, timestamp, json data);
 *
 *   listening_start - listening thread has started, other args empty
 *
 *   listening_stop - listening thread has stopped (w/o error), other args empty
 *
 *   data - json data of StreamerServiceType::type type returned from server
 *          with timestamp
 *
 *   notify - notify response indicating some 'urgent' message from server
 *
 *   timeout - listening thread has timed out, connection closed/reset,
 *             other args empty
 *
 *   error - error or exception in listening thread, connection closed/reset,
 *           json = {{"error": error message}}
 *
 * All callback types except 'data' have ServiceType::type::None
 *
 * DO NOT CALL BACK INTO StreamingSession from inside the callback.
 * DO NOT TRY TO STOP OR RESTART from inside the callback.
 * DO NOT BLOCK THE THREAD from inside the callback.
 *
 */
//typedef std::function<void(StreamingCallbackType cb_type, StreamerServiceType,
//                           unsigned long long, json)> streaming_cb_ty;


class StreamingSessionImpl{
    StreamerInfo _streamer_info;
    std::string _account_id;
    std::unique_ptr<conn::WebSocketClient> _client;
    streaming_cb_ty _callback;
    std::chrono::milliseconds _connect_timeout;
    std::chrono::milliseconds _listening_timeout;
    std::chrono::milliseconds _subscribe_timeout;
    std::thread _listener_thread;
    std::string _server_id;
    int _next_request_id;
    bool _logged_in;
    bool _listening;
    QOSType _qos;
    unsigned long long _last_heartbeat;
    ThreadSafeHashMap<int, PendingResponse> _responses_pending;
    bool _request_response_to_cout;

    class ListenerThreadTarget{
        static const std::string RESPONSE_TO_REQUEST;
        static const std::string RESPONSE_NOTIFY;
        static const std::string RESPONSE_SNAPSHOT;
        static const std::string RESPONSE_DATA;

        StreamingSessionImpl *_ss;

        class Timeout : public StreamingException {};

        void
        exec();

        void
        parse(const std::string& responses);

        void
        parse_response_to_request(const json& response);

        void
        parse_response_notify(const json& responses);

        void
        parse_response_snapshot(const json& response);

        void
        parse_response_data(const json& response);

    public:
        ListenerThreadTarget( StreamingSessionImpl *ss )
            : _ss(ss) {}

        void
        operator()();
    };

    bool
    _login();

    bool
    _logout();

    void
    _start_listener_thread();

    void
    _stop_listener_thread();

    void
    _reset();

    void
    _subscribe( const std::vector<StreamingSubscriptionImpl>& subscriptions,
                 PendingResponse::response_cb_ty callback = nullptr );

    void
    _exec_callback( StreamingCallbackType cb_type,
                      StreamerServiceType ss_type,
                      unsigned long long ts,
                      json j )
    {
        this->_callback( static_cast<int>(cb_type), static_cast<int>(ss_type),
                        ts, j.dump().c_str() );
    }

public:
    static const int TYPE_ID_LOW = numeric_limits<int>::max();
    static const int TYPE_ID_HIGH = TYPE_ID_LOW;

    StreamingSessionImpl( const StreamerInfo& streamer_info,
                        const std::string& account_id,
                        streaming_cb_ty callback,
                        std::chrono::milliseconds connect_timeout,
                        std::chrono::milliseconds listening_timeout,
                        std::chrono::milliseconds subscribe_timeout,
                        bool request_response_to_cout )
        :
            _streamer_info( streamer_info ),
            _account_id( account_id ),
            _client(nullptr),
            _callback( callback ),
            _connect_timeout( max(connect_timeout,
                                  StreamingSession::MIN_TIMEOUT) ),
            _listening_timeout( max(listening_timeout,
                                StreamingSession::MIN_LISTENING_TIMEOUT) ),
            _subscribe_timeout( max(listening_timeout,
                                StreamingSession::MIN_TIMEOUT) ),
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

    virtual
    ~StreamingSessionImpl()
    {
        D("destruct");
        stop();
    }

    StreamingSessionImpl( const StreamingSessionImpl& ) = delete;

    StreamingSessionImpl&
    operator=( const StreamingSessionImpl& ) = delete;

    std::deque<bool> // success/fails in the order passed
    start(const std::vector<StreamingSubscriptionImpl>& subscriptions);

    void
    stop();

    std::deque<bool> // success/fails in the order passed
    add_subscriptions(const std::vector<StreamingSubscriptionImpl>& subscriptions);

    QOSType
    get_qos() const
    { return _qos; }

    bool
    set_qos(const QOSType& qos);

    std::string
    get_account_id() const
    { return _account_id; }

    std::string
    get_primary_account_id() const
    { return _streamer_info.primary_acct_id; }
};


const string StreamingSessionImpl::ListenerThreadTarget::RESPONSE_TO_REQUEST("response");
const string StreamingSessionImpl::ListenerThreadTarget::RESPONSE_NOTIFY("notify");
const string StreamingSessionImpl::ListenerThreadTarget::RESPONSE_SNAPSHOT("snapshot");
const string StreamingSessionImpl::ListenerThreadTarget::RESPONSE_DATA("data");


void
StreamingSessionImpl::ListenerThreadTarget::operator()()
{
    _ss->_listening = true;

    D("call back (listening_start)");
    _ss->_exec_callback( StreamingCallbackType::listening_start,
                        StreamerServiceType::NONE, 0, json() );

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
    _ss->_exec_callback(cb_t, StreamerServiceType::NONE, 0, cb_j);

}

void
StreamingSessionImpl::ListenerThreadTarget::exec()
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
StreamingSessionImpl::ListenerThreadTarget::parse(const string& responses)
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
StreamingSessionImpl::ListenerThreadTarget::parse_response_to_request(
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
StreamingSessionImpl::ListenerThreadTarget::parse_response_notify(
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
        _ss->_exec_callback( StreamingCallbackType::notify,
                            StreamerServiceType::NONE, 0, response );
    }
}


void
StreamingSessionImpl::ListenerThreadTarget::parse_response_snapshot(
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
StreamingSessionImpl::ListenerThreadTarget::parse_response_data(
    const json& response
    )
{
    try{
        string service = response.at("service");
        _ss->_exec_callback( StreamingCallbackType::data,
                            streamer_service_from_str(service),
                            response.at("timestamp"), response.at("content") );
    }catch(exception& e){
        throw StreamingException("invalid 'data' response: " + string(e.what()));
    }
}



bool
StreamingSessionImpl::_login()
{
    D("login");

    map<string, string> params{
        {"token", _streamer_info.credentials.token},
        {"version", StreamingSession::VERSION},
        {"credential", _streamer_info.credentials_encoded}
    };

    int req_id = _next_request_id++;
    StreamingRequests requests(
        { AdminSubscriptionImpl(AdminCommandType::LOGIN, params) },
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

    if( service != to_string(StreamerServiceType::ADMIN) ||
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
StreamingSessionImpl::_logout()
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
            { AdminSubscriptionImpl(AdminCommandType::LOGOUT) },
            _account_id, 
            _streamer_info.credentials.app_id, 
           {req_id}
    );

    _client->send( requests.to_json().dump() );

    auto t_beg = steady_clock::now();
    auto t_remaining = StreamingSession::LOGOUT_TIMEOUT;
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

            if( service == to_string(StreamerServiceType::ADMIN) &&
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
        t_remaining = StreamingSession::LOGOUT_TIMEOUT - t_elapsed;
    }

    cerr<< "logout failed for lack of response" << endl;
    return false;
}


bool
StreamingSessionImpl::set_qos(const QOSType& qos)
{
    if( _client ){
        assert( _client->is_connected() );
    }else
        throw StreamingException("can not set QOS on a stopped session");    

    map<string, string> params{
        {"qoslevel", to_string(static_cast<unsigned int>(qos))}
    };
    AdminSubscriptionImpl sub(AdminCommandType::QOS, params);

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
StreamingSessionImpl::_subscribe( const vector<StreamingSubscriptionImpl>& subscriptions,
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
StreamingSessionImpl::add_subscriptions(
        const vector<StreamingSubscriptionImpl>& subscriptions
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
StreamingSessionImpl::start(const vector<StreamingSubscriptionImpl>& subscriptions)
{
    D("start");

    if( _client ){
        throw StreamingException("session has already started");
    }

    D("check unique session");
    string acct = get_primary_account_id();
    if( active_primary_accounts.count(acct) )
        throw StreamingException( "Can not start Session; "
            "one is already active for this primary account: " + acct );

    D("_client->reset");
    _client.reset( new conn::WebSocketClient(_streamer_info.url) );

    D("_client->connect");
    _client->connect( _connect_timeout );
    if( !_client->is_connected() ){
        _client.reset();
        throw StreamingException("streaming session failed to connect");
    }

    _logged_in = _login();
    if( !_logged_in )
        throw StreamingException("login failed");

    /* only after connect AND login do we consider this an active session */
    active_primary_accounts.insert(acct);
    active_accounts.insert(get_account_id());

    _start_listener_thread();
    return add_subscriptions(subscriptions);
}


void
StreamingSessionImpl::stop()
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
StreamingSessionImpl::_reset()
{
    D("_reset");
    _client.reset();
    _responses_pending.clear();
    _server_id.clear();
    try{
        active_primary_accounts.erase( get_primary_account_id() );
        active_accounts.erase( get_account_id() );
    }catch(...){}
}


void
StreamingSessionImpl::_start_listener_thread()
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
StreamingSessionImpl::_stop_listener_thread()
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


int
session_is_callable( StreamingSession_C *psession,
                        int allow_exceptions )
{
    if( !psession )
        return handle_error<ValueException>(
            "null session pointer", allow_exceptions
            );

    if( !psession->obj )
        return handle_error<ValueException>(
            "null session pointer->obj", allow_exceptions
            );

    if( psession->type_id < StreamingSessionImpl::TYPE_ID_LOW ||
        psession->type_id > StreamingSessionImpl::TYPE_ID_HIGH )
    {
        return handle_error<TypeException>(
            "session has invalid type id", allow_exceptions
            );
    }
    return 0;
}

int
call_session_with_subs(
    StreamingSession_C *psession,
    StreamingSubscription_C **subs,
    size_t nsubs,
    int *results_buffer,
    deque<bool>(*meth)(void*, const vector<StreamingSubscriptionImpl>&),
    int allow_exceptions )
{
    int err = session_is_callable(psession, allow_exceptions);
    if( err )
        return err;

    if( nsubs > STREAMING_MAX_SUBSCRIPTIONS )
        return handle_error<ValueException>(
            "nsubs > STREAMING_MAX_SUBSCRIPTIONS", allow_exceptions
            );

    if( nsubs == 0 )
        return handle_error<ValueException>("nsubs == 0", allow_exceptions);

    vector<StreamingSubscriptionImpl> res;
    try{
        for(size_t i = 0; i < nsubs; ++i){
            StreamingSubscription_C *c = subs[i];
            assert(c);
            res.emplace_back( C_sub_ptr_to_impl(c) );
        }
    }catch(exception& e){
        set_error_state(TDMA_API_STREAM_ERROR, e.what());
        if( allow_exceptions )
            throw;
        return TDMA_API_STREAM_ERROR;
    }

    deque<bool> results;
    tie(results, err) = CallImplFromABI(allow_exceptions, meth, psession->obj, res);
    if( err )
        return err;

    // NOTE fail results are not consider errors (should they be?)
    if( results_buffer ){
        int i = 0;
        for(bool b : results)
            results_buffer[i++] = static_cast<int>(b);
    }

    return 0;
}

} /*tdma*/

using namespace tdma;

int
StreamingSession_Create_ABI( struct Credentials *pcreds,
                                const char *account_id,
                                streaming_cb_ty callback,
                                unsigned long connect_timeout,
                                unsigned long listening_timeout,
                                unsigned long subscribe_timeout,
                                int request_response_to_cout,
                                StreamingSession_C *psession,
                                int allow_exceptions )
{
    if( !psession )
        return handle_error<ValueException>(
            "null session pointer", allow_exceptions
            );

    if( !pcreds ){
        psession->obj = psession->ctx = nullptr;
        psession->type_id = -1;
        return handle_error<ValueException>(
            "null credentials pointer", allow_exceptions
            );
    }

    if( !pcreds->access_token | !pcreds->refresh_token | !pcreds->client_id ){
        psession->obj = psession->ctx = nullptr;
        psession->type_id = -1;
        return handle_error<LocalCredentialException>(
            "invalid credentials struct", allow_exceptions
            );
    }

    if( !callback ){
        psession->obj = psession->ctx = nullptr;
        psession->type_id = -1;
        return handle_error<ValueException>("null callback", allow_exceptions);
    }

    static auto meth = +[](struct Credentials *pcreds, const char* a,
                           streaming_cb_ty cb, unsigned long cto,
                           unsigned long lto, unsigned long sto, int rrtc)
                           {
        using namespace std::chrono;
        StreamerInfo si = get_streamer_info(*pcreds);
        return new StreamingSessionImpl( si, a, cb, milliseconds(cto),
                                         milliseconds(lto), milliseconds(sto),
                                         static_cast<bool>(rrtc) );
    };

    int err;
    StreamingSessionImpl *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, pcreds, account_id,
                                    callback, connect_timeout, listening_timeout,
                                    subscribe_timeout, request_response_to_cout);
    if( err ){
        psession->obj = psession->ctx = nullptr;
        psession->type_id = -1;
        return err;
    }

    psession->obj = reinterpret_cast<void*>(obj);
    psession->ctx = nullptr;
    psession->type_id = StreamingSessionImpl::TYPE_ID_LOW;
    return 0;
}


int
StreamingSession_Destroy_ABI( StreamingSession_C *psession,
                                 int allow_exceptions )

{
    int err = session_is_callable(psession, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj){
        delete reinterpret_cast<StreamingSessionImpl*>(obj);
    };

    err = CallImplFromABI(allow_exceptions, meth, psession->obj);
    return err ? err : 0;
}

int
StreamingSession_Start_ABI( StreamingSession_C *psession,
                               StreamingSubscription_C **subs,
                               size_t nsubs,
                               int *results_buffer,
                               int allow_exceptions )
{
    auto meth = +[](void *obj, const vector<StreamingSubscriptionImpl>& s){
        return reinterpret_cast<StreamingSessionImpl*>(obj)->start(s);
    };

    return call_session_with_subs(psession, subs, nsubs, results_buffer,
                                  meth, allow_exceptions);
}

int
StreamingSession_Stop_ABI( StreamingSession_C *psession,
                               int allow_exceptions )
{
    int err = session_is_callable(psession, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj){
        reinterpret_cast<StreamingSessionImpl*>(obj)->stop();
    };

    return CallImplFromABI(allow_exceptions, meth, psession->obj);
}



int
StreamingSession_AddSubscriptions_ABI( StreamingSession_C *psession,
                                           StreamingSubscription_C **subs,
                                           size_t nsubs,
                                           int *results_buffer,
                                           int allow_exceptions )
{
    auto meth = +[](void *obj, const vector<StreamingSubscriptionImpl>& s){
        return reinterpret_cast<StreamingSessionImpl*>(obj)->add_subscriptions(s);
    };

    return call_session_with_subs(psession, subs, nsubs, results_buffer,
                                  meth, allow_exceptions);
}

int
StreamingSession_SetQOS_ABI( StreamingSession_C *psession,
                                int qos,
                                int *result,
                                int allow_exceptions )
{
    int err = session_is_callable(psession, allow_exceptions);
    if( err )
        return err;

    err = check_abi_enum(QOSType_is_valid, qos, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj, int q){
        return static_cast<int>(
            reinterpret_cast<StreamingSessionImpl*>(obj)->set_qos(
                static_cast<QOSType>(q)
                )
            );
    };

    tie(*result, err) = CallImplFromABI(allow_exceptions, meth,
                                        psession->obj, qos);
    return err ? err : 0;
}

int
StreamingSession_GetQOS_ABI( StreamingSession_C *psession,
                                int *qos,
                                int allow_exceptions )
{
    int err = session_is_callable(psession, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj){
        return static_cast<int>(
            reinterpret_cast<StreamingSessionImpl*>(obj)->get_qos()
            );
    };

    tie(*qos, err) = CallImplFromABI(allow_exceptions, meth, psession->obj);
    return err ? err : 0;
}
