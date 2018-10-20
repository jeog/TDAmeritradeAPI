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

#ifndef WEBSOCKET_CONNECT_H
#define WEBSOCKET_CONNECT_H

#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <signal.h>

#include "_common.h"
#include "../include/util.h"
#include "threadsafe_queue.h"

#include "../uWebSockets/uWS.h"

namespace conn{

class WebSocketClient{
    typedef uWS::WebSocket<uWS::CLIENT> uws_client_ty;

    struct Callbacks{
        static WebSocketClient *wsc;

        static void
        on_connect(uws_client_ty *ws, uWS::HttpRequest r);

        static void
        on_disconnect(uws_client_ty *ws, int code, char *msg, size_t msg_len);

        static void
        on_error(void *v);

        static void
        on_message(uws_client_ty *ws, char *msg, size_t msg_len, uWS::OpCode op);

        static void
        on_signal(uS::Async *a);
    };

    uWS::Hub _hub;
    std::string _url;
    uS::Async *_signal;
    std::thread _thread;
    ThreadSafeQueue<std::string> _in_queue; // in from server
    ThreadSafeQueue<std::string> _out_queue; // out to server
    std::condition_variable _init_cond;
    bool _init_flag;
    std::mutex _init_mtx;
    uws_client_ty *_ws; // sync issues with is_connected() ?
    Callbacks _callbacks;

    enum class CloseType {
        none,
        graceful,
        immediate
    };
    volatile CloseType _closing_state;

    struct SocketThreadTarget{
        WebSocketClient *_wsc;
        std::chrono::milliseconds _timeout;

        void
        operator()(){
            util::debug_out("WebSocket", "SocketThreadTarget IN", _wsc,
                            std::cout);
            _wsc->_hub.connect(_wsc->_url, nullptr, {}, _timeout.count());
            _wsc->_hub.run();
            util::debug_out("WebSocket", "SocketThreadTarget OUT", _wsc,
                            std::cout);
        }

        SocketThreadTarget(WebSocketClient *wsc,
                             std::chrono::milliseconds timeout)
            : _wsc(wsc), _timeout(timeout)
        {}
    };

public:
    WebSocketClient(std::string url);

    WebSocketClient( const WebSocketClient& ) = delete;

    WebSocketClient&
    operator=( const WebSocketClient& ) = delete;

    virtual
    ~WebSocketClient();

    void
    connect(std::chrono::milliseconds timeout);

    bool
    is_connected() const;

    void
    close(bool graceful=true);

    void
    send(std::string msg);

    void
    push_empty_message()
    { _in_queue.push(""); }

    size_t
    nready()
    { return _in_queue.size(); }

    std::string
    recv();

    std::string
    recv_or_wait();

    std::string
    recv_or_wait_for(std::chrono::milliseconds timeout);

    std::vector<std::string>
    recv_all();

    std::vector<std::string>
    recv_atleast_n_or_wait(size_t n);

    std::vector<std::string>
    recv_atleast_n_or_wait_for(size_t n, std::chrono::milliseconds timeout);

    std::vector<std::string>
    recv_atmost_n(size_t n);

    std::vector<std::string>
    recv_n_or_wait(size_t n);

    std::vector<std::string>
    recv_n_or_wait_for(size_t n, std::chrono::milliseconds timeout);
};

} /* conn */

#endif // WEBSOCKET_CONNECT_H
