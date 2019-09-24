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

using std::string;
using std::tie;
using std::chrono::milliseconds;


namespace tdma{

const milliseconds APIGetterImpl::DEF_WAIT_MSEC(500);

milliseconds APIGetterImpl::wait_msec(APIGetterImpl::DEF_WAIT_MSEC);
milliseconds APIGetterImpl::last_get_msec(
    util::get_msec_since_epoch<conn::clock_ty>()
    );

std::mutex APIGetterImpl::get_mtx;

int APIGetterImpl::current_connection_group = 0;

APIGetterImpl::APIGetterImpl( Credentials& creds,
                              api_on_error_cb_ty on_error_callback )
    :
        _on_error_callback(on_error_callback),
        _credentials(creds),
        _connection(
            (current_connection_group < 0)
                ? reinterpret_cast<conn::HTTPConnectionInterface*>(
                        new conn::HTTPConnection(conn::HttpMethod::http_get)
                        )
                : reinterpret_cast<conn::HTTPConnectionInterface*>(
                        new conn::SharedHTTPConnection(
                                conn::HttpMethod::http_get,
                                current_connection_group)
                        )
                )
    {
    }

void
APIGetterImpl::set_url(const string& url)
{
    _connection->set_url(url);
}

string
APIGetterImpl::get()
{
    return APIGetterImpl::throttled_get(*this);
}

void
APIGetterImpl::close()
{
    _connection->close();
}

bool
APIGetterImpl::is_closed() const
{
    return _connection->is_closed();
}

void
APIGetterImpl::set_timeout(milliseconds msec)
{
    _connection->set_timeout(
            std::min( std::numeric_limits<long>::max(), msec.count() )
            );
}

milliseconds
APIGetterImpl::get_timeout() const
{
    return milliseconds( _connection->get_timeout() );
}

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
    std::lock_guard<std::mutex> _(get_mtx);

    auto remaining = throttled_wait_remaining();
    if( remaining.count() > 0 ){
        /*
         * wait_msec and last_get_msec provide a global throttling mechanism
         * for ALL get requests to avoid excessive calls to TDMA servers
         */
        assert( remaining <= wait_msec );
        std::this_thread::sleep_for( remaining );
    }

    string s;
    conn::clock_ty::time_point tp;
    tie(s, tp) = connect_get( *(getter._connection), getter._credentials,
                              getter._on_error_callback );

    last_get_msec =
        std::chrono::duration_cast<milliseconds>(tp.time_since_epoch());
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
    std::lock_guard<std::mutex> _(get_mtx);
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
APIGetter_SetTimeout_ABI(Getter_C *pgetter, unsigned long long msec, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj, unsigned long long m){
        reinterpret_cast<APIGetterImpl*>(obj)->set_timeout(milliseconds(m));
    };

    return CallImplFromABI(allow_exceptions, meth, pgetter->obj, msec);
}

int
APIGetter_GetTimeout_ABI(Getter_C *pgetter, unsigned long long *msec, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(msec, "msec", allow_exceptions);

    static auto meth = +[](void* obj){
        return static_cast<unsigned long long>(
                reinterpret_cast<APIGetterImpl*>(obj)->get_timeout().count()
                );
    };

    tie(*msec,err) = CallImplFromABI(allow_exceptions, meth, pgetter->obj);
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
APIGetter_ShareConnections_ABI(int b, int allow_exceptions)
{
    return CallImplFromABI( allow_exceptions, APIGetterImpl::share_connections,
            static_cast<bool>(b) );
}

int
APIGetter_IsSharingConnections_ABI(int *b, int allow_exceptions)
{
    CHECK_PTR(b, "b", allow_exceptions);

    *b = static_cast<int>( APIGetterImpl::is_sharing_connections() );
    return 0;
}

int
PeriodType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(PeriodType, v, allow_exceptions);

    switch(static_cast<PeriodType>(v)){
    case PeriodType::day:
        return to_new_char_buffer("day", buf, n, allow_exceptions);
    case PeriodType::month:
        return to_new_char_buffer("month", buf, n, allow_exceptions);
    case PeriodType::year:
        return to_new_char_buffer("year", buf, n, allow_exceptions);
    case PeriodType::ytd:
        return to_new_char_buffer("ytd", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid PeriodType");
    }
}

int
FrequencyType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(FrequencyType, v, allow_exceptions);

    switch(static_cast<FrequencyType>(v)){
    case FrequencyType::minute:
        return to_new_char_buffer("minute", buf, n, allow_exceptions);
    case FrequencyType::daily:
        return to_new_char_buffer("daily", buf, n, allow_exceptions);
    case FrequencyType::weekly:
        return to_new_char_buffer("weekly", buf, n, allow_exceptions);
    case FrequencyType::monthly:
        return to_new_char_buffer("monthly", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid FrequencyType");
    }
}

int
OptionContractType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionContractType, v, allow_exceptions);

    switch(static_cast<OptionContractType>(v)){
    case OptionContractType::call:
        return to_new_char_buffer("CALL", buf, n, allow_exceptions);
    case OptionContractType::put:
        return to_new_char_buffer("PUT", buf, n, allow_exceptions);
    case OptionContractType::all:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionContractType");
    }
}

