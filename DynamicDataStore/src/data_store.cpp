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
#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <queue>
#include <mutex>

#include "common.h"
#include "tdma_data_store.h"
#include "backing_store.h"

#include "tdma_api_streaming.h"
#include "tdma_api_get.h"

#ifdef _WIN32
#define timegm _mkgmtime
#endif


/*
 *    [             Data Deque            ]
 *     -----------------------------------
 *    [ recent ]                  [  old  ]
 *    [ front ]                   [ back  ]
 *    [ end min ]             [ start min ]
 *    [ end indx ]           [ start indx ]
 */


namespace {

using namespace ds;

const int MSEC_IN_MIN = 60 * 1000;
const int MSEC_IN_DAY = MSEC_IN_MIN * 60 * 24;

const int NUPDATE_ATTEMPTS = 4;
const double UPDATE_EXPAND_FACTOR = 2.0;
const unsigned long long UPDATE_MIN_BARS = 24 * 60; // 1 day

const int CREDS_EXP_THRESHOLD_SEC = 2 * 24 * 60 * 60; // 2 days

const std::set<tdma::ChartEquitySubscription::FieldType>
EQUITY_CHART_SUB_FIELDS{
    tdma::ChartEquitySubscription::FieldType::open_price, // 1
    tdma::ChartEquitySubscription::FieldType::high_price, // 2
    tdma::ChartEquitySubscription::FieldType::low_price, // 3
    tdma::ChartEquitySubscription::FieldType::close_price, // 4
    tdma::ChartEquitySubscription::FieldType::volume, // 5
    tdma::ChartEquitySubscription::FieldType::sequence, // 6
    tdma::ChartEquitySubscription::FieldType::chart_time, // 7
};

const std::set<tdma::TimesaleEquitySubscription::FieldType>
EQUITY_TIMESALE_SUB_FIELDS{
    tdma::TimesaleEquitySubscription::FieldType::trade_time, // 1
    tdma::TimesaleEquitySubscription::FieldType::last_price, // 2
    tdma::TimesaleEquitySubscription::FieldType::last_size, // 3
    tdma::TimesaleEquitySubscription::FieldType::last_sequence, // 4
};

std::string directory_path;
std::string log_file_path;

std::shared_ptr<BackingStore> backing_store;
std::shared_ptr<tdma::StreamingSession> session;
std::shared_ptr<tdma::ChartEquitySubscription> sub_equity_chart;
std::shared_ptr<tdma::TimesaleEquitySubscription> sub_equity_timesale;

Credentials *credentials; // TODO

volatile bool is_initialized = false;

enum class UpdateState : int {
    failed = 0,
    succeeded,
    exhausted
};

class DataStoreError
        : public std::runtime_error{
    using std::runtime_error::runtime_error;
};


struct SymbolData {
    typedef std::map<std::string, SymbolData> all_ty;
    static all_ty all;

private:
    struct IOHelper{
        SymbolData *sdata;
        IOHelper( SymbolData * sdata ) : sdata(sdata) {}
    };

    struct WriteHelper : public IOHelper{
        std::deque<OHLCVData>::const_iterator b, e;
        WriteHelper( SymbolData * sdata )
            : IOHelper(sdata), b(sdata->data->cbegin()), e(sdata->data->cend())
        {}
    };

    struct FrontWriter : public WriteHelper {
        using WriteHelper::WriteHelper;
        int operator()(std::fstream& f){
            auto start = b + sdata->write_pos_begin; //exclusive
            auto pos = start;
            while( pos > b && f ){
                --pos;
                if( pos->close == 0 ){
                    assert( pos->is_empty_bar() );
                    if( (pos != (start-1)) && (pos != b) )
                        continue; // skip empty bars, not first or last
                }
                f << *pos << std::endl;
            }
            return (start - pos);
        }
    };

    struct BackWriter : public WriteHelper {
        using WriteHelper::WriteHelper;
        int operator()(std::fstream& f){
            auto start = b + sdata->write_pos_end; //inclusive
            auto pos = start;
            while( pos < e && f ){
                if( pos->close == 0 ){
                    assert( pos->is_empty_bar() );
                    if( (pos != start) && (pos != (e-1)) ){
                        ++pos;
                        continue; // skip empty bars, not first or last
                    }
                }
                f << *pos << std::endl;
                ++pos;
            }
            return (pos - start);
        }
    };

    struct FrontReader : public IOHelper{
        using IOHelper::IOHelper;
        int operator()(std::fstream& f){
            double open, high, low, close;
            long long volume, ngaps, dt, dt_last = -1;
            int i = 1;
            while( f >> dt >> open >> high >> low >> close >> volume )
            {
                if( dt_last > -1 ){
                    ngaps = dt - dt_last;
                    assert( ngaps >= 0 ); // allow duplicates
                    while( ngaps-- > 1 )
                        sdata->data->emplace_front( ++dt_last, 0,0,0,0,0 );
                }
                if( dt_last == -1 || dt > dt_last ) // drop duplicates
                    sdata->data->emplace_front(dt,open,high,low,close,volume);
                dt_last = dt;
                ++i;
            };
            return i;
        }
    };

    // TODO error check for bad dt
    struct BackReader : public IOHelper{
        using IOHelper::IOHelper;
        int operator()(std::fstream& f){
            double open, high, low, close;
            long long volume, ngaps, dt, dt_last = -1;
            int i = 1;
            while( f >> dt >> open >> high >> low >> close >> volume )
            {
                if( dt_last > -1 ){
                     ngaps = dt_last - dt;
                     assert( ngaps >= 0 ); // allow duplicates
                     while( ngaps-- > 1 )
                         sdata->data->emplace_back( --dt_last, 0,0,0,0,0 );
                 }
                if( dt_last == -1 || dt < dt_last ) // drop duplicates
                    sdata->data->emplace_back(dt,open,high,low,close,volume);
                dt_last = dt;
                ++i;
            };
            return i;
        }
    };

    template<bool IncrWritePositions>
    void
    _update(unsigned long long min)
    {
        if(IncrWritePositions){
            ++(write_pos_begin);
            ++(write_pos_end);
        }
        if( min > min_end )  // includes 0 check
            min_end = min;
        if( min < min_start || min_start == 0 )
            min_start = min;
    }

public:
    std::string symbol;
    std::unique_ptr<std::deque<OHLCVData>> data; // restricts copy / assign for us
    unsigned long long min_start;
    unsigned long long min_end;
    std::deque<OHLCVData>::size_type write_pos_begin; // < here goes to file_back
    std::deque<OHLCVData>::size_type write_pos_end; // >= here goes to file_front
    bool allow_reload;

    SymbolData() = delete;

    SymbolData( const std::string& symbol, bool allow_reload = false )
        :
            symbol( symbol ),
            data( nullptr ),
            min_start( 0 ),
            min_end( 0 ),
            write_pos_begin( 0 ),
            write_pos_end( 0 ),
            allow_reload( allow_reload )
        {
            assert( toupper(symbol) == symbol );
        }

    operator bool() const
    { return data.operator bool(); }

    void
    push_front( const OHLCVData& d )
    {
        data->push_front(d);
        _update<true>(d.min_since_epoch);
    }

    void
    push_back( const OHLCVData& d )
    {
        data->push_back(d);
        _update<false>(d.min_since_epoch);
    }

