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

#include "../include/_tdma_api.h"

namespace tdma {

using namespace std;

const string URL_ACCOUNT_INFO = URL_BASE + "accounts/";

/*
 *  The responses don't appear completely consistent w/ the docs so
 *  we have to define different callbacks. 
 */
void
account_api_on_error_callback(long code, const string& data, bool allow_refresh)
{
    /*
     *  The server responds with 401 error to indicate an expired access token. 
     *  We also don't need to unencode the opening '{' al la default_callback.
     *
     *  NOTE - WE SHOULD ONLY GET OUT OF HERE IF WE CAN REFRESH THE TOKEN
     */
    json data_json;
    string data_uesc, err_msg;

    switch(code){
    case 500:
        data_uesc = unescape_returned_post_data(data);
        data_json = json::parse(data_uesc);
        err_msg = data_json["error"];
        throw ServerError("unexpected server error: " + err_msg, code);
    case 503:
        throw ServerError("server (temporarily) unavailable", code);
    case 400:
        throw InvalidRequest("validation problem", code);
    case 401:
        if( !allow_refresh ){
            throw APIExecutionException("not allowed to refresh token", code);
        }
        data_json = json::parse(data);
        err_msg = data_json["error"];
        if( !error_msg_about_token_expiration(err_msg) ){
            throw APIExecutionException("unknow exception: " + err_msg, code);
        }
        return; /* REFRESH TOKEN */
    case 403:
        throw InvalidRequest("forbidden", code);
    case 404:
        throw InvalidRequest("not found", code);
    default:
        throw APIExecutionException("unknown exception", code);
    };
}


AccountGetterBase::AccountGetterBase( Credentials& creds,
                                      const string& account_id )
    :
       APIGetter(creds, account_api_on_error_callback),
       _account_id(account_id)
    {
       if( account_id.empty() )
           throw ValueException("account_id is empty");
    }

void
AccountGetterBase::set_account_id(const string& account_id)
{
    if( account_id.empty() )
        throw ValueException("account_id is empty");
    _account_id = account_id;
    build();
}


AccountInfoGetter::AccountInfoGetter( Credentials& creds,
                                      const string& account_id,
                                      bool positions,
                                      bool orders )
    :
        AccountGetterBase(creds, account_id),
        _positions(positions),
        _orders(orders)
    {
        _build();
    }

void
AccountInfoGetter::_build()
{
    string fields;
    if(_positions){
        fields = "?fields=positions";
        if(_orders){
            fields += ",orders";
        }
    }else if(_orders){
        fields = "?fields=orders";
    }

    string url = URL_ACCOUNT_INFO + util::url_encode(get_account_id()) + fields;
    APIGetter::set_url(url);
}

/*virtual*/ void
AccountInfoGetter::build()
{ _build(); }

void
AccountInfoGetter::return_positions(bool positions)
{
    _positions = positions;
    build();
}

void
AccountInfoGetter::return_orders(bool orders)
{
    _orders = orders;
    build();
}


PreferencesGetter::PreferencesGetter( Credentials& creds,
                                      const string& account_id)
    :
        AccountGetterBase(creds, account_id)
    {
        _build();
    }

void
PreferencesGetter::_build()
{
    string url = URL_ACCOUNT_INFO + util::url_encode(get_account_id())
                 + "/preferences";
    APIGetter::set_url(url);
}

/*virtual*/ void
PreferencesGetter::build()
{ _build(); }

/*
void
UpdatePreferences( Credentials& creds, string account_id)
{
    if( account_id.empty() ){
        throw ValueException("account_id is empty");
    }

    string url = URL_ACCOUNT_INFO + account_id + "/preferences";

    // TODO

    throw runtime_error("NOT IMPLEMENTED");
}
*/


UserPrincipalsGetter::UserPrincipalsGetter( Credentials& creds,                                           
                                            bool streamer_subscription_keys,
                                            bool streamer_connection_info,
                                            bool preferences,
                                            bool surrogate_ids )
    :
        APIGetter(creds, account_api_on_error_callback),
        _streamer_subscription_keys(streamer_subscription_keys),
        _streamer_connection_info(streamer_connection_info),
        _preferences(preferences),
        _surrogate_ids(surrogate_ids)
    {
        _build();
    }


void
UserPrincipalsGetter::_build()
{
    vector<string> fields;
    if(_streamer_subscription_keys)
        fields.emplace_back("streamerSubscriptionKeys");
    if(_streamer_connection_info)
        fields.emplace_back("streamerConnectionInfo");
    if(_preferences)
        fields.emplace_back("preferences");
    if(_surrogate_ids)
        fields.emplace_back("surrogateIds");

    string fields_str;
    if( !fields.empty() ){
        fields_str = "?fields=" + util::join(fields, ',');
    }

    string url = URL_BASE + "userprincipals" + fields_str;
    APIGetter::set_url(url);
}

/*virtual*/ void
UserPrincipalsGetter::build()
{ _build(); }

void
UserPrincipalsGetter::return_streamer_subscription_keys(
        bool streamer_subscription_keys
        )
{
    _streamer_subscription_keys = streamer_subscription_keys;
    build();
}

void
UserPrincipalsGetter::return_streamer_connection_info(
        bool streamer_connection_info
        )
{
    _streamer_connection_info = streamer_connection_info;
    build();
}

void
UserPrincipalsGetter::return_preferences(bool preferences)
{
    _preferences = preferences;
    build();
}

void
UserPrincipalsGetter::return_surrogate_ids(bool surrogate_ids)
{
    _surrogate_ids = surrogate_ids;
    build();
}


/* should go to streaming.cpp ? */
StreamerSubscriptionKeysGetter::StreamerSubscriptionKeysGetter(
        Credentials& creds,
        const string& account_id
        )
    :
        AccountGetterBase(creds, account_id)
    {
        _build();
    }

void
StreamerSubscriptionKeysGetter::_build()
{
    string params = "?accountIds=" + util::url_encode(get_account_id());
    string url = URL_BASE + "userprincipals/streamersubscriptionkeys"
                 + params;
    APIGetter::set_url(url);
}

/*virtual*/ void
StreamerSubscriptionKeysGetter::build()
{ _build(); }


TransactionHistoryGetter::TransactionHistoryGetter(
        Credentials& creds,
        const string& account_id,
        TransactionType transaction_type,
        const string& symbol,
        const string& start_date,
        const string& end_date
        )
    :
        AccountGetterBase(creds, account_id),
        _transaction_type(transaction_type),
        _symbol(symbol),
        _start_date(start_date),
        _end_date(end_date)
    {
        if( !start_date.empty() && !util::is_valid_iso8601_datetime(start_date) )
            throw ValueException("invalid ISO-8601 date: " + start_date);

        if( !end_date.empty() && !util::is_valid_iso8601_datetime(end_date) )
            throw ValueException("invalid ISO-8601 date: " + end_date);

        _build();
    }

void
TransactionHistoryGetter::_build()
{
    vector<pair<string,string>> params{{"type", to_string(_transaction_type)}};

    if( !_symbol.empty() )
        params.emplace_back("symbol", _symbol);
    if( !_start_date.empty() )
        params.emplace_back("startDate", _start_date);
    if( !_end_date.empty() )
        params.emplace_back("endDate", _end_date);

    string qstr = util::build_encoded_query_str(params);
    string url = URL_ACCOUNT_INFO + util::url_encode(get_account_id())
                 + "/transactions?" + qstr;
    APIGetter::set_url(url);
}

/*virtual*/ void
TransactionHistoryGetter::build()
{ _build(); }

void
TransactionHistoryGetter::set_transaction_type(
        TransactionType transaction_type
        )
{
    _transaction_type = transaction_type;
    build();
}

void
TransactionHistoryGetter::set_symbol(const string& symbol)
{
    _symbol = symbol;
    build();
}

void
TransactionHistoryGetter::set_start_date(const string& start_date)
{
    if( !start_date.empty() && !util::is_valid_iso8601_datetime(start_date) ){
        throw ValueException("invalid ISO-8601 date: " + start_date);
    }
    _start_date = start_date;
    build();
}

void
TransactionHistoryGetter::set_end_date(const string& end_date)
{
    if( !end_date.empty() && !util::is_valid_iso8601_datetime(end_date) ){
        throw ValueException("invalid ISO-8601 date: " + end_date);
    }
    _end_date = end_date;
    build();
}


IndividualTransactionHistoryGetter::IndividualTransactionHistoryGetter(
        Credentials& creds,
        const string& account_id,
        const string& transaction_id
        )
    :
        AccountGetterBase(creds, account_id),
        _transaction_id(transaction_id)
    {
        if( transaction_id.empty() )
            throw ValueException("transaction id is empty");

        _build();
    }

void
IndividualTransactionHistoryGetter::_build()
{

    string url = URL_ACCOUNT_INFO + util::url_encode(get_account_id())
                 + "/transactions/" + util::url_encode(_transaction_id);
    APIGetter::set_url(url);
}

/*virtual*/ void
IndividualTransactionHistoryGetter::build()
{ _build(); }

void
IndividualTransactionHistoryGetter::set_transaction_id(
        const string& transaction_id
        )
{
    if( transaction_id.empty() )
        throw ValueException("transaction id is empty");

    _transaction_id = transaction_id;
    build();
}


} /* tdma */
