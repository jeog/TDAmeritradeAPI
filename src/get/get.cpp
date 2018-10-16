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
#include <iostream>
#include <regex>
#include <cctype>
#include <mutex>
#include <string.h>

#include "../../include/_tdma_api.h"
#include "../../include/_get.h"

using namespace std;
using namespace chrono;

namespace tdma{

const milliseconds APIGetterImpl::DEF_WAIT_MSEC(500);

milliseconds APIGetterImpl::wait_msec(APIGetterImpl::DEF_WAIT_MSEC);
milliseconds APIGetterImpl::last_get_msec(
    util::get_msec_since_epoch<conn::clock_ty>()
    );

mutex APIGetterImpl::get_mtx;

APIGetterImpl::APIGetterImpl(Credentials& creds,
                                 api_on_error_cb_ty on_error_callback)
    :
        _on_error_callback(on_error_callback),
        _credentials(creds),
        _connection()
    {
    }

void
APIGetterImpl::set_url(string url)
{ _connection.SET_url(url); }

string
APIGetterImpl::get()
{
    if( is_closed() )
        TDMA_API_THROW(APIException, "connection is closed");

    assert( !_connection.is_closed() );
    return APIGetterImpl::throttled_get(*this);
}

void
APIGetterImpl::close()
{ _connection.close(); }

bool
APIGetterImpl::is_closed() const
{ return !_connection; }

string
APIGetterImpl::throttled_get(APIGetterImpl& getter)
{
    /*
     * get_mtx allows threaded api execution from different getters in
     * different threads AND the same getter in different threads.
     *
     * IT DOESN'T HANDLE OTHER OTHER SYNC ISSUES INSIDE THE CurlConnection
     * CLASSES.
     */
    lock_guard<mutex> _(get_mtx);
    auto remaining = throttled_wait_remaining();
    if( remaining.count() > 0 ){
        /*
         * wait_msec and last_get_msec provide a global throttling mechanism
         * for ALL get requests to avoid excessive calls to TDMA servers
         */
        assert( remaining <= wait_msec );
        this_thread::sleep_for( remaining );
    }

    string s;
    conn::clock_ty::time_point tp;
    tie(s, tp) = connect_get( getter._connection, getter._credentials,
                              getter._on_error_callback );

    last_get_msec = duration_cast<milliseconds>(tp.time_since_epoch());
    return s;
}

milliseconds
APIGetterImpl::throttled_wait_remaining()
{
    auto elapsed = util::get_msec_since_epoch<conn::clock_ty>() - last_get_msec;
    assert( elapsed.count() >= 0 );
    return wait_msec - elapsed;
}

milliseconds
APIGetterImpl::wait_remaining()
{
    static const milliseconds ms0(0);
    return max(throttled_wait_remaining(), ms0);
}

void
APIGetterImpl::set_wait_msec(milliseconds msec)
{
    lock_guard<mutex> _(get_mtx);
    wait_msec = msec;
}

milliseconds
APIGetterImpl::get_wait_msec()
{ return wait_msec; }


} /* tdma */

using namespace tdma;

int
APIGetter_Get_ABI( Getter_C *pgetter,
                     char **buf,
                     size_t *n,
                     int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<APIGetterImpl>(
            pgetter, &APIGetterImpl::get, buf, n, allow_exceptions
        );
}

int
APIGetter_Close_ABI(Getter_C *pgetter, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj){
        reinterpret_cast<APIGetterImpl*>(obj)->close();
    };

    return CallImplFromABI(allow_exceptions, meth, pgetter->obj);
}

int
APIGetter_IsClosed_ABI(Getter_C *pgetter, int*b, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(b, "b", allow_exceptions);

    static auto meth = +[](void* obj){
        return reinterpret_cast<APIGetterImpl*>(obj)->is_closed();
    };

    tie(*b, err) = CallImplFromABI(allow_exceptions, meth, pgetter->obj);
    return err;
}

int
APIGetter_SetWaitMSec_ABI(unsigned long long msec, int allow_exceptions)
{
    return CallImplFromABI( allow_exceptions, APIGetterImpl::set_wait_msec,
                            milliseconds(msec) );
}

int
APIGetter_GetWaitMSec_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    milliseconds ms;
    int err;
    tie(ms, err) = CallImplFromABI( allow_exceptions,
                                    APIGetterImpl::get_wait_msec );
    if(err)
        return err;

    *msec = static_cast<unsigned long long>(ms.count());
    return 0;
}

int
APIGetter_GetDefWaitMSec_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    *msec = static_cast<unsigned long long>(
        APIGetterImpl::DEF_WAIT_MSEC.count()
        );
    return 0;
}