    template<typename... Args>
    void
    emplace_front(unsigned long long min, Args&&... args)
    {
        data->emplace_front( min, args...);
        _update<true>(min);
    }

    template<typename... Args>
    void
    emplace_back(unsigned long long min, Args&&... args)
    {
        data->emplace_back( min, args...);
        _update<false>(min);
    }

    bool
    load()
    {
        if( !allow_reload && (*this) )
            throw DataStoreError("can't reload symbol data");

        if( !backing_store || !backing_store->is_valid() )
            throw DataStoreError("null/invalid backing store");

        data.reset( new std::deque<OHLCVData>  );
        if( !backing_store->read_from_symbol_store(
                symbol, FrontReader(this), BackReader(this) ) )
        {
            data.reset();
            return false;
        }

        if( !data->empty() ){
            min_start = data->back().min_since_epoch;
            min_end = data->front().min_since_epoch;
            write_pos_end = data->size();
            if( (data->size() - 1) != (min_end - min_start) )
                throw DataStoreError("size of data doesn't match time range");
        }

        return true;
    }

    bool
    store()
    {
        if( !backing_store || !backing_store->is_valid() )
            throw DataStoreError("null/invalid backing store");

        /*
         * TODO means for handling errors in here:
         *   - make this whole op atomic
         *   - backup index file ?
         */

        bool b = backing_store->write_to_symbol_store(
                   symbol, FrontWriter(this), BackWriter(this) );

        write_pos_begin = 0;
        write_pos_end = data->size();
        return b;
    }

    long long
    front_offset(unsigned long long min_since_epoch) const
    {
        return static_cast<long long>(min_end)
            -  static_cast<long long>(min_since_epoch);
    }

    long long
    back_offset(unsigned long long min_since_epoch) const
    {
        return static_cast<long long>(min_since_epoch)
            - static_cast<long long>(min_start);
    }

    typename std::deque<OHLCVData>::iterator
    find_safe( unsigned long long min_since_epoch ) const
    {
        // binary search O(log) - safer, but slower
        return find_desc(*data, OHLCVData(min_since_epoch), OHLCVData::IsOlder);
    }

    typename std::deque<OHLCVData>::iterator
    find_fast( unsigned long long min_since_epoch ) const
    {
        // index/lookup search O(C) - faster
        long long front = front_offset(min_since_epoch);
        long long sz = static_cast<long long>(data->size());
        assert( front + back_offset(min_since_epoch) + 1LL == sz );

        if( front < 0 || front > sz )
            return data->end();

        return data->begin() + front;
    }
};

std::map<std::string, SymbolData> SymbolData::all;


struct StreamingData {
    OHLCVData data;
    long long seq;

    StreamingData( unsigned long long min, double o, double h, double l,
                       double c, long long v, long long seq )
        :
            data(min, o, h, l, c, v),
            seq(seq)
        {}

    StreamingData( const OHLCVData& data, long long seq)
        :
            data(data),
            seq(seq)
        {}

    StreamingData() : StreamingData( OHLCVData::null, 0 ) {}

    bool
    operator==(const StreamingData& d) const
    { return data == d.data && seq == d.seq; }

    bool
    operator!=(const StreamingData& d) const
    { return !(*this == d); }

private:
    static std::map<std::string, std::queue<StreamingData>> queues;
    static std::map<std::string, StreamingData> active_bars;
    static std::set<std::string> initialized;
    static std::mutex queues_mtx;

public:
    static const StreamingData null;

