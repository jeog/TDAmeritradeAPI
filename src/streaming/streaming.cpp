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

#include <sstream>
#include <ctime>
#include <string>

#include "../../include/_streaming.h"

using std::string;

namespace {

#ifdef _WIN32
#define timegm _mkgmtime
#endif

long long
timestamp_to_ms(string ts)
{
    //"2018-06-12T02:18:23+0000"
    if( ts.size() != 24 || ts.substr(20,4) != "0000" ){
        TDMA_API_THROW( tdma::APIException,
                        "invalid timestamp from streamerInfo" );
    }

    auto pos = ts.begin();
    tm t = {0};
    t.tm_year = stoi(string(pos,pos + 4)) - 1900;
    pos += 5;
    t.tm_mon = stoi(string(pos, pos + 2)) - 1;
    pos += 3;
    t.tm_mday = stoi(string(pos, pos + 2));
    pos += 3;
    t.tm_hour = stoi(string(pos, pos + 2));
    pos += 3;
    t.tm_min = stoi(string(pos, pos + 2));
    pos += 3;
    t.tm_sec = stoi(string(pos, pos + 2));
    t.tm_isdst = 0;
    return timegm(&t) * 1000LL;
}

#undef timegm

} /* namespace */


namespace tdma {

// TODO allow to search multiple accounts; for now just default to first
StreamerInfo
get_streamer_info(Credentials& creds)
{
    json j = get_user_principals_for_streaming(creds);

    auto i_acct = j.find("accounts");
    if( i_acct == j.end() )
        TDMA_API_THROW(APIException,"returned user principals has no 'accounts'");

    auto i_sinfo = j.find("streamerInfo");
    if( i_sinfo == j.end() )
        TDMA_API_THROW(APIException,"returned user principals has no 'streamerInfo");

    json acct = (*i_acct)[0];
    json sinfo = *i_sinfo;

    StreamerInfo si;
    try{
        si.credentials.user_id = acct.at("accountId");
        si.credentials.token= sinfo.at("token");
        si.credentials.company = acct.at("company");
        si.credentials.segment = acct.at("segment");
        si.credentials.cd_domain = acct.at("accountCdDomainId");
        si.credentials.user_group = sinfo.at("userGroup");
        si.credentials.access_level = sinfo.at("accessLevel");
        si.credentials.authorized = true;
        si.credentials.timestamp = timestamp_to_ms(sinfo.at("tokenTimestamp"));
        si.credentials.app_id = sinfo.at("appId");
        si.credentials.acl = sinfo.at("acl");
        string addr = sinfo.at("streamerSocketUrl");
        si.url = "wss://" + addr + "/ws";
        si.primary_acct_id = j.at("primaryAccountId");
        si.encode_credentials();
    }catch(json::exception& e){
        TDMA_API_THROW(APIException,"failed to convert UserPrincipals JSON to"
                           " StreamerInfo: " + string(e.what()));
    }

    return si;
}


void
StreamerInfo::encode_credentials()
{
    std::stringstream ss;
    ss<< "userid" << "=" << credentials.user_id << "&"
      << "token" << "=" << credentials.token << "&"
      << "company" << "=" << credentials.company << "&"
      << "segment" << "=" << credentials.segment << "&"
      << "cddomain" << "=" << credentials.cd_domain << "&"
      << "usergroup" << "=" << credentials.user_group << "&"
      << "accesslevel" << "=" << credentials.access_level << "&"
      << "authorized" << "=" << (credentials.authorized ? "Y" : "N") << "&"
      << "acl" << "=" << credentials.acl << "&"
      << "timestamp" << "=" << credentials.timestamp << "&"
      << "appid" << "=" << credentials.app_id;

    credentials_encoded = util::url_encode(ss.str());
}

StreamerServiceType
streamer_service_from_str(string service_name)
{
    if( service_name == "NONE" )
        return StreamerServiceType::NONE;
    else if( service_name == "ADMIN" )
        return StreamerServiceType::ADMIN;
    else if( service_name == "ACTIVES_NASDAQ" )
        return StreamerServiceType::ACTIVES_NASDAQ;
    else if( service_name == "ACTIVES_NYSE" )
        return StreamerServiceType::ACTIVES_NYSE;
    else if( service_name == "ACTIVES_OTCBB" )
        return StreamerServiceType::ACTIVES_OTCBB;
    else if( service_name == "ACTIVES_OPTIONS" )
        return StreamerServiceType::ACTIVES_OPTIONS;
    else if( service_name == "CHART_EQUITY" )
        return StreamerServiceType::CHART_EQUITY;
    else if( service_name == "CHART_FOREX" )
        return StreamerServiceType::CHART_FOREX;
    else if( service_name == "CHART_FUTURES" )
        return StreamerServiceType::CHART_FUTURES;
    else if( service_name == "CHART_OPTIONS" )
        return StreamerServiceType::CHART_OPTIONS;
    else if( service_name == "QUOTE" )
        return StreamerServiceType::QUOTE;
    else if( service_name == "LEVELONE_FUTURES" )
        return StreamerServiceType::LEVELONE_FUTURES;
    else if( service_name == "LEVELONE_FOREX" )
        return StreamerServiceType::LEVELONE_FOREX;
    else if( service_name == "LEVELONE_FUTURES_OPTIONS" )
        return StreamerServiceType::LEVELONE_FUTURES_OPTIONS;
    else if( service_name == "OPTION" )
        return StreamerServiceType::OPTION;
    else if( service_name == "NEWS_HEADLINE" )
        return StreamerServiceType::NEWS_HEADLINE;
    else if( service_name == "TIMESALE_EQUITY" )
        return StreamerServiceType::TIMESALE_EQUITY;
    else if( service_name == "TIMESALE_FUTURES" )
        return StreamerServiceType::TIMESALE_FUTURES;
    else if( service_name == "TIMESALE_FOREX" )
        return StreamerServiceType::TIMESALE_FOREX;
    else if( service_name == "TIMESALE_OPTIONS" )
        return StreamerServiceType::TIMESALE_OPTIONS;
    else
        TDMA_API_THROW(ValueException,"invalid service name: " + service_name);
}

} /* tdma */