int
OptionStrategyType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionStrategyType, v, allow_exceptions);

    switch(static_cast<OptionStrategyType>(v)){
    //case OptionStrategyType::single: return "SINGLE";
    //case OptionStrategyType::analytical: return "ANALYTICAL";
    case OptionStrategyType::covered:
        return to_new_char_buffer("COVERED", buf, n, allow_exceptions);
    case OptionStrategyType::vertical:
        return to_new_char_buffer("VERTICAL", buf, n, allow_exceptions);
    case OptionStrategyType::calendar:
        return to_new_char_buffer("CALENDAR", buf, n, allow_exceptions);
    case OptionStrategyType::strangle:
        return to_new_char_buffer("STRANGLE", buf, n, allow_exceptions);
    case OptionStrategyType::straddle:
        return to_new_char_buffer("STRADDLE", buf, n, allow_exceptions);
    case OptionStrategyType::butterfly:
        return to_new_char_buffer("BUTTERFLY", buf, n, allow_exceptions);
    case OptionStrategyType::condor:
        return to_new_char_buffer("CONDOR", buf, n, allow_exceptions);
    case OptionStrategyType::diagonal:
        return to_new_char_buffer("DIAGONAL", buf, n, allow_exceptions);
    case OptionStrategyType::collar:
        return to_new_char_buffer("COLLAR", buf, n, allow_exceptions);
    case  OptionStrategyType::roll:
        return to_new_char_buffer("ROLL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionStrategyType");
    }
}


int
OptionRangeType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionRangeType, v, allow_exceptions);

    switch(static_cast<OptionRangeType>(v)){
    case OptionRangeType::null:
        return to_new_char_buffer("", buf, n, allow_exceptions);
    case OptionRangeType::itm:
        return to_new_char_buffer("ITM", buf, n, allow_exceptions);
    case OptionRangeType::ntm:
        return to_new_char_buffer("NTM", buf, n, allow_exceptions);
    case OptionRangeType::otm:
        return to_new_char_buffer("OTM", buf, n, allow_exceptions);
    case OptionRangeType::sak:
        return to_new_char_buffer("SAK", buf, n, allow_exceptions);
    case OptionRangeType::sbk:
        return to_new_char_buffer("SBK", buf, n, allow_exceptions);
    case OptionRangeType::snk:
        return to_new_char_buffer("SNK", buf, n, allow_exceptions);
    case OptionRangeType::all:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionRangeType");
    }
}


