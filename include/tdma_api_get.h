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

using json = nlohmann::json;
#endif /* __cplusplus */

DECL_C_CPP_TDMA_ENUM(PeriodType, 0, 3,
    BUILD_C_CPP_TDMA_ENUM_NAME(PeriodType, day),
    BUILD_C_CPP_TDMA_ENUM_NAME(PeriodType, month),
    BUILD_C_CPP_TDMA_ENUM_NAME(PeriodType, year),
    BUILD_C_CPP_TDMA_ENUM_NAME(PeriodType, ytd)
);

DECL_C_CPP_TDMA_ENUM(FrequencyType, 0, 3,
    BUILD_C_CPP_TDMA_ENUM_NAME(FrequencyType, minute),
    BUILD_C_CPP_TDMA_ENUM_NAME(FrequencyType, daily),
    BUILD_C_CPP_TDMA_ENUM_NAME(FrequencyType, weekly),
    BUILD_C_CPP_TDMA_ENUM_NAME(FrequencyType, monthly)
);

DECL_C_CPP_TDMA_ENUM(OptionContractType, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionContractType, call),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionContractType, put),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionContractType, all),
);

DECL_C_CPP_TDMA_ENUM(OptionStrategyType, 0, 9,
    // BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, single),
    // BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, analytical),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, covered),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, vertical),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, calendar),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, strangle),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, straddle),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, butterfly),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, condor),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, diagonal),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, collar),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrategyType, roll)
);

DECL_C_CPP_TDMA_ENUM(OptionRangeType, 1, 7,
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, null), // DO NOT USE
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, itm),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, ntm),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, otm),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, sak),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, sbk),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, snk),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionRangeType, all)
);

DECL_C_CPP_TDMA_ENUM(OptionExpMonth, 0, 12,
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, jan),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, feb),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, mar),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, apr),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, may),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, jun),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, jul),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, aug),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, sep),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, oct),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, nov),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, dec),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionExpMonth, all)
);

DECL_C_CPP_TDMA_ENUM(OptionType, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionType, s),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionType, ns),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionType, all)
);

DECL_C_CPP_TDMA_ENUM(TransactionType, 0, 9,
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, all),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, trade),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, buy_only),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, sell_only),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, cash_in_or_cash_out),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, checking),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, dividend),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, interest),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, other),
    BUILD_C_CPP_TDMA_ENUM_NAME(TransactionType, advisor_fees)
);

DECL_C_CPP_TDMA_ENUM(InstrumentSearchType, 0, 5,
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, symbol_exact),
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, symbol_search),
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, symbol_regex),
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, description_search),
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, description_regex),
    BUILD_C_CPP_TDMA_ENUM_NAME(InstrumentSearchType, cusip)
);

DECL_C_CPP_TDMA_ENUM(MarketType, 0, 4,
    BUILD_C_CPP_TDMA_ENUM_NAME(MarketType, equity),
    BUILD_C_CPP_TDMA_ENUM_NAME(MarketType, option),
    BUILD_C_CPP_TDMA_ENUM_NAME(MarketType, future),
    BUILD_C_CPP_TDMA_ENUM_NAME(MarketType, bond),
    BUILD_C_CPP_TDMA_ENUM_NAME(MarketType, forex)
);

DECL_C_CPP_TDMA_ENUM(MoversIndex, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversIndex, compx), /* $COMPX */
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversIndex, dji), /* $DJI */
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversIndex, spx) /* SPX */
);

DECL_C_CPP_TDMA_ENUM(MoversDirectionType, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversDirectionType, up),
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversDirectionType, down),
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversDirectionType, up_and_down)
);

DECL_C_CPP_TDMA_ENUM(MoversChangeType, 0, 1,
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversChangeType, value),
    BUILD_C_CPP_TDMA_ENUM_NAME(MoversChangeType, percent)
);

DECL_C_CPP_TDMA_ENUM(OptionStrikesType, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrikesType, n_atm),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrikesType, single),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrikesType, range),
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionStrikesType, none) // dont use
);

typedef union {
    unsigned int n_atm;
    double single;
#ifdef __cplusplus
    tdma::
#endif
    OptionRangeType range;
} OptionStrikesValue;

#ifdef __cplusplus

namespace tdma{

template<typename E>
struct EnumTypeAssert{
    static_assert(std::is_enum<E>::value
        && std::is_integral<typename std::underlying_type<E>::type>::value,
        "requires integral enum");
};

template<typename E>
struct EnumHash : public EnumTypeAssert<E> {
    size_t operator()(const E& e) const {
        return static_cast<long long>(e);
    }
};

template<typename E>
struct EnumCompare : public EnumTypeAssert<E> {
    size_t operator()(const E& l, const E& r) const {
        return static_cast<long long>(l) < static_cast<long long>(r);
    }
};

extern const
std::unordered_map<PeriodType, std::set<int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE;

extern const
std::unordered_map<PeriodType, std::set<FrequencyType, EnumCompare<FrequencyType>>,
                   EnumHash<PeriodType> >
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE;

extern const
std::unordered_map<FrequencyType, std::set<int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE;


typedef std::function<void(long, const std::string&)> api_on_error_cb_ty;

} /* tdma */

#else

const int
VALID_PERIODS_BY_PERIOD_TYPE[PeriodType_ytd + 1][8] = {
    {1,2,3,4,5,10,-1,-1},
    {1,2,3,6,-1,-1,-1,-1},
    {1,2,3,5,10,15,10,-1},
    {1,-1,-1,-1,-1,-1,-1,-1},
};

const FrequencyType
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[PeriodType_ytd + 1][4] = {
    {FrequencyType_minute, -1, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, FrequencyType_monthly, -1},
    {FrequencyType_weekly, -1,-1, -1}
};

const int
VALID_FREQUENCIES_BY_FREQUENCY_TYPE[FrequencyType_monthly + 1][5] = {
    {1, 5, 10, 30, -1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1}
};


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

#define DECL_CGETTER_STRUCT(name) typedef struct {void *obj; int type_id; } name

DECL_CGETTER_STRUCT(Getter_C);
DECL_CGETTER_STRUCT(QuoteGetter_C);
DECL_CGETTER_STRUCT(QuotesGetter_C);
DECL_CGETTER_STRUCT(MarketHoursGetter_C);
DECL_CGETTER_STRUCT(MoversGetter_C);
DECL_CGETTER_STRUCT(HistoricalPeriodGetter_C);
DECL_CGETTER_STRUCT(HistoricalRangeGetter_C);
DECL_CGETTER_STRUCT(OptionChainGetter_C);
DECL_CGETTER_STRUCT(OptionChainStrategyGetter_C);
DECL_CGETTER_STRUCT(OptionChainAnalyticalGetter_C);
DECL_CGETTER_STRUCT(AccountInfoGetter_C);
DECL_CGETTER_STRUCT(PreferencesGetter_C);
DECL_CGETTER_STRUCT(UserPrincipalsGetter_C);
DECL_CGETTER_STRUCT(StreamerSubscriptionKeysGetter_C);
DECL_CGETTER_STRUCT(TransactionHistoryGetter_C);
DECL_CGETTER_STRUCT(IndividualTransactionHistoryGetter_C);
DECL_CGETTER_STRUCT(InstrumentInfoGetter_C);

