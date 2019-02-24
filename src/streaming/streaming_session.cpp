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
#include "../../include/websocket_connect.h"

using std::string;
using std::vector;
using std::map;
using std::deque;
using std::set;
using std::stringstream;
using std::tie;
using std::mutex;
using std::cout;
using std::cerr;
using std::endl;
using std::chrono::milliseconds;

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

namespace tdma{

using std::to_string;

class StreamingSessionImpl;

void
D(string msg, StreamingSessionImpl *obj)
{ util::debug_out("StreamingSessionImpl", msg, obj, cout); }


set<string> active_accounts;

class AdminSubscriptionImpl
        : public StreamingSubscriptionImpl {
public:
    AdminSubscriptionImpl( CommandType command,
                           const map<string, string>& parameters = {} )
        :
            StreamingSubscriptionImpl( to_string(StreamerServiceType::ADMIN),
                                       to_string(command) )
        {
            set_parameters(parameters);
        }

};

struct PendingResponse{
    typedef std::function< void(int, string, string, unsigned long long,
                                int, string) >
    response_cb_ty;

    int request_id;
    string service;
    string command;
    response_cb_ty callback;

    PendingResponse( int request_id,
                     const string& service,
                     const string& command,
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


struct PendingResponseBundle{
    std::condition_variable cond;
    mutex mtx;
    int n;
    int ntarget;
    deque<bool> successes;
    string msg;

    bool
    is_ready() const
    { return n >= ntarget; }

    PendingResponseBundle(int target=1)
        :
            cond(),
            mtx(),
            n(0),
            ntarget(target),
            successes(ntarget, false),
            msg()
        {
        }

    PendingResponseBundle(const PendingResponseBundle&) = delete;

    PendingResponseBundle&
    operator=(const PendingResponseBundle&) = delete;
};


class StreamingRequest{
    string _service;
    string _command;
    string _account_id;
    string _source_id;
    int _request_id;
    map<string, string> _parameters;

public:
    StreamingRequest( const std::string& service,
                      const std::string& command,
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

    StreamingRequest( const StreamingSubscriptionImpl& subscription,
                      const string& account_id,
                      const string& source_id,
                      int request_id )
        :
            StreamingRequest( subscription.get_service_str(),
                subscription.get_command_str(), account_id, source_id,
                request_id, subscription.get_parameters() )
        {
        }

    json
    to_json() const
    {
        return {
           {"service", _service},
           {"requestid", to_string(_request_id)},
           {"command", _command},
           {"account", _account_id},
           {"source", _source_id},
           {"parameters", _parameters}
        };
    }
};


class StreamingRequests{
    vector<StreamingRequest> _requests;

public:
    StreamingRequests( std::initializer_list<StreamingRequest> requests )
        : _requests( requests )
    {}

    StreamingRequests( const vector<StreamingSubscriptionImpl>& subscriptions,
                       const string& account_id,
                       const string& source_id,
                       const vector<int>& request_ids )
        {
            assert( subscriptions.size() == request_ids.size() );

            for( size_t i = 0; i < subscriptions.size(); ++i ){
                _requests.emplace_back( subscriptions[i], account_id,
                                        source_id, request_ids[i] );
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
const milliseconds StreamingSession::MIN_TIMEOUT(STREAMING_MIN_TIMEOUT);
const milliseconds StreamingSession::MIN_LISTENING_TIMEOUT(
    STREAMING_MIN_LISTENING_TIMEOUT);
const milliseconds StreamingSession::LOGOUT_TIMEOUT(
    STREAMING_LOGOUT_TIMEOUT);
const milliseconds StreamingSession::DEF_CONNECT_TIMEOUT(
    STREAMING_DEF_CONNECT_TIMEOUT);
const milliseconds StreamingSession::DEF_LISTENING_TIMEOUT(
    STREAMING_DEF_LISTENING_TIMEOUT);
const milliseconds StreamingSession::DEF_SUBSCRIBE_TIMEOUT(
    STREAMING_DEF_SUBSCRIBE_TIMEOUT);


class StreamingSessionImpl{
    StreamerInfo _streamer_info;
    string _account_id;
    std::unique_ptr<conn::WebSocketClient> _client;
    streaming_cb_ty _callback;
    milliseconds _connect_timeout;
    milliseconds _listening_timeout;
    milliseconds _subscribe_timeout;
    std::thread _listener_thread;
    string _server_id;
    int _next_request_id;
    bool _logged_in;
    bool _listening;
    QOSType _qos;
    unsigned long long _last_heartbeat;
    ThreadSafeHashMap<int, PendingResponse> _responses_pending;

    class ListenerThreadTarget{
        static const string RESPONSE_TO_REQUEST;
        static const string RESPONSE_NOTIFY;
        static const string RESPONSE_SNAPSHOT;
        static const string RESPONSE_DATA;

        StreamingSessionImpl *_ss;

        class Timeout
            : public StreamingException {
        public:
            using StreamingException::StreamingException;
        };

        void
        exec();

        void
        parse(const string& responses);

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
    _send_requests( const vector<StreamingSubscriptionImpl>& subscriptions,
                    PendingResponse::response_cb_ty callback = nullptr );

    void
    _exec_callback( StreamingCallbackType cb_type,
                    StreamerServiceType ss_type,
                    unsigned long long ts,
                    json j )
    {
        if( _callback ){
            _callback( static_cast<int>(cb_type), static_cast<int>(ss_type),
                       ts, j.dump().c_str() );
        }
    }

public:
    static const int TYPE_ID_LOW = TYPE_ID_STREAMING_SESSION;
    static const int TYPE_ID_HIGH = TYPE_ID_STREAMING_SESSION;
    typedef StreamingSession ProxyType;

    StreamingSessionImpl( const StreamerInfo& streamer_info,
                          streaming_cb_ty callback,
                          milliseconds connect_timeout,
                          milliseconds listening_timeout,
                          milliseconds subscribe_timeout )
        :
            _streamer_info( streamer_info ),
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
            _responses_pending()
        {
            D("construct", this);
            D("primary account: " + streamer_info.primary_acct_id, this);
            D("connect_timeout: " + to_string(connect_timeout.count()), this);
            D("listening_timeout: " + to_string(listening_timeout.count()), this);
            D("subscribe_timeout: " + to_string(subscribe_timeout.count()), this);
        }

    virtual
    ~StreamingSessionImpl()
    {
        D("destruct", this);
        stop();
    }

    StreamingSessionImpl( const StreamingSessionImpl& ) = delete;

    StreamingSessionImpl&
    operator=( const StreamingSessionImpl& ) = delete;

    deque<bool> // success/fails in the order passed
    start(const vector<StreamingSubscriptionImpl>& subscriptions);

    void
    stop();

    bool
    is_active() const
    { return static_cast<bool>(_client); }

    deque<bool> // success/fails in the order passed
    add_subscriptions(const vector<StreamingSubscriptionImpl>& subscriptions);

    QOSType
    get_qos() const
    { return _qos; }

    bool
    set_qos(const QOSType& qos);

    string
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

    D("call back (listening_start)", _ss);
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
        D("listening thread TIMEOUT", _ss);
        _ss->_reset();
        cb_t = StreamingCallbackType::timeout;

    }catch( StreamingException& e ){
        /*
         * any type of StreamingException inside exec() close/reset connection
         */
        D(string("listening thread STREAMING EXCEPTION: ") + e.what(), _ss);
        _ss->_reset();
        cb_t = StreamingCallbackType::error;
        cb_j = { {"error:", e.what()} };

    }catch( std::exception& e ){
        /*
         * trouble regardless, do our best to close the connection first
         */
        D(string("listening thread EXCEPTION: ") + e.what(), _ss);
        _ss->_reset();
        throw;
    }

    _ss->_listening = false;

    D("call back (" + to_string(cb_t) + ")", _ss);
    /* callback should be last thing we do */
    _ss->_exec_callback(cb_t, StreamerServiceType::NONE, 0, cb_j);

}

void
StreamingSessionImpl::ListenerThreadTarget::exec()
{
    D("begin listening loop", _ss);
    while( _ss->_listening ){

        /* _client should *always* be connected while listening */
        if( !_ss->_client->is_connected() ){
            TDMA_API_THROW( StreamingException,
                            "client connection ended unexpectedly" );
        }

        /* BLOCK for _listening_timeout msec until we get at least 1 message */
        auto results = _ss->_client->recv_atleast_n_or_wait_for( 1,
                           _ss->_listening_timeout
                        );

        if( results.empty() ) /* TIMED OUT */
            throw Timeout("exec timeout", __LINE__, __FILE__);

        /* each message can have mutliple results */
        for(string& res : results){
            if( res.empty() ){
                /* empty message is the signal to stop listening */
                D("stop-listening message", _ss);
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
    D("end listening loop", _ss);
}


void
StreamingSessionImpl::ListenerThreadTarget::parse(const string& responses)
{
    auto resp = json::parse(responses);
    auto r = resp.begin();
    if( r == resp.end() )
        TDMA_API_THROW(StreamingException,"invalid response JSON");

    string resp_ty = r.key();
    auto resp_array = r.value();

    if(resp_ty == RESPONSE_TO_REQUEST){
        for(auto& resp : resp_array)
            parse_response_to_request(resp);
    }else if(resp_ty == RESPONSE_NOTIFY){
        for(auto& resp : resp_array)
            parse_response_notify(resp);
    }else if(resp_ty == RESPONSE_SNAPSHOT){
        for(auto& resp : resp_array)
            parse_response_snapshot(resp);
    }else if(resp_ty == RESPONSE_DATA){
        for(auto& resp : resp_array)
            parse_response_data(resp);
    }else{
        TDMA_API_THROW(StreamingException,"invalid response type");
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
    tie(pr, pr_exists) =
        _ss->_responses_pending.get_and_remove_safe(stoi(req_id));

    if( !pr_exists ){
        cerr<< " received duplicate or unexpected response:"
            << response.dump() << endl;
        return;
    }

    if( service != pr.service || command != pr.command ){
        stringstream ss;
        ss << "invalid response to request: " << response.dump()
           << ", (" << pr.service << "," << pr.command << ")";
        TDMA_API_THROW(StreamingException, ss.str() );
    }

    auto content = response["content"];
    if( pr.callback ){
        pr.callback( stoi(req_id), service, command, response["timestamp"],
                     content["code"], content["msg"] );
    }
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

#ifdef DEBUG_VERBOSE_1_
       stringstream ss;
       ss << response;
       D("notify: " + ss.str(), _ss);
#endif /* DEBUG_VERBOSE_1_ */

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
    }catch(std::exception& e){
        TDMA_API_THROW( StreamingException,
                        "invalid 'data' response: " + string(e.what()) );
    }
}

bool
StreamingSessionImpl::_login()
{
    D("login", this);

    map<string, string> params{
        {"token", _streamer_info.credentials.token},
        {"version", StreamingSession::VERSION},
        {"credential", _streamer_info.credentials_encoded}
    };

    int req_id = _next_request_id++;
    StreamingRequests requests(
        { AdminSubscriptionImpl(CommandType::LOGIN, params) },
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
        TDMA_API_THROW(StreamingException,"invalid response JSON");

    string response_ty = r.key();
    if( response_ty != "response" ){
        TDMA_API_THROW( StreamingException,
                        "invalid login response: " + response_ty );
    }

    auto info = r.value()[0];
    string service = info["service"];
    string command = info["command"];
    string response_req_id = info["requestid"];

    if( service != to_string(StreamerServiceType::ADMIN) ||
        command != to_string(CommandType::LOGIN ) ||
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

    json j = {
        {"request_id", response_req_id},
        {"command ", command},
        {"code", code},
        {"message", msg}
    };

    this->_exec_callback(StreamingCallbackType::request_response,
                         StreamerServiceType::ADMIN, info["timestamp"], j);
    
    if( code ){
        cerr<< "logout error"
            << "\t code: " << code << endl
            << "\t message: " << msg << endl;
        return false;
    }                    

    _server_id = msg;
    D("login success", this);
    return true;
}


bool
StreamingSessionImpl::_logout()
{
    using namespace std::chrono;

    D("logout", this);

    /*
     * need to be sure the listening thread is already dead so it can't
     * steal our confirm
     */
    assert( !_listening );
    assert( !_listener_thread.joinable() );

    int req_id = _next_request_id++;
    StreamingRequests requests(
            { AdminSubscriptionImpl(CommandType::LOGOUT) },
            _account_id, 
            _streamer_info.credentials.app_id, 
            { req_id }
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
            TDMA_API_THROW(StreamingException,"invalid logout response JSON");

        string response_ty = r.key();
        if( response_ty == "response" ){
            auto info = r.value()[0];
            string service = info["service"];
            string command = info["command"];
            string response_req_id = info["requestid"];

            if( service == to_string(StreamerServiceType::ADMIN) &&
                command == to_string(CommandType::LOGOUT ) &&
                response_req_id == to_string(req_id) )
            {
                auto content = info["content"];
                int code = content["code"];
                string msg = content["msg"];

                json j = {
                    {"request_id", response_req_id},
                    {"command ", command},
                    {"code", code},
                    {"message", msg}
                };

                this->_exec_callback( StreamingCallbackType::request_response,
                                      StreamerServiceType::ADMIN,
                                      info["timestamp"], j );
                
                if( code ){
                    cerr<< "logout error"
                        << "\t code: " << code << endl
                        << "\t message: " << msg << endl;
                    break;
                }
                D("logout success", this);
                return true;
            }
        }

        auto t_elapsed =
            duration_cast<milliseconds>(steady_clock::now() - t_beg);
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
    }else{
        TDMA_API_THROW( StreamingException,
                        "can not set QOS on a stopped session" );
    }

    std::shared_ptr<PendingResponseBundle> bndl(new PendingResponseBundle());
    PendingResponse::response_cb_ty cb =
        [=](int id, string serv, string cmd, unsigned long long ts,
            int code, string msg)
        {
            bndl->successes[0] = (code == 0);
            bndl->msg = msg;
            {
                std::lock_guard<mutex> _(bndl->mtx);
                ++(bndl->n);
            }
            bndl->cond.notify_all();

            json j = {
                {"request_id", id},
                {"command ", cmd},
                {"code", code},
                {"message", msg}
            };
            this->_exec_callback(StreamingCallbackType::request_response,
                                 streamer_service_from_str(serv), ts, j);
        };

    AdminSubscriptionImpl sub(
        CommandType::QOS,
        {{"qoslevel", to_string(static_cast<int>(qos))}}
    );
    _send_requests( {sub}, cb );

    std::unique_lock<mutex> l(bndl->mtx);
    if( !bndl->cond.wait_for(l, _subscribe_timeout,
                             [&](){ return bndl->is_ready(); } ) )
    {
        cerr<< "timed out trying to set QOS" << endl;
    }

    if( bndl->successes[0] ){
        _qos = qos;
        return true;
    }

    return false;
}


void
StreamingSessionImpl::_send_requests(
    const vector<StreamingSubscriptionImpl>& subscriptions,
    PendingResponse::response_cb_ty callback
    )
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
    for(auto& s : subscriptions)
        req_ids.push_back( _next_request_id++ );

    StreamingRequests requests( subscriptions, _account_id,
                                _streamer_info.credentials.app_id, req_ids );

    auto msg = requests.to_json().dump();
    _client->send( msg );

    for( size_t i = 0; i < subscriptions.size(); ++i ){
        _responses_pending.insert(
            req_ids[i],
            PendingResponse( req_ids[i],
                             subscriptions[i].get_service_str(),
                             subscriptions[i].get_command_str(),
                             callback )
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
    }else{
        TDMA_API_THROW( StreamingException,
                        "can not add subscriptions to a stopped session" );
    }

    if( subscriptions.empty() )
        return {};

    std::shared_ptr<PendingResponseBundle> bndl(
        new PendingResponseBundle(subscriptions.size())
    );

    PendingResponse::response_cb_ty cb = [=]( int id,
                                              string serv,
                                              string cmd,
                                              unsigned long long ts,
                                              int code,
                                              string msg )
        {
            bndl->successes[bndl->n] = ( code == 0 );
            json j = {
                  {"request_id", id},
                  {"command ", cmd},
                  {"code", code},
                  {"message", msg}
              };
            this->_exec_callback( StreamingCallbackType::request_response,
                                  streamer_service_from_str(serv), ts, j );
            {
                std::lock_guard<mutex> _(bndl->mtx);
                ++(bndl->n);
                if( !bndl->is_ready() )
                    return;
            }
            bndl->cond.notify_all();
        };

    _send_requests(subscriptions, cb);

    std::unique_lock<mutex> lock(bndl->mtx);
    if( !bndl->cond.wait_for( lock, _subscribe_timeout,
                              [&](){ return bndl->is_ready(); } ) )
    {
        /*
         * TODO - should we clear _pending_responses ??
         *                        signal bndl to ignore response/callback ??
         */
        cerr<< "timed out waiting for subscription response" << endl;
    }

    return bndl->successes;
}


deque<bool>
StreamingSessionImpl::start(
    const vector<StreamingSubscriptionImpl>& subscriptions
    )
{
    D("start", this);

    if( _client )
        TDMA_API_THROW(StreamingException,"session has already started");

    if( subscriptions.empty() )
        TDMA_API_THROW(StreamingException,"subscriptions is empty");

    for( auto& sub : subscriptions ){
        if( sub.get_command_str() != to_string(CommandType::SUBS) )
            TDMA_API_THROW(StreamingException, "can only start() session "
                "using Subscriptions with command 'SUBS'");
    }

    D("check unique session", this);
    string acct = get_primary_account_id();
    if( active_accounts.count(acct) ){
        TDMA_API_THROW( StreamingException,
                        "Can not start Session; one is already active "
                        "for this primary account: " + acct );
    }

    D("_client->reset", this);
    _client.reset( new conn::WebSocketClient(_streamer_info.url) );

    D("_client->connect", this);
    _client->connect( _connect_timeout );
    if( !_client->is_connected() ){
        _client.reset();
        TDMA_API_THROW( StreamingException,
                        "streaming session failed to connect" );
    }

    _logged_in = _login();
    if( !_logged_in )
        TDMA_API_THROW(StreamingException,"login failed");

    /* only after connect AND login do we consider this an active session */
    active_accounts.insert(acct);
    _start_listener_thread();
    return add_subscriptions(subscriptions);
}


void
StreamingSessionImpl::stop()
{
    D("stop", this);

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
    D("_reset", this);
    _client.reset();
    _responses_pending.clear();
    _server_id.clear();
    try{
        active_accounts.erase( get_primary_account_id() );
    }catch(...){}
}


void
StreamingSessionImpl::_start_listener_thread()
{
    D("start listener thread", this);
    assert( !_listening );

    D("join listener thread", this);
    if( _listener_thread.joinable() )
        _listener_thread.join();

    D("move new listener thread", this);
    _listener_thread = std::move( std::thread(ListenerThreadTarget(this)) );
}


void
StreamingSessionImpl::_stop_listener_thread()
{
    D("stop listening thread", this);
    /*
     * force listeners thread out of a wait, but allow it to consume messages
     * up to *this* point first by setting _listening to false in loop
     */
    if( _listening && _client )
        _client->push_empty_message();

    D("join listener thread", this);
    if( _listener_thread.joinable() )
        _listener_thread.join();
    D("join listener thread DONE", this);
}

} /*tdma*/


namespace{

int
call_session_with_subs(
    StreamingSession_C *psession,
    StreamingSubscription_C **subs,
    size_t nsubs,
    int *results_buffer,
    deque<bool>(*meth)(void*, const vector<tdma::StreamingSubscriptionImpl>&),
    int allow_exceptions )
{
    using namespace tdma;

    int err = proxy_is_callable<StreamingSessionImpl>(psession, allow_exceptions);
    if( err )
        return err;

    if( nsubs > STREAMING_MAX_SUBSCRIPTIONS ){
        return HANDLE_ERROR( ValueException,
                             "nsubs > STREAMING_MAX_SUBSCRIPTIONS",
                             allow_exceptions );
    }

    if( nsubs == 0 )
        return HANDLE_ERROR(ValueException,"nsubs == 0", allow_exceptions);

    vector<StreamingSubscriptionImpl> res;
    try{
        for(size_t i = 0; i < nsubs; ++i){
            StreamingSubscription_C *c = subs[i];
            assert(c);
            res.emplace_back( C_sub_ptr_to_impl(c) );
        }
    }catch(std::exception& e){
        return HANDLE_ERROR(StreamingException, e.what(), allow_exceptions);
    }

    deque<bool> results;
    tie(results, err) = CallImplFromABI( allow_exceptions, meth,
                                         psession->obj, res );
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

} /* namespace */


using namespace tdma;

int
StreamingSession_Create_ABI( struct Credentials *pcreds,
                             streaming_cb_ty callback,
                             unsigned long connect_timeout,
                             unsigned long listening_timeout,
                             unsigned long subscribe_timeout,
                             StreamingSession_C *psession,
                             int allow_exceptions )
{
    CHECK_PTR(psession, "session", allow_exceptions);
    CHECK_PTR_KILL_PROXY(pcreds, "credentials", allow_exceptions, psession);
    CHECK_PTR_KILL_PROXY(callback, "callback", allow_exceptions, psession);

    if( !pcreds->access_token | !pcreds->refresh_token | !pcreds->client_id ){
        return HANDLE_ERROR_EX( LocalCredentialException,
                                "invalid credentials struct",
                                allow_exceptions, psession );
    }

    static auto meth = +[](struct Credentials *pcreds, streaming_cb_ty cb,
                           unsigned long cto, unsigned long lto,
                           unsigned long sto)
                           {
        StreamerInfo si = get_streamer_info(*pcreds);
        return new StreamingSessionImpl( si, cb, milliseconds(cto),
                                         milliseconds(lto), milliseconds(sto) );
    };

    int err;
    StreamingSessionImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds,
                                     callback, connect_timeout,
                                     listening_timeout, subscribe_timeout);
    if( err ){
        kill_proxy(psession);
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

{ return destroy_proxy<StreamingSessionImpl>(psession, allow_exceptions); }

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
    int err = proxy_is_callable<StreamingSessionImpl>(psession, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj){
        reinterpret_cast<StreamingSessionImpl*>(obj)->stop();
    };

    return CallImplFromABI(allow_exceptions, meth, psession->obj);
}


int
StreamingSession_IsActive_ABI( StreamingSession_C *psession,
                               int *is_active,
                               int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSessionImpl>(psession, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj){
        return static_cast<int>(
            reinterpret_cast<StreamingSessionImpl*>(obj)->is_active()
            );
    };

    tie(*is_active, err) = CallImplFromABI(allow_exceptions, meth, psession->obj);
    return err;
}

int
StreamingSession_AddSubscriptions_ABI( StreamingSession_C *psession,
                                       StreamingSubscription_C **subs,
                                       size_t nsubs,
                                       int *results_buffer,
                                       int allow_exceptions )
{
    auto meth = +[](void *obj, const vector<StreamingSubscriptionImpl>& s){
        return reinterpret_cast<StreamingSessionImpl*>(obj)
            ->add_subscriptions(s);
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
    int err = proxy_is_callable<StreamingSessionImpl>(psession, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM(QOSType, qos, allow_exceptions);

    auto meth = +[](void *obj, int q){
        return static_cast<int>(
            reinterpret_cast<StreamingSessionImpl*>(obj)
                ->set_qos( static_cast<QOSType>(q) )
            );
    };

    tie(*result, err) = CallImplFromABI(allow_exceptions, meth,
                                        psession->obj, qos);
    return err;
}

int
StreamingSession_GetQOS_ABI( StreamingSession_C *psession,
                             int *qos,
                             int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSessionImpl>(psession, allow_exceptions);
    if( err )
        return err;

    auto meth = +[](void *obj){
        return static_cast<int>(
            reinterpret_cast<StreamingSessionImpl*>(obj)->get_qos()
            );
    };

    tie(*qos, err) = CallImplFromABI(allow_exceptions, meth, psession->obj);
    return err;
}