using namespace tdma;


int
QOSType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(QOSType, v, allow_exceptions);

    switch(static_cast<QOSType>(v)){
    case QOSType::delayed:
        return to_new_char_buffer("delayed", buf, n, allow_exceptions);
    case QOSType::express:
        return to_new_char_buffer("express", buf, n, allow_exceptions);
    case QOSType::fast:
        return to_new_char_buffer("fast", buf, n, allow_exceptions);
    case QOSType::moderate:
        return to_new_char_buffer("moderate", buf, n, allow_exceptions);
    case QOSType::real_time:
        return to_new_char_buffer("real-time", buf, n, allow_exceptions);
    case QOSType::slow:
        return to_new_char_buffer("slow", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid QOSType");
    }
}

int
CommandType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    // DONT CHECK_ENUM, need LOGIN, LOGOUT, QOS conversions even though private

    switch(static_cast<CommandType>(v)){
    case CommandType::SUBS:
        return to_new_char_buffer("SUBS", buf, n, allow_exceptions);
    case CommandType::UNSUBS:
        return to_new_char_buffer("UNSUBS", buf, n, allow_exceptions);
    case CommandType::ADD:
        return to_new_char_buffer("ADD", buf, n, allow_exceptions);
    case CommandType::VIEW:
        return to_new_char_buffer("VIEW", buf, n, allow_exceptions);
    case CommandType::LOGIN:
        return to_new_char_buffer("LOGIN", buf, n, allow_exceptions);
    case CommandType::LOGOUT:
        return to_new_char_buffer("LOGOUT", buf, n, allow_exceptions);
    case CommandType::QOS:
        return to_new_char_buffer("QOS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid CommandType");
    }
}

int
DurationType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(DurationType, v, allow_exceptions);

    switch(static_cast<DurationType>(v)){
    case DurationType::all_day:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    case DurationType::min_60:
        return to_new_char_buffer("3600", buf, n, allow_exceptions);
    case DurationType::min_30:
        return to_new_char_buffer("1800", buf, n, allow_exceptions);
    case DurationType::min_10:
        return to_new_char_buffer("600", buf, n, allow_exceptions);
    case DurationType::min_5:
        return to_new_char_buffer("300", buf, n, allow_exceptions);
    case DurationType::min_1:
        return to_new_char_buffer("60", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid DurationType");
    }
}

