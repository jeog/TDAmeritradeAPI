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
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "../include/_tdma_api.h"


#ifdef __cplusplus

namespace tdma{

using namespace std;

const unordered_map<PeriodType, set<int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE = {
    {PeriodType::day, set<int>{1,2,3,4,5,10}},
    {PeriodType::month, set<int>{1,2,3,6}},
    {PeriodType::year, set<int>{1,2,3,5,10,15,20}},
    {PeriodType::ytd, set<int>{1}},
};


const unordered_map<PeriodType, set<FrequencyType, EnumCompare<FrequencyType>>,
                    EnumHash<PeriodType>>
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE ={
    {PeriodType::day, {FrequencyType::minute} },
    {PeriodType::month, {FrequencyType::daily, FrequencyType::weekly} },
    {PeriodType::year, {FrequencyType::daily, FrequencyType::weekly,
                        FrequencyType::monthly} },
    {PeriodType::ytd, { FrequencyType::weekly} },
};


const unordered_map<FrequencyType, set<int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<int>{1,5,10,30}},
    {FrequencyType::daily, set<int>{1}},
    {FrequencyType::weekly, set<int>{1}},
    {FrequencyType::monthly, set<int>{1}},
};

} /* tdma */


#endif /* __cplusplus */


int
alloc_C_str(const std::string& s, char** buf, size_t* n, bool raise_exception)
{
    assert(buf);
    assert(n);
    *n = s.size() + 1;
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !*(buf) ){
        if( raise_exception ){
            throw tdma::MemoryError("not enough memory to allocate enum string");
        }
        return TDMA_API_MEMORY_ERROR;
    }
    strncpy(*buf, s.c_str(), (*n)-1);
    (*buf)[(*n)-1] = 0;
    return 0;
}

int
PeriodType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::PeriodType>(v)){
    case tdma::PeriodType::day:
        return alloc_C_str("day", buf, n, allow_exceptions);
    case tdma::PeriodType::month:
        return alloc_C_str("month", buf, n, allow_exceptions);
    case tdma::PeriodType::year:
        return alloc_C_str("year", buf, n, allow_exceptions);
    case tdma::PeriodType::ytd:
        return alloc_C_str("ytd", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid PeriodType");
    }
}

int
FrequencyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::FrequencyType>(v)){
    case tdma::FrequencyType::minute:
        return alloc_C_str("minute", buf, n, allow_exceptions);
    case tdma::FrequencyType::daily:
        return alloc_C_str("daily", buf, n, allow_exceptions);
    case tdma::FrequencyType::weekly:
        return alloc_C_str("weekly", buf, n, allow_exceptions);
    case tdma::FrequencyType::monthly:
        return alloc_C_str("monthly", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid FrequencyType");
    }
}

int
OptionContractType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionContractType>(v)){
    case tdma::OptionContractType::call:
        return alloc_C_str("CALL", buf, n, allow_exceptions);
    case tdma::OptionContractType::put:
        return alloc_C_str("PUT", buf, n, allow_exceptions);
    case tdma::OptionContractType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionContractType");
    }
}

