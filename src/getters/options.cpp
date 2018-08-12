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

#include "../../include/_tdma_api.h"

using namespace std;

namespace tdma {

class OptionChainGetterImpl
        : public APIGetterImpl {
    std::string _symbol;
    OptionStrikes _strikes;
    OptionContractType _contract_type;
    bool _include_quotes;
    std::string _from_date;
    std::string _to_date;
    OptionExpMonth _exp_month;
    OptionType _option_type;

    void
    _build()
    {
        auto params = build_query_params();
        string qstr = util::build_encoded_query_str(params);
        string url = URL_MARKETDATA + "chains?" + qstr;
        APIGetterImpl::set_url(url);
    }


    virtual void
    build()
    { _build(); }

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const
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
            break;
        case OptionStrikes::Type::none:
            throw ValueException("invalid OptionStrikesType: 'none'");
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

public:
    typedef OptionChainGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_OPTION_CHAIN;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL;

    OptionChainGetterImpl( Credentials& creds,
                               const std::string& symbol,
                               const OptionStrikes& strikes,
                               OptionContractType contract_type = OptionContractType::all,
                               bool include_quotes = false,
                               const std::string& from_date = "",
                               const std::string& to_date = "",
                               OptionExpMonth exp_month = OptionExpMonth::all,
                               OptionType option_type = OptionType::all )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbol(symbol),
            _strikes(strikes),
            _contract_type(contract_type),
            _include_quotes(include_quotes),
            _from_date(from_date),
            _to_date(to_date),
            _exp_month(exp_month),
            _option_type(option_type)
        {
            if( symbol.empty() )
                throw ValueException( "empty symbol" );

            if( !from_date.empty() && !util::is_valid_iso8601_datetime(from_date) )
               throw ValueException("invalid ISO-8601 date string: " + from_date);

            if( !to_date.empty() && !util::is_valid_iso8601_datetime(to_date) )
               throw ValueException("invalid ISO-8601 date string: " + to_date);

            _build();
        }

    std::string
    get_symbol() const
    { return _symbol; }

    OptionStrikes
    get_strikes() const
    { return _strikes; }

    OptionContractType
    get_contract_type() const
    { return _contract_type;}

    bool
    includes_quotes() const
    { return _include_quotes; }

    std::string
    get_from_date() const
    { return _from_date; }

    std::string
    get_to_date() const
    { return _to_date; }

    OptionExpMonth
    get_exp_month() const
    { return _exp_month; }

    OptionType
    get_option_type() const
    { return _option_type; }

    void
    set_symbol(const std::string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol");
        _symbol = symbol;
        build();
    }

    void
    set_strikes(OptionStrikes strikes)
    {
        _strikes = strikes;
        build();
    }

    void
    set_contract_type(OptionContractType contract_type)
    {
        _contract_type = contract_type;
        build();
    }

    void
    include_quotes(bool includes_quotes)
    {
        _include_quotes = includes_quotes;
        build();
    }

    void
    set_from_date(const std::string& from_date)
    {
        if( !from_date.empty() && !util::is_valid_iso8601_datetime(from_date) )
           throw ValueException("invalid ISO-8601 date string: " + from_date);

        _from_date = from_date;
        build();
    }

    void
    set_to_date(const std::string& to_date)
    {
        if( !to_date.empty() && !util::is_valid_iso8601_datetime(to_date) )
           throw ValueException("invalid ISO-8601 date string: " + to_date);

        _to_date = to_date;
        build();
    }

    void
    set_exp_month(OptionExpMonth exp_month)
    {
        _exp_month = exp_month;
        build();
    }

    void
    set_option_type(OptionType option_type)
    {
        _option_type = option_type;
        build();
    }
};


class OptionChainStrategyGetterImpl
        : public OptionChainGetterImpl {
    OptionStrategy _strategy;

    void
    _build()
    {
        string qstr = util::build_encoded_query_str( build_query_params() );
        string url = URL_MARKETDATA + "chains?" + qstr;
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const
    {
        vector<pair<string,string>> params =
            OptionChainGetterImpl::build_query_params();

        params.emplace_back("strategy", to_string(_strategy.get_strategy()));
        double intrvl = _strategy.get_spread_interval();
        if( intrvl >= 0.01 ){
            params.emplace_back("interval", to_string(intrvl));
        }
        return params;
    }

public:
    typedef OptionChainStrategyGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY;

    OptionChainStrategyGetterImpl(
            Credentials& creds,
            const std::string& symbol,
            OptionStrategy strategy,
            OptionStrikes strikes,
            OptionContractType contract_type = OptionContractType::all,
            bool include_quotes = false,
            const std::string& from_date = "",
            const std::string& to_date = "",
            OptionExpMonth exp_month = OptionExpMonth::all,
            OptionType option_type = OptionType::all )
        :
             OptionChainGetterImpl( creds, symbol, strikes, contract_type,
                                    include_quotes, from_date, to_date,
                                    exp_month, option_type ),
             _strategy(strategy)
        {
             _build();
        }

    OptionStrategy
    get_strategy() const
    { return _strategy; }

    void
    set_strategy(OptionStrategy strategy)
    {
        _strategy = strategy;
        build();
    }
};



class OptionChainAnalyticalGetterImpl
        : public OptionChainGetterImpl {
    double _volatility;
    double _underlying_price;
    double _interest_rate;
    unsigned int _days_to_exp;

    void
    _build()
    {
        string qstr = util::build_encoded_query_str( build_query_params() );
        string url = URL_MARKETDATA + "chains?" + qstr;
        APIGetterImpl::set_url(url);
    }

    /*virtual*/void
    build()
    { _build(); }

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const
    {
        vector<pair<string,string>> params =
                OptionChainGetterImpl::build_query_params();

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

public:
    typedef OptionChainAnalyticalGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL;

    OptionChainAnalyticalGetterImpl(
            Credentials& creds,
            const std::string& symbol,
            double volatility,
            double underlying_price,
            double interest_rate,
            unsigned int days_to_exp,
            OptionStrikes strikes,
            OptionContractType contract_type = OptionContractType::all,
            bool include_quotes = false,
            const std::string& from_date = "",
            const std::string& to_date = "",
            OptionExpMonth exp_month = OptionExpMonth::all,
            OptionType option_type = OptionType::all )
        :
            OptionChainGetterImpl( creds, symbol, strikes, contract_type,
                                   include_quotes, from_date, to_date, exp_month,
                                   option_type ),
            _volatility(volatility),
            _underlying_price(underlying_price),
            _interest_rate(interest_rate),
            _days_to_exp(days_to_exp)
        {
            _build();
        }

    double
    get_volatility() const
    { return _volatility; }

    double
    get_underlying_price() const
    { return _underlying_price; }

    double
    get_interest_rate() const
    { return _interest_rate; }

    unsigned int
    get_days_to_exp() const
    { return _days_to_exp; }

    void
    set_volatility(double volatility)
    {
        _volatility = volatility;
        build();
    }

    void
    set_underlying_price(double underlying_price)
    {
        _underlying_price = underlying_price;
        build();
    }

    void
    set_interest_rate(double interest_rate)
    {
        _interest_rate = interest_rate;
        build();
    }

    void
    set_days_to_exp(unsigned int days_to_exp)
    {
        _days_to_exp = days_to_exp;
        build();
    }
};

} /* tdma */

using namespace tdma;

int
OptionChainGetter_Create_ABI( struct Credentials *pcreds,
                                  const char* symbol,
                                  int strikes_type,
                                  OptionStrikesValue strikes_value,
                                  int contract_type,
                                  int include_quotes,
                                  const char* from_date,
                                  const char* to_date,
                                  int exp_month,
                                  int option_type,
                                  OptionChainGetter_C *pgetter,
                                  int allow_exceptions )
{
    using ImplTy = OptionChainGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY( OptionStrikesType, strikes_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionContractType, contract_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionType, option_type, allow_exceptions,
                               pgetter );
    CHECK_ENUM_KILL_PROXY( OptionExpMonth, exp_month, allow_exceptions,
                               pgetter );
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, pgetter);

    // null from_date/to_date -> empty string

    static auto meth =
        +[]( Credentials *c, const char* s, int st, OptionStrikesValue sv,
             int ct, int iq, const char* fd, const char* td, int em, int ot){

        OptionStrikes os(static_cast<OptionStrikesType>(st), sv);
        return new ImplTy( *c, s, os, static_cast<OptionContractType>(ct),
                           static_cast<bool>(iq), (fd ? fd : ""),
                           (td ? td : ""), static_cast<OptionExpMonth>(em),
                           static_cast<OptionType>(ot) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds, symbol,
                                     strikes_type, strikes_value, contract_type,
                                     include_quotes, from_date, to_date,
                                     exp_month, option_type );
    if( err ){
        kill_proxy(pgetter);
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    pgetter->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}

int
OptionChainGetter_Destroy_ABI( OptionChainGetter_C *pgetter,
                                  int allow_exceptions )
{ return destroy_proxy<OptionChainGetterImpl>(pgetter, allow_exceptions); }

int
OptionChainGetter_GetSymbol_ABI( OptionChainGetter_C *pgetter,
                                      char **buf,
                                      size_t *n,
                                      int allow_exceptions)
{
    return GetterImplAccessor<char**>::template
        get<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::get_symbol, buf, n, allow_exceptions
        );
}

int
OptionChainGetter_SetSymbol_ABI( OptionChainGetter_C *pgetter,
                                      const char *symbol,
                                      int allow_exceptions )
{
    return GetterImplAccessor<char**>::template
        set<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::set_symbol, symbol, allow_exceptions
        );
}

int
OptionChainGetter_GetStrikes_ABI( OptionChainGetter_C *pgetter,
                                          int *strikes_type,
                                          OptionStrikesValue *strikes_value,
                                          int allow_exceptions)
{
    int err = proxy_is_callable<OptionChainGetterImpl>(
        pgetter, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(strikes_type, "strikes_type", allow_exceptions);
    CHECK_PTR(strikes_value, "strikes_value", allow_exceptions);

    static auto mwrap = +[](void* obj){
        return reinterpret_cast<OptionChainGetterImpl*>(obj)->get_strikes();
    };

    OptionStrikes os;
    std::tie(os, err) = CallImplFromABI(allow_exceptions, mwrap, pgetter->obj);
    if( err )
        return err;

    *strikes_type = static_cast<int>(os.get_type());
    *strikes_value = os.get_value();
    return 0;
}

int
OptionChainGetter_SetStrikes_ABI( OptionChainGetter_C *pgetter,
                                          int strikes_type,
                                          OptionStrikesValue strikes_value,
                                          int allow_exceptions)
{
    int err = proxy_is_callable<OptionChainGetterImpl>(
        pgetter, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM(OptionStrikesType, strikes_type, allow_exceptions);

    static auto mwrap = +[](void* obj, int st, OptionStrikesValue sv){
        OptionStrikes os(static_cast<OptionStrikesType>(st), sv);
        return reinterpret_cast<OptionChainGetterImpl*>(obj)->set_strikes(os);
    };

    return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, strikes_type,
                           strikes_value);
}

int
OptionChainGetter_GetContractType_ABI( OptionChainGetter_C *pgetter,
                                           int *contract_type,
                                           int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        get<OptionChainGetterImpl, OptionContractType>(
            pgetter, &OptionChainGetterImpl::get_contract_type,
            contract_type, "contract_type", allow_exceptions
        );
}

int
OptionChainGetter_SetContractType_ABI( OptionChainGetter_C *pgetter,
                                            int contract_type,
                                           int allow_exceptions)
{
    CHECK_ENUM(OptionContractType, contract_type, allow_exceptions);

    return GetterImplAccessor<int>::template
        set<OptionChainGetterImpl, OptionContractType>(
            pgetter, &OptionChainGetterImpl::set_contract_type,
            contract_type, allow_exceptions
        );
}

int
OptionChainGetter_IncludesQuotes_ABI( OptionChainGetter_C *pgetter,
                                           int *includes_quotes,
                                           int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        get<OptionChainGetterImpl, bool>(
            pgetter, &OptionChainGetterImpl::includes_quotes,
            includes_quotes, "includes_quotes", allow_exceptions
        );
}

int
OptionChainGetter_IncludeQuotes_ABI( OptionChainGetter_C *pgetter,
                                          int include_quotes,
                                          int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        set<OptionChainGetterImpl, bool>(
            pgetter, &OptionChainGetterImpl::include_quotes,
            include_quotes, allow_exceptions
        );
}

int
OptionChainGetter_GetFromDate_ABI( OptionChainGetter_C *pgetter,
                                      char **buf,
                                      size_t *n,
                                      int allow_exceptions)
{
    return GetterImplAccessor<char**>::template
        get<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::get_from_date, buf, n,
            allow_exceptions
        );
}

int
OptionChainGetter_SetFromDate_ABI( OptionChainGetter_C *pgetter,
                                      const char *date,
                                      int allow_exceptions )
{
    return GetterImplAccessor<char**>::template
        set<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::set_from_date,
            date, allow_exceptions
        );
}

