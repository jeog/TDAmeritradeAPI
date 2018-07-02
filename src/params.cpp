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

namespace tdma{

using namespace std;

const unordered_map<PeriodType, set<unsigned int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE = {
    {PeriodType::day, set<unsigned int>{1,2,3,4,5,10}},
    {PeriodType::month, set<unsigned int>{1,2,3,6}},
    {PeriodType::year, set<unsigned int>{1,2,3,5,10,15,20}},
    {PeriodType::ytd, set<unsigned int>{1}},
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


const unordered_map<FrequencyType, set<unsigned int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<unsigned int>{1,5,10,30}},
    {FrequencyType::daily, set<unsigned int>{1}},
    {FrequencyType::weekly, set<unsigned int>{1}},
    {FrequencyType::monthly, set<unsigned int>{1}},
};



string
to_string(const PeriodType& ptype)
{
    switch(ptype){
    case PeriodType::day: return "day";
    case PeriodType::month: return "month";
    case PeriodType::year: return "year";
    case PeriodType::ytd: return "ytd";
    default: throw runtime_error("invalid PeriodType");
    }
}

ostream&
operator<<(ostream& out, const PeriodType& period_type)
{
    out<< to_string(period_type);
    return out;
}


string
to_string(const FrequencyType& ftype)
{
    switch(ftype){
    case FrequencyType::minute: return "minute";
    case FrequencyType::daily: return "daily";
    case FrequencyType::weekly: return "weekly";
    case FrequencyType::monthly: return "monthly";
    default: throw runtime_error("invalid FrequencyType");
    }
}

ostream&
operator<<(ostream& out, const FrequencyType& frequency_type)
{
    out<< to_string(frequency_type);
    return out;
}


string
to_string(const OptionContractType& contract_type)
{
    switch(contract_type){
    case OptionContractType::call: return "CALL";
    case OptionContractType::put: return "PUT";
    case OptionContractType::all: return "ALL";
    default: throw runtime_error("invalid OptionContractType");
    }
}

ostream&
operator<<(ostream& out, const OptionContractType& contract_type)
{
    out<< to_string(contract_type);
    return out;
}


string
to_string(const OptionStrategyType& strategy)
{
    switch(strategy){
    //case OptionStrategyType::single: return "SINGLE";
    //case OptionStrategyType::analytical: return "ANALYTICAL";
    case OptionStrategyType::covered: return "COVERED";
    case OptionStrategyType::vertical: return "VERTICAL";
    case OptionStrategyType::calendar: return "CALENDAR";
    case OptionStrategyType::strangle: return "STRANGLE";
    case OptionStrategyType::straddle: return "STRADDLE";
    case OptionStrategyType::butterfly: return "BUTTERFLY";
    case OptionStrategyType::condor: return "CONDOR";
    case OptionStrategyType::diagonal: return "DIAGONAL";
    case OptionStrategyType::collar: return "COLLAR";
    case OptionStrategyType::roll: return "ROLL";
    default: throw runtime_error("invalid OptionStrategyType");
    }
}

ostream&
operator<<(ostream& out, const OptionStrategyType& strategy)
{
    out<< to_string(strategy);
    return out;
}


string
to_string(const OptionRangeType& range)
{
    switch(range){
    case OptionRangeType::null: return "";
    case OptionRangeType::itm: return "ITM";
    case OptionRangeType::ntm: return "NTM";
    case OptionRangeType::otm: return "OTM";
    case OptionRangeType::sak: return "SAK";
    case OptionRangeType::sbk: return "SBK";
    case OptionRangeType::snk: return "SNK";
    case OptionRangeType::all: return "ALL";
    default: throw runtime_error("invalid OptionRangeType");
    }
}

ostream&
operator<<(ostream& out, const OptionRangeType& range)
{
    out<< to_string(range);
    return out;
}


string
to_string(const OptionExpMonth& exp_month)
{
    switch(exp_month){
    case OptionExpMonth::jan: return "JAN";
    case OptionExpMonth::feb: return "FEB";
    case OptionExpMonth::mar: return "MAR";
    case OptionExpMonth::apr: return "APR";
    case OptionExpMonth::may: return "MAY";
    case OptionExpMonth::jun: return "JUN";
    case OptionExpMonth::jul: return "JUL";
    case OptionExpMonth::aug: return "AUG";
    case OptionExpMonth::sep: return "SEP";
    case OptionExpMonth::oct: return "OCT";
    case OptionExpMonth::nov: return "NOV";
    case OptionExpMonth::dec: return "DEC";
    case OptionExpMonth::all: return "ALL";
    default: throw runtime_error("invalid OptionExpMonth");
    }
}