int
OptionExpMonth_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionExpMonth, v, allow_exceptions);

    switch(static_cast<OptionExpMonth>(v)){
    case OptionExpMonth::jan:
        return to_new_char_buffer("JAN", buf, n, allow_exceptions);
    case OptionExpMonth::feb:
        return to_new_char_buffer("FEB", buf, n, allow_exceptions);
    case OptionExpMonth::mar:
        return to_new_char_buffer("MAR", buf, n, allow_exceptions);
    case OptionExpMonth::apr:
        return to_new_char_buffer("APR", buf, n, allow_exceptions);
    case OptionExpMonth::may:
        return to_new_char_buffer("MAY", buf, n, allow_exceptions);
    case OptionExpMonth::jun:
        return to_new_char_buffer("JUN", buf, n, allow_exceptions);
    case OptionExpMonth::jul:
        return to_new_char_buffer("JUL", buf, n, allow_exceptions);
    case OptionExpMonth::aug:
        return to_new_char_buffer("AUG", buf, n, allow_exceptions);
    case OptionExpMonth::sep:
        return to_new_char_buffer("SEP", buf, n, allow_exceptions);
    case OptionExpMonth::oct:
        return to_new_char_buffer("OCT", buf, n, allow_exceptions);
    case OptionExpMonth::nov:
        return to_new_char_buffer("NOV", buf, n, allow_exceptions);
    case OptionExpMonth::dec:
        return to_new_char_buffer("DEC", buf, n, allow_exceptions);
    case OptionExpMonth::all:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionExpMonth");
    }
}


int
OptionType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionType, v, allow_exceptions);

    switch(static_cast<OptionType>(v)){
    case OptionType::s:
        return to_new_char_buffer("S", buf, n, allow_exceptions);
    case OptionType::ns:
        return to_new_char_buffer("NS", buf, n, allow_exceptions);
    case OptionType::all:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    default: throw std::runtime_error("invalid OptionType");
    }
}


int
TransactionType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(TransactionType, v, allow_exceptions);

    switch(static_cast<TransactionType>(v)){
    case TransactionType::all:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    case TransactionType::trade:
        return to_new_char_buffer("TRADE", buf, n, allow_exceptions);
    case TransactionType::buy_only:
        return to_new_char_buffer("BUY_ONLY", buf, n, allow_exceptions);
    case TransactionType::sell_only:
        return to_new_char_buffer("SELL_ONLY", buf, n, allow_exceptions);
    case TransactionType::cash_in_or_cash_out:
        return to_new_char_buffer("CASH_IN_OR_CASH_OUT", buf, n, allow_exceptions);
    case TransactionType::checking:
        return to_new_char_buffer("CHECKING", buf, n, allow_exceptions);
    case TransactionType::dividend:
        return to_new_char_buffer("DIVIDEND", buf, n, allow_exceptions);
    case TransactionType::interest:
        return to_new_char_buffer("INTEREST", buf, n, allow_exceptions);
    case TransactionType::other:
        return to_new_char_buffer("OTHER", buf, n, allow_exceptions);
    case TransactionType::advisor_fees:
        return to_new_char_buffer("ADVISOR_FEES", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invliad TransactionType");
    }
}

int
InstrumentSearchType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(InstrumentSearchType, v, allow_exceptions);

    switch(static_cast<InstrumentSearchType>(v)){
    case InstrumentSearchType::symbol_exact:
        return to_new_char_buffer("fundamental", buf, n, allow_exceptions);
    case InstrumentSearchType::symbol_search:
        return to_new_char_buffer("symbol-search", buf, n, allow_exceptions);
    case InstrumentSearchType::symbol_regex:
        return to_new_char_buffer("symbol-regex", buf, n, allow_exceptions);
    case InstrumentSearchType::description_search:
        return to_new_char_buffer("desc-search", buf, n, allow_exceptions);
    case InstrumentSearchType::description_regex:
        return to_new_char_buffer("desc-regex", buf, n, allow_exceptions);
    case InstrumentSearchType::cusip:
        return to_new_char_buffer("cusip", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid InstrumentSearchType");
    }
}


int
MarketType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(MarketType, v, allow_exceptions);

    switch(static_cast<MarketType>(v)){
    case MarketType::equity:
        return to_new_char_buffer("EQUITY", buf, n, allow_exceptions);
    case MarketType::option:
        return to_new_char_buffer("OPTION", buf, n, allow_exceptions);
    case MarketType::future:
        return to_new_char_buffer("FUTURE", buf, n, allow_exceptions);
    case MarketType::bond:
        return to_new_char_buffer("BOND", buf, n, allow_exceptions);
    case MarketType::forex:
        return to_new_char_buffer("FOREX", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid MarketType");
    }
}