int
OptionChainGetter_GetToDate_ABI( OptionChainGetter_C *pgetter,
                                      char **buf,
                                      size_t *n,
                                      int allow_exceptions)
{
    return GetterImplAccessor<char**>::template
        get<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::get_to_date, buf, n,
            allow_exceptions
        );
}


int
OptionChainGetter_SetToDate_ABI( OptionChainGetter_C *pgetter,
                                      const char *date,
                                      int allow_exceptions )
{
    return GetterImplAccessor<char**>::template
        set<OptionChainGetterImpl>(
            pgetter, &OptionChainGetterImpl::set_to_date,
            date, allow_exceptions
        );
}

int
OptionChainGetter_GetExpMonth_ABI( OptionChainGetter_C *pgetter,
                                           int *exp_month,
                                           int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        get<OptionChainGetterImpl, OptionExpMonth>(
            pgetter, &OptionChainGetterImpl::get_exp_month,
            exp_month, "exp_month", allow_exceptions
        );
}

int
OptionChainGetter_SetExpMonth_ABI( OptionChainGetter_C *pgetter,
                                           int exp_month,
                                           int allow_exceptions)
{
    CHECK_ENUM(OptionExpMonth, exp_month, allow_exceptions);

    return GetterImplAccessor<int>::template
        set<OptionChainGetterImpl, OptionExpMonth>(
            pgetter, &OptionChainGetterImpl::set_exp_month,
            exp_month, allow_exceptions
        );
}