#undef DECL_CGETTER_STRUCT


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
APIGetter_IsClosed_ABI(Getter_C *pgetter, int*b, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_SetWaitMSec_ABI(unsigned long long msec, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_GetWaitMSec_ABI(unsigned long long *msec, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_GetDefWaitMSec_ABI(unsigned long long *msec, int allow_exceptions);

/* QuoteGetter */
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

/* QuotesGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_Create_ABI( struct Credentials *pcreds,
                            const char** symbols,
                            size_t nsymbols,
                            QuotesGetter_C *pgetter,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_Destroy_ABI(QuotesGetter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_GetSymbols_ABI( QuotesGetter_C *pgetter,
                                char ***buf,
                                size_t *n,
                                int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_SetSymbols_ABI( QuotesGetter_C *pgetter,
                                 const char** symbols,
                                 size_t nymbols,
                                 int allow_exceptions );


/* MarketHoursGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_Create_ABI( struct Credentials *pcreds,
                                 int market_type,
                                 const char* date,
                                 MarketHoursGetter_C *pgetter,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_Destroy_ABI(MarketHoursGetter_C *pgetter,
                                 int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_GetMarketType_ABI( MarketHoursGetter_C *pgetter,
                                          int *market_type,
                                          int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_SetMarketType_ABI( MarketHoursGetter_C *pgetter,
                                         int market_type,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_GetDate_ABI( MarketHoursGetter_C *pgetter,
                                   char **buf,
                                   size_t *n,
                                   int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MarketHoursGetter_SetDate_ABI( MarketHoursGetter_C *pgetter,
                                  const char* date,
                                  int allow_exceptions );

/* MoversGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_Create_ABI( struct Credentials *pcreds,
                            int index,
                            int direction_type,
                            int change_type,
                            MoversGetter_C *pgetter,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_Destroy_ABI( MoversGetter_C *pgetter,
                             int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_GetIndex_ABI( MoversGetter_C *pgetter,
                              int *index,
                              int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_SetIndex_ABI( MoversGetter_C *pgetter,
                              int index,
                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_GetDirectionType_ABI( MoversGetter_C *pgetter,
                                       int *direction_type,
                                       int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_SetDirectionType_ABI( MoversGetter_C *pgetter,
                                       int direction_type,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_GetChangeType_ABI( MoversGetter_C *pgetter,
                                   int *change_type,
                                   int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
MoversGetter_SetChangeType_ABI( MoversGetter_C *pgetter,
                                   int change_type,
                                   int allow_exceptions );

/* HistoricalGetterBase */
EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_GetSymbol_ABI( Getter_C *pgetter,
                                        char **buf,
                                        size_t *n,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_SetSymbol_ABI( Getter_C *pgetter,
                                        const char *symbol,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_GetFrequency_ABI( Getter_C *pgetter,
                                           unsigned int *frequency,
                                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_GetFrequencyType_ABI( Getter_C *pgetter,
                                                int *frequency_type,
                                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_IsExtendedHours_ABI( Getter_C *pgetter,
                                               int *is_extended_hours,
                                               int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_SetExtendedHours_ABI( Getter_C *pgetter,
                                               int is_extended_hours,
                                               int allow_exceptions );

/* HistoricalPeriodGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_Create_ABI( struct Credentials *pcreds,
                                       const char* symbol,
                                       int period_type,
                                       unsigned int period,
                                       int frequency_type,
                                       unsigned int frequency,
                                       int extended_hours,
                                       HistoricalPeriodGetter_C *pgetter,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_Destroy_ABI( HistoricalPeriodGetter_C *pgetter,
                                        int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_GetPeriodType_ABI( HistoricalPeriodGetter_C *pgetter,
                                               int *period_type,
                                               int allow_exceptions );


EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_GetPeriod_ABI( HistoricalPeriodGetter_C *pgetter,
                                           unsigned int *period,
                                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_SetPeriod_ABI( HistoricalPeriodGetter_C *pgetter,
                                           int period_type,
                                           unsigned int period,
                                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalPeriodGetter_SetFrequency_ABI( HistoricalPeriodGetter_C *pgetter,
                                             int frequency_type,
                                             unsigned int frequency,
                                             int allow_exceptions );

/* HistoricalRangeGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_Create_ABI( struct Credentials *pcreds,
                                       const char* symbol,
                                       int frequency_type,
                                       unsigned int frequency,
                                       unsigned long long start_msec_since_epoch,
                                       unsigned long long end_msec_since_epoch,
                                       int extended_hours,
                                       HistoricalRangeGetter_C *pgetter,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_Destroy_ABI( HistoricalRangeGetter_C *pgetter,
                                        int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *end_msec,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long end_msec,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *start_msec,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long start_msec,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_SetFrequency_ABI( HistoricalRangeGetter_C *pgetter,
                                             int frequency_type,
                                             unsigned int frequency,
                                             int allow_exceptions );

/* OptionChainGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
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
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_Destroy_ABI(OptionChainGetter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetSymbol_ABI( OptionChainGetter_C *pgetter,
                              char **buf,
                              size_t *n,
                              int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetSymbol_ABI( OptionChainGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetStrikes_ABI( OptionChainGetter_C *pgetter,
                                          int *strikes_type,
                                          OptionStrikesValue *strikes_value,
                                          int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetStrikes_ABI( OptionChainGetter_C *pgetter,
                                          int strikes_type,
                                          OptionStrikesValue strikes_value,
                                          int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetContractType_ABI( OptionChainGetter_C *pgetter,
                                           int *contract_type,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetContractType_ABI( OptionChainGetter_C *pgetter,
                                            int contract_type,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_IncludesQuotes_ABI( OptionChainGetter_C *pgetter,
                                           int *includes_quotes,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_IncludeQuotes_ABI( OptionChainGetter_C *pgetter,
                                          int include_quotes,
                                          int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetFromDate_ABI( OptionChainGetter_C *pgetter,
                                      char **buf,
                                      size_t *n,
                                      int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetFromDate_ABI( OptionChainGetter_C *pgetter,
                                      const char *date,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetToDate_ABI( OptionChainGetter_C *pgetter,
                                      char **buf,
                                      size_t *n,
                                      int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetToDate_ABI( OptionChainGetter_C *pgetter,
                                      const char *date,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetExpMonth_ABI( OptionChainGetter_C *pgetter,
                                           int *exp_month,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetExpMonth_ABI( OptionChainGetter_C *pgetter,
                                           int exp_month,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_GetOptionType_ABI( OptionChainGetter_C *pgetter,
                                           int *option_type,
                                           int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainGetter_SetOptionType_ABI( OptionChainGetter_C *pgetter,
                                           int option_type,
                                           int allow_exceptions);

/* OptionChainStrategyGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
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
                                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainStrategyGetter_Destroy_ABI( OptionChainStrategyGetter_C *pgetter,
                                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainStrategyGetter_GetStrategy_ABI(
    OptionChainStrategyGetter_C *pgetter,
    int *strategy_type,
    double *spread_interval,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainStrategyGetter_SetStrategy_ABI(
    OptionChainStrategyGetter_C *pgetter,
    int strategy_type,
    double spread_interval,
    int allow_exceptions
    );

/* OptionChainAnalyticalGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
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
                                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_Destroy_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_GetVolatility_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *volatility,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_SetVolatility_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double volatility,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *underlying_price,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double underlying_price,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_GetInterestRate_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double *interest_rate,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_SetInterestRate_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    double interest_rate,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_GetDaysToExp_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int *days_to_exp,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionChainAnalyticalGetter_SetDaysToExp_ABI(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int days_to_exp,
    int allow_exceptions
    );

/* AccountGetterBase */
EXTERN_C_SPEC_ DLL_SPEC_ int
AccountGetterBase_GetAccountId_ABI( Getter_C *pgetter,
                                    char **buf,
                                    size_t *n,
                                    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
AccountGetterBase_SetAccountId_ABI( Getter_C *pgetter,
                                    const char *account_id,
                                    int allow_exceptions );

/* AccountInfoGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_Create_ABI( struct Credentials *pcreds,
                                 const char* account_id,
                                 int positions,
                                 int orders,
                                 AccountInfoGetter_C *pgetter,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_Destroy_ABI( AccountInfoGetter_C *pgetter,
                                  int allow_exceptions );


EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_ReturnsPositions_ABI( AccountInfoGetter_C *pgetter,
                                            int *returns_positions,
                                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_ReturnPositions_ABI( AccountInfoGetter_C *pgetter,
                                            int return_positions,
                                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_ReturnsOrders_ABI( AccountInfoGetter_C *pgetter,
                                            int *returns_orders,
                                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
AccountInfoGetter_ReturnOrders_ABI( AccountInfoGetter_C *pgetter,
                                            int return_orders,
                                            int allow_exceptions );

/* PreferencesGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
PreferencesGetter_Create_ABI( struct Credentials *pcreds,
                                 const char* account_id,
                                 PreferencesGetter_C *pgetter,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
PreferencesGetter_Destroy_ABI( PreferencesGetter_C *pgetter,
                                  int allow_exceptions );

/* StreamerSubscriptionKeysGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
StreamerSubscriptionKeysGetter_Create_ABI(
    struct Credentials *pcreds,
     const char* account_id,
     StreamerSubscriptionKeysGetter_C *pgetter,
     int allow_exceptions
     );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamerSubscriptionKeysGetter_Destroy_ABI(
    StreamerSubscriptionKeysGetter_C *pgetter,
    int allow_exceptions
    );

/* TransactionHistoryGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_Create_ABI( struct Credentials *pcreds,
                                         const char* account_id,
                                         int transaction_type,
                                         const char* symbol,
                                         const char* start_date,
                                         const char* end_date,
                                         TransactionHistoryGetter_C *pgetter,
                                         int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_Destroy_ABI( TransactionHistoryGetter_C *pgetter,
                                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_GetTransactionType_ABI(
    TransactionHistoryGetter_C *pgetter,
    int *transaction_type,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_SetTransactionType_ABI(
    TransactionHistoryGetter_C *pgetter,
    int transaction_type,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_GetSymbol_ABI(
    TransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_SetSymbol_ABI(
    TransactionHistoryGetter_C *pgetter,
    const char* symbol,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_GetStartDate_ABI(
    TransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_SetStartDate_ABI(
    TransactionHistoryGetter_C *pgetter,
    const char* start_date,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_GetEndDate_ABI(
    TransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n,
    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
TransactionHistoryGetter_SetEndDate_ABI(
    TransactionHistoryGetter_C *pgetter,
    const char* end_date,
    int allow_exceptions );

/* IndividualTransactionHistoryGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
IndividualTransactionHistoryGetter_Create_ABI(
    struct Credentials *pcreds,
    const char* account_id,
    const char* transaction_id,
    IndividualTransactionHistoryGetter_C *pgetter,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
IndividualTransactionHistoryGetter_Destroy_ABI(
    IndividualTransactionHistoryGetter_C *pgetter,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
IndividualTransactionHistoryGetter_GetTransactionId_ABI(
    IndividualTransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
IndividualTransactionHistoryGetter_SetTransactionId_ABI(
    IndividualTransactionHistoryGetter_C *pgetter,
    const char* transaction_id,
    int allow_exceptions
    );

/* UserPrincipalsGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_Create_ABI( struct Credentials *pcreds,
                                     int streamer_subscription_keys,
                                     int streamer_connection_info,
                                     int preferences,
                                     int surrogate_ids,
                                     UserPrincipalsGetter_C *pgetter,
                                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_Destroy_ABI( UserPrincipalsGetter_C *pgetter,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI(
    UserPrincipalsGetter_C *pgetter,
    int *returns_subscription_keys,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnSubscriptionKeys_ABI(
    UserPrincipalsGetter_C *pgetter,
    int return_subscription_keys,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnsConnectionInfo_ABI(
    UserPrincipalsGetter_C *pgetter,
    int *returns_connection_info,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnConnectionInfo_ABI(
    UserPrincipalsGetter_C *pgetter,
    int return_connection_info,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnsPreferences_ABI(
    UserPrincipalsGetter_C *pgetter,
    int *returns_preferences,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnPreferences_ABI(
    UserPrincipalsGetter_C *pgetter,
    int return_preferences,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnsSurrogateIds_ABI(
    UserPrincipalsGetter_C *pgetter,
    int *returns_surrogate_ids,
    int allow_exceptions
    );

EXTERN_C_SPEC_ DLL_SPEC_ int
UserPrincipalsGetter_ReturnSurrogateIds_ABI(
    UserPrincipalsGetter_C *pgetter,
    int return_surrogate_ids,
    int allow_exceptions
    );

/* InstrumentInfoGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
InstrumentInfoGetter_Create_ABI( struct Credentials *pcreds,
                                     int search_type,
                                     const char* query_string,
                                     InstrumentInfoGetter_C *pgetter,
                                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
InstrumentInfoGetter_Destroy_ABI( InstrumentInfoGetter_C *pgetter,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
InstrumentInfoGetter_GetSearchType_ABI( InstrumentInfoGetter_C *pgetter,
                                              int *search_type,
                                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
InstrumentInfoGetter_GetQueryString_ABI( InstrumentInfoGetter_C *pgetter,
                                              char **buf,
                                              size_t *n,
                                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
InstrumentInfoGetter_SetQuery_ABI( InstrumentInfoGetter_C *pgetter,
                                       int search_type,
                                       const char* query_string,
                                       int allow_exceptions );

#ifndef __cplusplus
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
{ return APIGetter_Get_ABI(pgetter, buf, n, 0); }

inline int
APIGetter_Close(Getter_C *pgetter)
{ return APIGetter_Close_ABI(pgetter, 0); }

inline int
APIGetter_IsClosed(Getter_C *pgetter, int*b)
{ return APIGetter_IsClosed_ABI(pgetter,b, 0); }

inline int
APIGetter_SetWaitMSec(unsigned long long msec)
{ return APIGetter_SetWaitMSec_ABI(msec, 0); }

inline int
APIGetter_GetWaitMSec(unsigned long long *msec)
{ return APIGetter_GetWaitMSec_ABI(msec, 0); }

inline int
APIGetter_GetDefWaitMSec(unsigned long long *msec)
{ return APIGetter_GetDefWaitMSec_ABI(msec, 0); }

/* declare derived versions of Get, Close, IsClosed for each getter*/
#define DECL_WRAPPED_API_GETTER_BASE_FUNCS(name) \
inline int \
name##_Get(name##_C *pgetter, char** buf, size_t *n) \
{ return APIGetter_Get_ABI( (Getter_C*)pgetter, buf, n, 0); } \
\
inline int \
name##_Close(name##_C *pgetter) \
{ return APIGetter_Close_ABI( (Getter_C*)pgetter, 0); } \
\
inline int \
name##_IsClosed(name##_C *pgetter, int *b) \
{ return APIGetter_IsClosed_ABI( (Getter_C*)pgetter, b, 0); }


/* QuoteGetter -> QuoteGetterImpl */
inline int
QuoteGetter_Create( struct Credentials *pcreds,
                      const char* symbol,
                      QuoteGetter_C *pgetter )
{ return QuoteGetter_Create_ABI(pcreds, symbol, pgetter, 0); }

inline int
QuoteGetter_Destroy(QuoteGetter_C *getter)
{ return QuoteGetter_Destroy_ABI(getter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(QuoteGetter)

inline int
QuoteGetter_GetSymbol(QuoteGetter_C *getter, char **buf, size_t *n)
{ return QuoteGetter_GetSymbol_ABI(getter, buf, n, 0); }

inline int
QuoteGetter_SetSymbol(QuoteGetter_C *getter, const char *symbol)
{ return QuoteGetter_SetSymbol_ABI(getter, symbol, 0); }


/* QuotesGetter -> QuotesGetterImpl */
inline int
QuotesGetter_Create( struct Credentials *pcreds,
                      const char** symbols,
                      size_t nsymbols,
                      QuotesGetter_C *pgetter )
{ return QuotesGetter_Create_ABI(pcreds, symbols, nsymbols, pgetter, 0); }

inline int
QuotesGetter_Destroy(QuotesGetter_C *getter)
{ return QuotesGetter_Destroy_ABI(getter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(QuotesGetter)

inline int
QuotesGetter_GetSymbols(QuotesGetter_C *getter, char ***buf, size_t *n)
{ return QuotesGetter_GetSymbols_ABI(getter, buf, n, 0); }

inline int
QuotesGetter_SetSymbols(QuotesGetter_C *getter, const char **symbols,
                        size_t nsymbols)
{ return QuotesGetter_SetSymbols_ABI(getter, symbols, nsymbols, 0); }


/* MarketHoursGetter */
inline int
MarketHoursGetter_Create( struct Credentials *pcreds,
                             MarketType market_type,
                             const char* date,
                             MarketHoursGetter_C *pgetter )
{ return MarketHoursGetter_Create_ABI(pcreds, (int)market_type, date, pgetter, 0); }

inline int
MarketHoursGetter_Destroy(MarketHoursGetter_C *pgetter, int allow_exceptions)
{ return MarketHoursGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(MarketHoursGetter)

inline int
MarketHoursGetter_GetMarketType( MarketHoursGetter_C *pgetter,
                                     MarketType *market_type )
{ return MarketHoursGetter_GetMarketType_ABI(pgetter, (int*)market_type, 0); }

inline int
MarketHoursGetter_SetMarketType( MarketHoursGetter_C *pgetter,
                                     MarketType market_type )
{ return MarketHoursGetter_SetMarketType_ABI(pgetter, (int)market_type, 0); }

inline int
MarketHoursGetter_GetDate(MarketHoursGetter_C *pgetter, char **buf, size_t *n)
{ return MarketHoursGetter_GetDate_ABI(pgetter, buf, n, 0); }

inline int
MarketHoursGetter_SetDate( MarketHoursGetter_C *pgetter, const char* date )
{ return MarketHoursGetter_SetDate_ABI(pgetter, date, 0); }

/* MoversGetter */
inline int
MoversGetter_Create( struct Credentials *pcreds,
                       MoversIndex index,
                       MoversDirectionType direction_type,
                       MoversChangeType change_type,
                       MoversGetter_C *pgetter)
{ return MoversGetter_Create_ABI(pcreds, (int)index, (int)direction_type,
                                 (int)change_type, pgetter, 0);}

inline int
MoversGetter_Destroy( MoversGetter_C *pgetter)
{ return MoversGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(MoversGetter)

inline int
MoversGetter_GetIndex( MoversGetter_C *pgetter, MoversIndex *index)
{ return MoversGetter_GetIndex_ABI(pgetter, (int*)index, 0); }

inline int
MoversGetter_SetIndex( MoversGetter_C *pgetter,  MoversIndex index)
{ return MoversGetter_SetIndex_ABI(pgetter, (int)index, 0); }

inline int
MoversGetter_GetDirectionType( MoversGetter_C *pgetter,
                                  MoversDirectionType *direction_type )
{ return MoversGetter_GetDirectionType_ABI(pgetter, (int*)direction_type, 0); }

inline int
MoversGetter_SetDirectionType( MoversGetter_C *pgetter,
                                  MoversDirectionType direction_type)
{ return MoversGetter_SetDirectionType_ABI(pgetter, (int)direction_type, 0); }

inline int
MoversGetter_GetChangeType( MoversGetter_C *pgetter,
                               MoversChangeType *change_type )
{ return MoversGetter_GetChangeType_ABI(pgetter, (int*)change_type, 0); }

inline int
MoversGetter_SetChangeType( MoversGetter_C *pgetter,
                               MoversChangeType change_type)
{ return MoversGetter_SetChangeType_ABI(pgetter, (int)change_type, 0); }


/* HistoricalPeriodGetter */
inline int
HistoricalPeriodGetter_Create( struct Credentials *pcreds,
                               const char* symbol,
                               int period_type,
                               unsigned int period,
                               int frequency_type,
                               unsigned int frequency,
                               int extended_hours,
                               HistoricalPeriodGetter_C *pgetter )
{
    return HistoricalPeriodGetter_Create_ABI(pcreds, symbol, period_type, period,
                                             frequency_type, frequency,
                                             extended_hours, pgetter, 0);
}

inline int
HistoricalPeriodGetter_Destroy( HistoricalPeriodGetter_C *pgetter )
{ return HistoricalPeriodGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(HistoricalPeriodGetter)

inline int
HistoricalPeriodGetter_GetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  char **buf,
                                  size_t *n )
{ return HistoricalGetterBase_GetSymbol_ABI( (Getter_C*)pgetter, buf, n, 0); }

inline int
HistoricalPeriodGetter_SetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  const char *symbol )
{ return HistoricalGetterBase_SetSymbol_ABI( (Getter_C*)pgetter, symbol, 0); }

inline int
HistoricalPeriodGetter_GetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     unsigned int *frequency )
{ return HistoricalGetterBase_GetFrequency_ABI( (Getter_C*)pgetter,
                                                 frequency, 0); }

inline int
HistoricalPeriodGetter_GetFrequencyType( HistoricalPeriodGetter_C *pgetter,
                                         FrequencyType *frequency_type )
{ return HistoricalGetterBase_GetFrequencyType_ABI( (Getter_C*)pgetter,
                                                    (int*)frequency_type, 0); }

inline int
HistoricalPeriodGetter_IsExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                               int *is_extended_hours )
{ return HistoricalGetterBase_IsExtendedHours_ABI( (Getter_C*)pgetter,
                                                   is_extended_hours, 0); }

inline int
HistoricalPeriodGetter_SetExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                               int is_extended_hours )
{ return HistoricalGetterBase_SetExtendedHours_ABI( (Getter_C*)pgetter,
                                                    is_extended_hours, 0); }

inline int
HistoricalPeriodGetter_GetPeriodType( HistoricalPeriodGetter_C *pgetter,
                                      PeriodType *period_type )
{ return HistoricalPeriodGetter_GetPeriodType_ABI( pgetter, (int*)period_type, 0); }


inline int
HistoricalPeriodGetter_GetPeriod(HistoricalPeriodGetter_C *pgetter,
                                 unsigned int *period)
{ return HistoricalPeriodGetter_GetPeriod_ABI( pgetter, period, 0); }

inline int
HistoricalPeriodGetter_SetPeriod( HistoricalPeriodGetter_C *pgetter,
                                   PeriodType period_type,
                                   unsigned int period )
{ return HistoricalPeriodGetter_SetPeriod_ABI( pgetter, (int)period_type, period, 0); }

inline int
HistoricalPeriodGetter_SetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency )
{ return HistoricalPeriodGetter_SetFrequency_ABI( pgetter, (int)frequency_type,
                                                  frequency, 0); }

/* HistoricalRangeGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_Create( struct Credentials *pcreds,
                                   const char* symbol,
                                   FrequencyType frequency_type,
                                   unsigned int frequency,
                                   unsigned long long start_msec_since_epoch,
                                   unsigned long long end_msec_since_epoch,
                                   int extended_hours,
                                   HistoricalRangeGetter_C *pgetter )
{
    return HistoricalRangeGetter_Create_ABI(pcreds, symbol, (int)frequency_type,
                                          frequency, start_msec_since_epoch,
                                          end_msec_since_epoch, extended_hours,
                                          pgetter, 0);
}

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalRangeGetter_Destroy( HistoricalRangeGetter_C *pgetter )
{ return HistoricalRangeGetter_Destroy_ABI( pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(HistoricalRangeGetter)

inline int
HistoricalRangeGetter_GetSymbol( HistoricalRangeGetter_C *pgetter,
                                  char **buf,
                                  size_t *n )
{ return HistoricalGetterBase_GetSymbol_ABI( (Getter_C*)pgetter, buf, n, 0); }

inline int
HistoricalRangeGetter_SetSymbol( HistoricalRangeGetter_C *pgetter,
                                  const char *symbol )
{ return HistoricalGetterBase_SetSymbol_ABI( (Getter_C*)pgetter, symbol, 0); }

inline int
HistoricalRangeGetter_GetFrequency( HistoricalRangeGetter_C *pgetter,
                                    unsigned int *frequency )
{ return HistoricalGetterBase_GetFrequency_ABI( (Getter_C*)pgetter,
                                                 frequency, 0); }

inline int
HistoricalRangeGetter_GetFrequencyType( HistoricalRangeGetter_C *pgetter,
                                        FrequencyType *frequency_type )
{ return HistoricalGetterBase_GetFrequencyType_ABI( (Getter_C*)pgetter,
                                                    (int*)frequency_type, 0); }

inline int
HistoricalRangeGetter_IsExtendedHours( HistoricalRangeGetter_C *pgetter,
                                               int *is_extended_hours )
{ return HistoricalGetterBase_IsExtendedHours_ABI( (Getter_C*)pgetter,
                                                   is_extended_hours, 0); }

inline int
HistoricalRangeGetter_SetExtendedHours( HistoricalRangeGetter_C *pgetter,
                                               int is_extended_hours )
{ return HistoricalGetterBase_SetExtendedHours_ABI( (Getter_C*)pgetter,
                                                    is_extended_hours, 0); }

inline int
HistoricalRangeGetter_GetEndMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *end_msec )
{ return HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(pgetter, end_msec, 0); }

inline int
HistoricalRangeGetter_SetEndMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long end_msec )
{ return HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(pgetter, end_msec, 0); }

inline int
HistoricalRangeGetter_GetStartMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *start_msec )
{ return HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(pgetter, start_msec, 0); }

inline int
HistoricalRangeGetter_SetStartMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long start_msec )
{ return HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI(pgetter, start_msec, 0); }

inline int
HistoricalRangeGetter_SetFrequency( HistoricalRangeGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency)
{ return HistoricalRangeGetter_SetFrequency_ABI(pgetter, (int)frequency_type,
                                                frequency, 0); }

/* OptionChainGetter */
inline int
OptionChainGetter_Create( struct Credentials *pcreds,
                          const char* symbol,
                          OptionStrikesType strikes_type,
                          OptionStrikesValue strikes_value,
                          OptionContractType contract_type,
                          int include_quotes,
                          const char* from_date,
                          const char* to_date,
                          OptionExpMonth exp_month,
                          OptionType option_type,
                          OptionChainGetter_C *pgetter )
{
    return OptionChainGetter_Create_ABI( pcreds, symbol, (int)strikes_type,
                                         strikes_value, (int)contract_type,
                                         include_quotes, from_date, to_date,
                                         (int)exp_month, (int)option_type,
                                         pgetter, 0 );
}

inline int
OptionChainGetter_Destroy(OptionChainGetter_C *pgetter )
{ return OptionChainGetter_Destroy_ABI(pgetter, 0); }

/* declare derived(and base) versions of OptionChain methods for derived getters*/
#define DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(name) \
inline int \
name##_GetSymbol(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetSymbol_ABI( (OptionChainGetter_C*)pgetter, \
                                          buf, n, 0); } \
\
inline int \
name##_SetSymbol(name##_C *pgetter, const char *symbol) \
{ return OptionChainGetter_SetSymbol_ABI( (OptionChainGetter_C*)pgetter, \
                                           symbol, 0); } \
\
inline int \
name##_GetStrikes( name##_C *pgetter, \
                   OptionStrikesType *strikes_type, \
                   OptionStrikesValue *strikes_value ) \
{ return OptionChainGetter_GetStrikes_ABI( (OptionChainGetter_C*)pgetter, \
                                           (int*)strikes_type, \
                                           strikes_value, 0); } \
\
inline int \
name##_SetStrikes( name##_C *pgetter, \
                   OptionStrikesType strikes_type, \
                   OptionStrikesValue strikes_value ) \
{ return OptionChainGetter_SetStrikes_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int)strikes_type, \
                                            strikes_value, 0); } \
\
inline int \
name##_GetContractType( name##_C *pgetter, OptionContractType *contract_type ) \
{ return OptionChainGetter_GetContractType_ABI( (OptionChainGetter_C*)pgetter, \
                                                (int*)contract_type, 0); } \
\
inline int \
name##_SetContractType( name##_C *pgetter, OptionContractType contract_type ) \
{ return OptionChainGetter_SetContractType_ABI( (OptionChainGetter_C*)pgetter, \
                                                (int)contract_type, 0); } \
\
inline int \
name##_IncludesQuotes( name##_C *pgetter, int *includes_quotes ) \
{ return OptionChainGetter_IncludesQuotes_ABI( (OptionChainGetter_C*)pgetter, \
                                                includes_quotes, 0); } \
\
inline int \
name##_IncludeQuotes( name##_C *pgetter, int include_quotes ) \
{ return OptionChainGetter_IncludeQuotes_ABI( (OptionChainGetter_C*)pgetter, \
                                              include_quotes, 0); } \
\
inline int \
name##_GetFromDate(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetFromDate_ABI( (OptionChainGetter_C*)pgetter, \
                                             buf, n, 0); } \
\
inline int \
name##_SetFromDate(name##_C *pgetter, const char *date) \
{ return OptionChainGetter_SetFromDate_ABI( (OptionChainGetter_C*)pgetter, \
                                             date, 0); } \
\
inline int \
name##_GetToDate(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetToDate_ABI( (OptionChainGetter_C*)pgetter, \
                                          buf, n, 0); } \
