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
#include <regex>

#include "../include/_tdma_api.h"


#ifdef __cplusplus

namespace tdma{

using namespace std;

std::string
BuildOptionSymbolImpl( const std::string& underlying,
                         unsigned int month,
                         unsigned int day,
                         unsigned int year,
                         bool is_call,
                         double strike )
{
    std::stringstream ss;

    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month (1-12)");

    if( day < 1 || day > 31 )
        TDMA_API_THROW(ValueException,"invalid day (1-31)");

    if( underlying.empty() )
        TDMA_API_THROW(ValueException,"underlying is empty");

    if( underlying.find('_') != std::string::npos )
        TDMA_API_THROW(ValueException,"invalid character in underlying: '_'" );

    std::string yy = std::to_string(year);
    if( yy.size() != 4 )
        TDMA_API_THROW(ValueException,"invalid number of digits in year");

    if( strike <= 0.0 )
        TDMA_API_THROW(ValueException,"strike price <= 0.0");

    ss << util::toupper(underlying) << '_'
       << std::setw(2) << std::setfill('0') << month
       << std::setw(2) << std::setfill('0') << day
       << std::setw(2) << std::setfill('0') << yy.substr( yy.size() - 2 )
       << (is_call ? 'C' : 'P');

    std::string s(std::to_string(strike));
    if( s.find('.') != std::string::npos ){
        s.erase( s.find_last_not_of('0') + 1, std::string::npos);
        s.erase( s.find_last_not_of('.') + 1, std::string::npos);
    }
    return ss.str() + s;
}

void
OptionSymbolCheckImpl(const std::string& option) // ABC_MMDDYY[C|P]{STRIKE}
{
    size_t sz = option.size();
    if( sz == 0 )
        TDMA_API_THROW(ValueException,"option string is empty");

    size_t uspos = option.find('_');
    if( uspos == std::string::npos )
        TDMA_API_THROW(ValueException,"option string is missing underscore");
    if( uspos == 0)
        TDMA_API_THROW(ValueException,"option string is missing underlying");

    size_t cppos = option.find_first_of("CP",uspos+1);
    if( cppos == std::string::npos )
        TDMA_API_THROW(ValueException,"option string is missing strike type(C/P)");

    if( (cppos - uspos - 1) != 6 )
        TDMA_API_THROW(ValueException,"option string contains invalid date substr size");

    static const std::regex DATE_RE("[0-9]{6}");
    if( !std::regex_match(option.substr(uspos+1, 6), DATE_RE) )
        TDMA_API_THROW(ValueException,"option string contains date substr with non-digits");

    int month = std::stoi(option.substr(uspos+1, 2));
    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month");

    int day = std::stoi(option.substr(uspos+3,2));
    if( day < 1 || day > 31 ) // TODO
        TDMA_API_THROW(ValueException,"invalid day");

    int year = std::stoi(option.substr(uspos+5,2));
    if( year < 0 || year > 99 )
        TDMA_API_THROW(ValueException,"invalid year");

    assert( cppos == uspos + 5 + 2);
    if( cppos + 1 == sz )
        TDMA_API_THROW(ValueException,"option string is missing strike");

    std::string strk(option.substr(uspos+8, std::string::npos));

    // TODO careful w/ non-standard strikes i.e split-adjusted
    static const std::regex STRK_RE(
        "^([1-9][0-9]*(\\.[0-9]*[1-9])?)|(\\.[0-9]*[1-9])$");
    if( !std::regex_match(strk, STRK_RE) )
        TDMA_API_THROW(ValueException,"option string contains invalid strike");
}

} /* tdma */


#endif /* __cplusplus */

using namespace tdma;

int
BuildOptionSymbol_ABI( const char* underlying,
                         unsigned int month,
                         unsigned int day,
                         unsigned int year,
                         int is_call,
                         double strike,
                         char **buf,
                         size_t *n,
                         int allow_exceptions )
{
    CHECK_PTR(underlying, "underlying", allow_exceptions);
    CHECK_PTR(buf, "buf", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    int err;
    std::string r;
    std::tie(r,err) = CallImplFromABI(allow_exceptions, BuildOptionSymbolImpl,
                                      underlying, month, day, year, is_call,
                                      strike);
    if( err )
        return err;

    *n = r.size() + 1; // NULL TERM
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !*buf ){
        return HANDLE_ERROR(tdma::MemoryError,
            "failed to allocate buffer memory", allow_exceptions
            );
    }
    (*buf)[(*n)-1] = 0;
    strncpy(*buf, r.c_str(), (*n)-1);
    return 0;
}