int
OptionChainGetter_GetOptionType_ABI( OptionChainGetter_C *pgetter,
                                           int *option_type,
                                           int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        get<OptionChainGetterImpl, OptionType>(
            pgetter, &OptionChainGetterImpl::get_option_type,
            option_type, "option_type", allow_exceptions
        );
}

int
OptionChainGetter_SetOptionType_ABI( OptionChainGetter_C *pgetter,
                                           int option_type,
                                           int allow_exceptions)
{
    CHECK_ENUM(OptionType, option_type, allow_exceptions);

    return GetterImplAccessor<int>::template
        set<OptionChainGetterImpl, OptionType>(
            pgetter, &OptionChainGetterImpl::set_option_type,
            option_type, allow_exceptions
        );
}


int
OptionChainStrategyGetter_Create_ABI( struct Credentials *pcreds,
                                          const char* symbol,
                                          int strategy_type,
                                          double spread_interval,
                                          int strikes_type,
                                          OptionStrikesValue strikes_value,
                                          int contract_type,
                                          int include_quotes,
                                          const char* from_date,
                                          const char* to_date,
                                          int exp_month,
                                          int option_type,
                                          OptionChainStrategyGetter_C *pgetter,
                                          int allow_exceptions )
{
    using ImplTy = OptionChainStrategyGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY( OptionStrategyType, strategy_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionStrikesType, strikes_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionContractType, contract_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionType, option_type, allow_exceptions,
                               pgetter );
    CHECK_ENUM_KILL_PROXY( OptionExpMonth, exp_month, allow_exceptions,
                               pgetter );
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, pgetter);

    static auto meth =
        +[]( Credentials *c, const char* s, int strat, double sprd, int st,
             OptionStrikesValue sv, int ct, int iq, const char* fd,
             const char* td, int em, int ot){

        OptionStrategy ostrat(static_cast<OptionStrategyType>(strat), sprd);
        OptionStrikes os(static_cast<OptionStrikesType>(st), sv);

        return new ImplTy(*c, s, ostrat, os, static_cast<OptionContractType>(ct),
                          static_cast<bool>(iq), (fd ? fd : ""), (td ? td : ""),
                          static_cast<OptionExpMonth>(em),
                          static_cast<OptionType>(ot) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds, symbol,
                                     strategy_type, spread_interval, strikes_type,
                                     strikes_value, contract_type, include_quotes,
                                     from_date, to_date, exp_month, option_type );
    if( err ){
        kill_proxy(pgetter);
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    pgetter->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}

int
OptionChainStrategyGetter_Destroy_ABI( OptionChainStrategyGetter_C *pgetter,
                                            int allow_exceptions )
{
    return destroy_proxy<OptionChainStrategyGetterImpl>(
        pgetter, allow_exceptions
        );
}

int
OptionChainStrategyGetter_GetStrategy_ABI(
    OptionChainStrategyGetter_C *pgetter,
    int *strategy_type,
    double *spread_interval,
    int allow_exceptions
    )
{
    int err = proxy_is_callable<OptionChainStrategyGetterImpl>(
        pgetter, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(strategy_type, "strategy_type", allow_exceptions);
    CHECK_PTR(spread_interval, "spread_interval", allow_exceptions);

    static auto mwrap = +[](void* obj){
        return reinterpret_cast<OptionChainStrategyGetterImpl*>(obj)
            ->get_strategy();
    };

    OptionStrategy os;
    std::tie(os, err) = CallImplFromABI(allow_exceptions, mwrap, pgetter->obj);
    if( err )
        return err;

    *strategy_type = static_cast<int>(os.get_strategy());
    *spread_interval = os.get_spread_interval();
    return 0;
}

int
OptionChainStrategyGetter_SetStrategy_ABI(
    OptionChainStrategyGetter_C *pgetter,
    int strategy_type,
    double spread_interval,
    int allow_exceptions
    )
{
    int err = proxy_is_callable<OptionChainStrategyGetterImpl>(
        pgetter, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM(OptionStrategyType, strategy_type, allow_exceptions);

    static auto mwrap = +[](void* obj, int st, double si){
        OptionStrategy os(static_cast<OptionStrategyType>(st), si);
        return reinterpret_cast<OptionChainStrategyGetterImpl*>(obj)
            ->set_strategy(os);
    };

    return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, strategy_type,
                           spread_interval);
}

/* OptionChainAnalyticalGetter */
int
OptionChainAnalyticalGetter_Create_ABI( struct Credentials *pcreds,
                                          const char* symbol,
                                          double volatility,
                                          double underlying_price,
                                          double interest_rate,
                                          unsigned int days_to_exp,
                                          int strikes_type,
                                          OptionStrikesValue strikes_value,
                                          int contract_type,
                                          int include_quotes,
                                          const char* from_date,
                                          const char* to_date,
                                          int exp_month,
                                          int option_type,
                                          OptionChainAnalyticalGetter_C *pgetter,
                                          int allow_exceptions )
{
    using ImplTy = OptionChainAnalyticalGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY( OptionStrikesType, strikes_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionContractType, contract_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( OptionType, option_type, allow_exceptions,
                               pgetter );
    CHECK_ENUM_KILL_PROXY( OptionExpMonth, exp_month, allow_exceptions,
                               pgetter );
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, pgetter);

    static auto meth =
        +[]( Credentials *c, const char* s, double vol, double up, double ir,
             unsigned int exp, int st, OptionStrikesValue sv, int ct, int iq,
             const char* fd, const char* td, int em, int ot){

        OptionStrikes os(static_cast<OptionStrikesType>(st), sv);
        return new ImplTy( *c, s, vol, up, ir, exp, os,
                           static_cast<OptionContractType>(ct),
                           static_cast<bool>(iq), (fd ? fd : ""), (td ? td : ""),
                           static_cast<OptionExpMonth>(em),
                           static_cast<OptionType>(ot) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds, symbol,
                                     volatility, underlying_price, interest_rate,
                                     days_to_exp, strikes_type,strikes_value,
                                     contract_type, include_quotes, from_date,
                                     to_date, exp_month, option_type );
    if( err ){
        kill_proxy(pgetter);
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    pgetter->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}

int
OptionChainAnalyticalGetter_Destroy_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    int allow_exceptions
    )
{
    return destroy_proxy<OptionChainAnalyticalGetterImpl>(
        pgetter, allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_GetVolatility_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *volatility,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        get<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::get_volatility,
            volatility, "volatility", allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_SetVolatility_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double volatility,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        set<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::set_volatility,
            volatility, allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *underlying_price,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        get<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::get_underlying_price,
            underlying_price, "underlying_price", allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double underlying_price,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        set<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::set_underlying_price,
            underlying_price, allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_GetInterestRate_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *interest_rate,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        get<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::get_interest_rate,
            interest_rate, "interest_rate", allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_SetInterestRate_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double interest_rate,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        set<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::set_interest_rate,
            interest_rate, allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_GetDaysToExp_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int *days_to_exp,
    int allow_exceptions
    )
{
    return GetterImplAccessor<unsigned int>::template
        get<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::get_days_to_exp,
            days_to_exp, "days_to_exp", allow_exceptions
        );
}

int
OptionChainAnalyticalGetter_SetDaysToExp_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int days_to_exp,
    int allow_exceptions
    )
{
    return GetterImplAccessor<double>::template
        set<OptionChainAnalyticalGetterImpl>(
            pgetter, &OptionChainAnalyticalGetterImpl::set_days_to_exp,
            days_to_exp, allow_exceptions
        );
}



