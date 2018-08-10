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

using namespace tdma;

int
FreeBuffer_ABI( char* buf, int allow_exceptions )
{
    if( buf )
        free( (void*)buf );
    return 0;
}

int
FreeBuffers_ABI( char** bufs, size_t n, int allow_exceptions )
{
    if( bufs ){
        while(n--){
          char *c = bufs[n];
          assert(c);
          free(c);
        }
        free(bufs);
    }
    return 0;
}

int
FreeFieldsBuffer_ABI( int* fields, int allow_exceptions )
{
    if( fields )
        free( (void*)fields );
    return 0;
}

int
alloc_C_str(const std::string& s, char** buf, size_t* n, bool raise_exception)
{
    assert(buf);
    assert(n);
    *n = s.size() + 1;
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !*(buf) ){
        if( raise_exception ){
            throw MemoryError("not enough memory to allocate enum string");
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
    int err = check_abi_enum(PeriodType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<PeriodType>(v)){
    case PeriodType::day:
        return alloc_C_str("day", buf, n, allow_exceptions);
    case PeriodType::month:
        return alloc_C_str("month", buf, n, allow_exceptions);
    case PeriodType::year:
        return alloc_C_str("year", buf, n, allow_exceptions);
    case PeriodType::ytd:
        return alloc_C_str("ytd", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid PeriodType");
    }
}

int
FrequencyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(FrequencyType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<FrequencyType>(v)){
    case FrequencyType::minute:
        return alloc_C_str("minute", buf, n, allow_exceptions);
    case FrequencyType::daily:
        return alloc_C_str("daily", buf, n, allow_exceptions);
    case FrequencyType::weekly:
        return alloc_C_str("weekly", buf, n, allow_exceptions);
    case FrequencyType::monthly:
        return alloc_C_str("monthly", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid FrequencyType");
    }
}

int
OptionContractType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionContractType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionContractType>(v)){
    case OptionContractType::call:
        return alloc_C_str("CALL", buf, n, allow_exceptions);
    case OptionContractType::put:
        return alloc_C_str("PUT", buf, n, allow_exceptions);
    case OptionContractType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionContractType");
    }
}

int
OptionStrategyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionStrategyType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionStrategyType>(v)){
    //case OptionStrategyType::single: return "SINGLE";
    //case OptionStrategyType::analytical: return "ANALYTICAL";
    case OptionStrategyType::covered:
        return alloc_C_str("COVERED", buf, n, allow_exceptions);
    case OptionStrategyType::vertical:
        return alloc_C_str("VERTICAL", buf, n, allow_exceptions);
    case OptionStrategyType::calendar:
        return alloc_C_str("CALENDAR", buf, n, allow_exceptions);
    case OptionStrategyType::strangle:
        return alloc_C_str("STRANGLE", buf, n, allow_exceptions);
    case OptionStrategyType::straddle:
        return alloc_C_str("STRADDLE", buf, n, allow_exceptions);
    case OptionStrategyType::butterfly:
        return alloc_C_str("BUTTERFLY", buf, n, allow_exceptions);
    case OptionStrategyType::condor:
        return alloc_C_str("CONDOR", buf, n, allow_exceptions);
    case OptionStrategyType::diagonal:
        return alloc_C_str("DIAGONAL", buf, n, allow_exceptions);
    case OptionStrategyType::collar:
        return alloc_C_str("COLLAR", buf, n, allow_exceptions);
    case  OptionStrategyType::roll:
        return alloc_C_str("ROLL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionStrategyType");
    }
}


int
OptionRangeType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionRangeType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionRangeType>(v)){
    case OptionRangeType::null:
        return alloc_C_str("", buf, n, allow_exceptions);
    case OptionRangeType::itm:
        return alloc_C_str("ITM", buf, n, allow_exceptions);
    case OptionRangeType::ntm:
        return alloc_C_str("NTM", buf, n, allow_exceptions);
    case OptionRangeType::otm:
        return alloc_C_str("OTM", buf, n, allow_exceptions);
    case OptionRangeType::sak:
        return alloc_C_str("SAK", buf, n, allow_exceptions);
    case OptionRangeType::sbk:
        return alloc_C_str("SBK", buf, n, allow_exceptions);
    case OptionRangeType::snk:
        return alloc_C_str("SNK", buf, n, allow_exceptions);
    case OptionRangeType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionRangeType");
    }
}


