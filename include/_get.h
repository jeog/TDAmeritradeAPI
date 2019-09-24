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

#include <string>
#include <chrono>

#include "curl_connect.h"
#include "tdma_api_get.h"

namespace tdma {

const int TYPE_ID_GETTER_QUOTE = 1;
const int TYPE_ID_GETTER_QUOTES = 2;
const int TYPE_ID_GETTER_MARKET_HOURS = 3;
const int TYPE_ID_GETTER_MOVERS = 4;
const int TYPE_ID_GETTER_HISTORICAL_PERIOD = 5;
const int TYPE_ID_GETTER_HISTORICAL_RANGE = 6;
const int TYPE_ID_GETTER_OPTION_CHAIN = 7;
const int TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY = 8;
const int TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL = 9;
const int TYPE_ID_GETTER_ACCOUNT_INFO = 10;
const int TYPE_ID_GETTER_PREFERENCES = 11;
const int TYPE_ID_GETTER_SUBSCRIPTION_KEYS = 12;
const int TYPE_ID_GETTER_TRANSACTION_HISTORY = 13;
const int TYPE_ID_GETTER_IND_TRANSACTION_HISTORY = 14;
const int TYPE_ID_GETTER_ORDER = 15;
const int TYPE_ID_GETTER_ORDERS = 16;
const int TYPE_ID_GETTER_USER_PRINCIPALS = 17;
const int TYPE_ID_GETTER_INSTRUMENT_INFO = 18;

class APIGetterImpl{
    static std::chrono::milliseconds wait_msec; // DEF_WAIT_MSEC
    static std::chrono::milliseconds last_get_msec; // 0
    static std::mutex get_mtx;
    static int current_connection_group;

    static std::string
    throttled_get(APIGetterImpl& getter);

    static std::chrono::milliseconds
    throttled_wait_remaining();

    api_on_error_cb_ty _on_error_callback;
    std::reference_wrapper<Credentials> _credentials;
    std::unique_ptr<conn::HTTPConnectionInterface> _connection;
    int _connection_group_id;

protected:
    APIGetterImpl(Credentials& creds, api_on_error_cb_ty on_error_callback);

    APIGetterImpl( const APIGetterImpl& ) = delete;

    APIGetterImpl&
    operator=( const APIGetterImpl& ) = delete;

    APIGetterImpl( APIGetterImpl&& ) = default;

    APIGetterImpl&
    operator=( APIGetterImpl&& ) = default;

    virtual
    ~APIGetterImpl(){}

    void
    set_url(const std::string& url);

public:
    typedef APIGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_QUOTE;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_INSTRUMENT_INFO;

    static const std::chrono::milliseconds DEF_WAIT_MSEC;

    static std::chrono::milliseconds
    get_wait_msec();

    static void
    set_wait_msec(std::chrono::milliseconds msec);

    static std::chrono::milliseconds
    wait_remaining();

    static void
    share_connections(bool share)
    { current_connection_group = (share ? 0 : -1); }

    static bool
    is_sharing_connections()
    { return current_connection_group == 0; }

    virtual std::string
    get();

    void
    close();

    bool
    is_closed() const;

    void
    set_timeout(std::chrono::milliseconds msec);

    std::chrono::milliseconds
    get_timeout() const;
};

} /* tdma */