\
inline int \
name##_SetToDate(name##_C *pgetter, const char *date) \
{ return OptionChainGetter_SetToDate_ABI( (OptionChainGetter_C*)pgetter, \
                                          date, 0); } \
\
inline int \
name##_GetExpMonth(name##_C *pgetter, OptionExpMonth *exp_month) \
{ return OptionChainGetter_GetExpMonth_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int*)exp_month, 0); } \
\
inline int \
name##_SetExpMonth(name##_C *pgetter, OptionExpMonth exp_month) \
{ return OptionChainGetter_SetExpMonth_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int)exp_month, 0); } \
\
inline int \
name##_GetOptionType(name##_C *pgetter, OptionType *option_type) \
{ return OptionChainGetter_GetOptionType_ABI( (OptionChainGetter_C*)pgetter, \
                                               (int*)option_type, 0); } \
\
inline int \
name##_SetOptionType(name##_C *pgetter, OptionType option_type) \
{ return OptionChainGetter_SetOptionType_ABI( (OptionChainGetter_C*)pgetter, \
                                               (int)option_type, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OptionChainGetter)
DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(OptionChainGetter)

/* OptionChainStrategyGetter */
inline int
OptionChainStrategyGetter_Create( struct Credentials *pcreds,
                                  const char* symbol,
                                  OptionStrategyType strategy_type,
                                  double spread_interval,
                                  OptionStrikesType strikes_type,
                                  OptionStrikesValue strikes_value,
                                  OptionContractType contract_type,
                                  int include_quotes,
                                  const char* from_date,
                                  const char* to_date,
                                  OptionExpMonth exp_month,
                                  OptionType option_type,
                                  OptionChainStrategyGetter_C *pgetter )
{
    return OptionChainStrategyGetter_Create_ABI( pcreds, symbol, (int)strategy_type,
                                                 spread_interval, (int)strikes_type,
                                                 strikes_value, (int)contract_type,
                                                 include_quotes, from_date, to_date,
                                                 (int)exp_month, (int)option_type,
                                                 pgetter, 0 );
}

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OptionChainStrategyGetter)

DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(OptionChainStrategyGetter)

inline int
OptionChainStrategyGetter_Destroy( OptionChainStrategyGetter_C *pgetter )
{ return OptionChainStrategyGetter_Destroy_ABI( pgetter, 0); }

inline int
OptionChainStrategyGetter_GetStrategy( OptionChainStrategyGetter_C *pgetter,
                                       OptionStrategyType *strategy_type,
                                       double *spread_interval )
{
    return OptionChainStrategyGetter_GetStrategy_ABI(pgetter, (int*)strategy_type,
                                                     spread_interval, 0);
}

inline int
OptionChainStrategyGetter_SetStrategy( OptionChainStrategyGetter_C *pgetter,
                                        OptionStrategyType strategy_type,
                                        double spread_interval )
{
    return OptionChainStrategyGetter_SetStrategy_ABI(pgetter, (int)strategy_type,
                                                     spread_interval, 0);
}

/* OptionChainAnalyticalGetter */
inline int
OptionChainAnalyticalGetter_Create( struct Credentials *pcreds,
                                      const char* symbol,
                                      double volatility,
                                      double underlying_price,
                                      double interest_rate,
                                      unsigned int days_to_exp,
                                      OptionStrikesType strikes_type,
                                      OptionStrikesValue strikes_value,
                                      OptionContractType contract_type,
                                      int include_quotes,
                                      const char* from_date,
                                      const char* to_date,
                                      OptionExpMonth exp_month,
                                      OptionType option_type,
                                      OptionChainAnalyticalGetter_C *pgetter )
{
    return OptionChainAnalyticalGetter_Create_ABI( pcreds, symbol, volatility,
                                                   underlying_price, interest_rate,
                                                   days_to_exp, (int)strikes_type,
                                                   strikes_value, (int)contract_type,
                                                   include_quotes, from_date,
                                                   to_date, (int)exp_month,
                                                   (int)option_type, pgetter, 0);
}

inline int
OptionChainAnalyticalGetter_Destroy( OptionChainAnalyticalGetter_C *pgetter )
{ return OptionChainAnalyticalGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OptionChainAnalyticalGetter)

DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(OptionChainAnalyticalGetter)

inline int
OptionChainAnalyticalGetter_GetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double *volatility
    )
{ return OptionChainAnalyticalGetter_GetVolatility_ABI(pgetter, volatility, 0); }

inline int
OptionChainAnalyticalGetter_SetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double volatility
    )
{ return OptionChainAnalyticalGetter_SetVolatility_ABI(pgetter, volatility, 0); }

inline int
OptionChainAnalyticalGetter_GetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double *underlying_price
    )
{
    return OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI(
        pgetter, underlying_price, 0
        );
}

inline int
OptionChainAnalyticalGetter_SetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double underlying_price
    )
{
    return OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI(
        pgetter, underlying_price, 0
        );
}

inline int
OptionChainAnalyticalGetter_GetInterestRate(
    OptionChainAnalyticalGetter_C *pgetter,
    double *interest_rate
    )
{
    return OptionChainAnalyticalGetter_GetInterestRate_ABI(
        pgetter, interest_rate, 0
        );
}

inline int
OptionChainAnalyticalGetter_SetInterestRate(
    OptionChainAnalyticalGetter_C *pgetter,
    double interest_rate
    )
{
    return OptionChainAnalyticalGetter_SetInterestRate_ABI(
        pgetter, interest_rate, 0
        );
}

inline int
OptionChainAnalyticalGetter_GetDaysToExp(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int *days_to_exp
    )
{ return OptionChainAnalyticalGetter_GetDaysToExp_ABI(pgetter, days_to_exp, 0); }

inline int
OptionChainAnalyticalGetter_SetDaysToExp(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int days_to_exp
    )
{ return OptionChainAnalyticalGetter_SetDaysToExp_ABI(pgetter, days_to_exp, 0); }

/* AccountGetterBase */
#define DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(name) \
inline int \
name##_GetAccountId( name##_C *pgetter, char **buf, size_t *n) \
{ return AccountGetterBase_GetAccountId_ABI( (Getter_C*)pgetter, buf, n, 0); } \
\
inline int \
name##_SetAccountId( name##_C *pgetter, const char *symbol ) \
{ return AccountGetterBase_SetAccountId_ABI( (Getter_C*)pgetter, symbol, 0); }

/* AccountInfoGetter */
inline int
AccountInfoGetter_Create( struct Credentials *pcreds,
                                 const char* account_id,
                                 int positions,
                                 int orders,
                                 AccountInfoGetter_C *pgetter )
{ return AccountInfoGetter_Create_ABI(pcreds, account_id, positions, orders,
                                      pgetter, 0); }

