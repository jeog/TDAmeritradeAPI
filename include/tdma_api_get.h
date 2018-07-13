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

#ifndef TDMA_API_GET_H
#define TDMA_API_GET_H

#include "_common.h"
#include "tdma_common.h"

#ifdef __cplusplus

#include <set>
#include <unordered_map>
#include <iostream>

#include "json.hpp"
//#include "curl_connect.h"


using json = nlohmann::json;

namespace tdma{

enum class PeriodType : unsigned int {
    day,
    month,
    year,
    ytd
};

enum class FrequencyType : unsigned int {
    minute,
    daily,
    weekly,
    monthly
};

enum class OptionContractType : unsigned int {
    call,
    put,
    all
};

enum class OptionStrategyType : unsigned int {
    // single,  /* this has its own method */
    // analytical, /* this shouldn't be used directly (has own method) */
    covered,
    vertical,
    calendar,
    strangle,
    straddle,
    butterfly,
    condor,
    diagonal,
    collar,
    roll
};

enum class OptionRangeType : unsigned int {
    null, /* this shouldn't be used directly  */
    itm, /* in-the-money */
    ntm, /* near-the-money */
    otm, /* out-of-the-monety */
    sak, /* strikes-above-market */
    sbk, /* strikes-below-market */
    snk, /* strikes-near-market */
    all
};

enum class OptionExpMonth : unsigned int {
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dec,
    all
};

enum class OptionType : unsigned int {
    s, /* standard */
    ns, /* non-standard */
    all
};

enum class TransactionType : unsigned int {
    all,
    trade,
    buy_only,
    sell_only,
    cash_in_or_cash_out,
    checking,
    dividend,
    interest,
    other,
    advisor_fees
};

enum class InstrumentSearchType : unsigned int{
    symbol_exact,
    symbol_search,
    symbol_regex,
    description_search,
    description_regex,
    cusip
};

enum class MarketType : unsigned int{
    equity,
    option,
    future,
    bond,
    forex
};

enum class MoversIndex : unsigned int{
    compx, /* $COMPX */
    dji, /* $DJI */
    spx /* SPX */
};

enum class MoversDirectionType : unsigned int{
    up,
    down,
    up_and_down
};

enum class MoversChangeType : unsigned int{
    value,
    percent
};


DLL_SPEC_ std::string
to_string(const PeriodType& ptype);

DLL_SPEC_ std::string
to_string(const FrequencyType& ftype);

DLL_SPEC_ std::string
to_string(const OptionContractType& contract_type);

DLL_SPEC_ std::string
to_string(const OptionStrategyType& strategy);

DLL_SPEC_ std::string
to_string(const OptionRangeType& range);

DLL_SPEC_ std::string
to_string(const OptionExpMonth& exp_month);

DLL_SPEC_ std::string
to_string(const OptionType& option_type);

DLL_SPEC_ std::string
to_string(const TransactionType& transaction_type);

DLL_SPEC_ std::string
to_string(const InstrumentSearchType& search_type);

DLL_SPEC_ std::string
to_string(const MarketType& market_type);

DLL_SPEC_ std::string
to_string(const MoversIndex& index);

DLL_SPEC_ std::string
to_string(const MoversDirectionType& direction_type);

DLL_SPEC_ std::string
to_string(const MoversChangeType& change_type);

using std::to_string;


DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const PeriodType& period_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const FrequencyType& frequency_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const OptionContractType& contract_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const OptionStrategyType& strategy);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const OptionRangeType& range);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const OptionExpMonth& exp_month);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const OptionType& option_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const TransactionType& transaction_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const InstrumentSearchType& search_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const MarketType& market_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out,const MoversIndex& index);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out,const MoversDirectionType& direction_type);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out,const MoversChangeType& change_type);


template<typename E>
struct EnumTypeAssert{
    static_assert(std::is_enum<E>::value
        && std::is_integral<typename std::underlying_type<E>::type>::value
        && std::is_unsigned<typename std::underlying_type<E>::type>::value,
        "requires unsigned integral enum");
};