ostream&
operator<<(ostream& out, const OptionExpMonth& exp_month)
{
    out<< to_string(exp_month);
    return out;
}


string
to_string(const OptionType& option_type)
{
    switch(option_type){
    case OptionType::s: return "S";
    case OptionType::ns: return "NS";
    case OptionType::all: return "ALL";
    default: throw runtime_error("invalid OptionType");
    }
}

ostream&
operator<<(ostream& out, const OptionType& option_type)
{
    out<< to_string(option_type);
    return out;
}


string
to_string(const TransactionType& transaction_type)
{
    switch(transaction_type){
    case TransactionType::all: return "ALL";
    case TransactionType::trade: return "TRADE";
    case TransactionType::buy_only: return "BUY_ONLY";
    case TransactionType::sell_only: return "SELL_ONLY";
    case TransactionType::cash_in_or_cash_out: return "CASH_IN_OR_CASH_OUT";
    case TransactionType::checking: return "CHECKING";
    case TransactionType::dividend: return "DIVIDEND";
    case TransactionType::interest: return "INTEREST";
    case TransactionType::other: return "OTHER";
    case TransactionType::advisor_fees: return "ADVISOR_FEES";
    default: throw runtime_error("invliad TransactionType");
    }
}

ostream&
operator<<(ostream& out, const TransactionType& transaction_type)
{
    out<< to_string(transaction_type);
    return out;
}


string
to_string(const InstrumentSearchType& search_type)
{
    switch(search_type){
    case InstrumentSearchType::symbol_exact: return "fundamental";
    case InstrumentSearchType::symbol_search: return "symbol-search";
    case InstrumentSearchType::symbol_regex: return "symbol-regex";
    case InstrumentSearchType::description_search: return "desc-search";
    case InstrumentSearchType::description_regex: return "desc-regex";
    case InstrumentSearchType::cusip: return "cusip";
    default: throw runtime_error("invalid InstrumentSearchType");
    }
}

ostream&
operator<<(ostream& out, const InstrumentSearchType& search_type)
{
    out<< to_string(search_type);
    return out;
}


string
to_string(const MarketType& market_type)
{
    switch(market_type){
    case MarketType::equity: return "EQUITY";
    case MarketType::option: return "OPTION";
    case MarketType::future: return "FUTURE";
    case MarketType::bond: return "BOND";
    case MarketType::forex: return "FOREX";
    default: throw runtime_error("Invalid MarketType");
    }
}

ostream&
operator<<(ostream& out, const MarketType& market_type)
{
    out << to_string(market_type);
    return out;
}

string
to_string(const OptionStrikes& strikes)
{
    switch( strikes.get_type() ){
    case OptionStrikes::Type::n_atm:
        return "n_atm(" + to_string(strikes.get_n_atm()) + ")";
    case OptionStrikes::Type::single:
        return "single(" + to_string(strikes.get_single()) + ")";
    case OptionStrikes::Type::range:
        return "range(" + to_string(strikes.get_range()) + ")";
    default:
        throw runtime_error("invalid OptionStrikes::Type");
    }
}

ostream&
operator<<(ostream& out, const OptionStrikes& strikes)
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

ostream&
operator<<(ostream& out, const OptionStrategy& strategy)
{
    out<< to_string(strategy);
    return out;
}


std::string
to_string(const MoversIndex& index)
{
    switch( index ){
    case MoversIndex::compx: return "$COMPX";
    case MoversIndex::dji: return "$DJI";
    case MoversIndex::spx: return "$SPX.X";
    default: throw runtime_error("invalid MoversIndex");
    }
}

std::ostream&
operator<<(std::ostream& out,const MoversIndex& index)
{
    out<< to_string(index);
    return out;
}


std::string
to_string(const MoversDirectionType& direction_type)
{
    switch(direction_type){
    case MoversDirectionType::up: return "up";
    case MoversDirectionType::down: return "down";
    case MoversDirectionType::up_and_down: return "up_and_down";
    default: throw runtime_error("invalid MoversDirectionType");
    }
}

std::ostream&
operator<<(std::ostream& out,const MoversDirectionType& direction_type)
{
    out<< to_string(direction_type);
    return out;
}


std::string
to_string(const MoversChangeType& change_type)
{
    switch(change_type){
    case MoversChangeType::percent: return "percent";
    case MoversChangeType::value: return "value";
    default: throw runtime_error("invalid MoversChangeType");
    }
}

std::ostream&
operator<<(std::ostream& out,const MoversChangeType& change_type)
{
    out<< to_string(change_type);
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