int
OptionExpMonth_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionExpMonth_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionExpMonth>(v)){
    case OptionExpMonth::jan:
        return alloc_C_str("JAN", buf, n, allow_exceptions);
    case OptionExpMonth::feb:
        return alloc_C_str("FEB", buf, n, allow_exceptions);
    case OptionExpMonth::mar:
        return alloc_C_str("MAR", buf, n, allow_exceptions);
    case OptionExpMonth::apr:
        return alloc_C_str("APR", buf, n, allow_exceptions);
    case OptionExpMonth::may:
        return alloc_C_str("MAY", buf, n, allow_exceptions);
    case OptionExpMonth::jun:
        return alloc_C_str("JUN", buf, n, allow_exceptions);
    case OptionExpMonth::jul:
        return alloc_C_str("JUL", buf, n, allow_exceptions);
    case OptionExpMonth::aug:
        return alloc_C_str("AUG", buf, n, allow_exceptions);
    case OptionExpMonth::sep:
        return alloc_C_str("SEP", buf, n, allow_exceptions);
    case OptionExpMonth::oct:
        return alloc_C_str("OCT", buf, n, allow_exceptions);
    case OptionExpMonth::nov:
        return alloc_C_str("NOV", buf, n, allow_exceptions);
    case OptionExpMonth::dec:
        return alloc_C_str("DEC", buf, n, allow_exceptions);
    case OptionExpMonth::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionExpMonth");
    }
}


int
OptionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionType>(v)){
    case OptionType::s:
        return alloc_C_str("S", buf, n, allow_exceptions);
    case OptionType::ns:
        return alloc_C_str("NS", buf, n, allow_exceptions);
    case OptionType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    default: throw std::runtime_error("invalid OptionType");
    }
}


int
TransactionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(TransactionType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<TransactionType>(v)){
    case TransactionType::all:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    case TransactionType::trade:
        return alloc_C_str("TRADE", buf, n, allow_exceptions);
    case TransactionType::buy_only:
        return alloc_C_str("BUY_ONLY", buf, n, allow_exceptions);
    case TransactionType::sell_only:
        return alloc_C_str("SELL_ONLY", buf, n, allow_exceptions);
    case TransactionType::cash_in_or_cash_out:
        return alloc_C_str("CASH_IN_OR_CASH_OUT", buf, n, allow_exceptions);
    case TransactionType::checking:
        return alloc_C_str("CHECKING", buf, n, allow_exceptions);
    case TransactionType::dividend:
        return alloc_C_str("DIVIDEND", buf, n, allow_exceptions);
    case TransactionType::interest:
        return alloc_C_str("INTEREST", buf, n, allow_exceptions);
    case TransactionType::other:
        return alloc_C_str("OTHER", buf, n, allow_exceptions);
    case TransactionType::advisor_fees:
        return alloc_C_str("ADVISOR_FEES", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invliad TransactionType");
    }
}

int
InstrumentSearchType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(InstrumentSearchType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<InstrumentSearchType>(v)){
    case InstrumentSearchType::symbol_exact:
        return alloc_C_str("fundamental", buf, n, allow_exceptions);
    case InstrumentSearchType::symbol_search:
        return alloc_C_str("symbol-search", buf, n, allow_exceptions);
    case InstrumentSearchType::symbol_regex:
        return alloc_C_str("symbol-regex", buf, n, allow_exceptions);
    case InstrumentSearchType::description_search:
        return alloc_C_str("desc-search", buf, n, allow_exceptions);
    case InstrumentSearchType::description_regex:
        return alloc_C_str("desc-regex", buf, n, allow_exceptions);
    case InstrumentSearchType::cusip:
        return alloc_C_str("cusip", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid InstrumentSearchType");
    }
}