int
OptionSymbolCheck_ABI(const char* symbol, int allow_exceptions)
{
    CHECK_PTR(symbol, "symbol", allow_exceptions);
    return CallImplFromABI(allow_exceptions, OptionSymbolCheckImpl, symbol);
}

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
        HANDLE_ERROR( MemoryError, "not enough memory to allocate enum string",
                      raise_exception );
    }
    strncpy(*buf, s.c_str(), (*n)-1);
    (*buf)[(*n)-1] = 0;
    return 0;
}

int
PeriodType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(PeriodType, v, allow_exceptions);

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
    CHECK_ENUM(FrequencyType, v, allow_exceptions);

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
    CHECK_ENUM(OptionContractType, v, allow_exceptions);

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
    CHECK_ENUM(OptionStrategyType, v, allow_exceptions);

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
    CHECK_ENUM(OptionRangeType, v, allow_exceptions);

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
    CHECK_ENUM(OptionExpMonth, v, allow_exceptions);

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
    CHECK_ENUM(OptionType, v, allow_exceptions);

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
    CHECK_ENUM(TransactionType, v, allow_exceptions);

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
    CHECK_ENUM(InstrumentSearchType, v, allow_exceptions);

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
    CHECK_ENUM(MarketType, v, allow_exceptions);

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
    CHECK_ENUM(MoversIndex, v, allow_exceptions);

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
    CHECK_ENUM(MoversDirectionType, v, allow_exceptions);

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
    CHECK_ENUM(MoversChangeType, v, allow_exceptions);

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
    CHECK_ENUM(OptionStrikesType, v, allow_exceptions);

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
        throw std::runtime_error("invalid OptionStrikesType");
    }
}

int
AdminCommandType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(AdminCommandType, v, allow_exceptions);

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
    CHECK_ENUM(QOSType, v, allow_exceptions);

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
    CHECK_ENUM(DurationType, v, allow_exceptions);

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
    CHECK_ENUM(VenueType, v, allow_exceptions);

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
    CHECK_ENUM(StreamingCallbackType, v, allow_exceptions);

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
        CHECK_ENUM(StreamerServiceType, v, allow_exceptions);
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
    CHECK_ENUM(name, v, allow_exceptions); \
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

int
OrderSession_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderSession, v, allow_exceptions);

    switch(static_cast<OrderSession>(v)){
    case OrderSession::NORMAL:
        return alloc_C_str("NORMAL", buf, n, allow_exceptions);
    case OrderSession::AM:
        return alloc_C_str("AM", buf, n, allow_exceptions);
    case OrderSession::PM:
        return alloc_C_str("PM", buf, n, allow_exceptions);
    case OrderSession::SEAMLESS:
        return alloc_C_str("SEAMLESS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderSession");
    }
}

int
OrderDuration_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderDuration, v, allow_exceptions);

    switch(static_cast<OrderDuration>(v)){
    case OrderDuration::DAY:
        return alloc_C_str("DAY", buf, n, allow_exceptions);
    case OrderDuration::GOOD_TILL_CANCEL:
        return alloc_C_str("GOOD_TILL_CANCEL", buf, n, allow_exceptions);
    case OrderDuration::FILL_OR_KILL:
        return alloc_C_str("FILL_OR_KILL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderDuration");
    }
}


int
OrderAssetType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderAssetType, v, allow_exceptions);

    switch(static_cast<OrderAssetType>(v)){
    case OrderAssetType::EQUITY:
        return alloc_C_str("EQUITY", buf, n, allow_exceptions);
    case OrderAssetType::OPTION:
        return alloc_C_str("OPTION", buf, n, allow_exceptions);
    case OrderAssetType::INDEX:
        return alloc_C_str("INDEX", buf, n, allow_exceptions);
    case OrderAssetType::MUTUAL_FUND:
        return alloc_C_str("MUTUAL_FUND", buf, n, allow_exceptions);
    case OrderAssetType::CASH_EQUIVALENT:
        return alloc_C_str("CASH_EQUIVALENT", buf, n, allow_exceptions);
    case OrderAssetType::FIXED_INCOME:
        return alloc_C_str("FIXED_INCOME", buf, n, allow_exceptions);
    case OrderAssetType::CURRENCY:
        return alloc_C_str("CURRENCY", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderAssetType");
    }
}