inline int
AccountInfoGetter_Destroy( AccountInfoGetter_C *pgetter)
{ return AccountInfoGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(AccountInfoGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(AccountInfoGetter)

inline int
AccountInfoGetter_ReturnsPositions( AccountInfoGetter_C *pgetter,
                                            int *returns_positions)
{ return AccountInfoGetter_ReturnsPositions_ABI(pgetter, returns_positions, 0); }

inline int
AccountInfoGetter_ReturnPositions( AccountInfoGetter_C *pgetter,
                                            int return_positions )
{ return AccountInfoGetter_ReturnPositions_ABI(pgetter, return_positions, 0); }

inline int
AccountInfoGetter_ReturnsOrders( AccountInfoGetter_C *pgetter,
                                 int *returns_orders)
{ return AccountInfoGetter_ReturnsOrders_ABI(pgetter, returns_orders, 0); }

inline int
AccountInfoGetter_ReturnOrders( AccountInfoGetter_C *pgetter,
                                            int return_orders )
{ return AccountInfoGetter_ReturnOrders_ABI(pgetter, return_orders, 0); }

/* PreferencesGetter */
inline int
PreferencesGetter_Create( struct Credentials *pcreds,
                                 const char* account_id,
                                 PreferencesGetter_C *pgetter )
{ return PreferencesGetter_Create_ABI(pcreds, account_id, pgetter, 0); }

inline int
PreferencesGetter_Destroy( PreferencesGetter_C *pgetter)
{ return PreferencesGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(PreferencesGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(PreferencesGetter)

/* StreamerSubscriptionKeysGetter */
inline int
StreamerSubscriptionKeysGetter_Create(
    struct Credentials *pcreds,
     const char* account_id,
     StreamerSubscriptionKeysGetter_C *pgetter )
{ return StreamerSubscriptionKeysGetter_Create_ABI(pcreds, account_id, pgetter, 0); }

inline int
StreamerSubscriptionKeysGetter_Destroy(StreamerSubscriptionKeysGetter_C *pgetter)
{ return StreamerSubscriptionKeysGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(StreamerSubscriptionKeysGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(StreamerSubscriptionKeysGetter)

/* TransactionHistoryGetter */
inline int
TransactionHistoryGetter_Create( struct Credentials *pcreds,
                                 const char* account_id,
                                 TransactionType transaction_type,
                                 const char* symbol,
                                 const char* start_date,
                                 const char* end_date,
                                 TransactionHistoryGetter_C *pgetter )
{
    return TransactionHistoryGetter_Create_ABI( pcreds, account_id,
                                               (int)transaction_type, symbol,
                                               start_date, end_date, pgetter, 0);
}

inline int
TransactionHistoryGetter_Destroy( TransactionHistoryGetter_C *pgetter )
{ return TransactionHistoryGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(TransactionHistoryGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(TransactionHistoryGetter)

inline int
TransactionHistoryGetter_GetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType *transaction_type )
{
    return TransactionHistoryGetter_GetTransactionType_ABI(
        pgetter, (int*)transaction_type, 0
    );
}

inline int
TransactionHistoryGetter_SetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType transaction_type )
{
    return TransactionHistoryGetter_SetTransactionType_ABI(
        pgetter, (int)transaction_type, 0
    );
}

inline int
TransactionHistoryGetter_GetSymbol( TransactionHistoryGetter_C *pgetter,
                                    char **buf,
                                    size_t *n )
{ return TransactionHistoryGetter_GetSymbol_ABI(pgetter, buf, n, 0); }

inline int
TransactionHistoryGetter_SetSymbol( TransactionHistoryGetter_C *pgetter,
                                    const char* symbol )
{ return TransactionHistoryGetter_SetSymbol_ABI(pgetter, symbol, 0); }

inline int
TransactionHistoryGetter_GetStartDate( TransactionHistoryGetter_C *pgetter,
                                       char **buf,
                                       size_t *n )
{ return TransactionHistoryGetter_GetStartDate_ABI(pgetter, buf, n, 0); }

inline int
TransactionHistoryGetter_SetStartDate( TransactionHistoryGetter_C *pgetter,
                                       const char* start_date )
{ return TransactionHistoryGetter_SetStartDate_ABI(pgetter, start_date, 0); }

inline int
TransactionHistoryGetter_GetEndDate( TransactionHistoryGetter_C *pgetter,
                                     char **buf,
                                     size_t *n )
{ return TransactionHistoryGetter_GetEndDate_ABI(pgetter, buf, n, 0); }

inline int
TransactionHistoryGetter_SetEndDate( TransactionHistoryGetter_C *pgetter,
                                     const char* end_date )
{ return TransactionHistoryGetter_SetEndDate_ABI(pgetter, end_date, 0); }

/* IndividualTransactionHistoryGetter */
inline int
IndividualTransactionHistoryGetter_Create(
    struct Credentials *pcreds,
    const char* account_id,
    const char* transaction_id,
    IndividualTransactionHistoryGetter_C *pgetter )
{
    return IndividualTransactionHistoryGetter_Create_ABI(
        pcreds, account_id, transaction_id, pgetter, 0
        );
}

inline int
IndividualTransactionHistoryGetter_Destroy(
    IndividualTransactionHistoryGetter_C *pgetter )
{ return IndividualTransactionHistoryGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(IndividualTransactionHistoryGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(IndividualTransactionHistoryGetter)

inline int
IndividualTransactionHistoryGetter_GetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n )
{
    return IndividualTransactionHistoryGetter_GetTransactionId_ABI(
        pgetter, buf, n, 0
        );
}

inline int
IndividualTransactionHistoryGetter_SetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    const char* transaction_id )
{
    return IndividualTransactionHistoryGetter_SetTransactionId_ABI(
            pgetter, transaction_id, 0
            );
}

/* UserPrincipalsGetter */
inline int
UserPrincipalsGetter_Create( struct Credentials *pcreds,
                                     int streamer_subscription_keys,
                                     int streamer_connection_info,
                                     int preferences,
                                     int surrogate_ids,
                                     UserPrincipalsGetter_C *pgetter)
{
    return UserPrincipalsGetter_Create_ABI(
        pcreds, streamer_subscription_keys, streamer_connection_info,
        preferences, surrogate_ids, pgetter, 0
        );
}

inline int
UserPrincipalsGetter_Destroy( UserPrincipalsGetter_C *pgetter)
{ return UserPrincipalsGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(UserPrincipalsGetter)

inline int
UserPrincipalsGetter_ReturnsSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                              int *returns_subscription_keys )
{
    return UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI(
        pgetter, returns_subscription_keys, 0
        );
}


inline int
UserPrincipalsGetter_ReturnSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                             int return_subscription_keys)
{
    return UserPrincipalsGetter_ReturnSubscriptionKeys_ABI(
        pgetter, return_subscription_keys, 0
        );
}

inline int
UserPrincipalsGetter_ReturnsConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                            int *returns_connection_info )
{
    return UserPrincipalsGetter_ReturnsConnectionInfo_ABI(
        pgetter, returns_connection_info, 0
        );
}


inline int
UserPrincipalsGetter_ReturnConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                           int return_connection_info )
{
    return UserPrincipalsGetter_ReturnConnectionInfo_ABI(
            pgetter, return_connection_info, 0
            );
}

inline int
UserPrincipalsGetter_ReturnsPreferences( UserPrincipalsGetter_C *pgetter,
                                         int *returns_preferences)
{
    return UserPrincipalsGetter_ReturnsPreferences_ABI(
            pgetter, returns_preferences, 0
            );
}

inline int
UserPrincipalsGetter_ReturnPreferences( UserPrincipalsGetter_C *pgetter,
                                        int return_preferences )
{
    return UserPrincipalsGetter_ReturnPreferences_ABI(
            pgetter, return_preferences, 0
            );
}

inline int
UserPrincipalsGetter_ReturnsSurrogateIds( UserPrincipalsGetter_C *pgetter,
                                          int *returns_surrogate_ids )
{
    return UserPrincipalsGetter_ReturnsSurrogateIds_ABI(
            pgetter, returns_surrogate_ids, 0
            );
}

inline int
UserPrincipalsGetter_ReturnSurrogateIds(  UserPrincipalsGetter_C *pgetter,
                                          int return_surrogate_ids )
{
    return UserPrincipalsGetter_ReturnSurrogateIds_ABI(
            pgetter, return_surrogate_ids, 0
            );
}

/* InstrumentInfoGetter */
inline int
InstrumentInfoGetter_Create( struct Credentials *pcreds,
                                     InstrumentSearchType search_type,
                                     const char* query_string,
                                     InstrumentInfoGetter_C *pgetter )
{
    return InstrumentInfoGetter_Create_ABI( pcreds, (int)search_type,
                                            query_string, pgetter, 0 );
}


inline int
InstrumentInfoGetter_Destroy( InstrumentInfoGetter_C *pgetter )
{ return InstrumentInfoGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(InstrumentInfoGetter)

inline int
InstrumentInfoGetter_GetSearchType( InstrumentInfoGetter_C *pgetter,
                                    InstrumentSearchType *search_type )
{ return InstrumentInfoGetter_GetSearchType_ABI(pgetter, (int*)search_type, 0); }

inline int
InstrumentInfoGetter_GetQueryString( InstrumentInfoGetter_C *pgetter,
                                     char **buf,
                                     size_t *n )
{ return InstrumentInfoGetter_GetQueryString_ABI(pgetter, buf, n, 0); }

inline int
InstrumentInfoGetter_SetQuery( InstrumentInfoGetter_C *pgetter,
                               InstrumentSearchType search_type,
                               const char* query_string )
{ return InstrumentInfoGetter_SetQuery_ABI(pgetter, (int)search_type,
                                           query_string, 0); }


#else

/* C++ Interface */

namespace tdma{

/*
template<typename T>
struct func_traits;

template<typename R, typename ...Args>
struct func_traits<std::function<R(Args...)>>{
    template<size_t i>
    struct arg{
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};
*/

class APIGetter{
public:
    typedef Getter_C CType;

    static const int TYPE_ID_LOW = 1;
    static const int TYPE_ID_HIGH = 16;

private:
    std::unique_ptr<CType> _cgetter;

protected:
    template<typename CTy=CType>
    CTy*
    cgetter() const
    { return reinterpret_cast<CTy*>( const_cast<CType*>(_cgetter.get()) ); }

    template<typename CTy>
    void
    destroy_cgetter( int(*destroy)(CTy*, int) )
    {
        if( _cgetter && _cgetter->obj ){
            destroy(cgetter<CTy>(), 1);
        }
        _cgetter.reset();
    }

    template<typename CTy>
    std::string
    str_from_abi( int(*abicall)(CTy*, char**, size_t*, int) ) const
    {
        char *buf;
        size_t n;
        abicall(cgetter<CTy>(), &buf, &n, 1);
        std::string s(buf, n-1);
        if(buf)
            free(buf);
        return s;
    }

    // TODO deduce CTy from F
    template<typename CTy, typename F, typename... Args>
    APIGetter(
            CTy _,
            F create_func,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type *v,
            Args... args
            )
        :
            _cgetter(new CType{0,0})
        {
            /* IF WE THROW BEFORE HERE WE MAY LEAK IN DERIVED */
            if( create_func )
                create_func(args..., cgetter<CTy>(), 1);
        }

    template<typename CTy>
    APIGetter(
            CTy _,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type *v
                 = nullptr )
        :
            _cgetter(new CType{0,0})
        {
        }

    APIGetter( APIGetter&& getter ) = default;

    APIGetter&
    operator=( APIGetter&& getter ) = default;

    // NEED TO BE SURE destroy is called in derived so _cgetter still exists
    virtual
    ~APIGetter(){}

public:
    static std::chrono::milliseconds
    get_def_wait_msec()
    {
        unsigned long long w;
        APIGetter_GetDefWaitMSec_ABI(&w, 1);
        return std::chrono::milliseconds(w);
    }

    static std::chrono::milliseconds
    get_wait_msec()
    {
        unsigned long long w;
        APIGetter_GetWaitMSec_ABI(&w, 1);
        return std::chrono::milliseconds(w);
    }

    static void
    set_wait_msec(std::chrono::milliseconds msec)
    {
        APIGetter_SetWaitMSec_ABI(
            static_cast<unsigned long long>(msec.count()), 1
            );
    }

    json
    get()
    {
        char *buf;
        size_t n;
        APIGetter_Get_ABI(_cgetter.get(), &buf, &n, 1);
        json j = (n > 1) ? json::parse(std::string(buf)) : json();
        if(buf)
            free(buf);
        return j;
    }

    void
    close()
    { APIGetter_Close_ABI(_cgetter.get(), 1); }

    bool
    is_closed() const
    {
        int b;
        APIGetter_IsClosed_ABI(_cgetter.get(), &b, 1);
        return static_cast<bool>(b);
    }
};


class QuoteGetter
        : public APIGetter {
public:
    typedef QuoteGetter_C CType;

    static const int TYPE_ID_LOW = 1;
    static const int TYPE_ID_HIGH = 1;

    QuoteGetter( Credentials& creds, const std::string& symbol )
        :
            APIGetter( QuoteGetter_C{},
                       QuoteGetter_Create_ABI,
                       nullptr,
                       CPP_to_C(creds),
                       CPP_to_C(symbol) )
        {
        }

    ~QuoteGetter()
        {
            destroy_cgetter(QuoteGetter_Destroy_ABI);
        }

    QuoteGetter( QuoteGetter&& ) = default;

    QuoteGetter&
    operator=( QuoteGetter&& getter ) = default;

    std::string
    get_symbol() const
    { return str_from_abi<QuoteGetter_C>(QuoteGetter_GetSymbol_ABI); }

    void
    set_symbol(const std::string& symbol)
    { QuoteGetter_SetSymbol_ABI(cgetter<QuoteGetter_C>(), symbol.c_str(), 1); }
};

inline json
GetQuote(Credentials& creds, const std::string& symbol)
{ return QuoteGetter(creds, symbol).get(); }


class QuotesGetter
        : public APIGetter {

    static const char**
    set_to_cstrs(const std::set<std::string>& symbols )
    {
        if( symbols.empty() )
            throw ValueException("empty symbols set");
        const char **tmp = new const char*[symbols.size()];
        int cnt = 0;
        for(auto& s: symbols)
            tmp[cnt++] = s.c_str();
        return tmp;
    }
public:
    typedef QuotesGetter_C CType;

    static const int TYPE_ID_LOW = 2;
    static const int TYPE_ID_HIGH = 2;

    QuotesGetter(Credentials& creds, const std::set<std::string>& symbols)
        :
            APIGetter( QuotesGetter_C{} )
        {
            const char** s = nullptr;
            try{
                s = set_to_cstrs(symbols);
                QuotesGetter_Create_ABI(CPP_to_C(creds), s, symbols.size(),
                                        cgetter<QuotesGetter_C>(), 1);
            }catch(...){
                if( s ) delete[] s;
                throw;
            }
            if( s ) delete[] s;
        }

    ~QuotesGetter()
        {
            destroy_cgetter(QuotesGetter_Destroy_ABI);
        }

    QuotesGetter( QuotesGetter&& ) = default;

    QuotesGetter&
    operator=( QuotesGetter&& getter ) = default;

    std::set<std::string>
    get_symbols() const
    {
        char **buf;
        size_t n;
        std::set<std::string> strs;
        QuotesGetter_GetSymbols_ABI(cgetter<QuotesGetter_C>(), &buf, &n, 1);
        if( buf ){
            while(n--){
                char *c = buf[n];
                assert(c);
                strs.insert(c);
                free(c);
            }
            free(buf);
        }
        return strs;
    }

    void
    set_symbols(const std::set<std::string>& symbols)
    {
        const char** tmp = set_to_cstrs(symbols);
        try{
            QuotesGetter_SetSymbols_ABI(cgetter<QuotesGetter_C>(), tmp,
                                        symbols.size(), 1);
        }catch(...){
            if( tmp ) delete[] tmp;
            throw;
        }
        if( tmp ) delete[] tmp;
    }
};

inline json
GetQuotes(Credentials& creds, const std::set<std::string>& symbols)
{ return QuotesGetter(creds, symbols).get(); }

// note we only implement the single MarketType version
// could just add an 'all' field to MarketType enum
class MarketHoursGetter
        : public APIGetter {
public:
    typedef MarketHoursGetter_C CType;

    static const int TYPE_ID_LOW = 3;
    static const int TYPE_ID_HIGH = 3;

    MarketHoursGetter( Credentials& creds,
                          MarketType market_type,
                          const std::string& date )
        :
            APIGetter( MarketHoursGetter_C{},
                       MarketHoursGetter_Create_ABI,
                       nullptr,
                       CPP_to_C(creds),
                       static_cast<int>(market_type),
                       CPP_to_C(date) )
        {
        }

    ~MarketHoursGetter()
        {
            destroy_cgetter(MarketHoursGetter_Destroy_ABI);
        }

    MarketHoursGetter( MarketHoursGetter&& ) = default;

    MarketHoursGetter&
    operator=( MarketHoursGetter&& getter ) = default;

    std::string
    get_date() const
    { return str_from_abi<MarketHoursGetter_C>(MarketHoursGetter_GetDate_ABI); }

    void
    set_date(const std::string& date)
    {
        MarketHoursGetter_SetDate_ABI(
            cgetter<MarketHoursGetter_C>(), date.c_str(), true
            );
    }

    MarketType
    get_market_type() const
    {
        int m;
        MarketHoursGetter_GetMarketType_ABI(
            cgetter<MarketHoursGetter_C>(), &m, 1
            );
        return static_cast<MarketType>(m);
    }

    void
    set_market_type(MarketType market_type)
    {
        MarketHoursGetter_SetMarketType_ABI(
            cgetter<MarketHoursGetter_C>(), static_cast<int>(market_type), 1
            );
    }
};

inline json
GetMarketHours( Credentials& creds,
                  MarketType market_type,
                  const std::string& date )
{ return MarketHoursGetter(creds, market_type, date).get(); }



class MoversGetter
        : public APIGetter{
public:
    typedef MoversGetter_C CType;

    static const int TYPE_ID_LOW = 4;
    static const int TYPE_ID_HIGH = 4;

    MoversGetter( Credentials& creds,
                  MoversIndex index,
                  MoversDirectionType direction_type,
                  MoversChangeType change_type )
        :
            APIGetter( MoversGetter_C{},
                       MoversGetter_Create_ABI,
                       nullptr,
                       CPP_to_C(creds),
                       static_cast<int>(index),
                       static_cast<int>(direction_type),
                       static_cast<int>(change_type) )
        {}

    ~MoversGetter()
        {
            destroy_cgetter(MoversGetter_Destroy_ABI);
        }

    MoversGetter( MoversGetter&& ) = default;

    MoversGetter&
    operator=( MoversGetter&& getter ) = default;

    MoversIndex
    get_index() const
    {
        int m;
        MoversGetter_GetIndex_ABI(cgetter<MoversGetter_C>(), &m, 1);
        return static_cast<MoversIndex>(m);
    }

    MoversDirectionType
    get_direction_type() const
    {
        int m;
        MoversGetter_GetDirectionType_ABI(cgetter<MoversGetter_C>(), &m, 1);
        return static_cast<MoversDirectionType>(m);
    }

    MoversChangeType
    get_change_type() const
    {
        int m;
        MoversGetter_GetChangeType_ABI(cgetter<MoversGetter_C>(), &m, 1);
        return static_cast<MoversChangeType>(m);
    }

    void
    set_index(MoversIndex index)
    {
        MoversGetter_SetIndex_ABI(
            cgetter<MoversGetter_C>(), static_cast<int>(index), 1
            );
    }

    void
    set_direction_type(MoversDirectionType direction_type)
    {
        MoversGetter_SetDirectionType_ABI(
            cgetter<MoversGetter_C>(), static_cast<int>(direction_type), 1
            );
    }

    void
    set_change_type(MoversChangeType change_type)
    {
        MoversGetter_SetChangeType_ABI(
            cgetter<MoversGetter_C>(), static_cast<int>(change_type), 1
            );
    }
};

inline json
GetMovers( Credentials& creds,
           MoversIndex index,
           MoversDirectionType direction_type,
           MoversChangeType change_type )
{ return MoversGetter(creds, index, direction_type, change_type).get(); }



class HistoricalGetterBase
        : public APIGetter {
protected:
    template<typename CTy, typename F, typename... Args>
    HistoricalGetterBase( CTy _,
                             F func,
                             Credentials& creds,
                             const std::string& symbol,
                             Args... args )
        :
            APIGetter( _, func, nullptr, CPP_to_C(creds), CPP_to_C(symbol), args... )
        {
        }

    HistoricalGetterBase( HistoricalGetterBase&& ) = default;

    HistoricalGetterBase&
    operator=( HistoricalGetterBase&& getter ) = default;

public:
    typedef Getter_C CType;

    static const int TYPE_ID_LOW = 5;
    static const int TYPE_ID_HIGH = 6;

    std::string
    get_symbol() const
    { return str_from_abi<>(HistoricalGetterBase_GetSymbol_ABI); }

    unsigned int
    get_frequency() const
    {
        unsigned int f;
        HistoricalGetterBase_GetFrequency_ABI(cgetter<>(), &f, 1);
        return f;
    }

    FrequencyType
    get_frequency_type() const
    {
        int ft;
        HistoricalGetterBase_GetFrequencyType_ABI(cgetter<>(), &ft, 1);
        return static_cast<FrequencyType>(ft);
    }

    bool
    is_extended_hours() const
    {
        int et;
        HistoricalGetterBase_IsExtendedHours_ABI(cgetter<>(), &et, 1);
        return static_cast<bool>(et);
    }

    void
    set_symbol(const std::string& symbol)
    {
        HistoricalGetterBase_SetSymbol_ABI(cgetter<>(), symbol.c_str(), 1);
    }

    void
    set_extended_hours(bool extended_hours)
    {
        HistoricalGetterBase_SetExtendedHours_ABI(
            cgetter<>(), static_cast<int>(extended_hours), 1
            );
    }

};


// TODO MAKE CLEAR IN DOCS THE ORDER WE NEED TO CHANGE PERIODS/FREQUENCIES
//      BECUASE OF THE INTERNAL CHECKS/CONTINGENCIES
class HistoricalPeriodGetter
        : public HistoricalGetterBase {
public:
    typedef HistoricalPeriodGetter_C CType;

    static const int TYPE_ID_LOW = 5;
    static const int TYPE_ID_HIGH = 5;

    HistoricalPeriodGetter( Credentials& creds,
                            const std::string& symbol,
                            PeriodType period_type,
                            unsigned int period,
                            FrequencyType frequency_type,
                            unsigned int frequency,
                            bool extended_hours )
        :
            HistoricalGetterBase( HistoricalPeriodGetter_C{},
                                  HistoricalPeriodGetter_Create_ABI,
                                  creds,
                                  symbol,
                                  static_cast<int>(period_type),
                                  period,
                                  static_cast<int>(frequency_type),
                                  frequency,
                                  static_cast<int>(extended_hours) )
        {
        }

    ~HistoricalPeriodGetter()
        {
            destroy_cgetter(HistoricalPeriodGetter_Destroy_ABI);
        }

    HistoricalPeriodGetter( HistoricalPeriodGetter&& ) = default;

    HistoricalPeriodGetter&
    operator=( HistoricalPeriodGetter&& getter ) = default;

    PeriodType
    get_period_type() const
    {
        int pt;
        HistoricalPeriodGetter_GetPeriodType_ABI(
            cgetter<HistoricalPeriodGetter_C>(), &pt, 1
            );
        return static_cast<PeriodType>(pt);
    }

    unsigned int get_period() const
    {
        unsigned int p;
        HistoricalPeriodGetter_GetPeriod_ABI(
                cgetter<HistoricalPeriodGetter_C>(), &p, 1
               );
        return p;
    }

    void
    set_period(PeriodType period_type, unsigned int period)
    {
        HistoricalPeriodGetter_SetPeriod_ABI(
           cgetter<HistoricalPeriodGetter_C>(), static_cast<int>(period_type),
           period, 1
           );
    }

    void
    set_frequency(FrequencyType frequency_type, unsigned int frequency)
    {
        HistoricalPeriodGetter_SetFrequency_ABI(
           cgetter<HistoricalPeriodGetter_C>(), static_cast<int>(frequency_type),
           frequency, 1
           );
    }
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



class HistoricalRangeGetter
        : public HistoricalGetterBase {
public:
    typedef HistoricalRangeGetter_C CType;

    static const int TYPE_ID_LOW = 6;
    static const int TYPE_ID_HIGH = 6;

    HistoricalRangeGetter( Credentials& creds,
                              const std::string& symbol,
                              FrequencyType frequency_type,
                              unsigned int frequency,
                              unsigned long long start_msec_since_epoch,
                              unsigned long long end_msec_since_epoch,
                              bool extended_hours )
        :
            HistoricalGetterBase( HistoricalRangeGetter_C{},
                                  HistoricalRangeGetter_Create_ABI,
                                  creds,
                                  symbol,
                                  static_cast<int>(frequency_type),
                                  frequency,
                                  start_msec_since_epoch,
                                  end_msec_since_epoch,
                                  static_cast<int>(extended_hours) )
        {
        }

    ~HistoricalRangeGetter()
        {
            destroy_cgetter(HistoricalRangeGetter_Destroy_ABI);
        }

    HistoricalRangeGetter( HistoricalRangeGetter&& ) = default;

    HistoricalRangeGetter&
    operator=( HistoricalRangeGetter&& getter ) = default;

    unsigned long long
    get_end_msec_since_epoch() const
    {
        unsigned long long ms;
        HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(
                cgetter<HistoricalRangeGetter_C>(), &ms, 1
               );
        return ms;
    }

    unsigned long long
    get_start_msec_since_epoch() const
    {
       unsigned long long ms;
       HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(
               cgetter<HistoricalRangeGetter_C>(), &ms, 1
              );
       return ms;
    }

    void
    set_frequency(FrequencyType frequency_type, unsigned int frequency)
    {
       HistoricalRangeGetter_SetFrequency_ABI(
          cgetter<HistoricalRangeGetter_C>(), static_cast<int>(frequency_type),
          frequency, 1
          );
    }

    void
    set_end_msec_since_epoch(unsigned long long end_msec_since_epoch)
    {
       HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(
               cgetter<HistoricalRangeGetter_C>(), end_msec_since_epoch, 1
              );
    }

    void
    set_start_msec_since_epoch(unsigned long long start_msec_since_epoch)
    {
       HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI(
               cgetter<HistoricalRangeGetter_C>(), start_msec_since_epoch, 1
              );
    }
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

// TODO move inline OptionStrikes / OptionStrategy defs out of header

class OptionStrikes {
public:
    using Type = OptionStrikesType;

private:
    Type _type;
    OptionStrikesValue _value;

    OptionStrikes(Type type)
        : _type(type), _value()
    {}

public:
    OptionStrikes(Type type, OptionStrikesValue value)
        : _type(type), _value(value)
    {}

    /* our impl of the ABI layer requires def cnstr but not good conceptually */
    OptionStrikes()
        : _type(Type::none), _value()
    {}

    OptionStrikes( const OptionStrikes& ) = default;

    OptionStrikes&
    operator=( const OptionStrikes& ) = default;

    Type
    get_type() const
    { return _type; }

    OptionStrikesValue
    get_value() const
    { return _value; }

    unsigned int
    get_n_atm() const
    { return (_type == Type::n_atm) ? _value.n_atm : 0; }

    double
    get_single() const
    { return (_type == Type::single) ? _value.single : 0.0; }

    OptionRangeType
    get_range() const
    { return (_type == Type::range) ? _value.range : OptionRangeType::null; }

    static OptionStrikes
    N_ATM(unsigned int n)
    {
        if (n < 1)
            throw ValueException("number of strikes can not be < 1");
        OptionStrikes o(Type::n_atm);
        o._value.n_atm = n;
        return o;
    }

    static OptionStrikes
    Single(double price)
    {
        if (price < 0.1)
            throw ValueException("strike price can not be < 0.1");
        OptionStrikes o(Type::single);
        o._value.single = price;
        return o;
    }

    static OptionStrikes
    Range(OptionRangeType range)
    {
        if (range == OptionRangeType::null)
            throw ValueException("strike range can not be 'null'");
        OptionStrikes o(Type::range);
        o._value.range = range;
        return o;
    }
};

inline std::string // TODO move out of header
to_string(const OptionStrikes& strikes)
{
    switch (strikes.get_type()) {
    case OptionStrikes::Type::n_atm:
        return "n_atm(" + std::to_string(strikes.get_n_atm()) + ")";
    case OptionStrikes::Type::single:
        return "single(" + std::to_string(strikes.get_single()) + ")";
    case OptionStrikes::Type::range:
        return "range(" + to_string(strikes.get_range()) + ")";
    case OptionStrikes::Type::none:
        return "none()";
    default:
        throw std::runtime_error("invalid OptionStrikes::Type");
    }
}

inline std::ostream&
operator<<(std::ostream& out, const OptionStrikes& strikes)
{
    out << to_string(strikes);
    return out;
}


class OptionStrategy {
    OptionStrategyType _strategy;
    double _spread_interval;

public:
    /* our impl of the ABI layer requires def cnstr but not good conceptually */
    OptionStrategy()
        : _strategy(OptionStrategyType::vertical), _spread_interval(0)
        {}

    OptionStrategy(OptionStrategyType strategy)
        : _strategy(strategy), _spread_interval(0.0)
        {}

    OptionStrategy(OptionStrategyType strategy, double spread_interval)
        :
            _strategy(strategy),
            _spread_interval(spread_interval)
        {
            if (strategy != OptionStrategyType::covered &&
                strategy != OptionStrategyType::calendar &&
                spread_interval < .01)
            {
                throw ValueException(to_string(strategy)
                    + " strategy requires spread interval >= .01");
            }
        }

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

inline std::string // TODO move out of header
to_string(const OptionStrategy& strategy)
{
    double d = strategy.get_spread_interval();
    std::string s = to_string(strategy.get_strategy());
    return d ? (s + "(" + std::to_string(d) + ")") : s;
}

inline std::ostream&
operator<<(std::ostream& out, const OptionStrategy& strategy)
{
    out << to_string(strategy);
    return out;
}


class OptionChainGetter
        : public APIGetter {
protected:
    template<typename CTy, typename F, typename... Args>
    OptionChainGetter( CTy _,
                         F func,
                         Credentials& creds,
                         const std::string& symbol,
                         Args... args )
        :
            APIGetter( _, func, nullptr, CPP_to_C(creds), CPP_to_C(symbol), args... )
        {
        }

public:
    typedef OptionChainGetter_C CType;

    static const int TYPE_ID_LOW = 7;
    static const int TYPE_ID_HIGH = 9;

    OptionChainGetter( Credentials& creds,
                       const std::string& symbol,
                       const OptionStrikes& strikes,                    
                       OptionContractType contract_type = OptionContractType::all,
                       bool include_quotes = false,
                       const std::string& from_date = "",
                       const std::string& to_date = "",
                       OptionExpMonth exp_month = OptionExpMonth::all,
                       OptionType option_type = OptionType::all )
        :
            OptionChainGetter( OptionChainGetter_C{},
                               OptionChainGetter_Create_ABI,
                               creds,
                               symbol,
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>(include_quotes),
                               CPP_to_C(from_date),
                               CPP_to_C(to_date),
                               static_cast<int>(exp_month),
                               static_cast<int>(option_type) )
        {}

    ~OptionChainGetter()
        {
            destroy_cgetter(OptionChainGetter_Destroy_ABI);
        }

    OptionChainGetter( OptionChainGetter&& ) = default;

    OptionChainGetter&
    operator=( OptionChainGetter&& getter ) = default;

    std::string
    get_symbol() const
    { return str_from_abi<OptionChainGetter_C>(OptionChainGetter_GetSymbol_ABI); }

    OptionStrikes
    get_strikes() const
    {
        OptionStrikesValue sv;
        int st;
        OptionChainGetter_GetStrikes_ABI(
            cgetter<OptionChainGetter_C>(), &st, &sv, 1
            );
        return OptionStrikes(static_cast<OptionStrikesType>(st), sv);
    }

    OptionContractType
    get_contract_type() const
    {
        int ct;
        OptionChainGetter_GetContractType_ABI(cgetter<OptionChainGetter_C>(), &ct, 1);
        return static_cast<OptionContractType>(ct);
    }

    bool
    includes_quotes() const
    {
        int iq;
        OptionChainGetter_IncludesQuotes_ABI(cgetter<OptionChainGetter_C>(), &iq, 1);
        return static_cast<bool>(iq);
    }

    std::string
    get_from_date() const
    { return str_from_abi<OptionChainGetter_C>(OptionChainGetter_GetFromDate_ABI); }

    std::string
    get_to_date() const
    { return str_from_abi<OptionChainGetter_C>(OptionChainGetter_GetToDate_ABI); }

    OptionExpMonth
    get_exp_month() const
    {
        int em;
        OptionChainGetter_GetExpMonth_ABI(cgetter<OptionChainGetter_C>(), &em, 1);
        return static_cast<OptionExpMonth>(em);
    }

    OptionType
    get_option_type() const
    {
        int ot;
        OptionChainGetter_GetOptionType_ABI(cgetter<OptionChainGetter_C>(), &ot, 1);
        return static_cast<OptionType>(ot);
    }

    void
    set_symbol(const std::string& symbol)
    {
        OptionChainGetter_SetSymbol_ABI(
            cgetter<OptionChainGetter_C>(), symbol.c_str(), 1
            );
    }

    void
    set_strikes(OptionStrikes strikes)
    {
        OptionChainGetter_SetStrikes_ABI(
            cgetter<OptionChainGetter_C>(), static_cast<int>(strikes.get_type()),
            strikes.get_value(), 1
            );
    }

    void
    set_contract_type(OptionContractType contract_type)
    {
        OptionChainGetter_SetContractType_ABI(
            cgetter<OptionChainGetter_C>(), static_cast<int>(contract_type), 1
            );
    }

    void
    include_quotes(bool include_quotes)
    {
        OptionChainGetter_IncludeQuotes_ABI(
            cgetter<OptionChainGetter_C>(), static_cast<int>(include_quotes), 1
            );
    }

    void
    set_from_date(const std::string& from_date)
    {
        OptionChainGetter_SetFromDate_ABI(
            cgetter<OptionChainGetter_C>(), from_date.c_str(), 1
            );
    }

    void
    set_to_date(const std::string& to_date)
    {
        OptionChainGetter_SetToDate_ABI(
            cgetter<OptionChainGetter_C>(), to_date.c_str(), 1
            );
    }

    void
    set_exp_month(OptionExpMonth exp_month)
    {
        OptionChainGetter_SetExpMonth_ABI(
            cgetter<OptionChainGetter_C>(), static_cast<int>(exp_month), 1
            );
    }

    void
    set_option_type(OptionType option_type)
    {
        OptionChainGetter_SetOptionType_ABI(
            cgetter<OptionChainGetter_C>(), static_cast<int>(option_type), 1
            );
    }
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


class OptionChainStrategyGetter
        : public OptionChainGetter {
public:
    typedef OptionChainStrategyGetter_C CType;

    static const int TYPE_ID_LOW = 8;
    static const int TYPE_ID_HIGH = 8;

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
            OptionType option_type = OptionType::all )
        :
            OptionChainGetter( OptionChainStrategyGetter_C{},
                               OptionChainStrategyGetter_Create_ABI,
                               creds,
                               CPP_to_C(symbol),
                               static_cast<int>( strategy.get_strategy() ),
                               strategy.get_spread_interval(),
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>( include_quotes ),
                               CPP_to_C(from_date),
                               CPP_to_C(to_date),
                               static_cast<int>( exp_month ),
                               static_cast<int>( option_type ) )
        {
        }

    ~OptionChainStrategyGetter()
        {
            destroy_cgetter(OptionChainStrategyGetter_Destroy_ABI);
        }

    OptionChainStrategyGetter( OptionChainStrategyGetter&& ) = default;

    OptionChainStrategyGetter&
    operator=( OptionChainStrategyGetter&& getter ) = default;

    OptionStrategy 
    get_strategy() const
    {
        int strat;
        double spread;
        OptionChainStrategyGetter_GetStrategy_ABI(
            cgetter<OptionChainStrategyGetter_C>(), &strat, &spread, 1
            );
        return OptionStrategy(static_cast<OptionStrategyType>(strat), spread);
    }

    void
    set_strategy(OptionStrategy strategy)
    {
        OptionChainStrategyGetter_SetStrategy_ABI(
            cgetter<OptionChainStrategyGetter_C>(),
            static_cast<int>(strategy.get_strategy()),
            strategy.get_spread_interval(),
            1
            );
    }
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


class OptionChainAnalyticalGetter
        : public OptionChainGetter {
public:
    typedef OptionChainAnalyticalGetter_C CType;

    static const int TYPE_ID_LOW = 9;
    static const int TYPE_ID_HIGH = 9;

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
            OptionType option_type = OptionType::all )
        :
            OptionChainGetter( OptionChainAnalyticalGetter_C{},
                               OptionChainAnalyticalGetter_Create_ABI,
                               creds,
                               CPP_to_C(symbol),
                               volatility,
                               underlying_price,
                               interest_rate,
                               days_to_exp,
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>( include_quotes ),
                               CPP_to_C(from_date),
                               CPP_to_C(to_date),
                               static_cast<int>( exp_month ),
                               static_cast<int>( option_type ) )
        {
        }

    ~OptionChainAnalyticalGetter()
        {
            destroy_cgetter(OptionChainAnalyticalGetter_Destroy_ABI);
        }

    OptionChainAnalyticalGetter( OptionChainAnalyticalGetter&& ) = default;

    OptionChainAnalyticalGetter&
    operator=( OptionChainAnalyticalGetter&& getter ) = default;

    double
    get_volatility() const
    {
        double v;
        OptionChainAnalyticalGetter_GetVolatility_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), &v, 1
            );
        return v;
    }

    double
    get_underlying_price() const
    {
        double p;
        OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), &p, 1
            );
        return p;
    }

    double
    get_interest_rate() const
    {
        double r;
        OptionChainAnalyticalGetter_GetInterestRate_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), &r, 1
            );
        return r;
    }

    unsigned int
    get_days_to_exp() const
    {
        unsigned int d;
        OptionChainAnalyticalGetter_GetDaysToExp_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), &d, 1
            );
        return d;
    }

    void
    set_volatility(double volatility)
    {
        OptionChainAnalyticalGetter_SetVolatility_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), volatility, 1
            );
    }

    void
    set_underlying_price(double underlying_price)
    {
        OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), underlying_price, 1
            );
    }

    void
    set_interest_rate(double interest_rate)
    {
        OptionChainAnalyticalGetter_SetInterestRate_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), interest_rate, 1
            );
    }

    void
    set_days_to_exp(unsigned int days_to_exp)
    {
        OptionChainAnalyticalGetter_SetDaysToExp_ABI(
            cgetter<OptionChainAnalyticalGetter_C>(), days_to_exp, 1
            );
    }
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