int
MarketType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(MarketType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<MarketType>(v)){
    case MarketType::equity:
        return alloc_C_str("EQUITY", buf, n, allow_exceptions);
    case MarketType::option:
        return alloc_C_str("OPTION", buf, n, allow_exceptions);
    case MarketType::future:
        return alloc_C_str("FUTURE", buf, n, allow_exceptions);
    case MarketType::bond:
        return alloc_C_str("BOND", buf, n, allow_exceptions);
    case MarketType::forex:
        return alloc_C_str("FOREX", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid MarketType");
    }
}

int
MoversIndex_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(MoversIndex_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<MoversIndex>(v)){
    case MoversIndex::compx:
        return alloc_C_str("$COMPX", buf, n, allow_exceptions);
    case MoversIndex::dji:
        return alloc_C_str("$DJI", buf, n, allow_exceptions);
    case MoversIndex::spx:
        return alloc_C_str("$SPX.X", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversIndex");
    }
}


int
MoversDirectionType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(MoversDirectionType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<MoversDirectionType>(v)){
    case MoversDirectionType::up:
        return alloc_C_str("up", buf, n, allow_exceptions);
    case MoversDirectionType::down:
        return alloc_C_str("down", buf, n, allow_exceptions);
    case MoversDirectionType::up_and_down:
        return alloc_C_str("up_and_down", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversDirectionType");
    }
}

int
MoversChangeType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(MoversChangeType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<MoversChangeType>(v)){
    case MoversChangeType::percent:
        return alloc_C_str("percent", buf, n, allow_exceptions);
    case MoversChangeType::value:
        return alloc_C_str("value", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversChangeType");
    }
}

int
OptionStrikesType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(OptionStrikesType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<OptionStrikesType>(v)){
    case OptionStrikesType::n_atm:
        return alloc_C_str("n_atm", buf, n, allow_exceptions);
    case OptionStrikesType::single:
        return alloc_C_str("single", buf, n, allow_exceptions);
    case OptionStrikesType::range:
        return alloc_C_str("range", buf, n, allow_exceptions);
    case OptionStrikesType::none:
        return alloc_C_str("none", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversChangeType");
    }
}

int
AdminCommandType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(AdminCommandType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<AdminCommandType>(v)){
    case AdminCommandType::LOGIN:
        return alloc_C_str("LOGIN", buf, n, allow_exceptions);
    case AdminCommandType::LOGOUT:
        return alloc_C_str("LOGOUT", buf, n, allow_exceptions);
    case AdminCommandType::QOS:
        return alloc_C_str("QOS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid AdminCommandType");
    }
}

int
QOSType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(QOSType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<QOSType>(v)){
    case QOSType::delayed:
        return alloc_C_str("delayed", buf, n, allow_exceptions);
    case QOSType::express:
        return alloc_C_str("express", buf, n, allow_exceptions);
    case QOSType::fast:
        return alloc_C_str("fast", buf, n, allow_exceptions);
    case QOSType::moderate:
        return alloc_C_str("moderate", buf, n, allow_exceptions);
    case QOSType::real_time:
        return alloc_C_str("real-time", buf, n, allow_exceptions);
    case QOSType::slow:
        return alloc_C_str("slow", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid QOSType");
    }
}



int
DurationType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(DurationType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<DurationType>(v)){
    case DurationType::all_day:
        return alloc_C_str("ALL", buf, n, allow_exceptions);
    case DurationType::min_60:
        return alloc_C_str("3600", buf, n, allow_exceptions);
    case DurationType::min_30:
        return alloc_C_str("1800", buf, n, allow_exceptions);
    case DurationType::min_10:
        return alloc_C_str("600", buf, n, allow_exceptions);
    case DurationType::min_5:
        return alloc_C_str("300", buf, n, allow_exceptions);
    case DurationType::min_1:
        return alloc_C_str("60", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid DurationType");
    }
}