template<typename E>
struct EnumHash : public EnumTypeAssert<E> {
    size_t operator()(const E& e) const {
        return static_cast<size_t>(e);
    }
};

template<typename E>
struct EnumCompare : public EnumTypeAssert<E> {
    size_t operator()(const E& l, const E& r) const {
        return static_cast<size_t>(l) < static_cast<size_t>(r);
    }
};


// TODO C versions

DLL_SPEC_ extern const
std::unordered_map<PeriodType, std::set<unsigned int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE;

DLL_SPEC_ extern const
std::unordered_map<PeriodType, std::set<FrequencyType, EnumCompare<FrequencyType>>,
                   EnumHash<PeriodType> >
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE;

DLL_SPEC_ extern const
std::unordered_map<FrequencyType, std::set<unsigned int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE;


typedef std::function<void(long, const std::string&)> api_on_error_cb_ty;

} /* tdma */

#endif /* __cplusplus */


/*
 * C / ABI bridge objects
 *
 * these are simple C objects that hold a pointer to the underlying
 * C++ Getter implementation object and a type id field we can check
 * in the C or ABI layer for additional type safety
 *
 * NOTE - APIGetter is a protected base so we can simply pass its corresponding
 *        impl pointer around as void* if easier
 *
 * NOTE - objects pointed to by 'obj' are allocated by library (probably
 *        using new) and SHOULD NOT be dealloced by the client
 */

typedef struct {
    void *obj;
    int type_id; // 0
} Getter_C;

typedef struct {
    void *obj;
    int type_id; // 1
} QuoteGetter_C;