class AccountGetterBase
        : public APIGetter{
protected:
    template< typename CTy, typename F, typename... Args>
    AccountGetterBase( CTy _,
                         F func,
                         Credentials& creds,
                         const std::string& account_id,
                         Args... args )
        :
            APIGetter( _, func, nullptr, CPP_to_C(creds), CPP_to_C(account_id), args... )
        {
        }

    AccountGetterBase( AccountGetterBase&& ) = default;

    AccountGetterBase&
    operator=( AccountGetterBase&& getter ) = default;

public:
    typedef Getter_C CType;

    static const int TYPE_ID_LOW = 10;
    static const int TYPE_ID_HIGH = 14;

    std::string
    get_account_id() const
    { return str_from_abi<>(AccountGetterBase_GetAccountId_ABI); }

    void
    set_account_id(const std::string& account_id)
    { AccountGetterBase_SetAccountId_ABI(cgetter<>(), account_id.c_str(), 1); }
};


class AccountInfoGetter
        : public AccountGetterBase{
public:
    typedef AccountInfoGetter_C CType;

    static const int TYPE_ID_LOW = 10;
    static const int TYPE_ID_HIGH = 10;

    AccountInfoGetter( Credentials& creds,
                       const std::string& account_id,
                       bool positions,
                       bool orders )
        :
            AccountGetterBase( AccountInfoGetter_C{},
                               AccountInfoGetter_Create_ABI,
                               creds,
                               account_id,
                               static_cast<int>(positions),
                               static_cast<int>(orders) )
        {
        }

    ~AccountInfoGetter()
        {
            destroy_cgetter(AccountInfoGetter_Destroy_ABI);
        }

    AccountInfoGetter( AccountInfoGetter&& ) = default;

    AccountInfoGetter&
    operator=( AccountInfoGetter&& getter ) = default;

    bool
    returns_positions() const
    {
        int p;
        AccountInfoGetter_ReturnsPositions_ABI(
            cgetter<AccountInfoGetter_C>(), &p, 1
            );
        return static_cast<bool>(p);
    }

    bool
    returns_orders() const
    {
        int o;
        AccountInfoGetter_ReturnsOrders_ABI(
            cgetter<AccountInfoGetter_C>(), &o, 1
            );
        return static_cast<bool>(o);
    }

    void
    return_positions(bool positions)
    {
        AccountInfoGetter_ReturnPositions_ABI(
            cgetter<AccountInfoGetter_C>(), static_cast<int>(positions), 1
            );
    }

    void
    return_orders(bool orders)
    {
        AccountInfoGetter_ReturnOrders_ABI(
            cgetter<AccountInfoGetter_C>(), static_cast<int>(orders), 1
            );
    }
};