int
OrderInstruction_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderInstruction, v, allow_exceptions);

    switch(static_cast<OrderInstruction>(v)){
    case OrderInstruction::BUY:
        return alloc_C_str("BUY", buf, n, allow_exceptions);
    case OrderInstruction::SELL:
        return alloc_C_str("SELL", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_COVER:
        return alloc_C_str("BUY_TO_COVER", buf, n, allow_exceptions);
    case OrderInstruction::SELL_SHORT:
        return alloc_C_str("SELL_SHORT", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_OPEN:
        return alloc_C_str("BUY_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_CLOSE:
        return alloc_C_str("BUY_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_OPEN:
        return alloc_C_str("SELL_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_CLOSE:
        return alloc_C_str("SELL_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::EXCHANGE:
        return alloc_C_str("EXCHANGE", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderInstruction");
    }
}


int
OrderType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderType, v, allow_exceptions);

    switch(static_cast<OrderType>(v)){
    case OrderType::MARKET:
        return alloc_C_str("MARKET", buf, n, allow_exceptions);
    case OrderType::LIMIT:
        return alloc_C_str("LIMIT", buf, n, allow_exceptions);
    case OrderType::STOP:
        return alloc_C_str("STOP", buf, n, allow_exceptions);
    case OrderType::STOP_LIMIT:
        return alloc_C_str("STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP:
        return alloc_C_str("TRAILING_STOP", buf, n, allow_exceptions);
    case OrderType::MARKET_ON_CLOSE:
        return alloc_C_str("MARKET_ON_CLOSE", buf, n, allow_exceptions);
    case OrderType::EXERCISE:
        return alloc_C_str("EXERCISE", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP_LIMIT:
        return alloc_C_str("TRAILING_STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::NET_DEBIT:
        return alloc_C_str("NET_DEBIT", buf, n, allow_exceptions);
    case OrderType::NET_CREDIT:
        return alloc_C_str("NET_CREDIT", buf, n, allow_exceptions);
    case OrderType::NET_ZERO:
        return alloc_C_str("NET_ZERO", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderType");
    }
}


int
ComplexOrderStrategyType_to_string_ABI(
    int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(ComplexOrderStrategyType, v, allow_exceptions);

    switch(static_cast<ComplexOrderStrategyType>(v)){
    case ComplexOrderStrategyType::NONE:
        return alloc_C_str("NONE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COVERED:
        return alloc_C_str("COVERED", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL:
        return alloc_C_str("VERTICAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BACK_RATIO:
        return alloc_C_str("BACK_RATIO", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CALENDAR:
        return alloc_C_str("CALENDAR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DIAGONAL:
        return alloc_C_str("DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRADDLE:
        return alloc_C_str("STRADDLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRANGLE:
        return alloc_C_str("STRANGLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_SYNTHETIC:
        return alloc_C_str("COLLAR_SYNTHETIC", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BUTTERFLY:
        return alloc_C_str("BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CONDOR:
        return alloc_C_str("CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::IRON_CONDOR:
        return alloc_C_str("IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL_ROLL:
        return alloc_C_str("VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_WITH_STOCK:
        return alloc_C_str("COLLAR_WITH_STOCK", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DOUBLE_DIAGONAL:
        return alloc_C_str("DOUBLE_DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_BUTTERFLY:
        return alloc_C_str("UNBALANCED_BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_CONDOR:
        return alloc_C_str("UNBALANCED_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_IRON_CONDOR:
        return alloc_C_str("UNBALANCED_IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_VERTICAL_ROLL:
        return alloc_C_str("UNBALANCED_VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CUSTOM:
        return alloc_C_str("CUSTOM", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid ComplexOrderStrategyType");
    }
}


int
OrderStrategyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderStrategyType, v, allow_exceptions);

    switch(static_cast<OrderStrategyType>(v)){
    case OrderStrategyType::SINGLE:
        return alloc_C_str("SINGLE", buf, n, allow_exceptions);
    case OrderStrategyType::OCO:
        return alloc_C_str("OCO", buf, n, allow_exceptions);
    case OrderStrategyType::TRIGGER:
        return alloc_C_str("TRIGGER", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderStrategyType");
    }
}





