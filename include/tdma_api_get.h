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

DECL_C_CPP_TDMA_ENUM(OrderStatusType, 0, 14,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, AWAITING_PARENT_ORDER),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, AWAITING_CONDITION),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, AWAITING_MANUAL_REVIEW),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, ACCEPTED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, AWAITING_UR_OUT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, PENDING_ACTIVATION),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, QUEUED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, WORKING),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, REJECTED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, PENDING_CANCEL),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, CANCELED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, PENDING_REPLACE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, REPLACED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, FILLED),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStatusType, EXPIRED)
);

typedef union {
    unsigned int n_atm;
    double single;
#ifdef __cplusplus
    tdma::
#endif
    OptionRangeType range;
} OptionStrikesValue;

#define THROW_VALUE_EXCEPTION(m) throw ValueException(m, __LINE__, __FILE__)

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

static const
std::unordered_map< PeriodType, std::set<int>, EnumHash<PeriodType> >
VALID_PERIODS_BY_PERIOD_TYPE = {
    {PeriodType::day, std::set<int>{1,2,3,4,5,10}},
    {PeriodType::month, std::set<int>{1,2,3,6}},
    {PeriodType::year, std::set<int>{1,2,3,5,10,15,20}},
    {PeriodType::ytd, std::set<int>{1}},
};

static const
std::unordered_map< PeriodType,
                    std::set<FrequencyType, EnumCompare<FrequencyType>>,
                    EnumHash<PeriodType> >
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE = {
    {PeriodType::day, {FrequencyType::minute} },
    {PeriodType::month, {FrequencyType::daily, FrequencyType::weekly} },
    {PeriodType::year, {FrequencyType::daily, FrequencyType::weekly,
                        FrequencyType::monthly} },
    {PeriodType::ytd, { FrequencyType::weekly} },
};


static const
std::unordered_map< FrequencyType, std::set<int>, EnumHash<FrequencyType> >
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, std::set<int>{1,5,10,30}},
    {FrequencyType::daily, std::set<int>{1}},
    {FrequencyType::weekly, std::set<int>{1}},
    {FrequencyType::monthly, std::set<int>{1}},
};

} /* tdma */

#else

static const int
VALID_PERIODS_BY_PERIOD_TYPE[PeriodType_ytd + 1][8] = {
    {1,2,3,4,5,10,-1,-1},
    {1,2,3,6,-1,-1,-1,-1},
    {1,2,3,5,10,15,10,-1},
    {1,-1,-1,-1,-1,-1,-1,-1},
};

static const FrequencyType
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[PeriodType_ytd + 1][4] = {
    {FrequencyType_minute, -1, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, FrequencyType_monthly, -1},
    {FrequencyType_weekly, -1,-1, -1}
};

static const int
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
DECL_CGETTER_STRUCT(OrderGetter_C);
DECL_CGETTER_STRUCT(OrdersGetter_C);

#undef DECL_CGETTER_STRUCT


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

