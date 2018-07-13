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

#include <vector>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <cctype>
#include <string>

#include "../include/_tdma_api.h"

using namespace std;

namespace tdma {

const string URL_MARKETDATA = URL_BASE + "marketdata/";

void
data_api_on_error_callback(long code, const string& data)
{
    /*
     *  codes 500, 503, 401, 403, 404 handled by base callback
     */
    switch(code){
    case 400:
        throw InvalidRequest("bad/malformed request", code);
    case 406:
        throw InvalidRequest("invalid regex or excessive requests", code);
    };
}


class QuoteGetterImpl
        : public APIGetterImpl {
    std::string _symbol;

    void
    _build()
    {
        string url = URL_MARKETDATA + util::url_encode(_symbol) + "/quotes";
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    QuoteGetterImpl( Credentials& creds, const string& symbol )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbol(symbol)
        {
            if( symbol.empty() )
                throw ValueException("empty symbol string");

            _build();
        }

    std::string
    get_symbol() const
    { return _symbol; }

    void
    set_symbol(const string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol string");

        _symbol = symbol;
        build();
    }
};

} /* tdma */



int
QuoteGetter_Create_ABI( Credentials *pcreds,
                           const char* symbol,
                           QuoteGetter_C *pgetter,
                           int allow_exceptions )
{
    if( !pgetter ){
        if( allow_exceptions ){
            throw tdma::ValueException("pgetter can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    if( !pcreds || !symbol ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        if( allow_exceptions ){
            throw tdma::ValueException("pcreds/symbol can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    if( !pcreds->access_token | !pcreds->refresh_token | !pcreds->client_id ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        if( allow_exceptions ){
            throw tdma::LocalCredentialException("invalid Credentials struct");
        }
        return TDMA_API_CRED_ERROR;
    }

    static auto meth = +[](Credentials *c, const char* s){
        return new tdma::QuoteGetterImpl(*c, s);
    };

    tdma::QuoteGetterImpl *obj;
    int err;
    tie(obj, err) = tdma::CallImplFromABI(allow_exceptions, meth, pcreds, symbol);
    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    pgetter->type_id = tdma::QuoteGetter::TYPE_ID;
    return 0;
}

int
QuoteGetter_Destroy_ABI(QuoteGetter_C *pgetter, int allow_exceptions)
{
    if( !pgetter || !pgetter->obj ){
        if( allow_exceptions ){
            throw tdma::ValueException("pgetter can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    if( pgetter->type_id != tdma::QuoteGetter::TYPE_ID ){
        if( allow_exceptions ){
            throw tdma::TypeException("pgetter has invalid type id");
        }
        return TDMA_API_TYPE_ERROR;
    }

    static auto meth = +[](void* obj){
        delete reinterpret_cast<tdma::QuoteGetterImpl*>(obj);
    };

    return tdma::CallImplFromABI(allow_exceptions, meth, pgetter->obj);
}

int
QuoteGetter_GetSymbol_ABI( QuoteGetter_C *pgetter,
                              char **buf,
                              size_t *n,
                              int allow_exceptions)
{
    if( !pgetter || !pgetter->obj || !buf || !n ){
        if( allow_exceptions ){
            throw tdma::ValueException("pgetter/buffer/n can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    if( pgetter->type_id != tdma::QuoteGetter::TYPE_ID ){
        if( allow_exceptions ){
            throw tdma::TypeException("pgetter has invalid type id");
        }
        return TDMA_API_TYPE_ERROR;
    }

    static auto meth = +[](void* obj){
        return reinterpret_cast<tdma::QuoteGetterImpl*>(obj)->get_symbol();
    };

    string r;
    int err;
    tie(r,err) = tdma::CallImplFromABI(allow_exceptions, meth, pgetter->obj);
    if( err )
        return err;

    *n = r.size() + 1;
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !buf ){
        if( allow_exceptions ){
            throw tdma::MemoryError("failed to allocate buffer memory");
        }
        return TDMA_API_MEMORY_ERROR;
    }
    (*buf)[(*n)-1] = 0;
    strncpy(*buf, r.c_str(), (*n)-1);
    return 0;
}

int
QuoteGetter_SetSymbol_ABI( QuoteGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions )
{
    if( !pgetter || !pgetter->obj || !symbol ){
        if( allow_exceptions ){
            throw tdma::ValueException("pgetter/symbol can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    if( pgetter->type_id != tdma::QuoteGetter::TYPE_ID ){
        if( allow_exceptions ){
            throw tdma::TypeException("pgetter has invalid type id");
        }
        return TDMA_API_TYPE_ERROR;
    }

    static auto meth = +[](void* obj, const char* symbol){
        return reinterpret_cast<tdma::QuoteGetterImpl*>(obj)->set_symbol(symbol);
    };

    return tdma::CallImplFromABI(allow_exceptions, meth, pgetter->obj, symbol);
}

namespace tdma {



/*


QuotesGetter::QuotesGetter( Credentials& creds, const set<string>& symbols)
    :
        APIGetter(creds, data_api_on_error_callback),
        _symbols(symbols)
    {
        if( symbols.empty() )
            throw ValueException("no symbols");

        _build();
    }


void
QuotesGetter::_build()
{
    string qstr = util::build_encoded_query_str(
        {{"symbol", util::join(_symbols,',')}}
    );
    string url = URL_MARKETDATA + "/quotes?" + qstr;
    APIGetter::set_url(url);
}


virtual void
QuotesGetter::build()
{ _build(); }


void
QuotesGetter::set_symbols(const set<string>& symbols)
{
    if( symbols.empty() )
        throw ValueException("no symbols");

    _symbols = symbols;
    build();
}


MarketHoursGetter::MarketHoursGetter( Credentials& creds,
                                      MarketType market_type,
                                      const string& date )
    :
        APIGetter(creds, data_api_on_error_callback),
        _market_type(market_type),
        _date(date)
    {
        _build();
    }


void
MarketHoursGetter::_build()
{
    string qstr = util::build_encoded_query_str({{"date", _date}});
    string url = URL_MARKETDATA + util::url_encode(to_string(_market_type))
                 + "/hours?" + qstr;
    APIGetter::set_url(url);
}


virtual void
MarketHoursGetter::build()
{ _build(); }


void
MarketHoursGetter::set_date(const string& date)
{
    if( !util::is_valid_iso8601_datetime(date) ){
        throw ValueException("invalid ISO-8601 date/time: " + date);
    }
    _date = date;
    build();
}


void
MarketHoursGetter::set_market_type(MarketType market_type)
{
    _market_type = market_type;
    build();
}


MoversGetter::MoversGetter( Credentials& creds,
                            MoversIndex index,
                            MoversDirectionType direction_type,
                            MoversChangeType change_type )
    :
        APIGetter(creds, data_api_on_error_callback),
        _index(index),
        _direction_type(direction_type),
        _change_type(change_type)
    {
        _build();
    }


void
MoversGetter::_build()
{
    vector<pair<string,string>> params = {{"change", to_string(_change_type)}};

    if( _direction_type != MoversDirectionType::up_and_down ){
        params.emplace_back("direction", to_string(_direction_type));
    }

    string qstr = util::build_encoded_query_str(params);
    string url = URL_MARKETDATA + util::url_encode(to_string(_index))
                 + "/movers?" + qstr;
    APIGetter::set_url(url);
}


virtual void
MoversGetter::build()
{ _build(); }


void
MoversGetter::set_index(MoversIndex index)
{
    _index = index;
    build();
}


void
MoversGetter::set_direction_type(MoversDirectionType direction_type)
{
    _direction_type = direction_type;
    build();
}


void
MoversGetter::set_change_type(MoversChangeType change_type)
{
    _change_type = change_type;
    build();
}


HistoricalGetterBase::HistoricalGetterBase( Credentials& creds,
                                            const string& symbol,
                                            FrequencyType frequency_type,
                                            unsigned int frequency,
                                            bool extended_hours )
    :
        APIGetter(creds, data_api_on_error_callback),
        _symbol(symbol),
        _frequency_type(frequency_type),
        _frequency(frequency),
        _extended_hours(extended_hours)
    {
    }


vector<pair<string,string>>
HistoricalGetterBase::build_query_params() const
{
    return { {"frequencyType", to_string(_frequency_type)},
             {"frequency", to_string(_frequency)},
             {"needExtendedHoursData", _extended_hours ? "true" : "false"} };
}


void
HistoricalGetterBase::set_frequency(unsigned int frequency)
{
    auto valid_frequencies =
            VALID_FREQUENCIES_BY_FREQUENCY_TYPE.find(_frequency_type);
    assert( valid_frequencies != end(VALID_FREQUENCIES_BY_FREQUENCY_TYPE) );
    if( valid_frequencies->second.count(frequency) == 0 ){
        throw ValueException("invalid frequency");
    }
    _frequency = frequency;
}


void
HistoricalGetterBase::set_frequency_type(FrequencyType frequency_type)
{ _frequency_type = frequency_type; }


void
HistoricalGetterBase::set_symbol(const string& symbol)
{
    _symbol = symbol;
    build();
}

void
HistoricalGetterBase::set_extended_hours(bool extended_hours)
{
    _extended_hours = extended_hours;
    build();
}


HistoricalPeriodGetter::HistoricalPeriodGetter( Credentials& creds,
                                                const string& symbol,
                                                PeriodType period_type,
                                                unsigned int period,
                                                FrequencyType frequency_type,
                                                unsigned int frequency,
                                                bool extended_hours )
    :
        HistoricalGetterBase(creds, symbol, frequency_type, frequency,
                             extended_hours),
        _period_type(period_type),
        _period(period)
    {
        _build();
    }


void
HistoricalPeriodGetter::_build()
{
    vector<pair<string,string>> params( build_query_params() );
    params.emplace_back( "periodType", to_string(_period_type) );
    params.emplace_back( "period", to_string(_period) );

    string qstr = util::build_encoded_query_str(params);
    string url = URL_MARKETDATA + util::url_encode(get_symbol())
                 + "/pricehistory?" + qstr;
    APIGetter::set_url(url);
}


virtual void
HistoricalPeriodGetter::build()
{ _build(); }


void
HistoricalPeriodGetter::set_period(PeriodType period_type, unsigned int period)
{
    auto valid_periods = VALID_PERIODS_BY_PERIOD_TYPE.find(period_type);
    assert( valid_periods != end(VALID_PERIODS_BY_PERIOD_TYPE) );
    if( valid_periods->second.count(period) == 0 ){
        throw ValueException("invalid period (" + to_string(period) + ")");
    }
    _period_type = period_type;
    _period = period;
    build();
}


void
HistoricalPeriodGetter::set_frequency(FrequencyType frequency_type,
                                         unsigned int frequency)
{
    auto valid_freq_types = 
            VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.find(_period_type);
    assert( valid_freq_types != end(VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE) );
    if( valid_freq_types->second.count(frequency_type) == 0 ){
        throw ValueException("invalid frequency type");
    }
    HistoricalGetterBase::set_frequency_type(frequency_type);
     need to change frequency type first so we can pass check
    HistoricalGetterBase::set_frequency(frequency);
    build();
}


HistoricalRangeGetter::HistoricalRangeGetter(
        Credentials& creds,
        const string& symbol,
        FrequencyType frequency_type,
        unsigned int frequency,
        unsigned long long start_msec_since_epoch,
        unsigned long long end_msec_since_epoch,
        bool extended_hours )
    :
        HistoricalGetterBase(creds, symbol, frequency_type, frequency,
                             extended_hours),
        _start_msec_since_epoch(start_msec_since_epoch),
        _end_msec_since_epoch(end_msec_since_epoch)
    {
        _build();
    }


void
HistoricalRangeGetter::_build()
{
    vector<pair<string,string>> params( build_query_params() );
    params.emplace_back( "startDate", to_string(_start_msec_since_epoch) );
    params.emplace_back( "endDate", to_string(_end_msec_since_epoch) );

    string qstr = util::build_encoded_query_str(params);
    string url = URL_MARKETDATA + util::url_encode(get_symbol())
                 + "/pricehistory?" + qstr;
    APIGetter::set_url(url);
}


virtual void
HistoricalRangeGetter::build()
{ _build(); }


void
HistoricalRangeGetter::set_end_msec_since_epoch(
        unsigned long long end_msec_since_epoch
        )
{
    _end_msec_since_epoch = end_msec_since_epoch;
    build();
}


void
HistoricalRangeGetter::set_start_msec_since_epoch(
        unsigned long long start_msec_since_epoch
        )
{
    _start_msec_since_epoch = start_msec_since_epoch;
    build();
}


void
HistoricalRangeGetter::set_frequency( FrequencyType frequency_type,
                                      unsigned int frequency )
{
    HistoricalGetterBase::set_frequency_type(frequency_type);
     need to change frequency type first so we can pass check
    HistoricalGetterBase::set_frequency(frequency);
    build();
}


OptionChainGetter::OptionChainGetter( Credentials& creds,
                                      const string& symbol,
                                      const OptionStrikes& strikes,
                                      OptionContractType contract_type,
                                      bool include_quotes ,
                                      const string& from_date,
                                      const string& to_date,
                                      OptionExpMonth exp_month,
                                      OptionType option_type )
    :
        APIGetter(creds, data_api_on_error_callback),
        _symbol(symbol),
        _strikes(strikes),
        _contract_type(contract_type),
        _include_quotes(include_quotes),
        _from_date(from_date),
        _to_date(to_date),
        _exp_month(exp_month),
        _option_type(option_type)
    {
        if( !from_date.empty() && !util::is_valid_iso8601_datetime(from_date) )
           throw ValueException("invalid ISO-8601 date string: " + from_date);
        
        if( !to_date.empty() && !util::is_valid_iso8601_datetime(to_date) )
           throw ValueException("invalid ISO-8601 date string: " + to_date);
        
        _build();
    }


vector<pair<string,string>>
OptionChainGetter::build_query_params() const
{
    vector<pair<string,string>> params { {"symbol", _symbol} };

    switch( _strikes.get_type() ){
    case OptionStrikes::Type::n_atm:
        params.emplace_back("strikeCount", to_string(_strikes.get_n_atm()));
        break;
    case OptionStrikes::Type::single:
        params.emplace_back("strike", to_string(_strikes.get_single()));
        break;
    case OptionStrikes::Type::range:
        params.emplace_back("range", to_string(_strikes.get_range()));
    }

    if( _contract_type != OptionContractType::all )
        params.emplace_back("contractType", to_string(_contract_type));

    if( _include_quotes )
        params.emplace_back("includeQuotes", "TRUE");

    if( !_from_date.empty() )
        params.emplace_back("fromDate", _from_date);

    if( !_to_date.empty() )
        params.emplace_back("toDate", _to_date);

    if( _exp_month != OptionExpMonth::all )
        params.emplace_back("expMonth", to_string(_exp_month));

    if( _option_type != OptionType::all )
        params.emplace_back("optionType", to_string(_option_type));

    return params;
}


void
OptionChainGetter::_build()
{
    auto params = build_query_params();
    string qstr = util::build_encoded_query_str(params);
    string url = URL_MARKETDATA + "chains?" + qstr;
    APIGetter::set_url(url);
}


virtual void
OptionChainGetter::build()
{ _build(); }


void
OptionChainGetter::set_symbol(const string& symbol)
{
    _symbol = symbol;
    build();
}


void
OptionChainGetter::set_strikes(OptionStrikes strikes)
{
    _strikes = strikes;
    build();
}


void
OptionChainGetter::set_contract_type(OptionContractType contract_type)
{
    _contract_type = contract_type;
    build();
}


void
OptionChainGetter::include_quotes(bool includes_quotes)
{
    _include_quotes = includes_quotes;
    build();
}


void
OptionChainGetter::set_from_date(const string& from_date)
{
    if( !from_date.empty() && !util::is_valid_iso8601_datetime(from_date) )
       throw ValueException("invalid ISO-8601 date string: " + from_date);
    
    _from_date = from_date;
    build();
}


void
OptionChainGetter::set_to_date(const string& to_date)
{
    if( !to_date.empty() && !util::is_valid_iso8601_datetime(to_date) )
       throw ValueException("invalid ISO-8601 date string: " + to_date);
    
    _to_date = to_date;
    build();
}


void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month)
{
    _exp_month = exp_month;
    build();
}


void
OptionChainGetter::set_option_type(OptionType option_type)
{
    _option_type = option_type;
    build();
}


OptionChainStrategyGetter::OptionChainStrategyGetter(
        Credentials& creds,
        const string& symbol,
        OptionStrategy strategy,
        OptionStrikes strikes,
        OptionContractType contract_type,
        bool include_quotes,
        const string& from_date,
        const string& to_date,
        OptionExpMonth exp_month,
        OptionType option_type
        )
    :
        OptionChainGetter( creds, symbol, strikes, contract_type,
                           include_quotes, from_date, to_date, exp_month,
                           option_type ),
        _strategy(strategy)
    {
        _build();
    }


vector<pair<string,string>>
OptionChainStrategyGetter::build_query_params() const
{
    vector<pair<string,string>> params = OptionChainGetter::build_query_params();

    params.emplace_back("strategy", to_string(_strategy.get_strategy()));
    double intrvl = _strategy.get_spread_interval();
    if( intrvl >= 0.01 ){
        params.emplace_back("interval", to_string(intrvl));
    }
    return params;
}


void
OptionChainStrategyGetter::_build()
{
    string qstr = util::build_encoded_query_str( build_query_params() );
    string url = URL_MARKETDATA + "chains?" + qstr;
    APIGetter::set_url(url);
}


virtual void
OptionChainStrategyGetter::build()
{ _build(); }


void
OptionChainStrategyGetter::set_strategy(OptionStrategy strategy)
{
    _strategy = strategy;
    build();
}


OptionChainAnalyticalGetter::OptionChainAnalyticalGetter(
        Credentials& creds,
        const string& symbol,
        double volatility,
        double underlying_price,
        double interest_rate,
        unsigned int days_to_exp,
        OptionStrikes strikes,
        OptionContractType contract_type,
        bool include_quotes,
        const string& from_date,
        const string& to_date,
        OptionExpMonth exp_month,
        OptionType option_type
        )
    :
        OptionChainGetter( creds, symbol, strikes, contract_type, include_quotes,
                           from_date, to_date, exp_month, option_type ),
        _volatility(volatility),
        _underlying_price(underlying_price),
        _interest_rate(interest_rate),
        _days_to_exp(days_to_exp)
    {
        _build();
    }


vector<pair<string,string>>
OptionChainAnalyticalGetter::build_query_params() const
{
    vector<pair<string,string>> params =
            OptionChainGetter::build_query_params();
    vector<pair<string,string>> aparams{
        {"strategy", "ANALYTICAL"},
        {"volatility", to_string(_volatility)},
        {"underlyingPrice", to_string(_underlying_price)},
        {"interestRate", to_string(_interest_rate)},
        {"daysToExpiration", to_string(_days_to_exp)}
    };
    params.insert(params.end(), aparams.begin(), aparams.end());
    return params;
}


void
OptionChainAnalyticalGetter::_build()
{
    string qstr = util::build_encoded_query_str( build_query_params() );
    string url = URL_MARKETDATA + "chains?" + qstr;
    APIGetter::set_url(url);
}


virtual void
OptionChainAnalyticalGetter::build()
{ _build(); }


void
OptionChainAnalyticalGetter::set_volatility(double volatility)
{
    _volatility = volatility;
    build();
}


void
OptionChainAnalyticalGetter::set_underlying_price(double underlying_price)
{
    _underlying_price = underlying_price;
    build();
}


void
OptionChainAnalyticalGetter::set_interest_rate(double interest_rate)
{
    _interest_rate = interest_rate;
    build();
}


void
OptionChainAnalyticalGetter::set_days_to_exp(unsigned int days_to_exp)
{
    _days_to_exp = days_to_exp;
    build();
}


OptionStrikes
OptionStrikes::N_ATM(unsigned int n)
{
    if( n < 1 )
        throw ValueException("number of strikes can not be < 1");
    OptionStrikes o(Type::n_atm);
    o._n_atm = n;
    return o;
}


OptionStrikes
OptionStrikes::Single(double price)
{
    if( price < 0.1 )
        throw ValueException("strike price can not be < 0.1");
    OptionStrikes o(Type::single);
    o._single = price;
    return o;
}


OptionStrikes
OptionStrikes::Range(OptionRangeType range) 
{
    if( range == OptionRangeType::null )
        throw ValueException("strike range can not be 'null'");
    OptionStrikes o(Type::range);
    o._range = range;
    return o;
}


OptionStrategy::OptionStrategy(OptionStrategyType strategy)
    :
         _strategy(strategy),
         _spread_interval(0.0)
    {
    }


OptionStrategy::OptionStrategy(OptionStrategyType strategy,
                               double spread_interval)
    :
         _strategy(strategy),
         _spread_interval(spread_interval)
    {
        if(spread_interval < .01)
            throw ValueException( to_string(strategy)
                                  + " strategy requires spread interval >= .01");        
    }


*/

} /* tdma */