    struct QueuesGuard : public std::lock_guard<std::mutex>{
        QueuesGuard() : std::lock_guard<std::mutex>(queues_mtx){}
    };

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static void
    RemoveQueue( const std::string& symbol )
    {
        initialized.erase(symbol);
        active_bars.erase(symbol);
        queues.erase(symbol);
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static std::map<std::string, std::queue<StreamingData>>
    GetQueueCopiesAndClear()
    {
        std::map<std::string, std::queue<StreamingData>> tmp;
        for( auto& p : queues )
            tmp[p.first] = std::move(p.second);
        queues.clear();
        return tmp;
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    template<typename... Args>
    static inline void
    Enqueue( const std::string& symbol, Args&&... args)
    {
        // adds queue if doesn't exist
        queues[symbol].emplace(args...);
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static long long
    UpdateActiveBar( const std::string& symbol,
                     unsigned long long time,
                     double last,
                     unsigned long long size,
                     unsigned long long seq ) // TODO bar gaps???
    {
        auto& A = active_bars[symbol];
        auto& d = A.data;

        long long gap = seq - A.seq;
        if( gap <= 0 )
            return gap;
        A.seq = seq;

        if( time > d.min_since_epoch ){
            d.min_since_epoch = time;
            d.high = d.low = d.close = d.open = last;
            d.volume = size;
        }else if( time == d.min_since_epoch ){
            d.close = last;
            if( last > d.high )
                d.high = last;
            else if( last < d.low )
                d.low = last;
            d.volume += size;
        }else{
            throw std::runtime_error("invalid active bar chronology");
        }

        return gap;
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static inline std::map<std::string, StreamingData>
    GetActiveBarCopies()
    {
        return active_bars;
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static inline StreamingData
    GetActiveBar( const std::string& symbol )
    {
        auto f = active_bars.find(symbol);
        return (f == active_bars.cend()) ? null : f->second;
    }

    /* CALLER NEEDS TO HOLD LOCK (via QueuesGuard) */
    static void
    ClearAll()
    {
        active_bars.clear();
        initialized.clear();
        queues.clear();
    }

    static bool
    IsInitialized( const std::string& symbol )
    {
        return initialized.count(symbol);
    }

    static void
    SetInitialized( const std::string& symbol )
    {
        initialized.insert( symbol );
    }


};

std::map<std::string, std::queue<StreamingData>> StreamingData::queues;
std::map<std::string, StreamingData> StreamingData::active_bars;
std::set<std::string> StreamingData::initialized;
std::mutex StreamingData::queues_mtx;
const StreamingData StreamingData::null;


std::ostream&
operator<<(std::ostream& out, const StreamingData& data)
{
    out <<  data.data << ' ' << data.seq;
    return out;
}


bool
is_valid_symbol( const std::string& symbol )
{
    if( symbol.empty() ){
        log_error("SYMBOL-CHECK", "empty signal");
        return false;
    }

    json j;
    try{
        tdma::InstrumentInfoGetter g( *credentials,
            tdma::InstrumentSearchType::symbol_search, symbol );
        j = g.get();

    }catch( tdma::APIException& e ){
        log_error( "SYMBOL-CHECK", "instrument info getter failed "
                   + std::string(e.what()), symbol );
        return {};
    }

    auto json_info = j.find(symbol);
    if( json_info == j.end() ){
        log_error("SYMBOL-CHECK", "symbol not in instrument info", symbol);
        return false;
    }

    auto json_atype = json_info->find("assetType");
    if( json_atype == json_info->end() ){
        log_error("SYMBOL-CHECK", "'assetType' not in symbol info", symbol);
        return false;
    }

    std::string atype = json_atype.value();
    log_info("SYMBOL-CHECK", "assetType=" + atype, symbol);

    if (atype != "EQUITY" && atype != "ETF"){
        log_error("SYMBOL-CHECK", "assetType != 'EQUITY' or 'ETF'", symbol);
        return false;
    }

    return true;
}


long long
seconds_until_expiration( const Credentials& creds )
{
    using namespace std::chrono;

    auto now_d = system_clock::now().time_since_epoch();
    auto now_sec = duration_cast<seconds>(now_d);
    return creds.epoch_sec_token_expiration - now_sec.count();
}


bool
is_valid_credentials( const Credentials& creds )
{
    if( !creds.access_token || creds.access_token[0] == '\0' ){
        log_error("CREDENTIALS", "invalid access token");
        return false;
    }

    if( !creds.refresh_token || creds.refresh_token[0] == '\0' ){
        log_error("CREDENTIALS", "invalid refresh token");
        return false;
    }

    if( !creds.client_id || creds.client_id[0] == '\0' ){
        log_error("CREDENTIALS", "invalid client id");
        return false;
    }

    if( creds.epoch_sec_token_expiration <= 0 ){
        log_error("CREDENTIALS", "refresh token has expired");
        return false;
    }

    if( seconds_until_expiration(creds) < CREDS_EXP_THRESHOLD_SEC ){
        log_error("CREDENTIALS", "refresh token too close to expiration");
        return false;
    }

    return true;
}


bool
store()  // TODO clear out index file first ?
{
    bool fail = false;
    for( auto& p : SymbolData::all ){
        try{
            if( !p.second.store() )
                fail = true;
        }catch(DataStoreError& e){
            log_error("STORE", "failed to store: " + std::string(e.what()),
                      p.first);
            fail = true;
        }
    }
    return !fail;
}


template<typename T>
inline std::string
to_int_string(T e, typename std::enable_if<std::is_enum<T>::value>::type *_ = 0)
{ return std::to_string( static_cast<int>(e) ); }


// BEING CALLED BY ANOTHER THREAD
void
session_callback( int callback_type,
                  int streamer_service_type,
                  unsigned long long time,
                  const char* data )
{
    using namespace std;
    using namespace tdma;

    using CEFTy = ChartEquitySubscription::FieldType;
    static const string K_CHART_TIME = to_int_string( CEFTy::chart_time );
    static const string K_OPEN = to_int_string( CEFTy::open_price );
    static const string K_HIGH = to_int_string( CEFTy::high_price );
    static const string K_LOW = to_int_string( CEFTy::low_price );
    static const string K_CLOSE = to_int_string( CEFTy::close_price );
    static const string K_VOLUME = to_int_string( CEFTy::volume );
    static const string K_SEQ = to_int_string( CEFTy::sequence );

    using TSEFTy = TimesaleEquitySubscription::FieldType;
    static const string K_TRADE_TIME = to_int_string( TSEFTy::trade_time );
    static const string K_LAST_PRICE = to_int_string( TSEFTy::last_price );
    static const string K_LAST_SIZE = to_int_string( TSEFTy::last_size );
    static const string K_LAST_SEQ = to_int_string( TSEFTy::last_sequence );

    static const string K_SYMBOL("key");

    auto cbty = static_cast<StreamingCallbackType>(callback_type);
    switch( cbty ){
    case StreamingCallbackType::listening_start:
        log_info("STREAMING", "started");
        return;
    case StreamingCallbackType::listening_stop:
        log_info("STREAMING", "stopped");
        return;
    case StreamingCallbackType::request_response:
        log_info("STREAMING", "request response", string(data));
        return;
    case StreamingCallbackType::error:
        log_error("STREAMING", "error", string(data));
        return;
    case StreamingCallbackType::timeout:
        log_error("STREAMING", "timeout");
        return;
    case StreamingCallbackType::notify:
        log_error("STREAMING", "notify", (data ? string(data) : "" ) );
        return;
    case StreamingCallbackType::data:
        break;
    }

    auto ssty = static_cast<StreamerServiceType>(streamer_service_type);
    if( !data ){
        log_error("STREAMING", "null data, service type", to_string(ssty));
        return;
    }

    json j = json::parse(string(data));
    unsigned long long t;

    StreamingData::QueuesGuard lock;
    if( !is_initialized )
        return;

    switch( ssty ){
    case StreamerServiceType::CHART_EQUITY:
        for( auto& elemj : j ){
            t = elemj[K_CHART_TIME];
            StreamingData::Enqueue(
                elemj[K_SYMBOL],
                t / MSEC_IN_MIN,
                elemj[K_OPEN],
                elemj[K_HIGH],
                elemj[K_LOW],
                elemj[K_CLOSE],
                elemj[K_VOLUME],
                elemj[K_SEQ]
            );
        }
        break;

    case StreamerServiceType::TIMESALE_EQUITY:
        for( auto& elemj : j ){
            t = elemj[K_TRADE_TIME];
            long long gap = StreamingData::UpdateActiveBar(
                elemj[K_SYMBOL],
                t / MSEC_IN_MIN,
                elemj[K_LAST_PRICE],
                elemj[K_LAST_SIZE],
                elemj[K_LAST_SEQ]
            );
            if( gap != 1 ){
                stringstream ss;
                string sym = elemj[K_SYMBOL];
                ss << "timesale sequence gap of " << gap << " for (" << sym
                   << ',' << t / MSEC_IN_MIN << ','
                   << static_cast<double>(elemj[K_LAST_PRICE]) << ','
                   << static_cast<unsigned long>(elemj[K_LAST_SIZE]) << ','
                   << static_cast<unsigned long>(elemj[K_LAST_SEQ]) << ')';
                StreamingData ab = StreamingData::GetActiveBar(sym);
                if( ab != StreamingData::null )
                    ss << " against active bar: " << ab;
                log_info("STREAMING", ss.str());
            }
        }
        break;

    default:
        log_error( "STREAMING", "Invalid Service Type: ", to_string(ssty) );
        return;
    }
}


json
get_historical_range( const std::string& symbol,
                   unsigned long long start_min,
                   unsigned long long end_min )
{
    assert( end_min >= start_min );

    json j;
    try{
        // TODO persist (at least on startup for new gap fills)
        tdma::HistoricalRangeGetter g( *credentials, symbol,
                                       tdma::FrequencyType::minute, 1,
                                       start_min * MSEC_IN_MIN,
                                       (end_min+1) * MSEC_IN_MIN, true );
        j = g.get();

    }catch( tdma::APIException& e ){
        log_error("GET-HIST-RANGE", "historical getter failed", e.what());
        return {};
    }

    auto jcandles_iter = j.find("candles");
    if( jcandles_iter == j.end() ){
        log_error("GET-HIST-RANGE", "bad json, no 'candles'", symbol);
        return {};
    }

    auto jsymbol_iter = j.find("symbol");
    if( jsymbol_iter == j.end() ){
        log_error("GET-HIST-RANGE", "bad json, no 'symbol'", symbol);
        return {};
    }

    if( symbol != *jsymbol_iter ){
        log_error("GET-HIST-RANGE", "bad json, wrong symbol", symbol);
        return {};
    }

    size_t n = jcandles_iter->size();
    if( n == 0 ){
        log_info("GET-HIST-RANGE", "no candles returned", symbol);
        return {};
    }

    unsigned long long dt = jcandles_iter->at(0)["datetime"];
    dt /= MSEC_IN_MIN;
    if( dt > start_min ){
        std::ostringstream ss;
        ss << "starts later than expected(" << dt << ',' << start_min << ')';
        log_info("GET-HIST-RANGE", ss.str(), symbol);
    }

    dt = jcandles_iter->at(n-1)["datetime"];
    dt /= MSEC_IN_MIN;
    if( dt < end_min ){
        std::ostringstream ss;
        ss << "ends earlier than expected(" << dt << ',' << end_min << ')';
        log_info("GET-HIST-RANGE", ss.str(), symbol);
    }

    return *jcandles_iter;
}


/*
 * For some reason, get_historical_range/HistoricalRangeGetter is returning data
 * before start, so we ignore if outside intended range
 *
 * This means we could be pulling a lot of data unnecessarily while filling
 * the gaps between the last Stop() and Start()
 *
 * When pulling older data we force at least 1 days worth so the issue is limited.
 */

template<bool ToFront>
inline void
update_with_empty_bars( unsigned long long start_min,
                        unsigned long long end_min,
                        SymbolData& sdata )
{
    if( ToFront )
        for( auto i = start_min; i <= end_min; ++i )
            sdata.emplace_front( i );
    else
        for( auto i = end_min; i >= start_min; --i )
            sdata.emplace_back( i );
}


bool
update_front_from_historical( unsigned long long start_min,
                              unsigned long long end_min,
                              SymbolData& sdata )
{
    json j = get_historical_range( sdata.symbol, start_min, end_min );
    if( j.empty() ){
        update_with_empty_bars<true>(start_min, end_min, sdata);
        return false;
    }

    long long last = start_min-1;

    // oldest first
    for( auto elemj : j ){
        unsigned long long dt = elemj["datetime"];
        dt /= MSEC_IN_MIN;
        if( dt < start_min )
            continue;
        else if( dt > end_min )
            break;
        else{
            assert( dt - last > 0 );
            update_with_empty_bars<true>( last + 1, dt - 1, sdata );

            sdata.emplace_front( dt, elemj["open"], elemj["high"], elemj["low"],
                              elemj["close"], elemj["volume"] );
            last = dt;
        }
    }

    // account for everything up to end (or all if we have no valid)
    assert( end_min - last >= 0 );
    update_with_empty_bars<true>( last + 1, end_min, sdata );
    return true;
}



// missing bars get replaced with NULLs
bool
update_back_from_historical( unsigned long long start_min,
                             unsigned long long end_min,
                             SymbolData& sdata )
{
    json j = get_historical_range( sdata.symbol, start_min, end_min );
    if( j.empty() ){
        update_with_empty_bars<false>(start_min, end_min, sdata);
        return false;
    }

    long long last = end_min + 1;

    // newest first
    for( auto iter = j.rbegin() ; iter != j.rend(); ++iter ){
        auto& elemj = *iter;
        unsigned long long dt = elemj["datetime"];
        dt /= MSEC_IN_MIN;
        if( dt > end_min )
            continue;
        else if(dt < start_min )
            break;
        else{
            assert( last - dt > 0 );
            update_with_empty_bars<false>(dt + 1, last - 1, sdata);

            sdata.emplace_back( dt, elemj["open"], elemj["high"], elemj["low"],
                                elemj["close"], elemj["volume"] );
            last = dt;
        }
    }

    // account for everything up to start (or all if we have no valid)
    assert( last - start_min >= 0 );
    update_with_empty_bars<false>( start_min, last - 1, sdata );
    return true;
}


void
handle_duplicate( SymbolData& sdata,
                  OHLCVData& d,
                  bool is_active,
                  long long gap )
{
    auto& D = *(sdata.data);
    assert( gap <= 0 );

    if( !is_active ){
        std::stringstream ss;
        ss << "replace TIMESALE bar " << D[-gap] << " with CHART bar " << d;
        log_info("UPDATE", ss.str(), sdata.symbol);
    }else if( d.volume <= D[-gap].volume  ){
        // if active bar w/ no new volume just ignore
        return;
    }

    D[-gap] = d;
}


void
handle_gap( SymbolData& sdata, OHLCVData& d, long long gap )
{
    std::stringstream ss;
    unsigned long long b = sdata.min_end + 1;
    unsigned long long e = d.min_since_epoch - 1;
    assert( e >= b );
    assert( gap == ((long long)e - (long long)b + 2) );

    /*
     * we only need to update the gap from historical on the
     * FIRST streaming bar, otherwise fill the gap with empties
     */
    if( !StreamingData::IsInitialized(sdata.symbol) ){
        ss << "update-front-from-historical";
        if( !update_front_from_historical( b, e, sdata ) )
            log_error( "UPDATE", ss.str() + " failed", sdata.symbol );
    }else{
        ss << "update-with-empty-bars";
        update_with_empty_bars<true>(b, e, sdata);
    }

    ss << " between " << b << " and " << e;
    log_info("UPDATE", ss.str(), sdata.symbol);
}


// TODO implement seq checking
bool
update_front_from_streaming( SymbolData& sdata,
                             std::queue<StreamingData>&& qdata )
{
    /*
     * Where all the magic happens...
     *
     * attempts to sync up the recently enqueued bars (full bars via
     * CHART_EQUITY sub; active bars via TIMESALE_EQUITY sub) with the past
     * bars inside SymbolData.data.
     *
     * There are three states queued (streaming) data can be in wrt SymbolData:
     *   1) same bar or older - see 'handle_duplicate' to REPLACE/IGNORE
     *   2) > 1 bar ahead - see 'handle_gap' to fill gap and PUSH FRONT
     *   3) 1 bar ahead - simply PUSH FRONT
     */

    // oldest first
    while( !qdata.empty() ){
        StreamingData& sd = qdata.front();
        OHLCVData& d = sd.data;

        // only need to fill in gaps if we already have some data
        if( sdata.min_end > 0 ){
            assert( !sdata.data->empty() );
            assert( sdata.min_start > 0 );

            long long gap = d.min_since_epoch - sdata.min_end;
            if( gap <= 0 ){  // REPLACE OR IGNORE
                handle_duplicate(sdata, d, (sd.seq < 0), gap);
                StreamingData::SetInitialized(sdata.symbol);
                // don't push to sdata
                qdata.pop();
                continue;
            }else if( gap > 1 ) // FILL
                handle_gap(sdata, d, gap);
         }

         StreamingData::SetInitialized(sdata.symbol);
         sdata.push_front(d);
         qdata.pop(); // do last, we use refs to it above
    }
    return true;
}


UpdateState
try_to_expand_start( SymbolData& sdata,
                     unsigned long long back, // min
                     std::function<bool(void)> have_enough )
{
    assert( back > 0 );
    unsigned long long start = sdata.min_start;

    if( back > start )
        back = start;

    int n = 0;
    for( ; n < NUPDATE_ATTEMPTS; ++n ){

        if( start < 1 ){
            log_error("TRY-EXPAND", "can't expand past epoch", sdata.symbol);
            return UpdateState::failed;
        }

        if( !update_back_from_historical(start-back, start-1, sdata) )
        {
            // this should fail-safe, filling the range with NULLs
            log_info("TRY-EXPAND", "update-back-from-hist failed", sdata.symbol);
        }

        if( have_enough() )
            return UpdateState::succeeded;

        if( back == start ){ // exhausted
            log_error("TRY-EXPAND", "exhausted early", sdata.symbol);
            ++n; // account for this attempt
            break;
        }

        log_info("TRY-EXPAND", "expand window for update back (hist)");
        log_info("TRY-EXPAND", "    from [" + std::to_string(start - back)
                                 + "-" + std::to_string(start) + "]");

        // in case update succeeded, but didn't get enough
        start = sdata.min_start;

        back *= UPDATE_EXPAND_FACTOR;
        if( back > start )
            back = start;

        log_info("TRY-EXPAND", "    to [" + std::to_string(start - back)
                                 + "-" + std::to_string(start) + "]");
    }

    log_error("TRY-EXPAND", "update back (hist) failed to get enough data "
              "after trying " + std::to_string(n) + " attempts." );

   return UpdateState::exhausted;
}


bool
control_session( const std::set<std::string> symbols, bool add_not_remove )
{
    using namespace tdma;

    if( symbols.empty() ){
        log_info("SESSION", "no symbols to subscribe/add to session");
        return true;
    }

    assert(session);
    std::deque<bool> ret;
    std::string cmd_str;

    try{
        sub_equity_chart.reset(
            new ChartEquitySubscription(symbols, EQUITY_CHART_SUB_FIELDS)
        );

        sub_equity_timesale.reset(
            new TimesaleEquitySubscription(symbols, EQUITY_TIMESALE_SUB_FIELDS)
        );

        if( session->is_active() ){
            if( add_not_remove ){
                sub_equity_chart->set_command( tdma::CommandType::ADD );
                sub_equity_timesale->set_command( tdma::CommandType::ADD );
                cmd_str = "ADD";
                ret = session->add_subscriptions(
                    {*sub_equity_chart, *sub_equity_timesale}
                );
            }else{
                sub_equity_chart->set_command( tdma::CommandType::UNSUBS );
                sub_equity_timesale->set_command( tdma::CommandType::UNSUBS );
                cmd_str = "UNSUBS";
                ret = session->add_subscriptions(
                    {*sub_equity_chart, *sub_equity_timesale}
                );
            }
        }else{
            if( add_not_remove ){
                cmd_str = "SUBS";
                ret = session->start( {*sub_equity_chart, *sub_equity_timesale});
            }else{
                log_info("SESSION", "can't remove from inactive session");
                return false;
            }
        }
    }catch(tdma::APIException& e){
        log_error("SESSION", "failed to update session(API ERROR)", e.what());
        return false;
    }

    assert( ret.size() == 2 );
    if( !ret[0] ){
        log_error("SESSION", "'" + cmd_str + "' failed", "CHART_EQUITY");
        return false;
    }else if( !ret[1] ){
        log_error("SESSION", "'" + cmd_str + "' failed", "TIMESALE_EQUITY");
        return false;
    }

    log_info("SESSION", "'" + cmd_str + "' succeeded");
    return true;
}


/*
std::pair<DataAccessor::const_iterator, DataAccessor::const_iterator>
range_between(const std::deque<OHLCVData>& sdata, long long start, long long end)
{
    // all checks done already, but just to be sure...
    assert( start >= 0 );
    assert( end >= 0 );
    assert( start <= end );

    typedef std::pair<unsigned long long, unsigned long long> ullp_ty;
    struct cmp{
        bool operator()( const OHLCVData& a, const ullp_ty& b )
        { return a.min_since_epoch > b.second; }
        bool operator()( const ullp_ty& a, const OHLCVData& b )
        { return b.min_since_epoch < a.first; }
    };

    ullp_ty p{start, end};
    /*
     * TODO: take advantage of contiguity for O(C) search
     *//*
    return std::equal_range( sdata.begin(), sdata.end(), p, cmp() );
}
*/


#define INIT_CHECK_AND_(tag, action) \
do{ \
    if( !is_initialized ){ \
        log_error(tag, "DataStore has not been initialized"); \
        action; \
    } \
}while(0)
#define INIT_CHECK_AND_RETURN(tag, r) INIT_CHECK_AND_(tag, return r)
#define INIT_CHECK_AND_THROW(tag) INIT_CHECK_AND_(tag, \
        throw std::logic_error("DataStore has not been initialized") )

#define LOG_AND_THROW_(tag, msg, msg2, exc) \
do{ \
    log_error(tag, msg, msg2); \
    throw exc(msg); \
}while(0)
#define THROW_BAD_ARG(tag, msg, msg2) \
    LOG_AND_THROW_(tag, msg, msg2, std::invalid_argument)
#define THROW_LOGIC_ERR(tag, msg, msg2) \
    LOG_AND_THROW_(tag, msg, msg2, std::logic_error)

const std::chrono::minutes NO_MINUTES(0);

#define MINUTE_CHECK_AND_THROW(m, tag, symbol) \
do{ \
    if( m < NO_MINUTES ){ \
        log_error(tag,"minutes < 0", symbol); \
        throw std::invalid_argument("minutes < 0"); \
    } \
}while(0)


SymbolData&
get_symbol_data_or_throw( const std::string& symbol )
{
    auto f = SymbolData::all.find(symbol);
    if( f == SymbolData::all.cend() )
        THROW_LOGIC_ERR("SYMBOL", "symbol data doesn't exist", symbol);
    return f->second;
}


}; /* namespace */



namespace ds {

using namespace std::chrono;

const OHLCVData OHLCVData::null;

bool
Initialize( const std::string& dir_path, Credentials& creds )
{
    if( is_initialized )
        return true;

    if( !BackingStore::directory_exists(dir_path) )
        return false;

    directory_path = dir_path;
    if( directory_path.back() != '/' )
        directory_path.push_back('/');

    log_file_path = directory_path + "log.log";
    if( !log_init(log_file_path) )
        log_info("INIT", "failed to open log file, using stdout/stderr");

    if( !is_valid_credentials(creds) )
        return false;

    credentials = &creds;

    backing_store.reset( new BackingStore(directory_path) );
    if( !backing_store->is_valid() ){
        backing_store.reset();
        log_error("INIT", "can't Initialize, failed to build backing store",
                  directory_path);
        return false;
    }

    for( auto& s : backing_store->get_symbols() ){
        SymbolData sdata(s);
        if( !sdata.load() || !sdata ){
            log_error("INIT", "can't Initialize, failed to load SymbolData", s);
            return false;
        }
        log_info("INIT", "Initialize successfully built symbol data", s);
        SymbolData::all.emplace( s, std::move(sdata) );
    }

    return (is_initialized = true);
}


bool
Start( milliseconds listening_timeout )
{
    INIT_CHECK_AND_RETURN("START", false);

    if( session )
        return true;

    if( listening_timeout  < tdma::StreamingSession::MIN_LISTENING_TIMEOUT ){
        log_error("START", "timeout too small, use MIN_LISTENING_TIMEOUT");
        listening_timeout  = tdma::StreamingSession::MIN_LISTENING_TIMEOUT;
    }
    log_info( "START","listening timeout",
              std::to_string(listening_timeout.count()) );

    // CREATE SESSION
    try{
        session = tdma::StreamingSession::Create(
            *credentials,
            session_callback,
            tdma::StreamingSession::DEF_CONNECT_TIMEOUT,
            listening_timeout
            );
    }catch(tdma::APIException& e){
        log_error("START", "failed to create streaming session",
                  e.what());
        session.reset();
        return false;
    }
    log_info("START", "successfully created streaming session");

    std::set<std::string> symbols;
    for( auto& p : SymbolData::all )
        symbols.insert(p.first);

    // STARTS IF WE HAVE SYMBOLS
    if( !control_session( symbols, true )){
        log_error("START", "failed to update session");
        session.reset();
        return false;
    }

    return true;
}


bool
Add( const std::string& symbol )
{
    INIT_CHECK_AND_RETURN("ADD-STORE", false);

    std::string s = toupper(symbol);
    if( SymbolData::all.count(s) ){
        log_info("ADD-STORE", "symbol already exists", s);
        return true;
    }

    if( !is_valid_symbol(s) ){
        log_error("ADD-STORE", "invalid symbol", s);
        return false;
    }

    if( !backing_store->add_symbol_store( s ) ){
        log_error("ADD-STORE", "failed to add to backing", s);
        return false;
    }

    SymbolData sdata(s);
    if( !sdata.load() ){
        log_error("ADD-STORE", "failed to load symbol data", s);
        return false;
    }
    log_info("ADD-STORE", "successfully built symbol data", s);
    SymbolData::all.emplace( s, std::move(sdata) );

    if( session ){
        auto old_symbols = sub_equity_chart->get_symbols();
        assert( old_symbols == sub_equity_timesale->get_symbols() );
        old_symbols.insert(s);
        if( !control_session( old_symbols, true )){
            log_error("ADD-STORE", "failed to update session");
            return false;
        }

        Update();
    }

    return true;
}


bool
Remove( const std::string& symbol, bool delete_file )
{
    INIT_CHECK_AND_RETURN("REMOVE-STORE", false);

    std::string s = toupper(symbol);
    auto f = SymbolData::all.find(s);
    if( f == SymbolData::all.cend() ){
        log_info("REMOVE-STORE", "symbol doesn't exist", s);
        return false;
    }

    if( IsRunning() ){
        if( !control_session( {s}, false) ){
            log_error("REMOVE-STORE", "failed to update session", s);
            return false;
        }
    }

    Update(); //one last update
    {
        StreamingData::QueuesGuard lock;
        StreamingData::RemoveQueue(s); // might not exist yet
    }

    bool ret = true;
    // TODO catch exc
    try{
        if( !f->second.store() ){
            log_error("REMOVE-STORE", "failed to store data", s);
            ret = false;
        }
    }catch(DataStoreError& e){
        log_error("REMOVE-STORE", "failed to store data: "
                                  + std::string(e.what()), s);
        ret = false;
    }

    if( SymbolData::all.erase(s) < 1 ){
        log_error("REMOVE", "failed to remove symbol data from collection", s);
        ret = false;
    }

    if( !backing_store->remove_symbol_store( s ) ){
        log_error("REMOVE-STORE", "failed to remove from backing store", s);
        ret = false;
    }

    if( delete_file && !backing_store->delete_symbol_store(s) ){
        log_error("REMOVE-STORE", "failed to delete backing store file", s);
        ret = false;
    }

    return ret;
}


void
Stop()
{
    if( !is_initialized )
        return;

    if( IsRunning() ){
        // stop streaming session
        try{
            session->stop();
        }catch(tdma::APIException& e){
            log_error("STOP", "failed to stop streaming session", e.what());
            return;
        }
        log_info("STOP", "successfully stopped streaming session");
    }

    Update();
}


void
Finalize()
{
    Stop();

    if( is_initialized && !store() )
        log_error("FINALIZE", "failed to store (ALL)");

    SymbolData::all.clear();

    is_initialized = false;

    StreamingData::QueuesGuard lock;
    StreamingData::ClearAll();
}


bool
IsInitialized()
{
    return is_initialized;
}


bool
IsRunning()
{
    if( session )
        return session->is_active();
    return false;
}


std::set<std::string>
GetSymbols()
{
    INIT_CHECK_AND_RETURN("GET-SYMBOLS", {});

    using S = SymbolData;
    std::set<std::string> s;
    std::transform( S::all.cbegin(), S::all.cend(), std::inserter(s, s.end()),
                    [](const S::all_ty::value_type& p){ return p.first; } );
    return s;
}


bool
Contains( const std::string& symbol )
{
    INIT_CHECK_AND_RETURN("CONTAINS", false);

    return SymbolData::all.count( toupper(symbol) ) > 0;
}


void
Update()
{
    if( !IsInitialized() )
        return;

    std::map<std::string, std::queue<StreamingData>> queue_copies;
    std::map<std::string, StreamingData> abar_copies;
    {
        StreamingData::QueuesGuard lock;
        queue_copies = StreamingData::GetQueueCopiesAndClear();
        abar_copies = StreamingData::GetActiveBarCopies();
    }
    /*
     * limit what we do under the lock, each 'update' call may
     * need network I/O and require blocking/throttling
     */
    std::set<std::string> actives;
    for( auto& p : queue_copies )
        actives.insert(p.first);
    for( auto& p : abar_copies )
        actives.insert(p.first);

    for( auto& s : actives ){
        auto iter_sd = SymbolData::all.find(s);
        if( iter_sd == SymbolData::all.cend() ){
            log_info("UPDATE", "symbol data no longer available", s);
            continue;
        }

        auto& Q = queue_copies[s]; // def constr if not already there

        auto iter_ab = abar_copies.find(s);
        if( iter_ab != abar_copies.cend() ){
            // if active bar newer than last CHART BAR enqueue
            if( Q.empty() ||
                iter_ab->second.data.min_since_epoch
                    > Q.front().data.min_since_epoch )
            {
                /*
                 * NOTE if timesale/active_bar we overwrite the seq to -1
                 * so the streaming handler can differentiate
                 */
               Q.emplace( iter_ab->second.data, -1 );
            }
        }

        if( !update_front_from_streaming(iter_sd->second, std::move(Q)) )
            log_error("UPDATE", "front-from-streaming failed", s);
        // p.second no longer valid
    }
    // qcopies no longer valid
}


/* *** DATA ACCESSOR *** */

DataAccessor::DataAccessor( const std::string& symbol )
    {
        INIT_CHECK_AND_THROW("DATA-ACCESS-CREATE");
        _set_symbol(symbol);
    }


void
DataAccessor::_set_symbol( const std::string& symbol )
{
    _symbol = toupper(symbol);

    if( SymbolData::all.count( _symbol ) < 1 )
        THROW_LOGIC_ERR("DATA-ACCESS-SET", "symbol not in store", _symbol);

    Update();
}


void
DataAccessor::set_symbol( const std::string& symbol )
{
    INIT_CHECK_AND_THROW("DATA-ACCESS-SET");
    _set_symbol(symbol);
}


std::string
DataAccessor::get_symbol() const
{
    return _symbol;
}


minutes
DataAccessor::start_minute() const
{
    INIT_CHECK_AND_THROW("GET-START-TIME");
    return _start_minute();
}


minutes
DataAccessor::end_minute() const
{
    INIT_CHECK_AND_THROW("GET-END-TIME");
    return _end_minute();
}


std::pair<minutes, minutes>
DataAccessor::start_end_minutes() const
{
    INIT_CHECK_AND_THROW("GET-START-END-TIME");

    auto& D = get_symbol_data_or_throw(_symbol);
    if( D.min_start > 0 ){
        assert( D.min_end > 0 );
        return {minutes(D.min_start), minutes(D.min_end)};
    }
    log_info("GET-START-END-TIME", "symbol doesn't have data yet", _symbol);
    return {ERROR_MINUTES, ERROR_MINUTES};
}


int
DataAccessor::start_index() const
{
    INIT_CHECK_AND_THROW("GET-START-INDX");

    auto& D = get_symbol_data_or_throw(_symbol);
    size_t n = D.data->size();
    if( n == 0 )
        log_info("GET-START-INDX", "symbol doesn't have data yet", _symbol);
    return n - 1;
}


int
DataAccessor::minutes_to_index( minutes min_since_epoch ) const
{
    INIT_CHECK_AND_THROW("MIN-TO-INDX");
    MINUTE_CHECK_AND_THROW(min_since_epoch, "MIN-TO-INDX", _symbol);

    auto& D = get_symbol_data_or_throw(_symbol);

    auto d = D.find_fast( min_since_epoch.count() );
    if( d == D.data->end() )
        return -1;

    return d - D.data->begin();
}


minutes
DataAccessor::index_to_minutes( unsigned int index ) const
{
    INIT_CHECK_AND_THROW("INDX-TO-MIN");

    auto& D= get_symbol_data_or_throw(_symbol);

    if( index >= D.data->size() )
        return ERROR_MINUTES;

    return minutes( (*(D.data))[index].min_since_epoch );
}


std::vector<OHLCVData>
DataAccessor::copy_all() const
{
    INIT_CHECK_AND_THROW("COPY-ALL");
    return ToSequence( _all() );
}


OHLCVData
DataAccessor::copy_at(minutes min_since_epoch) const
{
    INIT_CHECK_AND_THROW("COPY-AT-TIME");
    MINUTE_CHECK_AND_THROW(min_since_epoch, "COPY-AT-TIME", _symbol);
    return ToObject( _between(min_since_epoch, min_since_epoch) );
}


OHLCVData
DataAccessor::copy_at( unsigned int indx ) const
{
    INIT_CHECK_AND_THROW("COPY-AT-INDX");
    return ToObject( _between(indx,indx) );
}

OHLCVData
DataAccessor::operator[](minutes min_since_epoch) const
{
    INIT_CHECK_AND_THROW("OPERATOR[]-TIME");
    return ToObject( _between(min_since_epoch, min_since_epoch) );
}


OHLCVData
DataAccessor::operator[](unsigned int indx) const
{
    INIT_CHECK_AND_THROW("OPERATOR[]-INDX");
    return ToObject( _between(indx,indx) );
}


std::vector<OHLCVData>
DataAccessor::copy_between( minutes start_min_since_epoch,
                            minutes end_min_since_epoch ) const
{
    INIT_CHECK_AND_THROW("COPY-BETWEEN-TIME");
    MINUTE_CHECK_AND_THROW(start_min_since_epoch, "COPY-BETWEEN-TIME", _symbol);
    MINUTE_CHECK_AND_THROW(end_min_since_epoch, "COPY-BETWEEN-TIME", _symbol);
    return ToSequence( _between(start_min_since_epoch, end_min_since_epoch) );
}


std::vector<OHLCVData>
DataAccessor::copy_from(minutes start_min_since_epoch) const
{
    INIT_CHECK_AND_THROW("COPY-FROM");
    MINUTE_CHECK_AND_THROW(start_min_since_epoch, "COPY-FROM", _symbol);
    return ToSequence( _from(start_min_since_epoch) );
}


std::vector<OHLCVData>
DataAccessor::copy_between( unsigned int start_indx,
                            unsigned int end_indx ) const
{
    INIT_CHECK_AND_THROW("COPY-BETWEEN-INDX");
    return ToSequence( _between(start_indx, end_indx) );
}


DataAccessor::const_iterator
DataAccessor::cbegin() const // newest
{
    INIT_CHECK_AND_THROW("ITER-CBEGIN");
    return _cbegin();
}


DataAccessor::const_iterator
DataAccessor::cend() const // oldest + 1
{
    INIT_CHECK_AND_THROW("ITER-CEND");
    return _cend();
}


std::pair< DataAccessor::const_iterator, // newest
           DataAccessor::const_iterator> // oldest + 1
DataAccessor::at(minutes min_since_epoch) const
{
    INIT_CHECK_AND_THROW("ITER-AT-TIME");
    MINUTE_CHECK_AND_THROW(min_since_epoch, "ITER-AT-TIME", _symbol);
    return _between(min_since_epoch, min_since_epoch);
}


std::pair< DataAccessor::const_iterator, // newest
           DataAccessor::const_iterator> // oldest + 1
DataAccessor::at(unsigned int indx) const
{
    INIT_CHECK_AND_THROW("ITER-AT-INDX");
    return _between(indx, indx);
}


std::pair< DataAccessor::const_iterator, // newest
           DataAccessor::const_iterator> // oldest + 1
DataAccessor::between(minutes start_min_since_epoch,
                      minutes end_min_since_epoch) const
{
    INIT_CHECK_AND_THROW("ITER-BETWEEN-TIME");
    MINUTE_CHECK_AND_THROW(start_min_since_epoch, "ITER-BETWEEN_TIME", _symbol);
    MINUTE_CHECK_AND_THROW(end_min_since_epoch, "ITER-BETWEEN-TIME", _symbol);
    return _between(start_min_since_epoch, end_min_since_epoch);
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::between(unsigned int start_indx, unsigned int end_indx) const
{
    INIT_CHECK_AND_THROW("ITER-BETWEEN-INDX");
    return _between(start_indx, end_indx);
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::from(minutes start_min_since_epoch) const
{
    INIT_CHECK_AND_THROW("ITER-FROM");
    MINUTE_CHECK_AND_THROW(start_min_since_epoch, "ITER-FROM", _symbol);
    return _from(start_min_since_epoch);
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::all() const
{
    INIT_CHECK_AND_THROW("ITER-ALL");
    return _all();
}


minutes
DataAccessor::_start_minute() const
{
    auto& D = get_symbol_data_or_throw(_symbol);
    if( D.min_start == 0 ){
        log_info("GET-START-TIME", "symbol doesn't have data yet", _symbol);
        return ERROR_MINUTES;
    }
    return minutes(D.min_start);
}


minutes
DataAccessor::_end_minute() const
{
    auto& D = get_symbol_data_or_throw(_symbol);
    if( D.min_end == 0 ){
        log_info("GET-END-TIME", "symbol doesn't have data yet", _symbol);
        return ERROR_MINUTES;
    }
    return minutes(D.min_end);
}


DataAccessor::const_iterator
DataAccessor::_cbegin() const // newest
{
    return get_symbol_data_or_throw(_symbol).data->cbegin();
}


DataAccessor::const_iterator
DataAccessor::_cend() const // oldest + 1
{
    return get_symbol_data_or_throw(_symbol).data->cend();
}

std::pair< DataAccessor::const_iterator, // newest
           DataAccessor::const_iterator> // oldest + 1
DataAccessor::_between(minutes start_min_since_epoch,
                       minutes end_min_since_epoch) const
{
    if( start_min_since_epoch > end_min_since_epoch )
        THROW_BAD_ARG("BETWEEN-TIME", "start > end", _symbol);

    long long start = start_min_since_epoch.count();
    long long end = end_min_since_epoch.count();

    if( !fits_in_signed(start) )
        THROW_BAD_ARG("BETWEEN-TIME", "start > LONGLONG_MAX", _symbol);

    SymbolData& D = get_symbol_data_or_throw(_symbol);
    if( D.min_start == 0 ){
        assert( D.min_end == 0 );
        /* NOTE - if no data yet we DONT THROW */
        log_info("BETWEEN-TIME", "symbol doesn't have data yet", _symbol);
        auto e = D.data->cend();
        return {e, e};
    }

    long long end_offset = end - static_cast<long long>(D.min_end);
    long long start_offset = static_cast<long long>(D.min_start) - start;

    /* NOTE - if we need more recent, have to be running */
    if( end_offset >= 1 && !IsRunning() )
        THROW_LOGIC_ERR("BETWEEN-TIME", "not running (end)", _symbol);

    UpdateState ustate = UpdateState::succeeded;
    if( start_offset >= 1 ){
        /* NOTE - if we need older, have to be running */
        if( !IsRunning() )
            THROW_LOGIC_ERR("BETWEEN-TIME", "not running (start)", _symbol);

        ustate = try_to_expand_start(
                D,
                std::max( static_cast<unsigned long long>(start_offset),
                          UPDATE_MIN_BARS ),
                [&](){ return D.min_start <= start; } // ignore sign warn
            );
    }

    /*
     * NOTE - The following uses a constant-time indexing approach for finding
     *        the range. It requires contiguity in the data. If this becomes
     *        an issue go back to 'between_range' which uses log-time
     *        binary search.
     */
    long long sz = static_cast<long long>(D.data->size());
    long long front = bounded(D.front_offset(end), 0LL, sz);
    long long back = bounded(D.back_offset(start), 0LL, sz);
    auto tmp = std::make_pair(D.data->cbegin() + front, D.data->cend() - back);

    if( ustate != UpdateState::succeeded ){
        std::stringstream ss;
        ss << "returning reduced range: ";
        if( tmp.first == tmp.second )
            ss << "nothing";
        else
            ss <<" times [" << tmp.first->min_since_epoch  << ','
               << tmp.second->min_since_epoch << "]";
        log_info("BETWEEN-TIME", ss.str(), _symbol);
    }

    assert( tmp.first == tmp.second
            || ((long long)(tmp.first->min_since_epoch)
                    == (static_cast<long long>(D.min_end) - front)) );
    assert( tmp.first == tmp.second
            || ((long long)((tmp.second - 1)->min_since_epoch)
                    == (static_cast<long long>(D.min_start) + back)) );

    Update();

    return tmp;
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::_between(unsigned int start_indx, unsigned int end_indx) const
{
    if( start_indx < end_indx )
        THROW_BAD_ARG("BETWEEN-INDX", "start_indx < end_indx", _symbol);

    if( !fits_in_signed(start_indx) )
        THROW_BAD_ARG("BETWEEN-INDX", "start > INT_MAX", _symbol);

    /*
     * NOTE - since indices are relative and we want to insure '0' is
     *        the most recent we need to be running/streaming
     */
    if( !IsRunning() )
        THROW_LOGIC_ERR("BETWEEN-INDX", "can't get data, not running", _symbol);

    auto& D = get_symbol_data_or_throw(_symbol);
    if( D.min_start == 0 ){
        /* NOTE - if no data yet we DONT THROW */
        log_info("BETWEEN-INDX", "symbol doesn't have data yet", _symbol);
        auto e = D.data->cend();
        return {e, e};
    }

    long long sindx = static_cast<long long>(start_indx);
    auto have_enough = [&](){
        return (sindx - static_cast<long long>(D.data->size())) < 0;
    };

    UpdateState ustate = UpdateState::succeeded;
    if( !have_enough() ){
        long long needed = sindx - static_cast<long long>(D.data->size());
        ++needed;
        assert( needed > 0 );
        auto back = std::max( static_cast<unsigned long long>(needed),
                              UPDATE_MIN_BARS );
        ustate = try_to_expand_start(D, back, have_enough);
    }

    long long sz = static_cast<long long>(D.data->size());
    long long front = std::min(static_cast<long long>(end_indx), sz);
    long long back = std::max( sz - sindx - 1LL, 0LL );
    auto tmp = std::make_pair(D.data->begin() + front, D.data->end() - back);

    if( ustate != UpdateState::succeeded ){
        std::stringstream ss;
        ss << "returning reduced range: ";
        if( tmp.first == tmp.second )
            ss << "nothing";
        else
            ss <<" indices [" << front  << ',' << sz - back - 1LL << "]";
        log_info("BETWEEN-INDX", ss.str(), _symbol);
    }

    Update();
    return tmp;
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::_from(minutes start_min_since_epoch) const
{
    minutes end = _end_minute();
    if( end == ERROR_MINUTES ){
        log_error("FROM", "failed to get end minute", _symbol);
        auto e = _cend(); // *should* have already thrown if invalid
        return {e,e};
    }

    return _between( start_min_since_epoch, end );
}


std::pair<DataAccessor::const_iterator, // newest
          DataAccessor::const_iterator> // oldest + 1
DataAccessor::_all() const
{
    auto& D = get_symbol_data_or_throw(_symbol);
    auto p = std::make_pair(D.data->cbegin(), D.data->cend());
    Update();
    return p;
}


int
DataAccessor::ToMinuteOfHour( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    return gmtime(&sec)->tm_min;
}

int
DataAccessor::ToHourOfDay( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    return gmtime(&sec)->tm_hour;
}

int
DataAccessor::ToDayOfMonth( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    return gmtime(&sec)->tm_mday;
}

int
DataAccessor::ToMonthOfYear( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    return gmtime(&sec)->tm_mon + 1;
}

int
DataAccessor::ToYear( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    return gmtime(&sec)->tm_year + 1900;
}

std::tuple<int, int, int, int, int>
DataAccessor::ToYYMMDDhhmm( minutes min_since_epoch )
{
    time_t sec = min_since_epoch.count() * 60;
    tm *t = gmtime( &sec );
    return std::make_tuple(
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min
    );
}

std::string
DataAccessor::ToFormatString( minutes min_since_epoch,
                              const std::string& format_str )
{
    char buf[128];
    time_t sec = min_since_epoch.count() * 60;
    if( !std::strftime(buf, sizeof(buf), format_str.c_str(), gmtime(&sec) ) ){
        log_error("DATETIME", "formatted string conversion failed");
        return "";
    }
    return buf;
}

minutes
DataAccessor::BuildMinutesSinceEpoch( unsigned int year,
                                      unsigned int month,
                                      unsigned int day,
                                      unsigned int hour,
                                      unsigned int minute )
{
    if( year < 1970 ){
        log_error("DATETIME", "year < 1970");
        return ERROR_MINUTES;
    }
    if( month < 1 || month > 12 ){
        log_error("DATETIME", "month < 1 or month > 12");
        return ERROR_MINUTES;
    }
    if( day < 1 || day > 31 ){
        log_error("DATETIME", "day < 1 or day > 31");
        return ERROR_MINUTES;
    }
    if( hour > 59 ){
        log_error("DATETIME", "hour > 59");
        return ERROR_MINUTES;
    }
    if( minute > 59 ){
        log_error("DATETIME", "minute > 59");
        return ERROR_MINUTES;
    }

    std::tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    time_t tt = timegm(&t);
    if (tt < 0 )
        return ERROR_MINUTES;
    return minutes(tt / 60);
}


std::ostream&
operator<<(std::ostream& out, const OHLCVData& data)
{
    out << data.min_since_epoch << ' ' << data.open << ' ' << data.high
        << ' ' << data.low << ' ' << data.close << ' ' << data.volume;
    return out;
}

};