inline json
GetAccountInfo( Credentials& creds,
                const std::string& account_id,
                bool positions,
                bool orders )
{ return AccountInfoGetter(creds, account_id, positions, orders).get(); }


class PreferencesGetter
        : public AccountGetterBase{
public:
    typedef PreferencesGetter_C CType;

    static const int TYPE_ID_LOW = 11;
    static const int TYPE_ID_HIGH = 11;

    PreferencesGetter( Credentials& creds, const std::string& account_id )
        :
            AccountGetterBase( PreferencesGetter_C{},
                               PreferencesGetter_Create_ABI,
                               creds,
                               account_id )
        {
        }

    ~PreferencesGetter()
        {
            destroy_cgetter(PreferencesGetter_Destroy_ABI);
        }

    PreferencesGetter( PreferencesGetter&& ) = default;

    PreferencesGetter&
    operator=( PreferencesGetter&& getter ) = default;
};

inline json
GetPreferences(Credentials& creds, const std::string& account_id)
{ return PreferencesGetter(creds, account_id).get(); }


class StreamerSubscriptionKeysGetter
        : public AccountGetterBase{
public:
    typedef StreamerSubscriptionKeysGetter_C CType;

    static const int TYPE_ID_LOW = 12;
    static const int TYPE_ID_HIGH = 12;

    StreamerSubscriptionKeysGetter( Credentials& creds,
                                        const std::string& account_id )
        :
            AccountGetterBase( StreamerSubscriptionKeysGetter_C{},
                               StreamerSubscriptionKeysGetter_Create_ABI,
                               creds,
                               account_id )
        {
        }

    ~StreamerSubscriptionKeysGetter()
        {
            destroy_cgetter(StreamerSubscriptionKeysGetter_Destroy_ABI);
        }

    StreamerSubscriptionKeysGetter(StreamerSubscriptionKeysGetter&&)
        = default;

    StreamerSubscriptionKeysGetter&
    operator=( StreamerSubscriptionKeysGetter&& getter ) = default;
};

