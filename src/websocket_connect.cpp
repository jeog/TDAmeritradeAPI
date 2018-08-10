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
#include <assert.h>

#include "../include/websocket_connect.h"



namespace conn{

using namespace std;

void
D(string msg, WebSocketClient *obj)
{ util::debug_out("WebSocket", msg, obj, std::cout); }

WebSocketClient *WebSocketClient::Callbacks::wsc = nullptr;

WebSocketClient::WebSocketClient(string url)
    :
        _hub(),
        _url(url),
        _signal(new uS::Async(_hub.getLoop())),
        _thread(),
        _in_queue(),
        _out_queue(),
        _init_cond(),
        _init_flag(false),
        _init_mtx(),
        _ws(nullptr),
        _closing_state( CloseType::none )
    {
        Callbacks::wsc = this;
        _hub.onConnection( Callbacks::on_connect );
        _hub.onDisconnection( Callbacks::on_disconnect );
        _hub.onError( Callbacks::on_error );
        _hub.onMessage( Callbacks::on_message );
        _signal->start( Callbacks::on_signal );
        _signal->setData( reinterpret_cast<void*>(this) );
        D("construct", this);
    }


WebSocketClient::~WebSocketClient()
{
    D("destruct", this);
    close(false); 
}


void
WebSocketClient::Callbacks::on_connect( uws_client_ty *ws, uWS::HttpRequest r)
{
    D("on_connect", wsc);

    assert(wsc);
    wsc->_ws = ws;
    {
        lock_guard<mutex> _(wsc->_init_mtx);
        wsc->_init_flag = true;
    }
    wsc->_init_cond.notify_all();
}


void
WebSocketClient::Callbacks::on_disconnect( uws_client_ty *ws,
                                                int code,
                                                char* msg,
                                                size_t msg_len )
{
    D("on_disconnect", wsc);

    assert(wsc);
    wsc->_ws = nullptr;

    D("on_disconnect, _signal->close", wsc);
    wsc->_signal->close();
}


void
WebSocketClient::Callbacks::on_error(void *v)
{
    D("on_error", wsc);

    assert(wsc);
    wsc->_ws = nullptr;

    D("on_error, _signal->close", wsc);
    wsc->_signal->close();
    {
        lock_guard<mutex> _(wsc->_init_mtx);
        wsc->_init_flag = true;
    }
    wsc->_init_cond.notify_all();
}


void
WebSocketClient::Callbacks::on_message( uws_client_ty *ws,
                                             char* msg,
                                             size_t msg_len,
                                             uWS::OpCode op )
{
    D("on_message", wsc);

    assert(wsc);
    string msg_s(msg, msg_len);
    assert( !msg_s.empty() );

    D("message: " + msg_s, wsc);
    wsc->_in_queue.emplace( msg_s );
}


void
WebSocketClient::Callbacks::on_signal(uS::Async *a)
{
    D("on_signal", wsc);

    auto wsc = reinterpret_cast<WebSocketClient*>(a->getData());
    assert(wsc);
    assert(wsc == WebSocketClient::Callbacks::wsc);
    assert(wsc->_ws);

    if( wsc->_closing_state == CloseType::immediate ){
        D("on_signal, _ws->terminate", wsc);
        wsc->_ws->terminate();
        return;
    }

    while( !wsc->_out_queue.empty() ){
        string msg = wsc->_out_queue.front();
        wsc->_out_queue.pop();
        D("on_signal, _ws->send: " + msg, wsc);
        wsc->_ws->send(msg.c_str(), msg.size(), uWS::OpCode::TEXT);
    }

    if( wsc->_closing_state == CloseType::graceful ){
        D("on_signal, _ws->close", wsc);
        wsc->_ws->close();
    }
}


void
WebSocketClient::connect(chrono::milliseconds timeout)
{
    D("connect", this);
    if( _ws || _closing_state != CloseType::none )
        return;

    D("connect, move new _thread", this);
    _thread = move( thread( SocketThreadTarget(this, timeout) ) );

    D("connect, wait for callback notify", this);
    /*
     * let the socket handle the timeout
     *
     * this will eventually cause synchronization problems when a client
     * tries to aggressively reconnect considering:
     *    listening timeouts in the socket,
     *    listening timeouts in the streaming session,
     *    connect timeouts in the socket,
     *    connect timeouts in the streaming session
     *
     *  (CAN WE DEADLOCK HERE ??)
     */
    unique_lock<mutex> lock(_init_mtx);
    _init_cond.wait( lock, [this]{ return _init_flag; } );
}


bool
WebSocketClient::is_connected() const
{ return _ws && _closing_state == CloseType::none; }


void
WebSocketClient::close(bool graceful)
{
    D("close", this);
    if( is_connected() ){
        _closing_state = graceful ? CloseType::graceful : CloseType::immediate;
        D("close, _signal->send", this);
        _signal->send();
    }

    D("close, join _thread", this);
    if( _thread.joinable() ){
        _thread.join();
    }
    D("close, join _thread DONE", this);
}


void
WebSocketClient::send(string msg)
{
    if( is_connected() ){
        _out_queue.emplace(msg);
        D("send, _signal->send: " + msg, this);
        _signal->send();
    }
}


string
WebSocketClient::recv()
{
    auto p = _in_queue.front_safe();
    return p.second ? p.first : "";
}


string
WebSocketClient::recv_or_wait()
{
    return _in_queue.pop_front_or_wait();
}


string
WebSocketClient::recv_or_wait_for(chrono::milliseconds timeout)
{
    auto p = _in_queue.pop_front_or_wait_for(timeout);
    return p.second ? p.first : "";
}


vector<string>
WebSocketClient::recv_all()
{
    vector<string> ret;

    auto p = _in_queue.pop_front_safe();
    while( p.second ){
        ret.emplace_back( p.first );
        p = _in_queue.pop_front_safe();
    }
    return ret;
}

/* 
 * NOTE atleast_n versions could theoretically return more than they currently
 *      do (e.g. a bunch of data comes in between recv_all and recv_n_or_wait,
 *      there may be more than n - all_n entries in the queue)
 *      but this is good enough for now
 *
 * also, we don't account for elapsed time in the initial part of the call
 */
vector<string>
WebSocketClient::recv_atleast_n_or_wait(size_t n)
{
    vector<string> all = recv_all();
    size_t all_n = all.size();
    if( all_n >= n )
        return all;

    return recv_n_or_wait(n - all_n);
}


vector<string>
WebSocketClient::recv_atleast_n_or_wait_for(size_t n, chrono::milliseconds timeout)
{
    vector<string> all = recv_all();
    size_t all_n = all.size();
    if( all_n >= n )
        return all;

    return recv_n_or_wait_for(n - all_n, timeout);
}


vector<string>
WebSocketClient::recv_atmost_n(size_t n)
{
    vector<string> ret;
    while( ret.size() < n ){
        auto p = _in_queue.pop_front_safe();
        if( !p.second )
            break;
        ret.emplace_back(p.first);
    }
    return ret;
}


vector<string>
WebSocketClient::recv_n_or_wait(size_t n)
{
    vector<string> ret;
    while( ret.size() < n ){
        auto p = _in_queue.pop_front_or_wait();
        ret.emplace_back(p);
    }
    return ret;
}


vector<string>
WebSocketClient::recv_n_or_wait_for(size_t n, chrono::milliseconds timeout)
{
    using namespace chrono;

    vector<string> ret;
    auto t_beg = steady_clock::now();
    auto t_left = timeout;

    while( ret.size() < n && t_left.count() >= 0 ){
        auto p = _in_queue.pop_front_or_wait_for(t_left);
        if( !p.second )
            break;
        ret.emplace_back(p.first);
        auto t_elapsed = duration_cast<milliseconds>(steady_clock::now() - t_beg);
        t_left = timeout - t_elapsed;
    }
    return ret;
}

} /* conn */