int
VenueType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(VenueType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<VenueType>(v)){
    case VenueType::opts:
        return alloc_C_str("OPTS", buf, n, allow_exceptions);
    case VenueType::calls:
        return alloc_C_str("CALLS", buf, n, allow_exceptions);
    case VenueType::puts:
        return alloc_C_str("PUTS", buf, n, allow_exceptions);
    case VenueType::opts_desc:
        return alloc_C_str("OPTS-DESC", buf, n, allow_exceptions);
    case VenueType::calls_desc:
        return alloc_C_str("CALLS-DESC", buf, n, allow_exceptions);
    case VenueType::puts_desc:
        return alloc_C_str("PUTS-DESC", buf, n, allow_exceptions);
    default: throw std::runtime_error("Invalid VenueType");
    }
}


int
StreamingCallbackType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    int err = check_abi_enum(StreamingCallbackType_is_valid, v, allow_exceptions);
    if( err )
        return err;

    switch(static_cast<StreamingCallbackType>(v)){
    case StreamingCallbackType::listening_start:
        return alloc_C_str("listening_start", buf, n, allow_exceptions);
    case StreamingCallbackType::listening_stop:
        return alloc_C_str("listening_stop", buf, n, allow_exceptions);
    case StreamingCallbackType::data:
        return alloc_C_str("data", buf, n, allow_exceptions);
    case StreamingCallbackType::request_response:
        return alloc_C_str("request_response", buf, n, allow_exceptions);
    case StreamingCallbackType::notify:
        return alloc_C_str("notify", buf, n, allow_exceptions);
    case StreamingCallbackType::timeout:
        return alloc_C_str("timeout", buf, n, allow_exceptions);
    case StreamingCallbackType::error:
        return alloc_C_str("error", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid StreamingCallbackType");
    }
}

int
StreamerServiceType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    if( v ){ // ::NONE (0) can't be allowed to fail in this particular case
        int err = check_abi_enum(StreamerServiceType_is_valid, v,
                                 allow_exceptions);
        if( err )
            return err;
    }

    switch( static_cast<StreamerServiceType>(v) ){
    case StreamerServiceType::NONE:
        return alloc_C_str("NONE", buf, n, allow_exceptions);
    case StreamerServiceType::ADMIN:
        return alloc_C_str("ADMIN", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_NASDAQ:
        return alloc_C_str("ACTIVES_NASDAQ", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_NYSE:
        return alloc_C_str("ACTIVES_NYSE", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_OTCBB:
        return alloc_C_str("ACTIVES_OTCBB", buf, n, allow_exceptions);
    case StreamerServiceType::ACTIVES_OPTIONS:
        return alloc_C_str("ACTIVES_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_EQUITY:
        return alloc_C_str("CHART_EQUITY", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_FOREX:
        return alloc_C_str("CHART_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_FUTURES:
        return alloc_C_str("CHART_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::CHART_OPTIONS:
        return alloc_C_str("CHART_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::QUOTE:
        return alloc_C_str("QUOTE", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FUTURES:
        return alloc_C_str("LEVELONE_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FOREX:
        return alloc_C_str("LEVELONE_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::LEVELONE_FUTURES_OPTIONS:
        return alloc_C_str("LEVELONE_FUTURES_OPTIONS", buf, n, allow_exceptions);
    case StreamerServiceType::OPTION:
        return alloc_C_str("OPTION", buf, n, allow_exceptions);
    case StreamerServiceType::NEWS_HEADLINE:
        return alloc_C_str("NEWS_HEADLINE", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_EQUITY:
        return alloc_C_str("TIMESALE_EQUITY", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_FUTURES:
        return alloc_C_str("TIMESALE_FUTURES", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_FOREX:
        return alloc_C_str("TIMESALE_FOREX", buf, n, allow_exceptions);
    case StreamerServiceType::TIMESALE_OPTIONS:
        return alloc_C_str("TIMESALE_OPTIONS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid StreamerServiceType");
    }
}


/* TODO return actual strings for fields */
#define DEF_TEMP_FIELD_TO_STRING(name) \
int \
name##_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions) \
{ \
    int err = check_abi_enum(name##_is_valid, v, allow_exceptions); \
    if( err ) \
        return err; \
    return alloc_C_str(#name"-" + std::to_string(v), buf, n, allow_exceptions); \
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


/*
 * NOTE FieldType enums are not defined!
 */