int
MoversIndex_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(MoversIndex, v, allow_exceptions);

    switch(static_cast<MoversIndex>(v)){
    case MoversIndex::compx:
        return to_new_char_buffer("$COMPX", buf, n, allow_exceptions);
    case MoversIndex::dji:
        return to_new_char_buffer("$DJI", buf, n, allow_exceptions);
    case MoversIndex::spx:
        return to_new_char_buffer("$SPX.X", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversIndex");
    }
}


int
MoversDirectionType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(MoversDirectionType, v, allow_exceptions);

    switch(static_cast<MoversDirectionType>(v)){
    case MoversDirectionType::up:
        return to_new_char_buffer("up", buf, n, allow_exceptions);
    case MoversDirectionType::down:
        return to_new_char_buffer("down", buf, n, allow_exceptions);
    case MoversDirectionType::up_and_down:
        return to_new_char_buffer("up_and_down", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversDirectionType");
    }
}

int
MoversChangeType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(MoversChangeType, v, allow_exceptions);

    switch(static_cast<MoversChangeType>(v)){
    case MoversChangeType::percent:
        return to_new_char_buffer("percent", buf, n, allow_exceptions);
    case MoversChangeType::value:
        return to_new_char_buffer("value", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid MoversChangeType");
    }
}

int
OptionStrikesType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OptionStrikesType, v, allow_exceptions);

    switch(static_cast<OptionStrikesType>(v)){
    case OptionStrikesType::n_atm:
        return to_new_char_buffer("n_atm", buf, n, allow_exceptions);
    case OptionStrikesType::single:
        return to_new_char_buffer("single", buf, n, allow_exceptions);
    case OptionStrikesType::range:
        return to_new_char_buffer("range", buf, n, allow_exceptions);
    case OptionStrikesType::none:
        return to_new_char_buffer("none", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OptionStrikesType");
    }
}

int
OrderStatusType_to_string_ABI( TDMA_API_TO_STRING_ABI_ARGS )
{
    CHECK_ENUM(OrderStatusType, v, allow_exceptions);

    switch(static_cast<OrderStatusType>(v) ){
    case OrderStatusType::AWAITING_PARENT_ORDER:
        return to_new_char_buffer("AWAITING_PARENT_ORDER", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_CONDITION:
        return to_new_char_buffer("AWAITING_CONDITION", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_MANUAL_REVIEW:
        return to_new_char_buffer("AWAITING_MANUAL_REVIEW", buf, n, allow_exceptions);
    case OrderStatusType::ACCEPTED:
        return to_new_char_buffer("ACCEPTED", buf, n, allow_exceptions);
    case OrderStatusType::AWAITING_UR_OUT:
        return to_new_char_buffer("AWAITING_UR_OUT", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_ACTIVATION:
        return to_new_char_buffer("PENDING_ACTIVATION", buf, n, allow_exceptions);
    case OrderStatusType::QUEUED:
        return to_new_char_buffer("QUEUED", buf, n, allow_exceptions);
    case OrderStatusType::WORKING:
        return to_new_char_buffer("WORKING", buf, n, allow_exceptions);
    case OrderStatusType::REJECTED:
        return to_new_char_buffer("REJECTED", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_CANCEL:
        return to_new_char_buffer("PENDING_CANCEL", buf, n, allow_exceptions);
    case OrderStatusType::CANCELED:
        return to_new_char_buffer("CANCELED", buf, n, allow_exceptions);
    case OrderStatusType::PENDING_REPLACE:
        return to_new_char_buffer("PENDING_REPLACE", buf, n, allow_exceptions);
    case OrderStatusType::REPLACED:
        return to_new_char_buffer("REPLACED", buf, n, allow_exceptions);
    case OrderStatusType::FILLED:
        return to_new_char_buffer("FILLED", buf, n, allow_exceptions);
    case OrderStatusType::EXPIRED:
        return to_new_char_buffer("EXPIRED", buf, n, allow_exceptions);
    case OrderStatusType::ALL:
        return to_new_char_buffer("ALL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("invalid OrderStatusType");
    }
}