int
APIGetter_WaitRemaining_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    *msec = static_cast<unsigned long long>(
        APIGetterImpl::wait_remaining().count()
        );
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
        throw runtime_error("invalid PeriodType");
    }
}

int
FrequencyType_to_string_ABI( int v,
                                char** buf,
                                size_t* n,
                                int allow_exceptions )
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
        throw runtime_error("invalid FrequencyType");
    }
}

int
OptionContractType_to_string_ABI( int v,
                                      char** buf,
                                      size_t* n,
                                      int allow_exceptions )
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
        throw runtime_error("invalid OptionContractType");
    }
}

int
OptionStrategyType_to_string_ABI( int v,
                                      char** buf,
                                      size_t* n,
                                      int allow_exceptions )
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
        throw runtime_error("invalid OptionStrategyType");
    }
}


int
OptionRangeType_to_string_ABI( int v,
                                    char** buf,
                                    size_t* n,
                                    int allow_exceptions )
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
        throw runtime_error("invalid OptionRangeType");
    }
}


int
OptionExpMonth_to_string_ABI( int v,
                                  char** buf,
                                  size_t* n,
                                  int allow_exceptions )
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
        throw runtime_error("invalid OptionExpMonth");
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
    default: throw runtime_error("invalid OptionType");
    }
}


int
TransactionType_to_string_ABI( int v,
                                    char** buf,
                                    size_t* n,
                                    int allow_exceptions )
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
        throw runtime_error("invliad TransactionType");
    }
}

int
InstrumentSearchType_to_string_ABI( int v,
                                        char** buf,
                                        size_t* n,
                                        int allow_exceptions )
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
        throw runtime_error("invalid InstrumentSearchType");
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
        throw runtime_error("Invalid MarketType");
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
        throw runtime_error("invalid MoversIndex");
    }
}


int
MoversDirectionType_to_string_ABI( int v,
                                        char** buf,
                                        size_t* n,
                                        int allow_exceptions )
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
        throw runtime_error("invalid MoversDirectionType");
    }
}

int
MoversChangeType_to_string_ABI( int v,
                                    char** buf,
                                    size_t* n,
                                    int allow_exceptions )
{
    CHECK_ENUM(MoversChangeType, v, allow_exceptions);

    switch(static_cast<MoversChangeType>(v)){
    case MoversChangeType::percent:
        return alloc_C_str("percent", buf, n, allow_exceptions);
    case MoversChangeType::value:
        return alloc_C_str("value", buf, n, allow_exceptions);
    default:
        throw runtime_error("invalid MoversChangeType");
    }
}

int
OptionStrikesType_to_string_ABI( int v,
                                     char** buf,
                                     size_t* n,
                                     int allow_exceptions )
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
        throw runtime_error("invalid OptionStrikesType");
    }
}

int
OrderStatusType_to_string_ABI( int v,
                                   char** buf,
                                   size_t *n,
                                   int allow_exceptions )
{
    CHECK_ENUM(OrderStatusType, v, allow_exceptions);

    switch(static_cast<OrderStatusType>(v) ){
    case OrderStatusType::AWAITING_PARENT_ORDER:
        return alloc_C_str("AWAITING_PARENT_ORDER", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_CONDITION:
        return alloc_C_str("AWAITING_CONDITION", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_MANUAL_REVIEW:
        return alloc_C_str("AWAITING_MANUAL_REVIEW", buf, n, allow_exceptions);
    case OrderStatusType::ACCEPTED:
        return alloc_C_str("ACCEPTED", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_UR_OUT:
        return alloc_C_str("AWAITING_UR_OUT", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_ACTIVATION:
        return alloc_C_str("PENDING_ACTIVATION", buf, n, allow_exceptions);
    case OrderStatusType::QUEUED:
        return alloc_C_str("QUEUED", buf, n, allow_exceptions);
    case OrderStatusType::WORKING:
        return alloc_C_str("WORKING", buf, n, allow_exceptions);
    case OrderStatusType::REJECTED:
        return alloc_C_str("REJECTED", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_CANCEL:
        return alloc_C_str("PENDING_CANCE", buf, n, allow_exceptions);
    case OrderStatusType::CANCELED:
        return alloc_C_str("CANCELED", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_REPLACE:
        return alloc_C_str("PENDING_REPLACE", buf, n, allow_exceptions);
    case OrderStatusType::REPLACED:
        return alloc_C_str("REPLACED", buf, n, allow_exceptions);
    case OrderStatusType::FILLED:
        return alloc_C_str("FILLED", buf, n, allow_exceptions);
    case OrderStatusType::EXPIRED:
        return alloc_C_str("EXPIRED", buf, n, allow_exceptions);
    default:
        throw runtime_error("invalid OrderStatusType");
    }
}