EXTERN_C_SPEC_ DLL_SPEC_ int
APIGetter_WaitRemaining_ABI(unsigned long long *msec, int allow_exceptions);

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

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_AddSymbol_ABI( QuotesGetter_C *pgetter,
                               const char *symbol,
                               int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_RemoveSymbol_ABI( QuotesGetter_C *pgetter,
                                  const char *symbol,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_AddSymbols_ABI( QuotesGetter_C *pgetter,
                                 const char** symbols,
                                 size_t nymbols,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
QuotesGetter_RemoveSymbols_ABI( QuotesGetter_C *pgetter,
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

EXTERN_C_SPEC_ DLL_SPEC_ int
HistoricalGetterBase_SetFrequency_ABI( Getter_C *pgetter,
                                             int frequency_type,
                                             unsigned int frequency,
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
OptionChainGetter_Destroy_ABI( OptionChainGetter_C *pgetter,
                                   int allow_exceptions );

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
OptionChainAnalyticalGetter_Create_ABI(
    struct Credentials *pcreds,
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
    int allow_exceptions
    );

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

/* OrderGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
OrderGetter_Create_ABI( struct Credentials *pcreds,
                           const char* account_id,
                           const char* order_id,
                           OrderGetter_C *pgetter,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderGetter_Destroy_ABI(OrderGetter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderGetter_GetOrderId_ABI( OrderGetter_C *pgetter,
                                char **buf,
                                size_t *n,
                                int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderGetter_SetOrderId_ABI( OrderGetter_C *pgetter,
                                const char *order_id,
                                int allow_exceptions );


/* OrdersGetter */
EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_Create_ABI( struct Credentials *pcreds,
                           const char* account_id,
                           unsigned int nmax_results,
                           const char* from_entered_time,
                           const char* to_entered_time,
                           int order_status_type,
                           OrdersGetter_C *pgetter,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_Destroy_ABI(OrdersGetter_C *pgetter, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_GetNMaxResults_ABI( OrdersGetter_C *pgetter,
                                     unsigned int *nmax_results,
                                    int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_SetNMaxResults_ABI( OrdersGetter_C *pgetter,
                                     unsigned int nmax_results,
                                    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_GetFromEnteredTime_ABI( OrdersGetter_C *pgetter,
                                         char** buf,
                                         size_t *n,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_SetFromEnteredTime_ABI( OrdersGetter_C *pgetter,
                                          const char* from_entered_time,
                                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_GetToEnteredTime_ABI( OrdersGetter_C *pgetter,
                                       char** buf,
                                       size_t *n,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_SetToEnteredTime_ABI( OrdersGetter_C *pgetter,
                                        const char* to_entered_time,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_GetOrderStatusType_ABI( OrdersGetter_C *pgetter,
                                         int *order_status_type,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrdersGetter_SetOrderStatusType_ABI( OrdersGetter_C *pgetter,
                                         int order_status_type,
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
 * Getter objects created by Create need to be destroyed w/ Destroy
 *
 * NOTE - for Get and Close client can use the versions associate with the
 *        getter type or the 'base' versions: APIGetter_Get and APIGetter_Close
 */

/* APIGetter -> APIGetterImpl */
static inline int
APIGetter_Get(Getter_C *pgetter, char** buf, size_t *n)
{ return APIGetter_Get_ABI(pgetter, buf, n, 0); }

static inline int
APIGetter_Close(Getter_C *pgetter)
{ return APIGetter_Close_ABI(pgetter, 0); }

static inline int
APIGetter_IsClosed(Getter_C *pgetter, int*b)
{ return APIGetter_IsClosed_ABI(pgetter,b, 0); }

static inline int
APIGetter_SetWaitMSec(unsigned long long msec)
{ return APIGetter_SetWaitMSec_ABI(msec, 0); }

static inline int
APIGetter_GetWaitMSec(unsigned long long *msec)
{ return APIGetter_GetWaitMSec_ABI(msec, 0); }

static inline int
APIGetter_GetDefWaitMSec(unsigned long long *msec)
{ return APIGetter_GetDefWaitMSec_ABI(msec, 0); }

static inline int
APIGetter_WaitRemaining(unsigned long long *msec)
{ return APIGetter_WaitRemaining_ABI(msec, 0); }

/* declare derived versions of Get, Close, IsClosed for each getter*/
#define DECL_WRAPPED_API_GETTER_BASE_FUNCS(name) \
static inline int \
name##_Get(name##_C *pgetter, char** buf, size_t *n) \
{ return APIGetter_Get_ABI( (Getter_C*)pgetter, buf, n, 0); } \
\
static inline int \
name##_Close(name##_C *pgetter) \
{ return APIGetter_Close_ABI( (Getter_C*)pgetter, 0); } \
\
static inline int \
name##_IsClosed(name##_C *pgetter, int *b) \
{ return APIGetter_IsClosed_ABI( (Getter_C*)pgetter, b, 0); }


/* QuoteGetter -> QuoteGetterImpl */
static inline int
QuoteGetter_Create( struct Credentials *pcreds,
                      const char* symbol,
                      QuoteGetter_C *pgetter )
{ return QuoteGetter_Create_ABI(pcreds, symbol, pgetter, 0); }

static inline int
QuoteGetter_Destroy(QuoteGetter_C *getter)
{ return QuoteGetter_Destroy_ABI(getter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(QuoteGetter)

static inline int
QuoteGetter_GetSymbol(QuoteGetter_C *getter, char **buf, size_t *n)
{ return QuoteGetter_GetSymbol_ABI(getter, buf, n, 0); }

static inline int
QuoteGetter_SetSymbol(QuoteGetter_C *getter, const char *symbol)
{ return QuoteGetter_SetSymbol_ABI(getter, symbol, 0); }


/* QuotesGetter -> QuotesGetterImpl */
static inline int
QuotesGetter_Create( struct Credentials *pcreds,
                      const char** symbols,
                      size_t nsymbols,
                      QuotesGetter_C *pgetter )
{ return QuotesGetter_Create_ABI(pcreds, symbols, nsymbols, pgetter, 0); }

static inline int
QuotesGetter_Destroy(QuotesGetter_C *getter)
{ return QuotesGetter_Destroy_ABI(getter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(QuotesGetter)

static inline int
QuotesGetter_GetSymbols(QuotesGetter_C *getter, char ***buf, size_t *n)
{ return QuotesGetter_GetSymbols_ABI(getter, buf, n, 0); }

static inline int
QuotesGetter_SetSymbols(QuotesGetter_C *getter, const char **symbols,
                        size_t nsymbols)
{ return QuotesGetter_SetSymbols_ABI(getter, symbols, nsymbols, 0); }

static inline int
QuotesGetter_AddSymbol( QuotesGetter_C *pgetter,  const char *symbol)
{ return QuotesGetter_AddSymbol_ABI(pgetter, symbol, 0); }

static inline int
QuotesGetter_RemoveSymbol( QuotesGetter_C *pgetter, const char *symbol)
{ return QuotesGetter_RemoveSymbol_ABI(pgetter, symbol, 0); }

static inline int
QuotesGetter_AddSymbols(QuotesGetter_C *getter, const char **symbols,
                        size_t nsymbols)
{ return QuotesGetter_AddSymbols_ABI(getter, symbols, nsymbols, 0); }

static inline int
QuotesGetter_RemoveSymbols(QuotesGetter_C *getter, const char **symbols,
                           size_t nsymbols)
{ return QuotesGetter_RemoveSymbols_ABI(getter, symbols, nsymbols, 0); }

/* MarketHoursGetter */
static inline int
MarketHoursGetter_Create( struct Credentials *pcreds,
                             MarketType market_type,
                             const char* date,
                             MarketHoursGetter_C *pgetter )
{ return MarketHoursGetter_Create_ABI(pcreds, (int)market_type, date, pgetter, 0); }

static inline int
MarketHoursGetter_Destroy(MarketHoursGetter_C *pgetter)
{ return MarketHoursGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(MarketHoursGetter)

static inline int
MarketHoursGetter_GetMarketType( MarketHoursGetter_C *pgetter,
                                     MarketType *market_type )
{ return MarketHoursGetter_GetMarketType_ABI(pgetter, (int*)market_type, 0); }

static inline int
MarketHoursGetter_SetMarketType( MarketHoursGetter_C *pgetter,
                                     MarketType market_type )
{ return MarketHoursGetter_SetMarketType_ABI(pgetter, (int)market_type, 0); }

static inline int
MarketHoursGetter_GetDate(MarketHoursGetter_C *pgetter, char **buf, size_t *n)
{ return MarketHoursGetter_GetDate_ABI(pgetter, buf, n, 0); }

static inline int
MarketHoursGetter_SetDate( MarketHoursGetter_C *pgetter, const char* date )
{ return MarketHoursGetter_SetDate_ABI(pgetter, date, 0); }

/* MoversGetter */
static inline int
MoversGetter_Create( struct Credentials *pcreds,
                       MoversIndex index,
                       MoversDirectionType direction_type,
                       MoversChangeType change_type,
                       MoversGetter_C *pgetter)
{ return MoversGetter_Create_ABI(pcreds, (int)index, (int)direction_type,
                                 (int)change_type, pgetter, 0);}

static inline int
MoversGetter_Destroy( MoversGetter_C *pgetter)
{ return MoversGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(MoversGetter)

static inline int
MoversGetter_GetIndex( MoversGetter_C *pgetter, MoversIndex *index)
{ return MoversGetter_GetIndex_ABI(pgetter, (int*)index, 0); }

static inline int
MoversGetter_SetIndex( MoversGetter_C *pgetter,  MoversIndex index)
{ return MoversGetter_SetIndex_ABI(pgetter, (int)index, 0); }

static inline int
MoversGetter_GetDirectionType( MoversGetter_C *pgetter,
                                  MoversDirectionType *direction_type )
{ return MoversGetter_GetDirectionType_ABI(pgetter, (int*)direction_type, 0); }

static inline int
MoversGetter_SetDirectionType( MoversGetter_C *pgetter,
                                  MoversDirectionType direction_type)
{ return MoversGetter_SetDirectionType_ABI(pgetter, (int)direction_type, 0); }

static inline int
MoversGetter_GetChangeType( MoversGetter_C *pgetter,
                               MoversChangeType *change_type )
{ return MoversGetter_GetChangeType_ABI(pgetter, (int*)change_type, 0); }

static inline int
MoversGetter_SetChangeType( MoversGetter_C *pgetter,
                               MoversChangeType change_type)
{ return MoversGetter_SetChangeType_ABI(pgetter, (int)change_type, 0); }


/* HistoricalPeriodGetter */
static inline int
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

static inline int
HistoricalPeriodGetter_Destroy( HistoricalPeriodGetter_C *pgetter )
{ return HistoricalPeriodGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(HistoricalPeriodGetter)

static inline int
HistoricalPeriodGetter_GetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  char **buf,
                                  size_t *n )
{ return HistoricalGetterBase_GetSymbol_ABI( (Getter_C*)pgetter, buf, n, 0); }

static inline int
HistoricalPeriodGetter_SetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  const char *symbol )
{ return HistoricalGetterBase_SetSymbol_ABI( (Getter_C*)pgetter, symbol, 0); }

static inline int
HistoricalPeriodGetter_GetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     unsigned int *frequency )
{ return HistoricalGetterBase_GetFrequency_ABI( (Getter_C*)pgetter,
                                                 frequency, 0); }

static inline int
HistoricalPeriodGetter_GetFrequencyType( HistoricalPeriodGetter_C *pgetter,
                                         FrequencyType *frequency_type )
{ return HistoricalGetterBase_GetFrequencyType_ABI( (Getter_C*)pgetter,
                                                    (int*)frequency_type, 0); }

static inline int
HistoricalPeriodGetter_IsExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                               int *is_extended_hours )
{ return HistoricalGetterBase_IsExtendedHours_ABI( (Getter_C*)pgetter,
                                                   is_extended_hours, 0); }

static inline int
HistoricalPeriodGetter_SetExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                               int is_extended_hours )
{ return HistoricalGetterBase_SetExtendedHours_ABI( (Getter_C*)pgetter,
                                                    is_extended_hours, 0); }

static inline int
HistoricalPeriodGetter_GetPeriodType( HistoricalPeriodGetter_C *pgetter,
                                      PeriodType *period_type )
{ return HistoricalPeriodGetter_GetPeriodType_ABI( pgetter, (int*)period_type, 0); }


static inline int
HistoricalPeriodGetter_GetPeriod(HistoricalPeriodGetter_C *pgetter,
                                 unsigned int *period)
{ return HistoricalPeriodGetter_GetPeriod_ABI( pgetter, period, 0); }

static inline int
HistoricalPeriodGetter_SetPeriod( HistoricalPeriodGetter_C *pgetter,
                                   PeriodType period_type,
                                   unsigned int period )
{ return HistoricalPeriodGetter_SetPeriod_ABI( pgetter, (int)period_type,
                                               period, 0); }

static inline int
HistoricalPeriodGetter_SetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency )
{ return HistoricalGetterBase_SetFrequency_ABI( (Getter_C*)pgetter,
                                                (int)frequency_type,
                                                frequency, 0); }

/* HistoricalRangeGetter */
static inline int
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

static inline int
HistoricalRangeGetter_Destroy( HistoricalRangeGetter_C *pgetter )
{ return HistoricalRangeGetter_Destroy_ABI( pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(HistoricalRangeGetter)

static inline int
HistoricalRangeGetter_GetSymbol( HistoricalRangeGetter_C *pgetter,
                                  char **buf,
                                  size_t *n )
{ return HistoricalGetterBase_GetSymbol_ABI( (Getter_C*)pgetter, buf, n, 0); }

static inline int
HistoricalRangeGetter_SetSymbol( HistoricalRangeGetter_C *pgetter,
                                  const char *symbol )
{ return HistoricalGetterBase_SetSymbol_ABI( (Getter_C*)pgetter, symbol, 0); }

static inline int
HistoricalRangeGetter_GetFrequency( HistoricalRangeGetter_C *pgetter,
                                    unsigned int *frequency )
{ return HistoricalGetterBase_GetFrequency_ABI( (Getter_C*)pgetter,
                                                 frequency, 0); }

static inline int
HistoricalRangeGetter_GetFrequencyType( HistoricalRangeGetter_C *pgetter,
                                        FrequencyType *frequency_type )
{ return HistoricalGetterBase_GetFrequencyType_ABI( (Getter_C*)pgetter,
                                                    (int*)frequency_type, 0); }

static inline int
HistoricalRangeGetter_IsExtendedHours( HistoricalRangeGetter_C *pgetter,
                                               int *is_extended_hours )
{ return HistoricalGetterBase_IsExtendedHours_ABI( (Getter_C*)pgetter,
                                                   is_extended_hours, 0); }

static inline int
HistoricalRangeGetter_SetExtendedHours( HistoricalRangeGetter_C *pgetter,
                                               int is_extended_hours )
{ return HistoricalGetterBase_SetExtendedHours_ABI( (Getter_C*)pgetter,
                                                    is_extended_hours, 0); }

static inline int
HistoricalRangeGetter_GetEndMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *end_msec )
{ return HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(pgetter, end_msec, 0); }

static inline int
HistoricalRangeGetter_SetEndMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long end_msec )
{ return HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(pgetter, end_msec, 0); }

static inline int
HistoricalRangeGetter_GetStartMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *start_msec )
{ return HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(pgetter, start_msec, 0); }

static inline int
HistoricalRangeGetter_SetStartMSecSinceEpoch(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long start_msec )
{ return HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI(pgetter, start_msec, 0); }

static inline int
HistoricalRangeGetter_SetFrequency( HistoricalRangeGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency)
{ return HistoricalGetterBase_SetFrequency_ABI( (Getter_C*)pgetter,
                                                (int)frequency_type,
                                                frequency, 0); }

/* OptionChainGetter */
static inline int
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

static inline int
OptionChainGetter_Destroy(OptionChainGetter_C *pgetter )
{ return OptionChainGetter_Destroy_ABI(pgetter, 0); }

/* declare derived(and base) versions of OptionChain methods for derived getters*/
#define DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(name) \
static inline int \
name##_GetSymbol(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetSymbol_ABI( (OptionChainGetter_C*)pgetter, \
                                          buf, n, 0); } \
\
static inline int \
name##_SetSymbol(name##_C *pgetter, const char *symbol) \
{ return OptionChainGetter_SetSymbol_ABI( (OptionChainGetter_C*)pgetter, \
                                           symbol, 0); } \
\
static inline int \
name##_GetStrikes( name##_C *pgetter, \
                   OptionStrikesType *strikes_type, \
                   OptionStrikesValue *strikes_value ) \
{ return OptionChainGetter_GetStrikes_ABI( (OptionChainGetter_C*)pgetter, \
                                           (int*)strikes_type, \
                                           strikes_value, 0); } \
\
static inline int \
name##_SetStrikes( name##_C *pgetter, \
                   OptionStrikesType strikes_type, \
                   OptionStrikesValue strikes_value ) \
{ return OptionChainGetter_SetStrikes_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int)strikes_type, \
                                            strikes_value, 0); } \
\
static inline int \
name##_GetContractType( name##_C *pgetter, OptionContractType *contract_type ) \
{ return OptionChainGetter_GetContractType_ABI( (OptionChainGetter_C*)pgetter, \
                                                (int*)contract_type, 0); } \
\
static inline int \
name##_SetContractType( name##_C *pgetter, OptionContractType contract_type ) \
{ return OptionChainGetter_SetContractType_ABI( (OptionChainGetter_C*)pgetter, \
                                                (int)contract_type, 0); } \
\
static inline int \
name##_IncludesQuotes( name##_C *pgetter, int *includes_quotes ) \
{ return OptionChainGetter_IncludesQuotes_ABI( (OptionChainGetter_C*)pgetter, \
                                                includes_quotes, 0); } \
\
static inline int \
name##_IncludeQuotes( name##_C *pgetter, int include_quotes ) \
{ return OptionChainGetter_IncludeQuotes_ABI( (OptionChainGetter_C*)pgetter, \
                                              include_quotes, 0); } \
\
static inline int \
name##_GetFromDate(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetFromDate_ABI( (OptionChainGetter_C*)pgetter, \
                                             buf, n, 0); } \
\
static inline int \
name##_SetFromDate(name##_C *pgetter, const char *date) \
{ return OptionChainGetter_SetFromDate_ABI( (OptionChainGetter_C*)pgetter, \
                                             date, 0); } \
\
static inline int \
name##_GetToDate(name##_C *pgetter, char **buf, size_t *n) \
{ return OptionChainGetter_GetToDate_ABI( (OptionChainGetter_C*)pgetter, \
                                          buf, n, 0); } \
\
static inline int \
name##_SetToDate(name##_C *pgetter, const char *date) \
{ return OptionChainGetter_SetToDate_ABI( (OptionChainGetter_C*)pgetter, \
                                          date, 0); } \
\
static inline int \
name##_GetExpMonth(name##_C *pgetter, OptionExpMonth *exp_month) \
{ return OptionChainGetter_GetExpMonth_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int*)exp_month, 0); } \
\
static inline int \
name##_SetExpMonth(name##_C *pgetter, OptionExpMonth exp_month) \
{ return OptionChainGetter_SetExpMonth_ABI( (OptionChainGetter_C*)pgetter, \
                                            (int)exp_month, 0); } \
\
static inline int \
name##_GetOptionType(name##_C *pgetter, OptionType *option_type) \
{ return OptionChainGetter_GetOptionType_ABI( (OptionChainGetter_C*)pgetter, \
                                               (int*)option_type, 0); } \
\
static inline int \
name##_SetOptionType(name##_C *pgetter, OptionType option_type) \
{ return OptionChainGetter_SetOptionType_ABI( (OptionChainGetter_C*)pgetter, \
                                               (int)option_type, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OptionChainGetter)
DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(OptionChainGetter)

/* OptionChainStrategyGetter */
static inline int
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

static inline int
OptionChainStrategyGetter_Destroy( OptionChainStrategyGetter_C *pgetter )
{ return OptionChainStrategyGetter_Destroy_ABI( pgetter, 0); }

static inline int
OptionChainStrategyGetter_GetStrategy( OptionChainStrategyGetter_C *pgetter,
                                       OptionStrategyType *strategy_type,
                                       double *spread_interval )
{
    return OptionChainStrategyGetter_GetStrategy_ABI(pgetter, (int*)strategy_type,
                                                     spread_interval, 0);
}

static inline int
OptionChainStrategyGetter_SetStrategy( OptionChainStrategyGetter_C *pgetter,
                                        OptionStrategyType strategy_type,
                                        double spread_interval )
{
    return OptionChainStrategyGetter_SetStrategy_ABI(pgetter, (int)strategy_type,
                                                     spread_interval, 0);
}

/* OptionChainAnalyticalGetter */
static inline int
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

static inline int
OptionChainAnalyticalGetter_Destroy( OptionChainAnalyticalGetter_C *pgetter )
{ return OptionChainAnalyticalGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OptionChainAnalyticalGetter)

DECL_WRAPPED_OPTION_GETTER_BASE_FUNCS(OptionChainAnalyticalGetter)

static inline int
OptionChainAnalyticalGetter_GetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double *volatility
    )
{ return OptionChainAnalyticalGetter_GetVolatility_ABI(pgetter, volatility, 0); }

static inline int
OptionChainAnalyticalGetter_SetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double volatility
    )
{ return OptionChainAnalyticalGetter_SetVolatility_ABI(pgetter, volatility, 0); }

static inline int
OptionChainAnalyticalGetter_GetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double *underlying_price
    )
{
    return OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI(
        pgetter, underlying_price, 0
        );
}

static inline int
OptionChainAnalyticalGetter_SetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double underlying_price
    )
{
    return OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI(
        pgetter, underlying_price, 0
        );
}

static inline int
OptionChainAnalyticalGetter_GetInterestRate(
    OptionChainAnalyticalGetter_C *pgetter,
    double *interest_rate
    )
{
    return OptionChainAnalyticalGetter_GetInterestRate_ABI(
        pgetter, interest_rate, 0
        );
}

static inline int
OptionChainAnalyticalGetter_SetInterestRate(
    OptionChainAnalyticalGetter_C *pgetter,
    double interest_rate
    )
{
    return OptionChainAnalyticalGetter_SetInterestRate_ABI(
        pgetter, interest_rate, 0
        );
}

static inline int
OptionChainAnalyticalGetter_GetDaysToExp(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int *days_to_exp
    )
{ return OptionChainAnalyticalGetter_GetDaysToExp_ABI(pgetter, days_to_exp, 0); }

static inline int
OptionChainAnalyticalGetter_SetDaysToExp(
    OptionChainAnalyticalGetter_C *pgetter,
    unsigned int days_to_exp
    )
{ return OptionChainAnalyticalGetter_SetDaysToExp_ABI(pgetter, days_to_exp, 0); }

/* AccountGetterBase */
#define DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(name) \
static inline int \
name##_GetAccountId( name##_C *pgetter, char **buf, size_t *n) \
{ return AccountGetterBase_GetAccountId_ABI( (Getter_C*)pgetter, buf, n, 0); } \
\
static inline int \
name##_SetAccountId( name##_C *pgetter, const char *symbol ) \
{ return AccountGetterBase_SetAccountId_ABI( (Getter_C*)pgetter, symbol, 0); }

/* AccountInfoGetter */
static inline int
AccountInfoGetter_Create( struct Credentials *pcreds,
                          const char* account_id,
                          int positions,
                          int orders,
                          AccountInfoGetter_C *pgetter )
{ return AccountInfoGetter_Create_ABI(pcreds, account_id, positions, orders,
                                      pgetter, 0); }

static inline int
AccountInfoGetter_Destroy( AccountInfoGetter_C *pgetter)
{ return AccountInfoGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(AccountInfoGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(AccountInfoGetter)

static inline int
AccountInfoGetter_ReturnsPositions( AccountInfoGetter_C *pgetter,
                                    int *returns_positions)
{ return AccountInfoGetter_ReturnsPositions_ABI(pgetter, returns_positions, 0); }

static inline int
AccountInfoGetter_ReturnPositions( AccountInfoGetter_C *pgetter,
                                            int return_positions )
{ return AccountInfoGetter_ReturnPositions_ABI(pgetter, return_positions, 0); }

static inline int
AccountInfoGetter_ReturnsOrders( AccountInfoGetter_C *pgetter,
                                 int *returns_orders)
{ return AccountInfoGetter_ReturnsOrders_ABI(pgetter, returns_orders, 0); }

static inline int
AccountInfoGetter_ReturnOrders( AccountInfoGetter_C *pgetter,
                                            int return_orders )
{ return AccountInfoGetter_ReturnOrders_ABI(pgetter, return_orders, 0); }

/* PreferencesGetter */
static inline int
PreferencesGetter_Create( struct Credentials *pcreds,
                          const char* account_id,
                          PreferencesGetter_C *pgetter )
{ return PreferencesGetter_Create_ABI(pcreds, account_id, pgetter, 0); }

static inline int
PreferencesGetter_Destroy( PreferencesGetter_C *pgetter)
{ return PreferencesGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(PreferencesGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(PreferencesGetter)

/* StreamerSubscriptionKeysGetter */
static inline int
StreamerSubscriptionKeysGetter_Create(
    struct Credentials *pcreds,
     const char* account_id,
     StreamerSubscriptionKeysGetter_C *pgetter )
{ return StreamerSubscriptionKeysGetter_Create_ABI(pcreds, account_id, pgetter, 0); }

static inline int
StreamerSubscriptionKeysGetter_Destroy(StreamerSubscriptionKeysGetter_C *pgetter)
{ return StreamerSubscriptionKeysGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(StreamerSubscriptionKeysGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(StreamerSubscriptionKeysGetter)

/* TransactionHistoryGetter */
static inline int
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

static inline int
TransactionHistoryGetter_Destroy( TransactionHistoryGetter_C *pgetter )
{ return TransactionHistoryGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(TransactionHistoryGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(TransactionHistoryGetter)

static inline int
TransactionHistoryGetter_GetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType *transaction_type )
{
    return TransactionHistoryGetter_GetTransactionType_ABI(
        pgetter, (int*)transaction_type, 0
    );
}

static inline int
TransactionHistoryGetter_SetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType transaction_type )
{
    return TransactionHistoryGetter_SetTransactionType_ABI(
        pgetter, (int)transaction_type, 0
    );
}

static inline int
TransactionHistoryGetter_GetSymbol( TransactionHistoryGetter_C *pgetter,
                                    char **buf,
                                    size_t *n )
{ return TransactionHistoryGetter_GetSymbol_ABI(pgetter, buf, n, 0); }

static inline int
TransactionHistoryGetter_SetSymbol( TransactionHistoryGetter_C *pgetter,
                                    const char* symbol )
{ return TransactionHistoryGetter_SetSymbol_ABI(pgetter, symbol, 0); }

static inline int
TransactionHistoryGetter_GetStartDate( TransactionHistoryGetter_C *pgetter,
                                       char **buf,
                                       size_t *n )
{ return TransactionHistoryGetter_GetStartDate_ABI(pgetter, buf, n, 0); }

static inline int
TransactionHistoryGetter_SetStartDate( TransactionHistoryGetter_C *pgetter,
                                       const char* start_date )
{ return TransactionHistoryGetter_SetStartDate_ABI(pgetter, start_date, 0); }

static inline int
TransactionHistoryGetter_GetEndDate( TransactionHistoryGetter_C *pgetter,
                                     char **buf,
                                     size_t *n )
{ return TransactionHistoryGetter_GetEndDate_ABI(pgetter, buf, n, 0); }

static inline int
TransactionHistoryGetter_SetEndDate( TransactionHistoryGetter_C *pgetter,
                                     const char* end_date )
{ return TransactionHistoryGetter_SetEndDate_ABI(pgetter, end_date, 0); }

/* IndividualTransactionHistoryGetter */
static inline int
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

static inline int
IndividualTransactionHistoryGetter_Destroy(
    IndividualTransactionHistoryGetter_C *pgetter
    )
{ return IndividualTransactionHistoryGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(IndividualTransactionHistoryGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(IndividualTransactionHistoryGetter)

static inline int
IndividualTransactionHistoryGetter_GetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n )
{
    return IndividualTransactionHistoryGetter_GetTransactionId_ABI(
        pgetter, buf, n, 0
        );
}

static inline int
IndividualTransactionHistoryGetter_SetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    const char* transaction_id )
{
    return IndividualTransactionHistoryGetter_SetTransactionId_ABI(
            pgetter, transaction_id, 0
            );
}

/* UserPrincipalsGetter */
static inline int
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

static inline int
UserPrincipalsGetter_Destroy( UserPrincipalsGetter_C *pgetter)
{ return UserPrincipalsGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(UserPrincipalsGetter)

static inline int
UserPrincipalsGetter_ReturnsSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                              int *returns_subscription_keys )
{
    return UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI(
        pgetter, returns_subscription_keys, 0
        );
}


static inline int
UserPrincipalsGetter_ReturnSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                             int return_subscription_keys)
{
    return UserPrincipalsGetter_ReturnSubscriptionKeys_ABI(
        pgetter, return_subscription_keys, 0
        );
}

static inline int
UserPrincipalsGetter_ReturnsConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                            int *returns_connection_info )
{
    return UserPrincipalsGetter_ReturnsConnectionInfo_ABI(
        pgetter, returns_connection_info, 0
        );
}


static inline int
UserPrincipalsGetter_ReturnConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                           int return_connection_info )
{
    return UserPrincipalsGetter_ReturnConnectionInfo_ABI(
            pgetter, return_connection_info, 0
            );
}

static inline int
UserPrincipalsGetter_ReturnsPreferences( UserPrincipalsGetter_C *pgetter,
                                         int *returns_preferences)
{
    return UserPrincipalsGetter_ReturnsPreferences_ABI(
            pgetter, returns_preferences, 0
            );
}

static inline int
UserPrincipalsGetter_ReturnPreferences( UserPrincipalsGetter_C *pgetter,
                                        int return_preferences )
{
    return UserPrincipalsGetter_ReturnPreferences_ABI(
            pgetter, return_preferences, 0
            );
}

static inline int
UserPrincipalsGetter_ReturnsSurrogateIds( UserPrincipalsGetter_C *pgetter,
                                          int *returns_surrogate_ids )
{
    return UserPrincipalsGetter_ReturnsSurrogateIds_ABI(
            pgetter, returns_surrogate_ids, 0
            );
}

static inline int
UserPrincipalsGetter_ReturnSurrogateIds(  UserPrincipalsGetter_C *pgetter,
                                          int return_surrogate_ids )
{
    return UserPrincipalsGetter_ReturnSurrogateIds_ABI(
            pgetter, return_surrogate_ids, 0
            );
}

/* InstrumentInfoGetter */
static inline int
InstrumentInfoGetter_Create( struct Credentials *pcreds,
                             InstrumentSearchType search_type,
                             const char* query_string,
                             InstrumentInfoGetter_C *pgetter )
{
    return InstrumentInfoGetter_Create_ABI( pcreds, (int)search_type,
                                            query_string, pgetter, 0 );
}


static inline int
InstrumentInfoGetter_Destroy( InstrumentInfoGetter_C *pgetter )
{ return InstrumentInfoGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(InstrumentInfoGetter)

static inline int
InstrumentInfoGetter_GetSearchType( InstrumentInfoGetter_C *pgetter,
                                    InstrumentSearchType *search_type )
{ return InstrumentInfoGetter_GetSearchType_ABI(pgetter, (int*)search_type, 0); }

static inline int
InstrumentInfoGetter_GetQueryString( InstrumentInfoGetter_C *pgetter,
                                     char **buf,
                                     size_t *n )
{ return InstrumentInfoGetter_GetQueryString_ABI(pgetter, buf, n, 0); }

static inline int
InstrumentInfoGetter_SetQuery( InstrumentInfoGetter_C *pgetter,
                               InstrumentSearchType search_type,
                               const char* query_string )
{ return InstrumentInfoGetter_SetQuery_ABI(pgetter, (int)search_type,
                                           query_string, 0); }

/* OrderGetter */
static inline int
OrderGetter_Create( struct Credentials *pcreds,
                    const char* account_id,
                    const char* order_id,
                    OrderGetter_C *pgetter )
{ return OrderGetter_Create_ABI( pcreds, account_id, order_id, pgetter, 0); }

static inline int
OrderGetter_Destroy(OrderGetter_C *pgetter)
{ return OrderGetter_Destroy_ABI( pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OrderGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(OrderGetter)

static inline int
OrderGetter_GetOrderId( OrderGetter_C *pgetter, char **buf, size_t *n )
{ return OrderGetter_GetOrderId_ABI( pgetter, buf, n, 0 ); }

static inline int
OrderGetter_SetOrderId( OrderGetter_C *pgetter, const char *order_id )
{ return OrderGetter_SetOrderId_ABI( pgetter, order_id, 0 ); }

/* OrdersGetter */
static inline int
OrdersGetter_Create( struct Credentials *pcreds,
                     const char* account_id,
                     unsigned int nmax_results,
                     const char* from_entered_time,
                     const char* to_entered_time,
                     OrderStatusType order_status_type,
                     OrdersGetter_C *pgetter )
{ return OrdersGetter_Create_ABI(pcreds, account_id, nmax_results,
                                 from_entered_time, to_entered_time,
                                 (int)order_status_type, pgetter, 0); }

static inline int
OrdersGetter_Destroy(OrdersGetter_C *pgetter )
{ return OrdersGetter_Destroy_ABI(pgetter, 0); }

DECL_WRAPPED_API_GETTER_BASE_FUNCS(OrdersGetter)

DECL_WRAPPED_ACCOUNT_GETTER_BASE_FUNCS(OrdersGetter)

static inline int
OrdersGetter_GetNMaxResults( OrdersGetter_C *pgetter, unsigned int *nmax_results )
{ return OrdersGetter_GetNMaxResults_ABI(pgetter, nmax_results, 0); }

static inline int
OrdersGetter_SetNMaxResults( OrdersGetter_C *pgetter, unsigned int nmax_results )
{ return OrdersGetter_SetNMaxResults_ABI(pgetter, nmax_results, 0); }

static inline int
OrdersGetter_GetFromEnteredTime( OrdersGetter_C *pgetter, char** buf, size_t *n )
{ return OrdersGetter_GetFromEnteredTime_ABI(pgetter, buf, n, 0); }

static inline int
OrdersGetter_SetFromEnteredTime( OrdersGetter_C *pgetter,
                                 const char* from_entered_time )
{ return OrdersGetter_SetFromEnteredTime_ABI(pgetter, from_entered_time, 0); }

static inline int
OrdersGetter_GetToEnteredTime( OrdersGetter_C *pgetter, char** buf, size_t *n )
{ return OrdersGetter_GetToEnteredTime_ABI(pgetter, buf, n, 0); }

static inline int
OrdersGetter_SetToEnteredTime( OrdersGetter_C *pgetter,
                               const char* to_entered_time )
{ return OrdersGetter_SetToEnteredTime_ABI(pgetter, to_entered_time, 0); }

static inline int
OrdersGetter_GetOrderStatusType( OrdersGetter_C *pgetter,
                                 OrderStatusType *order_status_type )
{ return OrdersGetter_GetOrderStatusType_ABI(pgetter, (int*)order_status_type, 0); }

static inline int
OrdersGetter_SetOrderStatusType( OrdersGetter_C *pgetter,
                                 OrderStatusType order_status_type )
{ return OrdersGetter_SetOrderStatusType_ABI(pgetter, (int)order_status_type, 0); }


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

private:
    std::unique_ptr<CType, CProxyDestroyer<CType>> _cgetter;

protected:
    template<typename CTy=CType>
    CTy*
    cgetter() const
    { return reinterpret_cast<CTy*>( const_cast<CType*>(_cgetter.get()) ); }

    template<typename CTy, typename F, typename F2, typename... Args>
    APIGetter(
            CTy _,
            F create_func,
            F2 destroy_func,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type *v,
            Args... args
            )
        :
            _cgetter(new CType{0,0}, CProxyDestroyer<CType>(destroy_func))
        {
            /* IF WE THROW BEFORE HERE WE MAY LEAK IN DERIVED */
            if( create_func )
                call_abi( create_func, args..., cgetter<CTy>() );
        }

    template<typename CTy, typename F>
    APIGetter(
            CTy _,
            F destroy_func,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type *v
                 = nullptr )
        :
            _cgetter(new CType{0,0}, CProxyDestroyer<CType>(destroy_func))
        {
        }

    virtual
    ~APIGetter(){}

public:
    APIGetter( APIGetter&& ) = default;

    APIGetter&
    operator=( APIGetter&& ) = default;

    APIGetter( const APIGetter& ) = delete;

    APIGetter&
    operator=( const APIGetter& ) = delete;

    static std::chrono::milliseconds
    get_def_wait_msec()
    {
        unsigned long long w;
        call_abi( APIGetter_GetDefWaitMSec_ABI, &w );
        return std::chrono::milliseconds(w);
    }

    static std::chrono::milliseconds
    get_wait_msec()
    {
        unsigned long long w;
        call_abi( APIGetter_GetWaitMSec_ABI, &w );
        return std::chrono::milliseconds(w);
    }

    static void
    set_wait_msec(std::chrono::milliseconds msec)
    {
        call_abi( APIGetter_SetWaitMSec_ABI,
                  static_cast<unsigned long long>(msec.count()) );
    }

    static std::chrono::milliseconds
    wait_remaining()
    {
        unsigned long long w;
        call_abi( APIGetter_WaitRemaining_ABI, &w );
        return std::chrono::milliseconds(w);
    }

    json
    get()
    {
        char *buf;
        size_t n;
        call_abi( APIGetter_Get_ABI, _cgetter.get(), &buf, &n );
        json j = (n > 1) ? json::parse(std::string(buf)) : json();
        if(buf)
            free(buf);
        return j;
    }

    void
    close()
    { call_abi(APIGetter_Close_ABI, _cgetter.get() ); }

    bool
    is_closed() const
    {
        int b;
        call_abi( APIGetter_IsClosed_ABI, _cgetter.get(), &b );
        return static_cast<bool>(b);
    }
};


class QuoteGetter
        : public APIGetter {
public:
    typedef QuoteGetter_C CType;

    QuoteGetter( Credentials& creds, const std::string& symbol )
        :
            APIGetter( QuoteGetter_C{},
                       QuoteGetter_Create_ABI,
                       QuoteGetter_Destroy_ABI,
                       nullptr,
                       &creds,
                       symbol.c_str() )
        {
        }

    std::string
    get_symbol() const
    { return str_from_abi(QuoteGetter_GetSymbol_ABI, cgetter<CType>()); }

    void
    set_symbol(const std::string& symbol)
    {
        call_abi( QuoteGetter_SetSymbol_ABI, cgetter<CType>(), symbol.c_str() );
    }
};

inline json
GetQuote(Credentials& creds, const std::string& symbol)
{ return QuoteGetter(creds, symbol).get(); }


class QuotesGetter
        : public APIGetter {
public:
    typedef QuotesGetter_C CType;

private:
    void
    _str_set_to_abi(
        int(*abicall)(CType*, const char**, size_t, int),
        const std::set<std::string>& symbols
        ) const
    {
        const char** tmp = set_to_new_cstrs(symbols);
        new_array_to_abi( abicall, tmp, cgetter<CType>(), tmp, symbols.size() );
    }

public:
    QuotesGetter(Credentials& creds, const std::set<std::string>& symbols)
        :
            APIGetter( CType{}, QuotesGetter_Destroy_ABI )
        {
            const char** tmp = set_to_new_cstrs(symbols);
            new_array_to_abi( QuotesGetter_Create_ABI, tmp, &creds, tmp,
                              symbols.size(), cgetter<CType>() );
        }

    std::set<std::string>
    get_symbols() const
    {
        return set_of_strs_from_abi( QuotesGetter_GetSymbols_ABI,
                                     cgetter<CType>() );
    }

    void
    set_symbols(const std::set<std::string>& symbols)
    { _str_set_to_abi(QuotesGetter_SetSymbols_ABI, symbols); }

    void
    add_symbol(const std::string& symbol)
    {
        call_abi( QuotesGetter_AddSymbol_ABI, cgetter<CType>(),
                  symbol.c_str() );
    }

    void
    remove_symbol(const std::string& symbol)
    {
        call_abi( QuotesGetter_RemoveSymbol_ABI, cgetter<CType>(),
                  symbol.c_str() );
    }

    void
    add_symbols(const std::set<std::string>& symbols)
    { _str_set_to_abi(QuotesGetter_AddSymbols_ABI, symbols);}

    void
    remove_symbols(const std::set<std::string>& symbols)
    { _str_set_to_abi(QuotesGetter_RemoveSymbols_ABI, symbols); }
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

    MarketHoursGetter( Credentials& creds,
                          MarketType market_type,
                          const std::string& date )
        :
            APIGetter( MarketHoursGetter_C{},
                       MarketHoursGetter_Create_ABI,
                       MarketHoursGetter_Destroy_ABI,
                       nullptr,
                       &creds,
                       static_cast<int>(market_type),
                       date.c_str() )
        {
        }

    std::string
    get_date() const
    { return str_from_abi(MarketHoursGetter_GetDate_ABI, cgetter<CType>()); }

    void
    set_date(const std::string& date)
    {
        call_abi( MarketHoursGetter_SetDate_ABI, cgetter<CType>(),
                  date.c_str() );
    }

    MarketType
    get_market_type() const
    {
        int m;
        call_abi( MarketHoursGetter_GetMarketType_ABI, cgetter<CType>(), &m );
        return static_cast<MarketType>(m);
    }

    void
    set_market_type(MarketType market_type)
    {
        call_abi( MarketHoursGetter_SetMarketType_ABI, cgetter<CType>(),
                  static_cast<int>(market_type) );
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

    MoversGetter( Credentials& creds,
                  MoversIndex index,
                  MoversDirectionType direction_type,
                  MoversChangeType change_type )
        :
            APIGetter( MoversGetter_C{},
                       MoversGetter_Create_ABI,
                       MoversGetter_Destroy_ABI,
                       nullptr,
                       &creds,
                       static_cast<int>(index),
                       static_cast<int>(direction_type),
                       static_cast<int>(change_type) )
        {
        }

    MoversIndex
    get_index() const
    {
        int m;
        call_abi( MoversGetter_GetIndex_ABI, cgetter<CType>(), &m );
        return static_cast<MoversIndex>(m);
    }

    MoversDirectionType
    get_direction_type() const
    {
        int m;
        call_abi( MoversGetter_GetDirectionType_ABI, cgetter<CType>(), &m );
        return static_cast<MoversDirectionType>(m);
    }

    MoversChangeType
    get_change_type() const
    {
        int m;
        call_abi( MoversGetter_GetChangeType_ABI, cgetter<CType>(), &m );
        return static_cast<MoversChangeType>(m);
    }

    void
    set_index(MoversIndex index)
    {
        call_abi( MoversGetter_SetIndex_ABI, cgetter<CType>(),
                  static_cast<int>(index) );
    }

    void
    set_direction_type(MoversDirectionType direction_type)
    {
        call_abi( MoversGetter_SetDirectionType_ABI, cgetter<CType>(),
                  static_cast<int>(direction_type) );
    }

    void
    set_change_type(MoversChangeType change_type)
    {
        call_abi( MoversGetter_SetChangeType_ABI, cgetter<CType>(),
                  static_cast<int>(change_type) );
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
    template<typename CTy, typename F, typename F2, typename... Args>
    HistoricalGetterBase( CTy _,
                             F create_func,
                             F2 destroy_func,
                             Credentials& creds,
                             const std::string& symbol,
                             Args... args )
        :
            APIGetter( _,
                       create_func,
                       destroy_func,
                       nullptr,
                       &creds,
                       symbol.c_str(),
                       args... )
        {
        }

public:
    std::string
    get_symbol() const
    { return str_from_abi(HistoricalGetterBase_GetSymbol_ABI, cgetter<>()); }

    unsigned int
    get_frequency() const
    {
        unsigned int f;
        call_abi( HistoricalGetterBase_GetFrequency_ABI, cgetter<>(), &f );
        return f;
    }

    FrequencyType
    get_frequency_type() const
    {
        int ft;
        call_abi( HistoricalGetterBase_GetFrequencyType_ABI, cgetter<>(), &ft );
        return static_cast<FrequencyType>(ft);
    }

    bool
    is_extended_hours() const
    {
        int et;
        call_abi( HistoricalGetterBase_IsExtendedHours_ABI, cgetter<>(), &et );
        return static_cast<bool>(et);
    }

    void
    set_symbol(const std::string& symbol)
    {
        call_abi( HistoricalGetterBase_SetSymbol_ABI, cgetter<>(),
                  symbol.c_str() );
    }

    void
    set_extended_hours(bool extended_hours)
    {
        call_abi( HistoricalGetterBase_SetExtendedHours_ABI, cgetter<>(),
                  static_cast<int>(extended_hours) );
    }

    void
    set_frequency(FrequencyType frequency_type, unsigned int frequency)
    {
        call_abi( HistoricalGetterBase_SetFrequency_ABI, cgetter<>(),
                  static_cast<int>(frequency_type), frequency );
    }

};


class HistoricalPeriodGetter
        : public HistoricalGetterBase {
public:
    typedef HistoricalPeriodGetter_C CType;

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
                                  HistoricalPeriodGetter_Destroy_ABI,
                                  creds,
                                  symbol,
                                  static_cast<int>(period_type),
                                  period,
                                  static_cast<int>(frequency_type),
                                  frequency,
                                  static_cast<int>(extended_hours) )
        {
        }

    PeriodType
    get_period_type() const
    {
        int pt;
        call_abi( HistoricalPeriodGetter_GetPeriodType_ABI,
                  cgetter<CType>(), &pt );
        return static_cast<PeriodType>(pt);
    }

    unsigned int
    get_period() const
    {
        unsigned int p;
        call_abi( HistoricalPeriodGetter_GetPeriod_ABI, cgetter<CType>(), &p );
        return p;
    }

    void
    set_period(PeriodType period_type, unsigned int period)
    {
        call_abi( HistoricalPeriodGetter_SetPeriod_ABI, cgetter<CType>(),
                  static_cast<int>(period_type), period );
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
                                  HistoricalRangeGetter_Destroy_ABI,
                                  creds,
                                  symbol,
                                  static_cast<int>(frequency_type),
                                  frequency,
                                  start_msec_since_epoch,
                                  end_msec_since_epoch,
                                  static_cast<int>(extended_hours) )
        {
        }

    unsigned long long
    get_end_msec_since_epoch() const
    {
        unsigned long long ms;
        call_abi( HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI,
                  cgetter<CType>(), &ms );
        return ms;
    }

    unsigned long long
    get_start_msec_since_epoch() const
    {
       unsigned long long ms;
       call_abi( HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI,
                 cgetter<CType>(), &ms );
       return ms;
    }

    void
    set_end_msec_since_epoch(unsigned long long end_msec_since_epoch)
    {
        call_abi( HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI,
                  cgetter<CType>(), end_msec_since_epoch );
    }

    void
    set_start_msec_since_epoch(unsigned long long start_msec_since_epoch)
    {
        call_abi( HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI,
                  cgetter<CType>(), start_msec_since_epoch );
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

    bool
    operator!=(const OptionStrikes& other)
    { return _type != other._type || _value.single != other._value.single; }

    bool
    operator==(const OptionStrikes& other)
    { return !(*this != other); }

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
            THROW_VALUE_EXCEPTION("empty symbols set");
        OptionStrikes o(Type::n_atm);
        o._value.n_atm = n;
        return o;
    }

    static OptionStrikes
    Single(double price)
    {
        if (price < 0.1)
            THROW_VALUE_EXCEPTION("strike price can not be < 0.1");
        OptionStrikes o(Type::single);
        o._value.single = price;
        return o;
    }

    static OptionStrikes
    Range(OptionRangeType range)
    {
        if (range == OptionRangeType::null)
            THROW_VALUE_EXCEPTION("strike range can not be 'null'");
        OptionStrikes o(Type::range);
        o._value.range = range;
        return o;
    }
};

inline std::string
to_string(const OptionStrikes& strikes)
{ return to_string(strikes.get_type()); }

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
                THROW_VALUE_EXCEPTION( to_string(strategy)
                    + " strategy requires spread interval >= .01");
            }
        }

    bool
    operator!=(const OptionStrategy& other)
    {
        return _strategy != other._strategy
            || _spread_interval != other._spread_interval;
    }

    bool
    operator==(const OptionStrategy& other)
    { return !(*this != other); }

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

inline std::string
to_string(const OptionStrategy& strategy)
{
    double d = strategy.get_spread_interval();
    return to_string(strategy.get_strategy())
        + (d ? "(" + util::to_fixedpoint_string(d) + ")" : "");
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
    template<typename CTy, typename F, typename F2, typename... Args>
    OptionChainGetter( CTy _,
                         F create_func,
                         F2 destroy_func,
                         Credentials& creds,
                         const std::string& symbol,
                         Args... args )
        :
            APIGetter( _,
                       create_func,
                       destroy_func,
                       nullptr,
                       &creds,
                       symbol.c_str(),
                       args... )
        {
        }

public:
    typedef OptionChainGetter_C CType;

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
                               OptionChainGetter_Destroy_ABI,
                               creds,
                               symbol,
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>(include_quotes),
                               from_date.c_str(),
                               to_date.c_str(),
                               static_cast<int>(exp_month),
                               static_cast<int>(option_type) )
        {
        }

    std::string
    get_symbol() const
    { return str_from_abi(OptionChainGetter_GetSymbol_ABI, cgetter<CType>()); }

    OptionStrikes
    get_strikes() const
    {
        OptionStrikesValue sv;
        int st;
        call_abi( OptionChainGetter_GetStrikes_ABI, cgetter<CType>(), &st, &sv );
        return OptionStrikes(static_cast<OptionStrikesType>(st), sv);
    }

    OptionContractType
    get_contract_type() const
    {
        int ct;
        call_abi( OptionChainGetter_GetContractType_ABI, cgetter<CType>(), &ct );
        return static_cast<OptionContractType>(ct);
    }

    bool
    includes_quotes() const
    {
        int iq;
        call_abi( OptionChainGetter_IncludesQuotes_ABI, cgetter<CType>(), &iq );
        return static_cast<bool>(iq);
    }

    std::string
    get_from_date() const
    { return str_from_abi(OptionChainGetter_GetFromDate_ABI, cgetter<CType>()); }

    std::string
    get_to_date() const
    { return str_from_abi(OptionChainGetter_GetToDate_ABI, cgetter<CType>()); }

    OptionExpMonth
    get_exp_month() const
    {
        int em;
        call_abi( OptionChainGetter_GetExpMonth_ABI, cgetter<CType>(), &em );
        return static_cast<OptionExpMonth>(em);
    }

    OptionType
    get_option_type() const
    {
        int ot;
        call_abi( OptionChainGetter_GetOptionType_ABI, cgetter<CType>(), &ot );
        return static_cast<OptionType>(ot);
    }

    void
    set_symbol(const std::string& symbol)
    {
        call_abi( OptionChainGetter_SetSymbol_ABI, cgetter<CType>(),
                  symbol.c_str() );
    }

    void
    set_strikes(OptionStrikes strikes)
    {
        call_abi( OptionChainGetter_SetStrikes_ABI, cgetter<CType>(),
                  static_cast<int>(strikes.get_type()), strikes.get_value() );
    }

    void
    set_contract_type(OptionContractType contract_type)
    {
        call_abi( OptionChainGetter_SetContractType_ABI, cgetter<CType>(),
                  static_cast<int>(contract_type) );
    }

    void
    include_quotes(bool include_quotes)
    {
        call_abi( OptionChainGetter_IncludeQuotes_ABI, cgetter<CType>(),
                  static_cast<int>(include_quotes) );
    }

    void
    set_from_date(const std::string& from_date)
    {
        call_abi( OptionChainGetter_SetFromDate_ABI, cgetter<CType>(),
                  from_date.c_str() );
    }

    void
    set_to_date(const std::string& to_date)
    {
        call_abi( OptionChainGetter_SetToDate_ABI, cgetter<CType>(),
                  to_date.c_str() );
    }

    void
    set_exp_month(OptionExpMonth exp_month)
    {
        call_abi( OptionChainGetter_SetExpMonth_ABI, cgetter<CType>(),
                  static_cast<int>(exp_month) );
    }

    void
    set_option_type(OptionType option_type)
    {
        call_abi( OptionChainGetter_SetOptionType_ABI, cgetter<CType>(),
                  static_cast<int>(option_type) );
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
                               OptionChainStrategyGetter_Destroy_ABI,
                               creds,
                               symbol.c_str(),
                               static_cast<int>( strategy.get_strategy() ),
                               strategy.get_spread_interval(),
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>( include_quotes ),
                               from_date.c_str(),
                               to_date.c_str(),
                               static_cast<int>( exp_month ),
                               static_cast<int>( option_type ) )
        {
        }

    OptionStrategy 
    get_strategy() const
    {
        int strat;
        double spread;
        call_abi( OptionChainStrategyGetter_GetStrategy_ABI,
                  cgetter<CType>(), &strat, &spread );
        return OptionStrategy(static_cast<OptionStrategyType>(strat), spread);
    }

    void
    set_strategy(OptionStrategy strategy)
    {
        call_abi( OptionChainStrategyGetter_SetStrategy_ABI,
                  cgetter<CType>(), static_cast<int>(strategy.get_strategy()),
                  strategy.get_spread_interval() );
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
                               OptionChainAnalyticalGetter_Destroy_ABI,
                               creds,
                               symbol.c_str(),
                               volatility,
                               underlying_price,
                               interest_rate,
                               days_to_exp,
                               static_cast<int>( strikes.get_type() ),
                               strikes.get_value(),
                               static_cast<int>( contract_type ),
                               static_cast<int>( include_quotes ),
                               from_date.c_str(),
                               to_date.c_str(),
                               static_cast<int>( exp_month ),
                               static_cast<int>( option_type ) )
        {
        }

    double
    get_volatility() const
    {
        double v;
        call_abi( OptionChainAnalyticalGetter_GetVolatility_ABI,
                  cgetter<CType>(), &v );
        return v;
    }

    double
    get_underlying_price() const
    {
        double p;
        call_abi( OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI,
                  cgetter<CType>(), &p );
        return p;
    }

    double
    get_interest_rate() const
    {
        double r;
        call_abi( OptionChainAnalyticalGetter_GetInterestRate_ABI,
                  cgetter<CType>(), &r );
        return r;
    }

    unsigned int
    get_days_to_exp() const
    {
        unsigned int d;
        call_abi( OptionChainAnalyticalGetter_GetDaysToExp_ABI,
                  cgetter<CType>(), &d );
        return d;
    }

    void
    set_volatility(double volatility)
    {
        call_abi( OptionChainAnalyticalGetter_SetVolatility_ABI,
                  cgetter<CType>(), volatility );
    }

    void
    set_underlying_price(double underlying_price)
    {
        call_abi( OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI,
                  cgetter<CType>(), underlying_price );
    }

    void
    set_interest_rate(double interest_rate)
    {
        call_abi( OptionChainAnalyticalGetter_SetInterestRate_ABI,
                  cgetter<CType>(), interest_rate );
    }

    void
    set_days_to_exp(unsigned int days_to_exp)
    {
        call_abi( OptionChainAnalyticalGetter_SetDaysToExp_ABI,
                  cgetter<CType>(), days_to_exp );
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
    template< typename CTy, typename F, typename F2, typename... Args>
    AccountGetterBase( CTy _,
                         F create_func,
                         F2 destroy_func,
                         Credentials& creds,
                         const std::string& account_id,
                         Args... args )
        :
            APIGetter( _,
                       create_func,
                       destroy_func,
                       nullptr,
                       &creds,
                       account_id.c_str(),
                       args... )
        {
        }

public:
    std::string
    get_account_id() const
    { return str_from_abi(AccountGetterBase_GetAccountId_ABI, cgetter<>()); }

    void
    set_account_id(const std::string& account_id)
    {
        call_abi( AccountGetterBase_SetAccountId_ABI, cgetter<>(),
                  account_id.c_str() );
    }
};


class AccountInfoGetter
        : public AccountGetterBase{
public:
    typedef AccountInfoGetter_C CType;

    AccountInfoGetter( Credentials& creds,
                       const std::string& account_id,
                       bool positions,
                       bool orders )
        :
            AccountGetterBase( AccountInfoGetter_C{},
                               AccountInfoGetter_Create_ABI,
                               AccountInfoGetter_Destroy_ABI,
                               creds,
                               account_id,
                               static_cast<int>(positions),
                               static_cast<int>(orders) )
        {
        }

    bool
    returns_positions() const
    {
        int p;
        call_abi( AccountInfoGetter_ReturnsPositions_ABI, cgetter<CType>(), &p );
        return static_cast<bool>(p);
    }

    bool
    returns_orders() const
    {
        int o;
        call_abi( AccountInfoGetter_ReturnsOrders_ABI, cgetter<CType>(), &o );
        return static_cast<bool>(o);
    }

    void
    return_positions(bool positions)
    {
        call_abi( AccountInfoGetter_ReturnPositions_ABI, cgetter<CType>(),
                  static_cast<int>(positions) );
    }

    void
    return_orders(bool orders)
    {
        call_abi( AccountInfoGetter_ReturnOrders_ABI, cgetter<CType>(),
                  static_cast<int>(orders) );
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

    PreferencesGetter( Credentials& creds, const std::string& account_id )
        :
            AccountGetterBase( PreferencesGetter_C{},
                               PreferencesGetter_Create_ABI,
                               PreferencesGetter_Destroy_ABI,
                               creds,
                               account_id )
        {
        }
};

inline json
GetPreferences(Credentials& creds, const std::string& account_id)
{ return PreferencesGetter(creds, account_id).get(); }


class StreamerSubscriptionKeysGetter
        : public AccountGetterBase{
public:
    typedef StreamerSubscriptionKeysGetter_C CType;

    StreamerSubscriptionKeysGetter( Credentials& creds,
                                        const std::string& account_id )
        :
            AccountGetterBase( StreamerSubscriptionKeysGetter_C{},
                               StreamerSubscriptionKeysGetter_Create_ABI,
                               StreamerSubscriptionKeysGetter_Destroy_ABI,
                               creds,
                               account_id )
        {
        }
};

inline json
GetStreamerSubscriptionKeys(Credentials& creds, const std::string& account_id)
{ return StreamerSubscriptionKeysGetter(creds, account_id).get(); }


class TransactionHistoryGetter
        : public AccountGetterBase{
public:
    typedef TransactionHistoryGetter_C CType;

    TransactionHistoryGetter( Credentials& creds,
                              const std::string& account_id,
                              TransactionType transaction_type = TransactionType::all,
                              const std::string& symbol = "",
                              const std::string& start_date = "",
                              const std::string& end_date = "")
        :
            AccountGetterBase( TransactionHistoryGetter_C{},
                               TransactionHistoryGetter_Create_ABI,
                               TransactionHistoryGetter_Destroy_ABI,
                               creds,
                               account_id,
                               static_cast<int>(transaction_type),
                               symbol.c_str(),
                               start_date.c_str(),
                               end_date.c_str() )
        {
        }

    TransactionType
    get_transaction_type() const
    {
        int tt;
        call_abi( TransactionHistoryGetter_GetTransactionType_ABI,
                  cgetter<CType>(), &tt );
        return static_cast<TransactionType>(tt);
    }

    std::string
    get_symbol() const
    {
        return str_from_abi( TransactionHistoryGetter_GetSymbol_ABI,
                             cgetter<CType>() );
    }

    std::string
    get_start_date() const
    {
        return str_from_abi( TransactionHistoryGetter_GetStartDate_ABI,
                             cgetter<CType>() );
    }

    std::string
    get_end_date() const
    {
        return str_from_abi( TransactionHistoryGetter_GetEndDate_ABI,
                             cgetter<CType>() );
    }

    void
    set_transaction_type(TransactionType transaction_type)
    {
        call_abi( TransactionHistoryGetter_SetTransactionType_ABI,
                  cgetter<CType>(), static_cast<int>(transaction_type) );
    }

    void
    set_symbol(const std::string& symbol)
    {
        call_abi( TransactionHistoryGetter_SetSymbol_ABI,
                  cgetter<CType>(), symbol.c_str() );
    }

    void
    set_start_date(const std::string& start_date)
    {
        call_abi( TransactionHistoryGetter_SetStartDate_ABI,
                  cgetter<CType>(), start_date.c_str() );
    }

    void
    set_end_date(const std::string& end_date)
    {
        call_abi( TransactionHistoryGetter_SetEndDate_ABI,
                  cgetter<CType>(), end_date.c_str() );
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

    IndividualTransactionHistoryGetter(
            Credentials& creds,
            const std::string& account_id,
            const std::string& transaction_id )
         :
             AccountGetterBase( IndividualTransactionHistoryGetter_C{},
                                IndividualTransactionHistoryGetter_Create_ABI,
                                IndividualTransactionHistoryGetter_Destroy_ABI,
                                creds,
                                account_id,
                                transaction_id.c_str() )
         {
         }

    std::string
    get_transaction_id() const
    {
        return str_from_abi(
            IndividualTransactionHistoryGetter_GetTransactionId_ABI,
            cgetter<CType>() );
    }

    void
    set_transaction_id(const std::string& transaction_id)
    {
        call_abi( IndividualTransactionHistoryGetter_SetTransactionId_ABI,
                  cgetter<CType>(), transaction_id.c_str() );
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

    UserPrincipalsGetter( Credentials& creds,                         
                          bool streamer_subscription_keys,
                          bool streamer_connection_info,
                          bool preferences,
                          bool surrogate_ids )
        :
            APIGetter( UserPrincipalsGetter_C{},
                       UserPrincipalsGetter_Create_ABI,
                       UserPrincipalsGetter_Destroy_ABI,
                       nullptr,
                       &creds,
                       static_cast<int>(streamer_subscription_keys),
                       static_cast<int>(streamer_connection_info),
                       static_cast<int>(preferences),
                       static_cast<int>(surrogate_ids) )
        {
        }

    bool
    returns_streamer_subscription_keys() const
    {
        int k;
        call_abi( UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI,
                  cgetter<CType>(), &k );
        return static_cast<bool>(k);
    }

    bool
    returns_streamer_connection_info() const
    {
        int ci;
        call_abi( UserPrincipalsGetter_ReturnsConnectionInfo_ABI,
                  cgetter<CType>(), &ci );
        return static_cast<bool>(ci);
    }

    bool
    returns_preferences() const
    {
        int p;
        call_abi( UserPrincipalsGetter_ReturnsPreferences_ABI,
                  cgetter<CType>(), &p );
        return static_cast<bool>(p);
    }

    bool
    returns_surrogate_ids() const
    {
        int si;
        call_abi( UserPrincipalsGetter_ReturnsSurrogateIds_ABI,
                  cgetter<CType>(), &si );
        return static_cast<bool>(si);
    }

    void
    return_streamer_subscription_keys(bool streamer_subscription_keys)
    {
        call_abi( UserPrincipalsGetter_ReturnSubscriptionKeys_ABI,
                  cgetter<CType>(),
                  static_cast<int>(streamer_subscription_keys) );
    }

    void
    return_streamer_connection_info(bool streamer_connection_info)
    {
        call_abi( UserPrincipalsGetter_ReturnConnectionInfo_ABI,
                  cgetter<CType>(), static_cast<int>(streamer_connection_info) );
    }

    void
    return_preferences(bool preferences)
    {
        call_abi( UserPrincipalsGetter_ReturnPreferences_ABI,
                  cgetter<CType>(), static_cast<int>(preferences) );
    }

    void
    return_surrogate_ids(bool surrogate_ids)
    {
        call_abi( UserPrincipalsGetter_ReturnSurrogateIds_ABI,
                  cgetter<CType>(), static_cast<int>(surrogate_ids) );
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

    InstrumentInfoGetter( Credentials& creds,
                            InstrumentSearchType search_type,
                            const std::string& query_string )
        : APIGetter( InstrumentInfoGetter_C{},
                     InstrumentInfoGetter_Create_ABI,
                     InstrumentInfoGetter_Destroy_ABI,
                     nullptr,
                     &creds,
                     static_cast<int>(search_type),
                     query_string.c_str() )
        {
        }

    std::string
    get_query_string() const
    {
        return str_from_abi( InstrumentInfoGetter_GetQueryString_ABI,
                             cgetter<CType>() );
    }

    InstrumentSearchType
    get_search_type() const
    {
        int ist;
        call_abi( InstrumentInfoGetter_GetSearchType_ABI, cgetter<CType>(),
                  &ist );
        return static_cast<InstrumentSearchType>(ist);
    }

    void
    set_query( InstrumentSearchType search_type,
                const std::string& query_string )
    {
        call_abi( InstrumentInfoGetter_SetQuery_ABI, cgetter<CType>(),
                  static_cast<int>(search_type), query_string.c_str() );
    }
};

inline json
GetInstrumentInfo( Credentials& creds,
                   InstrumentSearchType search_type,
                   const std::string& query_string )
{ return InstrumentInfoGetter(creds, search_type, query_string).get(); }


class OrderGetter
        : public AccountGetterBase{
public:
    typedef OrderGetter_C CType;

    OrderGetter( Credentials& creds,
                  const std::string& account_id,
                  const std::string& order_id )
        :
            AccountGetterBase( OrderGetter_C{},
                               OrderGetter_Create_ABI,
                               OrderGetter_Destroy_ABI,
                               creds,
                               account_id,
                               order_id.c_str() )
        {
        }

    std::string
    get_order_id() const
    { return str_from_abi(OrderGetter_GetOrderId_ABI, cgetter<CType>()); }

    void
    set_order_id(const std::string& order_id)
    { call_abi(OrderGetter_SetOrderId_ABI, cgetter<CType>(), order_id.c_str()); }
};

inline json
GetOrder( Credentials& creds,
          const std::string& account_id,
          const std::string& order_id )
{ return OrderGetter(creds, account_id, order_id). get(); }


class OrdersGetter
        : public AccountGetterBase{
public:
    typedef OrdersGetter_C CType;

    OrdersGetter( Credentials& creds,
                    const std::string& account_id,
                    unsigned int nmax_results,
                    const std::string& from_entered_time,
                    const std::string& to_entered_time,
                    OrderStatusType order_status_type )
        :
            AccountGetterBase( OrdersGetter_C{},
                               OrdersGetter_Create_ABI,
                               OrdersGetter_Destroy_ABI,
                               creds,
                               account_id,
                               nmax_results,
                               from_entered_time.c_str(),
                               to_entered_time.c_str(),
                               static_cast<int>(order_status_type) )
        {
        }

    unsigned int
    get_nmax_results() const
    {
        unsigned int m;
        call_abi( OrdersGetter_GetNMaxResults_ABI, cgetter<CType>(),
                  &m );
        return m;
    }

    std::string
    get_from_entered_time() const
    { return str_from_abi( OrdersGetter_GetFromEnteredTime_ABI,
                           cgetter<CType>() ); }

    std::string
    get_to_entered_time() const
    { return str_from_abi( OrdersGetter_GetToEnteredTime_ABI,
                           cgetter<CType>() ); }

    OrderStatusType
    get_order_status_type() const
    {
        int os;
        call_abi( OrdersGetter_GetOrderStatusType_ABI, cgetter<CType>(), &os );
        return static_cast<OrderStatusType>(os);
    }

    void
    set_nmax_results(unsigned int nmax_results)
    { call_abi( OrdersGetter_SetNMaxResults_ABI, cgetter<CType>(), nmax_results); }

    void
    set_from_entered_time(const std::string& from_entered_time)
    { call_abi( OrdersGetter_SetFromEnteredTime_ABI, cgetter<CType>(),
                from_entered_time.c_str() ); }

    void
    set_to_entered_time(const std::string& to_entered_time)
    { call_abi( OrdersGetter_SetToEnteredTime_ABI, cgetter<CType>(),
                to_entered_time.c_str() ); }

    void
    set_order_status_type(OrderStatusType order_status_type)
    { call_abi( OrdersGetter_SetOrderStatusType_ABI, cgetter<CType>(),
                static_cast<int>(order_status_type) ); }
};

inline json
GetOrders( Credentials& creds,
            const std::string& account_id,
            size_t nmax_results,
            const std::string& from_entered_time,
            const std::string& to_entered_time,
            OrderStatusType order_status_type  )
{ return OrdersGetter(creds, account_id, nmax_results, from_entered_time,
                      to_entered_time, order_status_type). get(); }

} /* tdma */

#undef THROW_VALUE_EXCEPTION

#endif /* __cplusplus */

#endif // TDMA_API_GET_H