int
OptionStrategyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionStrategyType>(v)){
    //case OptionStrategyType::single: return "SINGLE";
    //case OptionStrategyType::analytical: return "ANALYTICAL";
    case tdma::OptionStrategyType::covered:
        return alloc_C_str("COVERED", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::vertical:
        return alloc_C_str("VERTICAL", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::calendar:
        return alloc_C_str("CALENDAR", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::strangle:
        return alloc_C_str("STRANGLE", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::straddle:
        return alloc_C_str("STRADDLE", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::butterfly:
        return alloc_C_str("BUTTERFLY", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::condor:
        return alloc_C_str("CONDOR", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::diagonal:
        return alloc_C_str("DIAGONAL", buf, n, allow_exceptions);
    case tdma::OptionStrategyType::collar:
        return alloc_C_str("COLLAR", buf, n, allow_exceptions);
    case tdma:: OptionStrategyType::roll:
        return alloc_C_str("ROLL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionStrategyType");
    }
}


int
OptionRangeType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionRangeType>(v)){
    case tdma::OptionRangeType::null:
        return alloc_C_str("", buf, n, allow_exceptions);
    case tdma::OptionRangeType::itm:
        return alloc_C_str("ITM", buf, n, allow_exceptions);
    case tdma::OptionRangeType::ntm:
        return alloc_C_str("NTM", buf, n, allow_exceptions);
    case tdma::OptionRangeType::otm:
        return alloc_C_str("OTM", buf, n, allow_exceptions);
    case tdma::OptionRangeType::sak:
        return alloc_C_str("SAK", buf, n, allow_exceptions);
    case tdma::OptionRangeType::sbk:
        return alloc_C_str("SBK", buf, n, allow_exceptions);
    case tdma::OptionRangeType::snk:
        return alloc_C_str("SNK", buf, n, allow_exceptions);
    case tdma::OptionRangeType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionRangeType");
    }
}


int
OptionExpMonth_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionExpMonth>(v)){
    case tdma::OptionExpMonth::jan:
        return alloc_C_str("JAN", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::feb:
        return alloc_C_str("FEB", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::mar:
        return alloc_C_str("MAR", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::apr:
        return alloc_C_str("APR", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::may:
        return alloc_C_str("MAY", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::jun:
        return alloc_C_str("JUN", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::jul:
        return alloc_C_str("JUL", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::aug:
        return alloc_C_str("AUG", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::sep:
        return alloc_C_str("SEP", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::oct:
        return alloc_C_str("OCT", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::nov:
        return alloc_C_str("NOV", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::dec:
        return alloc_C_str("DEC", buf, n, allow_exceptions);
    case tdma::OptionExpMonth::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionExpMonth");
    }
}


int
OptionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionType>(v)){
    case tdma::OptionType::s:
        return alloc_C_str("S", buf, n, allow_exceptions);
    case tdma::OptionType::ns:
        return alloc_C_str("NS", buf, n, allow_exceptions);
    case tdma::OptionType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default: throw std::runtime_error("invalid OptionType");
    }
}


int
TransactionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::TransactionType>(v)){
    case tdma::TransactionType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    case tdma::TransactionType::trade:
        return alloc_C_str("TRADE", buf, n, allow_exceptions);
    case tdma::TransactionType::buy_only:
        return alloc_C_str("BUY_ONLY", buf, n, allow_exceptions);
    case tdma::TransactionType::sell_only:
        return alloc_C_str("SELL_ONLY", buf, n, allow_exceptions);
    case tdma::TransactionType::cash_in_or_cash_out:
        return alloc_C_str("CASH_IN_OR_CASH_OUT", buf, n, allow_exceptions);
    case tdma::TransactionType::checking:
        return alloc_C_str("CHECKING", buf, n, allow_exceptions);
    case tdma::TransactionType::dividend:
        return alloc_C_str("DIVIDEND", buf, n, allow_exceptions);
    case tdma::TransactionType::interest:
        return alloc_C_str("INTEREST", buf, n, allow_exceptions);
    case tdma::TransactionType::other:
        return alloc_C_str("OTHER", buf, n, allow_exceptions);
    case tdma::TransactionType::advisor_fees:
        return alloc_C_str("ADVISOR_FEES", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invliad TransactionType");
    }
}

int
InstrumentSearchType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::InstrumentSearchType>(v)){
    case tdma::InstrumentSearchType::symbol_exact:
        return alloc_C_str("fundamental", buf, n, allow_exceptions);
    case tdma::InstrumentSearchType::symbol_search:
        return alloc_C_str("symbol-search", buf, n, allow_exceptions);
    case tdma::InstrumentSearchType::symbol_regex:
        return alloc_C_str("symbol-regex", buf, n, allow_exceptions);
    case tdma::InstrumentSearchType::description_search:
        return alloc_C_str("desc-search", buf, n, allow_exceptions);
    case tdma::InstrumentSearchType::description_regex:
        return alloc_C_str("desc-regex", buf, n, allow_exceptions);
    case tdma::InstrumentSearchType::cusip:
        return alloc_C_str("cusip", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid InstrumentSearchType");
    }
}


int
MarketType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::MarketType>(v)){
    case tdma::MarketType::equity:
        return alloc_C_str("EQUITY", buf, n, allow_exceptions);
    case tdma::MarketType::option:
        return alloc_C_str("OPTION", buf, n, allow_exceptions);
    case tdma::MarketType::future:
        return alloc_C_str("FUTURE", buf, n, allow_exceptions);
    case tdma::MarketType::bond:
        return alloc_C_str("BOND", buf, n, allow_exceptions);
    case tdma::MarketType::forex:
        return alloc_C_str("FOREX", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid MarketType");
    }
}

int
MoversIndex_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::MoversIndex>(v)){
    case tdma::MoversIndex::compx:
        return alloc_C_str("$COMPX", buf, n, allow_exceptions);
    case tdma::MoversIndex::dji:
        return alloc_C_str("$DJI", buf, n, allow_exceptions);
    case tdma::MoversIndex::spx:
        return alloc_C_str("$SPX.X", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversIndex");
    }
}


int
MoversDirectionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::MoversDirectionType>(v)){
    case tdma::MoversDirectionType::up:
        return alloc_C_str("up", buf, n, allow_exceptions);
    case tdma::MoversDirectionType::down:
        return alloc_C_str("down", buf, n, allow_exceptions);
    case tdma::MoversDirectionType::up_and_down:
        return alloc_C_str("up_and_down", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversDirectionType");
    }
}

int
MoversChangeType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::MoversChangeType>(v)){
    case tdma::MoversChangeType::percent:
        return alloc_C_str("percent", buf, n, allow_exceptions);
    case tdma::MoversChangeType::value:
        return alloc_C_str("value", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversChangeType");
    }
}

int
OptionStrikesType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    switch(static_cast<tdma::OptionStrikesType>(v)){
    case tdma::OptionStrikesType::n_atm:
        return alloc_C_str("n_atm", buf, n, allow_exceptions);
    case tdma::OptionStrikesType::single:
        return alloc_C_str("single", buf, n, allow_exceptions);
    case tdma::OptionStrikesType::range:
        return alloc_C_str("range", buf, n, allow_exceptions);
    case tdma::OptionStrikesType::none:
        return alloc_C_str("none", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversChangeType");
    }
}


namespace tdma{

string
to_string(const OptionStrikes& strikes)
{
    switch( strikes.get_type() ){
    case tdma::OptionStrikes::Type::n_atm:
        return "n_atm(" + to_string(strikes.get_n_atm()) + ")";
    case tdma::OptionStrikes::Type::single:
        return "single(" + to_string(strikes.get_single()) + ")";
    case tdma::OptionStrikes::Type::range:
        return "range(" + to_string(strikes.get_range()) + ")";
    case tdma::OptionStrikes::Type::none:
        return "none()";
    default:
        throw runtime_error("invalid OptionStrikes::Type");
    }
}

std::ostream&
operator<<(std::ostream& out, const OptionStrikes& strikes)
{
    out << to_string(strikes);
    return out;
}

string
to_string(const OptionStrategy& strategy)
{
    unsigned int i = strategy.get_spread_interval();
    string s = to_string(strategy.get_strategy());
    return i ? (s + "(" + to_string(i) + ")") : s;
}

std::ostream&
operator<<(std::ostream& out, const OptionStrategy& strategy)
{
    out << to_string(strategy);
    return out;
}


string
to_string(const AdminCommandType& command)
{
    switch(command){
    case AdminCommandType::LOGIN: return "LOGIN";
    case AdminCommandType::LOGOUT: return "LOGOUT";
    case AdminCommandType::QOS: return "QOS";
    default: throw runtime_error("Invalid AdminCommandType");
    }
}

std::ostream&
operator<<(std::ostream& out, const AdminCommandType& command)
{
    out << to_string(command);
    return out;
}

std::string
to_string(const QOSType& qos)
{
    switch(qos){
    case QOSType::delayed: return "delayed";
    case QOSType::express: return "express";
    case QOSType::fast: return "fast";
    case QOSType::moderate: return "moderate";
    case QOSType::real_time: return "real-time";
    case QOSType::slow: return "slow";
    default: throw runtime_error("Invalid QOSType");
    }
}

std::ostream&
operator<<(std::ostream& out, const QOSType& qos)
{
    out << to_string(qos);
    return out;
}

std::string
to_string(const ActivesSubscriptionBase::DurationType& duration)
{
    switch(duration){
    case ActivesSubscriptionBase::DurationType::all_day: return "ALL";
    case ActivesSubscriptionBase::DurationType::min_60: return "3600";
    case ActivesSubscriptionBase::DurationType::min_30: return "1800";
    case ActivesSubscriptionBase::DurationType::min_10: return "600";
    case ActivesSubscriptionBase::DurationType::min_5: return "300";
    case ActivesSubscriptionBase::DurationType::min_1: return "60";
    default: throw runtime_error("Invalid ActivesSubscriptionBase::DurationType");
    }
}

std::ostream&
operator<<( std::ostream& out,
            const ActivesSubscriptionBase::DurationType& duration )
{
    out << to_string(duration);
    return out;
}


std::string
to_string(const OptionActivesSubscription::VenueType& venue)
{
    switch(venue){
    case OptionActivesSubscription::VenueType::opts: return "OPTS";
    case OptionActivesSubscription::VenueType::calls: return "CALLS";
    case OptionActivesSubscription::VenueType::puts: return "PUTS";
    case OptionActivesSubscription::VenueType::opts_desc: return "OPTS-DESC";
    case OptionActivesSubscription::VenueType::calls_desc: return "CALLS-DESC";
    case OptionActivesSubscription::VenueType::puts_desc: return "PUTS-DESC";
    default: throw runtime_error("Invalid OptionActivesSubscription:VenueType");
    }
}

std::ostream&
operator<<(std::ostream& out, const OptionActivesSubscription::VenueType& venue)
{
    out<< to_string(venue);
    return out;
}



string
to_string(const StreamerService& service)
{
    switch( static_cast<StreamerService::type>(service) ){
    case StreamerService::type::NONE: return "NONE";
    case StreamerService::type::ADMIN: return "ADMIN";
    case StreamerService::type::ACTIVES_NASDAQ: return "ACTIVES_NASDAQ";
    case StreamerService::type::ACTIVES_NYSE: return "ACTIVES_NYSE";
    case StreamerService::type::ACTIVES_OTCBB: return "ACTIVES_OTCBB";
    case StreamerService::type::ACTIVES_OPTIONS: return "ACTIVES_OPTIONS";
    case StreamerService::type::CHART_EQUITY: return "CHART_EQUITY";
    //case StreamerService::type::CHART_FOREX: return "CHART_FOREX";
    case StreamerService::type::CHART_FUTURES: return "CHART_FUTURES";
    case StreamerService::type::CHART_OPTIONS: return "CHART_OPTIONS";
    case StreamerService::type::QUOTE: return "QUOTE";
    case StreamerService::type::LEVELONE_FUTURES: return "LEVELONE_FUTURES";
    case StreamerService::type::LEVELONE_FOREX: return "LEVELONE_FOREX";
    case StreamerService::type::LEVELONE_FUTURES_OPTIONS:
        return "LEVELONE_FUTURES_OPTIONS";
    case StreamerService::type::OPTION: return "OPTION";
    case StreamerService::type::NEWS_HEADLINE: return "NEWS_HEADLINE";
    case StreamerService::type::TIMESALE_EQUITY: return "TIMESALE_EQUITY";
    case StreamerService::type::TIMESALE_FUTURES: return "TIMESALE_FUTURES";
    //case StreamerService::type::TIMESALE_FOREX: return "TIMESALE_FOREX";
    case StreamerService::type::TIMESALE_OPTIONS: return "TIMESALE_OPTIONS";
    default: throw runtime_error("Invalid StreamerService::type");
    }
}

std::ostream&
operator<<(std::ostream& out, const StreamerService& service)
{
    out << to_string(service);
    return out;
}


string
to_string(const StreamingCallbackType& callback_type)
{
    switch(callback_type){
    case StreamingCallbackType::listening_start: return "listening_start";
    case StreamingCallbackType::listening_stop: return "listening_stop";
    case StreamingCallbackType::data: return "data";
    case StreamingCallbackType::notify: return "notify";
    case StreamingCallbackType::timeout: return "timeout";
    case StreamingCallbackType::error: return "error";
    default: throw runtime_error("Invalid StreamingCallbackType");
    }
}


ostream&
operator<<(std::ostream& out, const StreamingCallbackType& callback_type)
{
    out << to_string(callback_type);
    return out;
}



} /* tdma */