inline json
GetStreamerSubscriptionKeys(Credentials& creds, const std::string& account_id)
{ return StreamerSubscriptionKeysGetter(creds, account_id).get(); }


class TransactionHistoryGetter
        : public AccountGetterBase{
public:
    typedef TransactionHistoryGetter_C CType;

     static const int TYPE_ID_LOW = 13;
     static const int TYPE_ID_HIGH = 13;

    TransactionHistoryGetter( Credentials& creds,
                              const std::string& account_id,
                              TransactionType transaction_type = TransactionType::all,
                              const std::string& symbol = "",
                              const std::string& start_date = "",
                              const std::string& end_date = "")
        :
            AccountGetterBase( TransactionHistoryGetter_C{},
                               TransactionHistoryGetter_Create_ABI,
                               creds,
                               account_id,
                               static_cast<int>(transaction_type),
                               CPP_to_C(symbol),
                               CPP_to_C(start_date),
                               CPP_to_C(end_date) )
        {
        }

    ~TransactionHistoryGetter()
        {
            destroy_cgetter(TransactionHistoryGetter_Destroy_ABI);
        }

    TransactionHistoryGetter(TransactionHistoryGetter&&) = default;

    TransactionHistoryGetter&
    operator=( TransactionHistoryGetter&& getter ) = default;

    TransactionType
    get_transaction_type() const
    {
        int tt;
        TransactionHistoryGetter_GetTransactionType_ABI(
            cgetter<TransactionHistoryGetter_C>(), &tt, 1
            );
        return static_cast<TransactionType>(tt);
    }

    std::string
    get_symbol() const
    {
        return str_from_abi<TransactionHistoryGetter_C>(
            TransactionHistoryGetter_GetSymbol_ABI
            );
    }

    std::string
    get_start_date() const
    {
        return str_from_abi<TransactionHistoryGetter_C>(
            TransactionHistoryGetter_GetStartDate_ABI
            );
    }

    std::string
    get_end_date() const
    {
        return str_from_abi<TransactionHistoryGetter_C>(
            TransactionHistoryGetter_GetEndDate_ABI
            );
    }

    void
    set_transaction_type(TransactionType transaction_type)
    {
        TransactionHistoryGetter_SetTransactionType_ABI(
            cgetter<TransactionHistoryGetter_C>(),
            static_cast<int>(transaction_type),
            1
            );
    }

    void
    set_symbol(const std::string& symbol)
    {
        TransactionHistoryGetter_SetSymbol_ABI(
            cgetter<TransactionHistoryGetter_C>(), symbol.c_str(), 1
            );
    }

    void
    set_start_date(const std::string& start_date)
    {
        TransactionHistoryGetter_SetStartDate_ABI(
            cgetter<TransactionHistoryGetter_C>(), start_date.c_str(), 1
            );
    }

    void
    set_end_date(const std::string& end_date)
    {
        TransactionHistoryGetter_SetEndDate_ABI(
            cgetter<TransactionHistoryGetter_C>(), end_date.c_str(), 1
            );
    }

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


class IndividualTransactionHistoryGetter
        : public AccountGetterBase {
public:
    typedef IndividualTransactionHistoryGetter_C CType;

     static const int TYPE_ID_LOW = 14;
     static const int TYPE_ID_HIGH = 14;

    IndividualTransactionHistoryGetter(
            Credentials& creds,
            const std::string& account_id,
            const std::string& transaction_id )
         :
             AccountGetterBase( IndividualTransactionHistoryGetter_C{},
                                IndividualTransactionHistoryGetter_Create_ABI,
                                creds,
                                account_id,
                                CPP_to_C(transaction_id) )
         {
         }

    ~IndividualTransactionHistoryGetter()
        {
            destroy_cgetter(IndividualTransactionHistoryGetter_Destroy_ABI);
        }

    IndividualTransactionHistoryGetter(IndividualTransactionHistoryGetter&&)
        = default;

    IndividualTransactionHistoryGetter&
    operator=( IndividualTransactionHistoryGetter&& getter ) = default;

    std::string
    get_transaction_id() const
    {
        return str_from_abi<IndividualTransactionHistoryGetter_C>(
            IndividualTransactionHistoryGetter_GetTransactionId_ABI
            );
    }

    void
    set_transaction_id(const std::string& transaction_id)
    {
        IndividualTransactionHistoryGetter_SetTransactionId_ABI(
            cgetter<IndividualTransactionHistoryGetter_C>(),
            transaction_id.c_str(), 1
            );
    }

};

inline json
GetIndividualTransactionHistory( Credentials& creds,
                                 const std::string& account_id,
                                 const std::string& transaction_id )
{
    return IndividualTransactionHistoryGetter( creds, account_id,
                                               transaction_id ).get();
}


class UserPrincipalsGetter
        : public APIGetter{
public:
    typedef UserPrincipalsGetter_C CType;

    static const int TYPE_ID_LOW = 15;
    static const int TYPE_ID_HIGH = 15;

    UserPrincipalsGetter( Credentials& creds,                         
                          bool streamer_subscription_keys,
                          bool streamer_connection_info,
                          bool preferences,
                          bool surrogate_ids )
        :
            APIGetter( UserPrincipalsGetter_C{},
                       UserPrincipalsGetter_Create_ABI,
                       nullptr,
                       CPP_to_C(creds),
                       static_cast<int>(streamer_subscription_keys),
                       static_cast<int>(streamer_connection_info),
                       static_cast<int>(preferences),
                       static_cast<int>(surrogate_ids) )
        {
        }

    ~UserPrincipalsGetter()
        {
            destroy_cgetter(UserPrincipalsGetter_Destroy_ABI);
        }

    UserPrincipalsGetter( UserPrincipalsGetter&& ) = default;

    UserPrincipalsGetter&
    operator=( UserPrincipalsGetter&& getter ) = default;

    bool
    returns_streamer_subscription_keys() const
    {
        int k;
        UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI(
            cgetter<UserPrincipalsGetter_C>(), &k, 1
            );
        return static_cast<bool>(k);
    }

    bool
    returns_streamer_connection_info() const
    {
        int ci;
        UserPrincipalsGetter_ReturnsConnectionInfo_ABI(
            cgetter<UserPrincipalsGetter_C>(), &ci, 1
            );
        return static_cast<bool>(ci);
    }

    bool
    returns_preferences() const
    {
        int p;
        UserPrincipalsGetter_ReturnsPreferences_ABI(
            cgetter<UserPrincipalsGetter_C>(), &p, 1
            );
        return static_cast<bool>(p);
    }

    bool
    returns_surrogate_ids() const
    {
        int si;
        UserPrincipalsGetter_ReturnsSurrogateIds_ABI(
            cgetter<UserPrincipalsGetter_C>(), &si, 1
            );
        return static_cast<bool>(si);
    }

    void
    return_streamer_subscription_keys(bool streamer_subscription_keys)
    {
        UserPrincipalsGetter_ReturnSubscriptionKeys_ABI(
            cgetter<UserPrincipalsGetter_C>(),
            static_cast<int>(streamer_subscription_keys), 1
            );
    }

    void
    return_streamer_connection_info(bool streamer_connection_info)
    {
        UserPrincipalsGetter_ReturnConnectionInfo_ABI(
            cgetter<UserPrincipalsGetter_C>(),
            static_cast<int>(streamer_connection_info), 1
            );
    }

    void
    return_preferences(bool preferences)
    {
        UserPrincipalsGetter_ReturnPreferences_ABI(
            cgetter<UserPrincipalsGetter_C>(),
            static_cast<int>(preferences), 1
            );
    }

    void
    return_surrogate_ids(bool surrogate_ids)
    {
        UserPrincipalsGetter_ReturnSurrogateIds_ABI(
            cgetter<UserPrincipalsGetter_C>(),
            static_cast<int>(surrogate_ids), 1
            );
    }
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

class InstrumentInfoGetter
        : public APIGetter {
public:
    typedef InstrumentInfoGetter_C CType;

    static const int TYPE_ID_LOW = 16;
    static const int TYPE_ID_HIGH = 16;

    InstrumentInfoGetter( Credentials& creds,
                            InstrumentSearchType search_type,
                            const std::string& query_string )
        : APIGetter( InstrumentInfoGetter_C{},
                     InstrumentInfoGetter_Create_ABI,
                     nullptr,
                     CPP_to_C(creds),
                     static_cast<int>(search_type),
                     CPP_to_C(query_string) )
        {
        }

    ~InstrumentInfoGetter()
        {
            destroy_cgetter(InstrumentInfoGetter_Destroy_ABI);
        }

    InstrumentInfoGetter( InstrumentInfoGetter&& ) = default;

    InstrumentInfoGetter&
    operator=( InstrumentInfoGetter&& getter ) = default;

    std::string
    get_query_string() const
    {
        return str_from_abi<InstrumentInfoGetter_C>(
            InstrumentInfoGetter_GetQueryString_ABI
            );
    }

    InstrumentSearchType
    get_search_type() const
    {
        int ist;
        InstrumentInfoGetter_GetSearchType_ABI(
            cgetter<InstrumentInfoGetter_C>(), &ist, 1
            );
        return static_cast<InstrumentSearchType>(ist);
    }

    void
    set_query( InstrumentSearchType search_type,
                const std::string& query_string )
    {
        InstrumentInfoGetter_SetQuery_ABI(
            cgetter<InstrumentInfoGetter_C>(), static_cast<int>(search_type),
            query_string.c_str(), 1
            );
    }
};

inline json
GetInstrumentInfo( Credentials& creds,
                   InstrumentSearchType search_type,
                   const std::string& query_string )
{ return InstrumentInfoGetter(creds, search_type, query_string).get(); }

using std::to_string;

} /* tdma */

#endif /* __cplusplus */

#endif // TDMA_API_GET_H