/*
 * ABI bridge functions
 *
 *
 * How we provide C and C++ interface w/ a stable ABI
 * (e.g for QuoteGetter)
 *
 *            Interface      -->         ABI Bridge         -->     Implementation
 *
 *  C   QuoteGetter_GetSymbol()   QuoteGetter_GetSymbol_ABI()
 *  C++ QuoteGetter.get_symbol()                               QuoteGetterImpl.get_symbol()
 *
 *  Errors:
 *
 *  C        Error Code      <--        Error Code         <--      Exception
 *  C++      Exception       <--        Exception          <--      Exception
 *
 *
 * INLINE interface calls  so they stay on client-side of ABI
 */

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_Get_ABI( Getter_C *pgetter,
                     char** buf,
                     size_t *n,
                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_Close_ABI(Getter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_SetWaitMSec_ABI(unsigned long long msec, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_GetWaitMSec_ABI(unsigned long long *msec, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_GetDefWaitMSec_ABI(unsigned long long *msec, int allow_exceptions);


EXTERN_C_SPEC_ DLL_SPEC_ int
QuoteGetter_Create_ABI( struct Credentials *pcreds,
                           const char* symbol,
                           QuoteGetter_C *pgetter,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
QuoteGetter_Destroy_ABI(QuoteGetter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
QuoteGetter_GetSymbol_ABI( QuoteGetter_C *pgetter,
                              char **buf,
                              size_t *n,
                              int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
QuoteGetter_SetSymbol_ABI( QuoteGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions );


/*
 * C interface
 *
 * all calls return error status and populate vars/buffers
 *
 * char** buf - assigned a heap-allocated pointer to string w/ NULL term
 * size_t* n  - the size of the string AND NULL term
 *
 * CLIENTS NEED TO DEALLOC (CALL FREE) WHEN DONE, BEFORE USING AGAIN
 *
 *
 * Getter objects created by Create need to be destroyed w/ Destroy
 *
 * NOTE - for Get and Close client can use the versions associate with the
 *        getter type or the 'base' versions: APIGetter_Get and APIGetter_Close
 *
 */

/* APIGetter -> APIGetterImpl */
inline int
APIGetter_Get(Getter_C *pgetter, char** buf, size_t *n)
{ return APIGetter_Get_ABI(pgetter, buf, n, false); }

inline int
APIGetter_Close(Getter_C *pgetter)
{ return APIGetter_Close_ABI(pgetter, false); }

inline int
APIGetter_SetWaitMSec(unsigned long long msec)
{ return APIGetter_SetWaitMSec_ABI(msec, false); }

inline int
APIGetter_GetWaitMSec(unsigned long long *msec)
{ return APIGetter_GetWaitMSec_ABI(msec, false); }

inline int
APIGetter_GetDefWaitMSec(unsigned long long *msec)
{ return APIGetter_GetDefWaitMSec_ABI(msec, false); }


/* QuoteGetter -> QuoteGetterImpl */
inline int
QuoteGetter_Create( struct Credentials *pcreds,
                      const char* symbol,
                      QuoteGetter_C *pgetter )
{ return QuoteGetter_Create_ABI(pcreds, symbol, pgetter, false); }

inline int
QuoteGetter_Destroy(QuoteGetter_C *getter)
{ return QuoteGetter_Destroy_ABI(getter, false); }

inline int
QuoteGetter_Get(QuoteGetter_C *pgetter, char** buf, size_t *n)
{ return APIGetter_Get_ABI( (Getter_C*)pgetter, buf, n, false); }

inline int
QuoteGetter_Close(QuoteGetter_C *pgetter)
{ return APIGetter_Close_ABI( (Getter_C*)pgetter, false); }

inline int
QuoteGetter_GetSymbol(QuoteGetter_C *getter, char **buf, size_t *n)
{ return QuoteGetter_GetSymbol_ABI(getter, buf, n, false); }

inline int
QuoteGetter_SetSymbol(QuoteGetter_C *getter, const char *symbol)
{ return QuoteGetter_SetSymbol_ABI(getter, symbol, false); }


/* C++ Interface */

#ifdef __cplusplus

namespace tdma{

inline const char*
CPP_to_C(const std::string& in)
{ return in.c_str(); }

inline Credentials*
CPP_to_C(Credentials& in)
{ return &in; }



class APIGetter{
protected:
    std::unique_ptr<Getter_C> _pgetter;

    template<typename CTy>
    CTy*
    cgetter() const
    {
        return reinterpret_cast<CTy*>(
            const_cast<Getter_C*>(_pgetter.get())
            );
    }

    // can we deduce CTy from F ?
    template<typename CTy, typename F, typename... Args>
    APIGetter(CTy _, F func, Args... args)
        :
            _pgetter(new Getter_C)
        {
            static_assert(sizeof(CTy) == sizeof(Getter_C), "invalid C getters");
            func(args..., cgetter<CTy>(), true);
        }

    APIGetter( APIGetter&& getter )
        : _pgetter( move(getter._pgetter) )
    {}

    virtual
    ~APIGetter(){}

public:
    static std::chrono::milliseconds
    get_def_wait_msec()
    {
        unsigned long long w;
        APIGetter_GetDefWaitMSec_ABI(&w, true);
        return std::chrono::milliseconds(w);
    }

    static std::chrono::milliseconds
    get_wait_msec()
    {
        unsigned long long w;
        APIGetter_GetWaitMSec_ABI(&w, true);
        return std::chrono::milliseconds(w);
    }

    static void
    set_wait_msec(std::chrono::milliseconds msec)
    {
        APIGetter_SetWaitMSec_ABI(
            static_cast<unsigned long long>(msec.count()), true
            );
    }

    json
    get()
    {
        char *buf;
        size_t n;
        APIGetter_Get_ABI(_pgetter.get(), &buf, &n, true);
        json j(n > 1 ? buf : "");
        if(buf)
            free(buf);
        return j;
    }

    void
    close()
    { APIGetter_Close_ABI(_pgetter.get(), true); }

};



class QuoteGetter
        : public APIGetter {
public:
    static const int TYPE_ID = 1;

    QuoteGetter( Credentials& creds, const std::string& symbol )
        :
            APIGetter( QuoteGetter_C{}, QuoteGetter_Create_ABI,
                       CPP_to_C(creds), CPP_to_C(symbol) )
    {}

    ~QuoteGetter()
    {
        if( _pgetter && _pgetter->obj )
            QuoteGetter_Destroy_ABI(cgetter<QuoteGetter_C>(), true);
    }

    QuoteGetter( QuoteGetter&& ) = default;

    std::string
    get_symbol() const
    {
        char *buf;
        size_t n;
        QuoteGetter_GetSymbol_ABI(cgetter<QuoteGetter_C>(), &buf, &n, true);
        std::string s(buf, n-1);
        if(buf)
            free(buf);
        return s;
    }

    void
    set_symbol(const std::string& symbol)
    { QuoteGetter_SetSymbol_ABI(cgetter<QuoteGetter_C>(), symbol.c_str(), true); }
};

inline json
GetQuote(Credentials& creds, const std::string& symbol)
{ return QuoteGetter(creds, symbol).get(); }

/*
class DLL_SPEC_ QuotesGetter
        : public APIGetter {
    std::set<std::string> _symbols;

    void
    _build();

    virtual void
    build();

public:
    QuotesGetter( Credentials& creds,
                    const std::set<std::string>& symbols );

    std::set<std::string>
    get_symbols() const
    { return _symbols; }

    void
    set_symbols(const std::set<std::string>& symbols);
};

inline json
GetQuotes(Credentials& creds, const std::set<std::string>& symbols)
{ return QuotesGetter(creds, symbols).get(); }


// note we only implement the single MarketType version
// could just add an 'all' field to MarketType enum
class DLL_SPEC_ MarketHoursGetter
        : public APIGetter {
    MarketType _market_type;
    std::string _date;

    void
    _build();

    virtual void
    build();

public:
    MarketHoursGetter( Credentials& creds,
                          MarketType market_type,
                          const std::string& date );

    std::string
    get_date() const
    { return _date; }

    MarketType
    get_market_type() const
    { return _market_type; }

    void
    set_date(const std::string&);

    void
    set_market_type(MarketType market_type);
};

inline json
GetMarketHours( Credentials& creds,
                  MarketType market_type,
                  const std::string& date )
{ return MarketHoursGetter(creds, market_type, date).get(); }


class DLL_SPEC_ MoversGetter
        : public APIGetter{
    MoversIndex _index;
    MoversDirectionType _direction_type;
    MoversChangeType _change_type;

    void
    _build();

    virtual void
    build();

public:
    MoversGetter( Credentials& creds,
                  MoversIndex index,
                  MoversDirectionType direction_type,
                  MoversChangeType change_type );

    MoversIndex
    get_index() const
    { return _index; }

    MoversDirectionType
    get_direction_type() const
    { return _direction_type; }

    MoversChangeType
    get_change_type() const
    { return _change_type; }

    void
    set_index(MoversIndex index);

    void
    set_direction_type(MoversDirectionType direction_type);

    void
    set_change_type(MoversChangeType change_type);
};

inline json
GetMovers( Credentials& creds,
           MoversIndex index,
           MoversDirectionType direction_type,
           MoversChangeType change_type )
{ return MoversGetter(creds, index, direction_type, change_type).get(); }


class DLL_SPEC_ HistoricalGetterBase
        : public APIGetter {
    std::string _symbol;
    FrequencyType _frequency_type;
    unsigned int _frequency;
    bool _extended_hours;

    virtual void
    build() = 0;

protected:
    HistoricalGetterBase( Credentials& creds,
                          const std::string& symbol,
                          FrequencyType frequency_type,
                          unsigned int frequency,
                          bool extended_hours );

    std::vector<std::pair<std::string, std::string>>
    build_query_params() const;

    void
    set_frequency(unsigned int frequency);

    void
    set_frequency_type(FrequencyType frequency_type);

public:
    std::string
    get_symbol() const
    { return _symbol; }

    unsigned int
    get_frequency() const
    { return _frequency; }

    FrequencyType
    get_frequency_type() const
    { return _frequency_type; }

    bool
    is_extended_hours() const
    { return _extended_hours; }

    void
    set_symbol(const std::string& symbol);

    void
    set_extended_hours(bool extended_hours);

};


// TODO MAKE CLEAR IN DOCS THE ORDER WE NEED TO CHANGE PERIODS/FREQUENCIES
//      BECUASE OF THE INTERNAL CHECKS/CONTINGENCIES
class DLL_SPEC_ HistoricalPeriodGetter
        : public HistoricalGetterBase {
    PeriodType _period_type;
    unsigned int _period;

    void
    _build();

    /*virtual*//* void
    build();

public:
    HistoricalPeriodGetter( Credentials& creds,
                            const std::string& symbol,
                            PeriodType period_type,
                            unsigned int period,
                            FrequencyType frequency_type,
                            unsigned int frequency,
                            bool extended_hours );

    PeriodType
    get_period_type() const
    { return _period_type; }

    unsigned int get_period() const
    { return _period; } 

    void
    set_period(PeriodType period_type, unsigned int period);

    void
    set_frequency(FrequencyType frequency_type, unsigned int frequency);
};

inline json
GetHistoricalPeriod( Credentials& creds,
                        const std::string& symbol,
                        PeriodType period_type,
                        unsigned int period,
                        FrequencyType frequency_type,
                        unsigned int frequency,
                        bool extended_hours )
{
    return HistoricalPeriodGetter( creds, symbol, period_type, period,
                                   frequency_type, frequency,
                                   extended_hours ).get();
}




class DLL_SPEC_ HistoricalRangeGetter
        : public HistoricalGetterBase {
    unsigned long long _start_msec_since_epoch;
    unsigned long long _end_msec_since_epoch;

    void
    _build();

    /*virtual*//* void
    build();

public:
    HistoricalRangeGetter( Credentials& creds,
                              const std::string& symbol,
                              FrequencyType frequency_type,
                              unsigned int frequency,
                              unsigned long long start_msec_since_epoch,
                              unsigned long long end_msec_since_epoch,
                              bool extended_hours );

   unsigned long long
   get_end_msec_since_epoch() const
   { return _end_msec_since_epoch; }

   unsigned long long
   get_start_msec_since_epoch() const
   { return _start_msec_since_epoch; }

   void
   set_frequency(FrequencyType frequency_type, unsigned int frequency);

   void
   set_end_msec_since_epoch(unsigned long long end_msec_since_epoch);

   void
   set_start_msec_since_epoch(unsigned long long start_msec_since_epoch);
};

inline json
GetHistoricalRange( Credentials& creds,
                       const std::string& symbol,
                       FrequencyType ftype,
                       unsigned int frequency,
                       unsigned long long start_msec_since_epoch,
                       unsigned long long end_msec_since_epoch,
                       bool extended_hours )
{
    return HistoricalRangeGetter( creds, symbol, ftype, frequency,
                                  start_msec_since_epoch, end_msec_since_epoch,
                                  extended_hours ).get();
}

*/

class OptionStrikes {
public:
    enum class Type : unsigned int {
        n_atm,
        single,
        range
    };

private:
    union {
        unsigned int _n_atm;
        double _single;
        OptionRangeType _range;
    };
    Type _type;

    OptionStrikes(Type type)
        : _type(type) {}

public:
    Type
    get_type() const
    { return _type; }

    unsigned int
    get_n_atm() const
    { return (_type == Type::n_atm) ? _n_atm : 0; }

    double
    get_single() const
    { return (_type == Type::single) ? _single : 0.0; }

    OptionRangeType
    get_range() const
    { return (_type == Type::range) ? _range : OptionRangeType::null; }

    static OptionStrikes
    N_ATM(unsigned int n);

    static OptionStrikes
    Single(double price);

    static OptionStrikes
    Range(OptionRangeType range);
};

std::string
to_string(const OptionStrikes& strikes);

std::ostream&
operator<<(std::ostream& out, const OptionStrikes& strikes);



class OptionStrategy {
    OptionStrategyType _strategy;
    double _spread_interval;

    OptionStrategy(OptionStrategyType strategy);
    OptionStrategy(OptionStrategyType strategy, double spread_interval);

public:
    OptionStrategyType
    get_strategy() const
    { return _strategy; }

    double
    get_spread_interval() const
    { return _spread_interval; }

    static OptionStrategy
    Covered()
    { return {OptionStrategyType::covered}; }

    static OptionStrategy
    Calendar()
    { return {OptionStrategyType::calendar}; }

    static OptionStrategy
    Vertical(double spread_interval=1.0)
    { return {OptionStrategyType::vertical, spread_interval}; }

    static OptionStrategy
    Strangle(double spread_interval=1.0)
    { return {OptionStrategyType::strangle, spread_interval}; }

    static OptionStrategy
    Straddle(double spread_interval=1.0)
    { return {OptionStrategyType::straddle, spread_interval}; }

    static OptionStrategy
    Butterfly(double spread_interval=1.0)
    { return {OptionStrategyType::butterfly, spread_interval}; }

    static OptionStrategy
    Condor(double spread_interval=1.0)
    { return {OptionStrategyType::condor, spread_interval}; }

    static OptionStrategy
    Diagonal(double spread_interval=1.0)
    { return {OptionStrategyType::diagonal, spread_interval}; }

    static OptionStrategy
    Collar(double spread_interval=1.0)
    { return {OptionStrategyType::collar, spread_interval}; }

    static OptionStrategy
    Roll(double spread_interval=1.0)
    { return {OptionStrategyType::roll, spread_interval}; }
};

std::string
to_string(const OptionStrategy& strategy);

std::ostream&
operator<<(std::ostream& out, const OptionStrategy& strikes);

/*

class DLL_SPEC_ OptionChainGetter
        : public APIGetter {
    std::string _symbol;
    OptionStrikes _strikes;
    OptionContractType _contract_type;
    bool _include_quotes;
    std::string _from_date;
    std::string _to_date;
    OptionExpMonth _exp_month;
    OptionType _option_type;

    void
    _build();

    virtual void
    build();

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const;

public:
    OptionChainGetter( Credentials& creds,
                       const std::string& symbol,
                       const OptionStrikes& strikes,                    
                       OptionContractType contract_type = OptionContractType::all,
                       bool include_quotes = false,
                       const std::string& from_date = "",
                       const std::string& to_date = "",
                       OptionExpMonth exp_month = OptionExpMonth::all,
                       OptionType option_type = OptionType::all );

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
    set_symbol(const std::string& symbol);

    void
    set_strikes(OptionStrikes strikes);

    void
    set_contract_type(OptionContractType contract_type);

    void
    include_quotes(bool includes_quotes);

    void
    set_from_date(const std::string& from_date);

    void
    set_to_date(const std::string& to_date);

    void
    set_exp_month(OptionExpMonth exp_month);

    void
    set_option_type(OptionType option_type);
};

inline json
GetOptionChain( Credentials& creds,
                const std::string& symbol,
                OptionStrikes strikes,
                OptionContractType contract_type = OptionContractType::all,
                bool include_quotes = false,
                const std::string& from_date = "",
                const std::string& to_date = "",
                OptionExpMonth exp_month = OptionExpMonth::all,
                OptionType option_type = OptionType::all )
{
    return OptionChainGetter( creds, symbol, strikes, contract_type,
                              include_quotes, from_date, to_date, exp_month,
                              option_type ).get();
}


class DLL_SPEC_ OptionChainStrategyGetter
        : public OptionChainGetter {
    OptionStrategy _strategy;

    void
    _build();

    /*virtual*//* void
    build();

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const;

public:
    OptionChainStrategyGetter(
            Credentials& creds,
            const std::string& symbol,
            OptionStrategy strategy,
            OptionStrikes strikes,
            OptionContractType contract_type = OptionContractType::all,
            bool include_quotes = false,
            const std::string& from_date = "",
            const std::string& to_date = "",
            OptionExpMonth exp_month = OptionExpMonth::all,
            OptionType option_type = OptionType::all );

    OptionStrategy 
    get_strategy() const
    { return _strategy; }

    void
    set_strategy(OptionStrategy strategy);
};

inline json
GetOptionChainStrategy( Credentials& creds,
                        const std::string& symbol,
                        OptionStrategy strategy,
                        OptionStrikes strikes,
                        OptionContractType contract_type = OptionContractType::all,
                        bool include_quotes = false,
                        const std::string& from_date = "",
                        const std::string& to_date = "",
                        OptionExpMonth exp_month = OptionExpMonth::all,
                        OptionType option_type = OptionType::all )
{
    return OptionChainStrategyGetter( creds, symbol, strategy, strikes,
                                      contract_type, include_quotes, from_date,
                                      to_date, exp_month, option_type ).get();
}


class DLL_SPEC_ OptionChainAnalyticalGetter
        : public OptionChainGetter {
    double _volatility;
    double _underlying_price;
    double _interest_rate;
    unsigned int _days_to_exp;

    void
    _build();

    /*virtual*//* void
    build();

protected:
    std::vector<std::pair<std::string,std::string>>
    build_query_params() const;

public:
    OptionChainAnalyticalGetter(
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
            OptionType option_type = OptionType::all );

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
    set_volatility(double volatility);

    void
    set_underlying_price(double underlying_price);

    void
    set_interest_rate(double interest_rate);

    void
    set_days_to_exp(unsigned int days_to_exp);
};

inline json
GetOptionChainAnalytical( Credentials& creds,
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
{
    return OptionChainAnalyticalGetter( creds, symbol, volatility,
                                        underlying_price, interest_rate,
                                        days_to_exp, strikes,
                                        contract_type, include_quotes,
                                        from_date, to_date,
                                        exp_month, option_type ).get();
}


class DLL_SPEC_ AccountGetterBase
        : public APIGetter{
    std::string _account_id;

    virtual void
    build() = 0;

protected:
    AccountGetterBase( Credentials& creds,
                       const std::string& account_id );

public:
    std::string
    get_account_id() const
    { return _account_id; }

    void
    set_account_id(const std::string& account_id);
};


class DLL_SPEC_ AccountInfoGetter
        : public AccountGetterBase{
    std::string _account_id;
    bool _positions;
    bool _orders;

    void
    _build();

    /*virtual*//* void
    build();

public:
    AccountInfoGetter( Credentials& creds,
                       const std::string& account_id,
                       bool positions,
                       bool orders );

    bool
    returns_positions() const
    { return _positions; }

    bool
    returns_orders() const
    { return _orders;}

    void
    return_positions(bool positions);

    void
    return_orders(bool orders);
};

inline json
GetAccountInfo( Credentials& creds,
                const std::string& account_id,
                bool positions,
                bool orders )
{ return AccountInfoGetter(creds, account_id, positions, orders).get(); }


class DLL_SPEC_ PreferencesGetter
        : public AccountGetterBase{
    void
    _build();

    /*virtual*//* void
    build();

public:
    PreferencesGetter( Credentials& creds, const std::string& account_id );
};

inline json
GetPreferences(Credentials& creds, const std::string& account_id)
{ return PreferencesGetter(creds, account_id).get(); }


class DLL_SPEC_ UserPrincipalsGetter
        : public APIGetter{
    bool _streamer_subscription_keys;
    bool _streamer_connection_info;
    bool _preferences;
    bool _surrogate_ids;

    void
    _build();

    /*virtual*//* void
    build();

public:
    UserPrincipalsGetter( Credentials& creds,                         
                          bool streamer_subscription_keys,
                          bool streamer_connection_info,
                          bool preferences,
                          bool surrogate_ids );

    bool
    returns_streamer_subscription_keys() const
    { return _streamer_subscription_keys; }

    bool
    returns_streamer_connection_info() const
    { return _streamer_connection_info; }

    bool
    returns_preferences() const
    { return _preferences; }

    bool
    returns_surrogate_ids() const
    { return _surrogate_ids; }

    void
    return_streamer_subscription_keys(bool streamer_subscription_keys);

    void
    return_streamer_connection_info(bool streamer_connection_info);

    void
    return_preferences(bool preferences);

    void
    return_surrogate_ids(bool surrogate_ids);
};

inline json
GetUserPrincipals( Credentials& creds,                   
                   bool streamer_subscription_keys,
                   bool streamer_connection_info,
                   bool preferences,
                   bool surrogate_ids )
{
    return UserPrincipalsGetter( creds, streamer_subscription_keys,
                                 streamer_connection_info, preferences,
                                 surrogate_ids ).get();
}


class DLL_SPEC_ StreamerSubscriptionKeysGetter
        : public AccountGetterBase{
    void
    _build();

    /*virtual*//* void
    build();

public:
    StreamerSubscriptionKeysGetter( Credentials& creds,
                                    const std::string& account_id );
};

inline json
GetStreamerSubscriptionKeys(Credentials& creds, const std::string& account_id)
{ return StreamerSubscriptionKeysGetter(creds, account_id).get(); }


class DLL_SPEC_ TransactionHistoryGetter
        : public AccountGetterBase{
    std::string _transaction_id;
    TransactionType _transaction_type;
    std::string _symbol;
    std::string _start_date;
    std::string _end_date;

    void
    _build();

    /*virtual*//* void
    build();

public:
    TransactionHistoryGetter( Credentials& creds,
                              const std::string& account_id,
                              TransactionType transaction_type = TransactionType::all,
                              const std::string& symbol = "",
                              const std::string& start_date = "",
                              const std::string& end_date = "");

    TransactionType
    get_transaction_type() const
    { return _transaction_type; }

    std::string
    get_symbol() const
    { return _symbol; }

    std::string
    get_start_date() const
    { return _start_date; }

    std::string
    get_end_date() const
    { return _end_date; }

    void
    set_transaction_type(TransactionType transaction_type);

    void
    set_symbol(const std::string& symbol);

    void
    set_start_date(const std::string& start_date);

    void
    set_end_date(const std::string& end_date);

};

inline json
GetTransactionHistory( Credentials& creds,
                       const std::string& account_id,
                       TransactionType transaction_type,
                       const std::string& symbol,
                       const std::string& start_date,
                       const std::string& end_date )
{
    return TransactionHistoryGetter( creds, account_id, transaction_type,
                                     symbol, start_date, end_date ).get();
}


class DLL_SPEC_ IndividualTransactionHistoryGetter
        : public AccountGetterBase {
    std::string _transaction_id;

    void
    _build();

    /*virtual*//* void
    build();

public:
    IndividualTransactionHistoryGetter( Credentials& creds,
                                        const std::string& account_id,
                                        const std::string& transaction_id);

    std::string
    get_transaction_id() const
    { return _transaction_id; }

    void
    set_transaction_id(const std::string& transaction_id);

};

inline json
GetIndividualTransactionHistory( Credentials& creds,
                                 const std::string& account_id,
                                 const std::string& transaction_id )
{
    return IndividualTransactionHistoryGetter( creds, account_id,
                                               transaction_id ).get();
}


class DLL_SPEC_ InstrumentInfoGetter
        : public APIGetter {
    std::string _query_string;
    InstrumentSearchType _search_type;

    void
    _build();

    /*virtual*//* void
    build();

public:
    InstrumentInfoGetter( Credentials& creds,
                          InstrumentSearchType search_type,
                          const std::string& query_string );

    std::string
    get_query_string() const
    { return _query_string; }

    InstrumentSearchType
    get_search_type() const
    { return _search_type; }

    void
    set_query(InstrumentSearchType search_type, const std::string& query_string);
};

inline json
GetInstrumentInfo( Credentials& creds,
                   InstrumentSearchType search_type,
                   const std::string& query_string )
{ return InstrumentInfoGetter(creds, search_type, query_string).get(); }
*/


} /* tdma */

#endif /* __cplusplus */

#endif // TDMA_API_GET_H