int
VenueType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(VenueType, v, allow_exceptions);

    switch(static_cast<VenueType>(v)){
    case VenueType::opts:
        return to_new_char_buffer("OPTS", buf, n, allow_exceptions);
    case VenueType::calls:
        return to_new_char_buffer("CALLS", buf, n, allow_exceptions);
    case VenueType::puts:
        return to_new_char_buffer("PUTS", buf, n, allow_exceptions);
    case VenueType::opts_desc:
        return to_new_char_buffer("OPTS-DESC", buf, n, allow_exceptions);
    case VenueType::calls_desc:
        return to_new_char_buffer("CALLS-DESC", buf, n, allow_exceptions);
    case VenueType::puts_desc:
        return to_new_char_buffer("PUTS-DESC", buf, n, allow_exceptions);
    default: throw std::runtime_error("Invalid VenueType");
    }
}

int
StreamingCallbackType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(StreamingCallbackType, v, allow_exceptions);

    switch(static_cast<StreamingCallbackType>(v)){
    case StreamingCallbackType::listening_start:
        return to_new_char_buffer("listening_start", buf, n, allow_exceptions);
    case StreamingCallbackType::listening_stop:
        return to_new_char_buffer("listening_stop", buf, n, allow_exceptions);
    case StreamingCallbackType::data:
        return to_new_char_buffer("data", buf, n, allow_exceptions);
    case StreamingCallbackType::request_response:
        return to_new_char_buffer("request_response", buf, n, allow_exceptions);
    case StreamingCallbackType::notify:
        return to_new_char_buffer("notify", buf, n, allow_exceptions);
    case StreamingCallbackType::timeout:
        return to_new_char_buffer("timeout", buf, n, allow_exceptions);
    case StreamingCallbackType::error:
        return to_new_char_buffer("error", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid StreamingCallbackType");
    }
}

int
StreamerServiceType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    if( v ){ // ::NONE (0) can't be allowed to fail in this particular case
        CHECK_ENUM(StreamerServiceType, v, allow_exceptions);
    }

    switch( static_cast<StreamerServiceType>(v) ){
    case StreamerServiceType::NONE:
        return to_new_char_buffer("NONE", buf, n, allow_exceptions);
    case StreamerServiceType::ADMIN:
        return to_new_char_buffer("ADMIN", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_NASDAQ:
        return to_new_char_buffer("ACTIVES_NASDAQ", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_NYSE:
        return to_new_char_buffer("ACTIVES_NYSE", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_OTCBB:
        return to_new_char_buffer("ACTIVES_OTCBB", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_OPTIONS:
        return to_new_char_buffer("ACTIVES_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_EQUITY:
        return to_new_char_buffer("CHART_EQUITY", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_FOREX:
        return to_new_char_buffer("CHART_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_FUTURES:
        return to_new_char_buffer("CHART_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_OPTIONS:
        return to_new_char_buffer("CHART_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::QUOTE:
        return to_new_char_buffer("QUOTE", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FUTURES:
        return to_new_char_buffer("LEVELONE_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FOREX:
        return to_new_char_buffer("LEVELONE_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FUTURES_OPTIONS:
        return to_new_char_buffer("LEVELONE_FUTURES_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::OPTION:
        return to_new_char_buffer("OPTION", buf, n, allow_exceptions);
    case StreamerServiceType::NEWS_HEADLINE:
        return to_new_char_buffer("NEWS_HEADLINE", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_EQUITY:
        return to_new_char_buffer("TIMESALE_EQUITY", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_FUTURES:
        return to_new_char_buffer("TIMESALE_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_FOREX:
        return to_new_char_buffer("TIMESALE_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_OPTIONS:
        return to_new_char_buffer("TIMESALE_OPTIONS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid StreamerServiceType");
    }
}


/* TODO return actual strings for fields */
#define DEF_TEMP_FIELD_TO_STRING(name) \
int \
name##_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS ) \
{ \
    CHECK_ENUM(name, v, allow_exceptions); \
    return to_new_char_buffer( \
        #name"-" + std::to_string(v), buf, n, allow_exceptions \
        ); \
}

DEF_TEMP_FIELD_TO_STRING(QuotesSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(OptionsSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(LevelOneFuturesSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(LevelOneForexSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(LevelOneFuturesOptionsSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(NewsHeadlineSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(ChartEquitySubscriptionField)
DEF_TEMP_FIELD_TO_STRING(ChartSubscriptionField)
DEF_TEMP_FIELD_TO_STRING(TimesaleSubscriptionField)

#undef DEF_TEMP_FIELD_TO_STRING

