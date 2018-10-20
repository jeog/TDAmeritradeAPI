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

    static std::string
    throttled_get(APIGetterImpl& getter);

    static std::chrono::milliseconds
    throttled_wait_remaining();

    api_on_error_cb_ty _on_error_callback;
    std::reference_wrapper<Credentials> _credentials;
    conn::HTTPSGetConnection _connection;

protected:
    APIGetterImpl(Credentials& creds, api_on_error_cb_ty on_error_callback);

    /*
     * restrict copy and assign (for now at least):
     *
     *   1) should there ever be more than one of the same exact getter?
     *   2) want to restrict copy/assign of the underlying connection
     *      object to simplify things so if we share refs to it:
     *         a) one ref can close() on another
     *         b) destruction becomes more complicated
     *
     * allow move (consistent w/ underlying connection objects)
     */

    APIGetterImpl( APIGetterImpl&& ) = default;

    APIGetterImpl&
    operator=( APIGetterImpl&& ) = default;

    virtual
    ~APIGetterImpl(){}

    void
    set_url(std::string url);

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

    virtual std::string
    get();

    void
    close();

    bool
    is_closed() const;
};

} /* tdma */
