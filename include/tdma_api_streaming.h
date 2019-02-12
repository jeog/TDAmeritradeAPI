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

#ifndef TDMA_API_STREAMING_H
#define TDMA_API_STREAMING_H

#include "_common.h"
#include "tdma_common.h"

#ifdef __cplusplus
#include <map>
#include <set>
#include <memory>
#include <thread>
#include <string>

#include "websocket_connect.h"
#include "threadsafe_hashmap.h"

#endif /* __cplusplus */


#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamerServiceType, n)
DECL_C_CPP_TDMA_ENUM(StreamerServiceType, 1, 19,
    BUILD_ENUM_NAME( NONE ),
    BUILD_ENUM_NAME( QUOTE ),
    BUILD_ENUM_NAME( OPTION ),
    BUILD_ENUM_NAME( LEVELONE_FUTURES ),
    BUILD_ENUM_NAME( LEVELONE_FOREX ),
    BUILD_ENUM_NAME( LEVELONE_FUTURES_OPTIONS ),
    BUILD_ENUM_NAME( NEWS_HEADLINE ),
    BUILD_ENUM_NAME( CHART_EQUITY ),
    BUILD_ENUM_NAME( CHART_FOREX ), /* NOT WORKING */
    BUILD_ENUM_NAME( CHART_FUTURES ),
    BUILD_ENUM_NAME( CHART_OPTIONS ),
    BUILD_ENUM_NAME( TIMESALE_EQUITY ),
    BUILD_ENUM_NAME( TIMESALE_FOREX ), /* NOT WORKING */
    BUILD_ENUM_NAME( TIMESALE_FUTURES ),
    BUILD_ENUM_NAME( TIMESALE_OPTIONS ),
    BUILD_ENUM_NAME( ACTIVES_NASDAQ ),
    BUILD_ENUM_NAME( ACTIVES_NYSE ),
    BUILD_ENUM_NAME( ACTIVES_OTCBB ),
    BUILD_ENUM_NAME( ACTIVES_OPTIONS ),
    BUILD_ENUM_NAME( ADMIN ) // <- NOTE THIS DOESNT MATCH TYPE_ID_SUB_[] consts
    /* NOT IMPLEMENTED YET */
    //BUILD_ENUM_NAME( CHART_HISTORY_FUTURES),
    //BUILD_ENUM_NAME( ACCT_ACTIVITY),
    /* NOT DOCUMENTED BY TDMA */
    //BUILD_ENUM_NAME( FOREX_BOOK,
    //BUILD_ENUM_NAME( FUTURES_BOOK),
    //BUILD_ENUM_NAME( LISTED_BOOK),
    //BUILD_ENUM_NAME( NASDAQ_BOOK),
    //BUILD_ENUM_NAME( OPTIONS_BOOK),
    //BUILD_ENUM_NAME( FUTURES_OPTION_BOOK),
    //BUILD_ENUM_NAME( NEWS_STORY),
    //BUILD_ENUM_NAME( NEWS_HEADLINE_LIST),
    /* OLD API ? */
    //BUILD_ENUM_NAME( STREAMER_SERVER)
    );
#undef BUILD_ENUM_NAME


DECL_C_CPP_TDMA_ENUM(QOSType, 0, 5,
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, express),   /* 500 ms */
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, real_time), /* 750 ms */
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, fast),      /* 1000 ms DEFAULT */
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, moderate),  /* 1500 ms */
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, slow),      /* 3000 ms */
    BUILD_C_CPP_TDMA_ENUM_NAME(QOSType, delayed)    /* 5000 ms */
    );

DECL_C_CPP_TDMA_ENUM(CommandType, 0, 3,
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, SUBS),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, UNSUBS),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, ADD),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, VIEW),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, LOGIN),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, LOGOUT),
    BUILD_C_CPP_TDMA_ENUM_NAME(CommandType, QOS)
);


#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(QuotesSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(QuotesSubscriptionField, 0, 52,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(bid_price),
    BUILD_ENUM_NAME(ask_price),
    BUILD_ENUM_NAME(last_price),
    BUILD_ENUM_NAME(bid_size),
    BUILD_ENUM_NAME(ask_size),
    BUILD_ENUM_NAME(ask_id),
    BUILD_ENUM_NAME(bid_id),
    BUILD_ENUM_NAME(total_volume),
    BUILD_ENUM_NAME(last_size),
    BUILD_ENUM_NAME(trade_time),
    BUILD_ENUM_NAME(quote_time),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(bid_tick),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(exchange_id),
    BUILD_ENUM_NAME(marginable),
    BUILD_ENUM_NAME(shortable),
    BUILD_ENUM_NAME(island_bid),
    BUILD_ENUM_NAME(island_ask),
    BUILD_ENUM_NAME(island_volume),
    BUILD_ENUM_NAME(quote_day),
    BUILD_ENUM_NAME(trade_day),
    BUILD_ENUM_NAME(volatility),
    BUILD_ENUM_NAME(description),
    BUILD_ENUM_NAME(last_id),
    BUILD_ENUM_NAME(digits),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(net_change),
    BUILD_ENUM_NAME(high_52_week),
    BUILD_ENUM_NAME(low_52_week),
    BUILD_ENUM_NAME(pe_ratio),
    BUILD_ENUM_NAME(dividend_amount),
    BUILD_ENUM_NAME(dividend_yeild),
    BUILD_ENUM_NAME(island_bid_size),
    BUILD_ENUM_NAME(island_ask_size),
    BUILD_ENUM_NAME(nav),
    BUILD_ENUM_NAME(fund_price),
    BUILD_ENUM_NAME(exchanged_name),
    BUILD_ENUM_NAME(dividend_date),
    BUILD_ENUM_NAME(regular_market_quote),
    BUILD_ENUM_NAME(regular_market_trade),
    BUILD_ENUM_NAME(regular_market_last_price),
    BUILD_ENUM_NAME(regular_market_last_size),
    BUILD_ENUM_NAME(regular_market_trade_time),
    BUILD_ENUM_NAME(regular_market_trade_day),
    BUILD_ENUM_NAME(regular_market_net_change),
    BUILD_ENUM_NAME(security_status),
    BUILD_ENUM_NAME(mark),
    BUILD_ENUM_NAME(quote_time_as_long),
    BUILD_ENUM_NAME(trade_time_as_long),
    BUILD_ENUM_NAME(regular_market_trade_time_as_long)
    );
#undef BUILD_ENUM_NAME

DECL_C_CPP_TDMA_ENUM(DurationType, 0, 5,
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, all_day),
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, min_60),
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, min_30),
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, min_10),
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, min_5),
    BUILD_C_CPP_TDMA_ENUM_NAME(DurationType, min_1)
    );

DECL_C_CPP_TDMA_ENUM(VenueType, 0, 5,
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, opts),
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, calls),
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, puts),
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, opts_desc),
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, calls_desc),
    BUILD_C_CPP_TDMA_ENUM_NAME(VenueType, puts_desc)
    );

#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(OptionsSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(OptionsSubscriptionField, 0, 41,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(description),
    BUILD_ENUM_NAME(bid_price),
    BUILD_ENUM_NAME(ask_price),
    BUILD_ENUM_NAME(last_price),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(total_volume),
    BUILD_ENUM_NAME(open_interest),
    BUILD_ENUM_NAME(volatility),
    BUILD_ENUM_NAME(quote_time),
    BUILD_ENUM_NAME(trade_time),
    BUILD_ENUM_NAME(money_intrinsic_value),
    BUILD_ENUM_NAME(quote_day),
    BUILD_ENUM_NAME(trade_day),
    BUILD_ENUM_NAME(expiration_year),
    BUILD_ENUM_NAME(multiplier),
    BUILD_ENUM_NAME(digits),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(bid_size),
    BUILD_ENUM_NAME(ask_size),
    BUILD_ENUM_NAME(last_size),
    BUILD_ENUM_NAME(net_change),
    BUILD_ENUM_NAME(strike_price),
    BUILD_ENUM_NAME(contract_type),
    BUILD_ENUM_NAME(underlying),
    BUILD_ENUM_NAME(expiration_month),
    BUILD_ENUM_NAME(deliverables),
    BUILD_ENUM_NAME(time_value),
    BUILD_ENUM_NAME(expiration_day),
    BUILD_ENUM_NAME(days_to_expiration),
    BUILD_ENUM_NAME(delta),
    BUILD_ENUM_NAME(gamma),
    BUILD_ENUM_NAME(theta),
    BUILD_ENUM_NAME(vega),
    BUILD_ENUM_NAME(rho),
    BUILD_ENUM_NAME(security_status),
    BUILD_ENUM_NAME(theoretical_option_value),
    BUILD_ENUM_NAME(underlying_price),
    BUILD_ENUM_NAME(uv_expiration_type),
    BUILD_ENUM_NAME(mark)
    );
#undef BUILD_ENUM_NAME

#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(LevelOneFuturesSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(LevelOneFuturesSubscriptionField, 0, 35,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(bid_price),
    BUILD_ENUM_NAME(ask_price),
    BUILD_ENUM_NAME(last_price),
    BUILD_ENUM_NAME(bid_size),
    BUILD_ENUM_NAME(ask_size),
    BUILD_ENUM_NAME(ask_id),
    BUILD_ENUM_NAME(bid_id),
    BUILD_ENUM_NAME(total_volume),
    BUILD_ENUM_NAME(last_size),
    BUILD_ENUM_NAME(quote_time),
    BUILD_ENUM_NAME(trade_time),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(exchange_id),
    BUILD_ENUM_NAME(description),
    BUILD_ENUM_NAME(last_id),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(net_change),
    BUILD_ENUM_NAME(future_percent_change),
    BUILD_ENUM_NAME(exchange_name),
    BUILD_ENUM_NAME(security_status),
    BUILD_ENUM_NAME(open_interest),
    BUILD_ENUM_NAME(mark),
    BUILD_ENUM_NAME(tick),
    BUILD_ENUM_NAME(tick_amount),
    BUILD_ENUM_NAME(product),
    BUILD_ENUM_NAME(future_price_format),
    BUILD_ENUM_NAME(future_trading_hours),
    BUILD_ENUM_NAME(future_is_tradable),
    BUILD_ENUM_NAME(future_multiplier),
    BUILD_ENUM_NAME(future_is_active),
    BUILD_ENUM_NAME(future_settlement_price),
    BUILD_ENUM_NAME(future_active_symbol),
    BUILD_ENUM_NAME(future_expiration_date)
    );
#undef BUILD_ENUM_NAME

#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(LevelOneForexSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(LevelOneForexSubscriptionField, 0, 29,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(bid_price),
    BUILD_ENUM_NAME(ask_price),
    BUILD_ENUM_NAME(last_price),
    BUILD_ENUM_NAME(bid_size),
    BUILD_ENUM_NAME(ask_size),
    BUILD_ENUM_NAME(total_volume),
    BUILD_ENUM_NAME(last_size),
    BUILD_ENUM_NAME(quote_time),
    BUILD_ENUM_NAME(trade_time),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(exchange_id),
    BUILD_ENUM_NAME(description),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(net_change),
    BUILD_ENUM_NAME(percent_change),
    BUILD_ENUM_NAME(exchange_name),
    BUILD_ENUM_NAME(digits),
    BUILD_ENUM_NAME(security_status),
    BUILD_ENUM_NAME(tick),
    BUILD_ENUM_NAME(tick_amount),
    BUILD_ENUM_NAME(product),
    BUILD_ENUM_NAME(trading_hours),
    BUILD_ENUM_NAME(is_tradable),
    BUILD_ENUM_NAME(market_maker),
    BUILD_ENUM_NAME(high_52_week),
    BUILD_ENUM_NAME(low_52_week),
    BUILD_ENUM_NAME(mark)
    );
#undef BUILD_ENUM_NAME



#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(LevelOneFuturesOptionsSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(LevelOneFuturesOptionsSubscriptionField, 0, 35,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(bid_price),
    BUILD_ENUM_NAME(ask_price),
    BUILD_ENUM_NAME(last_price),
    BUILD_ENUM_NAME(bid_size),
    BUILD_ENUM_NAME(ask_size),
    BUILD_ENUM_NAME(ask_id),
    BUILD_ENUM_NAME(bid_id),
    BUILD_ENUM_NAME(total_volume),
    BUILD_ENUM_NAME(last_size),
    BUILD_ENUM_NAME(quote_time),
    BUILD_ENUM_NAME(trade_time),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(exchange_id),
    BUILD_ENUM_NAME(description),
    BUILD_ENUM_NAME(last_id),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(net_change),
    BUILD_ENUM_NAME(future_percent_change),
    BUILD_ENUM_NAME(exchange_name),
    BUILD_ENUM_NAME(security_status),
    BUILD_ENUM_NAME(open_interest),
    BUILD_ENUM_NAME(mark),
    BUILD_ENUM_NAME(tick),
    BUILD_ENUM_NAME(tick_amount),
    BUILD_ENUM_NAME(product),
    BUILD_ENUM_NAME(future_price_format),
    BUILD_ENUM_NAME(future_trading_hours),
    BUILD_ENUM_NAME(future_is_tradable),
    BUILD_ENUM_NAME(future_multiplier),
    BUILD_ENUM_NAME(future_is_active),
    BUILD_ENUM_NAME(future_settlement_price),
    BUILD_ENUM_NAME(future_active_symbol),
    BUILD_ENUM_NAME(future_expiration_date)
    );
#undef BUILD_ENUM_NAME


#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(NewsHeadlineSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(NewsHeadlineSubscriptionField, 0, 10,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(error_code),
    BUILD_ENUM_NAME(story_datetime),
    BUILD_ENUM_NAME(headline_id),
    BUILD_ENUM_NAME(status),
    BUILD_ENUM_NAME(headline),
    BUILD_ENUM_NAME(story_id),
    BUILD_ENUM_NAME(count_for_keyword),
    BUILD_ENUM_NAME(keyword_array),
    BUILD_ENUM_NAME(is_host),
    BUILD_ENUM_NAME(story_source)
    );
#undef BUILD_ENUM_NAME


#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(ChartEquitySubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(ChartEquitySubscriptionField, 0, 8,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(volume),
    BUILD_ENUM_NAME(sequence),
    BUILD_ENUM_NAME(chart_time),
    BUILD_ENUM_NAME(chart_day)
    );
#undef BUILD_ENUM_NAME

#define BUILD_ENUM_NAME(n) \
    BUILD_C_CPP_TDMA_ENUM_NAME(ChartSubscriptionField, n)
DECL_C_CPP_TDMA_ENUM(ChartSubscriptionField, 0, 6,
    BUILD_ENUM_NAME(symbol),
    BUILD_ENUM_NAME(chart_time),
    BUILD_ENUM_NAME(open_price),
    BUILD_ENUM_NAME(high_price),
    BUILD_ENUM_NAME(low_price),
    BUILD_ENUM_NAME(close_price),
    BUILD_ENUM_NAME(volume)
    );
#undef BUILD_ENUM_NAME

DECL_C_CPP_TDMA_ENUM(TimesaleSubscriptionField, 0, 5,
    BUILD_C_CPP_TDMA_ENUM_NAME(TimesaleSubscriptionField, symbol),
    BUILD_C_CPP_TDMA_ENUM_NAME(TimesaleSubscriptionField, trade_time),
    BUILD_C_CPP_TDMA_ENUM_NAME(TimesaleSubscriptionField, last_price),
    BUILD_C_CPP_TDMA_ENUM_NAME(TimesaleSubscriptionField, last_size),
    BUILD_C_CPP_TDMA_ENUM_NAME(TimesaleSubscriptionField, last_sequence)
    );

DECL_C_CPP_TDMA_ENUM(StreamingCallbackType, 0, 6,
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, listening_start),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, listening_stop),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, data),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, request_response),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, notify),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, timeout),
    BUILD_C_CPP_TDMA_ENUM_NAME(StreamingCallbackType, error)
    );



static const int SUBSCRIPTION_MAX_FIELDS = 100;
static const int SUBSCRIPTION_MAX_SYMBOLS = 5000;

#define DECL_CSUB_STRUCT(name) typedef struct{ void *obj; int type_id; } name

DECL_CSUB_STRUCT(QuotesSubscription_C);
DECL_CSUB_STRUCT(OptionsSubscription_C);
DECL_CSUB_STRUCT(LevelOneFuturesSubscription_C);
DECL_CSUB_STRUCT(LevelOneForexSubscription_C);
DECL_CSUB_STRUCT(LevelOneFuturesOptionsSubscription_C);
DECL_CSUB_STRUCT(NewsHeadlineSubscription_C);
DECL_CSUB_STRUCT(ChartEquitySubscription_C);
//DECL_CSUB_STRUCT(ChartForexSubscription_C);
DECL_CSUB_STRUCT(ChartFuturesSubscription_C);
DECL_CSUB_STRUCT(ChartOptionsSubscription_C);
DECL_CSUB_STRUCT(TimesaleEquitySubscription_C);
//DECL_CSUB_STRUCT(TimesalesForexSubscription_C);
DECL_CSUB_STRUCT(TimesaleFuturesSubscription_C);
DECL_CSUB_STRUCT(TimesaleOptionsSubscription_C);
DECL_CSUB_STRUCT(NasdaqActivesSubscription_C);
DECL_CSUB_STRUCT(NYSEActivesSubscription_C);
DECL_CSUB_STRUCT(OTCBBActivesSubscription_C);
DECL_CSUB_STRUCT(OptionActivesSubscription_C);
#undef DECL_CSUB_STRUCT

/* SUBSCRIPTION CREATE METHODS */

#define DECL_CSUB_FIELD_SYM_CREATE_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_Create_ABI( const char **symbols, size_t nsymbols, int *fields, \
                   size_t nfields, int command, name##_C *psub, \
                   int allow_exceptions )

#define DECL_CSUB_DURATION_CREATE_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_Create_ABI( int duration_type, int command, name##_C *psub, \
                   int allow_exceptions )

/* Create methods for subs that takes symbols and fields */
DECL_CSUB_FIELD_SYM_CREATE_FUNC(QuotesSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(OptionsSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneForexSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(NewsHeadlineSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartEquitySubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartFuturesSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartOptionsSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleEquitySubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleOptionsSubscription);
#undef DECL_CSUB_FIELD_SYM_CREATE_FUNC

/* Create methods for subs that take duration */
DECL_CSUB_DURATION_CREATE_FUNC(NasdaqActivesSubscription);
DECL_CSUB_DURATION_CREATE_FUNC(NYSEActivesSubscription);
DECL_CSUB_DURATION_CREATE_FUNC(OTCBBActivesSubscription);
#undef DECL_CSUB_DURATION_CREATE_FUNC

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionActivesSubscription_Create_ABI( int venue,
                                      int duration_type,
                                      int command,
                                      OptionActivesSubscription_C *psub,
                                      int allow_exceptions );


/* SUBSCRIPTION COPY (CONSTRUCT) METHODS */
#define DECL_CSUB_COPY_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_Copy_ABI(name##_C *from, name##_C *to, int allow_exceptions )

DECL_CSUB_COPY_FUNC(QuotesSubscription);
DECL_CSUB_COPY_FUNC(OptionsSubscription);
DECL_CSUB_COPY_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_COPY_FUNC(LevelOneForexSubscription);
DECL_CSUB_COPY_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_COPY_FUNC(NewsHeadlineSubscription);
DECL_CSUB_COPY_FUNC(ChartEquitySubscription);
DECL_CSUB_COPY_FUNC(ChartFuturesSubscription);
DECL_CSUB_COPY_FUNC(ChartOptionsSubscription);
DECL_CSUB_COPY_FUNC(TimesaleEquitySubscription);
DECL_CSUB_COPY_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_COPY_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_COPY_FUNC(NasdaqActivesSubscription);
DECL_CSUB_COPY_FUNC(NYSEActivesSubscription);
DECL_CSUB_COPY_FUNC(OTCBBActivesSubscription);
DECL_CSUB_COPY_FUNC(OptionActivesSubscription);
#undef DECL_CSUB_COPY_FUNC

/* generic copy */

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_Copy_ABI( StreamingSubscription_C *from,
                                StreamingSubscription_C * to,
                                int allow_exceptions );

/* SUBSCRIPTION COMPARE METHOD */
EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_IsSame_ABI( StreamingSubscription_C *l,
                                  StreamingSubscription_C *r,
                                  int *is_same,
                                  int allow_exceptions );

/* SUBSCRIPTION DESTROY METHODS */

#define DECL_CSUB_DESTROY_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_Destroy_ABI(name##_C *psub, int allow_exceptions)

/* Destroy for each subscription type */
DECL_CSUB_DESTROY_FUNC(QuotesSubscription);
DECL_CSUB_DESTROY_FUNC(OptionsSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneForexSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(NewsHeadlineSubscription);
DECL_CSUB_DESTROY_FUNC(ChartEquitySubscription);
DECL_CSUB_DESTROY_FUNC(ChartFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(ChartOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleEquitySubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(NasdaqActivesSubscription);
DECL_CSUB_DESTROY_FUNC(NYSEActivesSubscription);
DECL_CSUB_DESTROY_FUNC(OTCBBActivesSubscription);
DECL_CSUB_DESTROY_FUNC(OptionActivesSubscription);
#undef DECL_CSUB_DESTROY_FUNC

/* Generic destroy (cast to StreamingSubscription_C*) */
EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_Destroy_ABI( StreamingSubscription_C* psub,
                                   int allow_exceptions );


/* SUBSCRIPTION GET METHODS */

/* StreamingSubscription Base Methods */
EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_GetService_ABI( StreamingSubscription_C *psub,
                                      int *service,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_GetCommand_ABI( StreamingSubscription_C *psub,
                                      int *command,
                                      int allow_exceptions );

/* SubscriptionBySymbolBase Base Methods */
EXTERN_C_SPEC_ DLL_SPEC_ int
SubscriptionBySymbolBase_GetSymbols_ABI( StreamingSubscription_C *psub,
                                         char ***buffers,
                                         size_t *n,
                                         int allow_exceptions );

#define DECL_CSUB_GET_FIELDS_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_GetFields_ABI(name##_C *psub, int **fields, size_t *n, int allow_exceptions);

#define DECL_CSUB_GET_FIELDS_BASE_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_GetFields_ABI(StreamingSubscription_C *psub, int **fields, size_t *n, \
                     int allow_exceptions);

/* Get fields methods in derived */
DECL_CSUB_GET_FIELDS_FUNC(QuotesSubscription);
DECL_CSUB_GET_FIELDS_FUNC(OptionsSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneForexSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_GET_FIELDS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_GET_FIELDS_FUNC(ChartEquitySubscription);
#undef DECL_CSUB_GET_FIELDS_FUNC

/* Get fields methods in base */
DECL_CSUB_GET_FIELDS_BASE_FUNC(ChartSubscriptionBase);
DECL_CSUB_GET_FIELDS_BASE_FUNC(TimesaleSubscriptionBase);
#undef DECL_CSUB_GET_FIELDS_BASE_FUNC

/* Get duration in base */
EXTERN_C_SPEC_ DLL_SPEC_ int
ActivesSubscriptionBase_GetDuration_ABI( StreamingSubscription_C *psub,
                                         int *duration,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionActivesSubscription_GetVenue_ABI( OptionActivesSubscription_C *psub,
                                        int *venue,
                                        int allow_exceptions );


/* SUBSCRIPTION SET METHODS */

/* StreamingSubscription Base Methods */
EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSubscription_SetCommand_ABI( StreamingSubscription_C *psub,
                                      int command,
                                      int allow_exceptions );

/* SubscriptionBySymbolBase Base Methods */
EXTERN_C_SPEC_ DLL_SPEC_ int
SubscriptionBySymbolBase_SetSymbols_ABI( StreamingSubscription_C *psub,
                                         const char **buffers,
                                         size_t n,
                                         int allow_exceptions );

#define DECL_CSUB_SET_FIELDS_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_SetFields_ABI(name##_C *psub, int *fields, size_t n, int allow_exceptions);

#define DECL_CSUB_SET_FIELDS_BASE_FUNC(name) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
name##_SetFields_ABI(StreamingSubscription_C *psub, int *fields, size_t n, \
                     int allow_exceptions);

/* Set fields methods in derived */
DECL_CSUB_SET_FIELDS_FUNC(QuotesSubscription);
DECL_CSUB_SET_FIELDS_FUNC(OptionsSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneForexSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_SET_FIELDS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_SET_FIELDS_FUNC(ChartEquitySubscription);
#undef DECL_CSUB_SET_FIELDS_FUNC

/* Set fields methods in base */
DECL_CSUB_SET_FIELDS_BASE_FUNC(ChartSubscriptionBase);
DECL_CSUB_SET_FIELDS_BASE_FUNC(TimesaleSubscriptionBase);
#undef DECL_CSUB_SET_FIELDS_BASE_FUNC

/* Set duration in base */
EXTERN_C_SPEC_ DLL_SPEC_ int
ActivesSubscriptionBase_SetDuration_ABI( StreamingSubscription_C *psub,
                                         int duration,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OptionActivesSubscription_SetVenue_ABI( OptionActivesSubscription_C *psub,
                                        int venue,
                                        int allow_exceptions );


#ifndef __cplusplus

/* C Interface */

/* SUBSCRIPTION CREATE METHODS */

#define DECL_CSUB_FIELD_SYM_CREATE_FUNC(name, fname) \
static inline int \
name##_Create( const char **symbols, size_t nsymbols, fname *fields, \
               size_t nfields, CommandType command, name##_C *psub ) \
{ return name##_Create_ABI(symbols, nsymbols, (int*)fields, nfields, \
                           (int)command, psub, 0); }

#define DECL_CSUB_DURATION_CREATE_FUNC(name) \
static inline int \
name##_Create( DurationType duration_type, CommandType command, \
               name##_C *psub ) \
{ return name##_Create_ABI((int)duration_type, (int)command, psub, 0); }

/* Create methods for subs that takes symbols and fields */
DECL_CSUB_FIELD_SYM_CREATE_FUNC(QuotesSubscription,
    QuotesSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(OptionsSubscription,
    OptionsSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneFuturesSubscription,
    LevelOneFuturesSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneForexSubscription,
    LevelOneForexSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(LevelOneFuturesOptionsSubscription,
    LevelOneFuturesOptionsSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(NewsHeadlineSubscription,
    NewsHeadlineSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartEquitySubscription,
    ChartEquitySubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartFuturesSubscription,
    ChartSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(ChartOptionsSubscription,
    ChartSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleEquitySubscription,
    TimesaleSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleFuturesSubscription,
    TimesaleSubscriptionField);
DECL_CSUB_FIELD_SYM_CREATE_FUNC(TimesaleOptionsSubscription,
    TimesaleSubscriptionField);
#undef DECL_CSUB_FIELD_SYM_CREATE_FUNC

/* Create methods for subs that take duration */
DECL_CSUB_DURATION_CREATE_FUNC(NasdaqActivesSubscription);
DECL_CSUB_DURATION_CREATE_FUNC(NYSEActivesSubscription);
DECL_CSUB_DURATION_CREATE_FUNC(OTCBBActivesSubscription);
#undef DECL_CSUB_DURATION_CREATE_FUNC

static inline int
OptionActivesSubscription_Create( VenueType venue,
                                  DurationType duration_type,
                                  CommandType command,
                                  OptionActivesSubscription_C *psub)
{ return OptionActivesSubscription_Create_ABI((int)venue, (int)duration_type,
                                               (int)command, psub, 0); }

/* SUBSCRIPTION COPY (CONSTRUCTOR) METHODS */

#define DECL_CSUB_COPY_FUNC(name) \
static inline int \
name##_Copy(name##_C *from, name##_C *to) \
{ return name##_Copy_ABI(from, to, 0); }

/* Destroy for each subscription type */
DECL_CSUB_COPY_FUNC(QuotesSubscription);
DECL_CSUB_COPY_FUNC(OptionsSubscription);
DECL_CSUB_COPY_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_COPY_FUNC(LevelOneForexSubscription);
DECL_CSUB_COPY_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_COPY_FUNC(NewsHeadlineSubscription);
DECL_CSUB_COPY_FUNC(ChartEquitySubscription);
DECL_CSUB_COPY_FUNC(ChartFuturesSubscription);
DECL_CSUB_COPY_FUNC(ChartOptionsSubscription);
DECL_CSUB_COPY_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_COPY_FUNC(TimesaleEquitySubscription);
DECL_CSUB_COPY_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_COPY_FUNC(NasdaqActivesSubscription);
DECL_CSUB_COPY_FUNC(NYSEActivesSubscription);
DECL_CSUB_COPY_FUNC(OTCBBActivesSubscription);
DECL_CSUB_COPY_FUNC(OptionActivesSubscription);
#undef DECL_CSUB_DESTROY_FUNC

/* SUBSCRIPTION COMPARE METHOD */
static inline int
StreamingSubscription_IsSame( StreamingSubscription_C *l,
                              StreamingSubscription_C *r,
                              int *is_same )

{ return StreamingSubscription_IsSame_ABI(l, r, is_same, 0); }

/* SUBSCRIPTION DESTROY METHODS */

#define DECL_CSUB_DESTROY_FUNC(name) \
static inline int \
name##_Destroy(name##_C *psub) \
{ return name##_Destroy_ABI(psub, 0); }

/* Destroy for each subscription type */
DECL_CSUB_DESTROY_FUNC(QuotesSubscription);
DECL_CSUB_DESTROY_FUNC(OptionsSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneForexSubscription);
DECL_CSUB_DESTROY_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(NewsHeadlineSubscription);
DECL_CSUB_DESTROY_FUNC(ChartEquitySubscription);
DECL_CSUB_DESTROY_FUNC(ChartFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(ChartOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleEquitySubscription);
DECL_CSUB_DESTROY_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_DESTROY_FUNC(NasdaqActivesSubscription);
DECL_CSUB_DESTROY_FUNC(NYSEActivesSubscription);
DECL_CSUB_DESTROY_FUNC(OTCBBActivesSubscription);
DECL_CSUB_DESTROY_FUNC(OptionActivesSubscription);
#undef DECL_CSUB_DESTROY_FUNC

/* Generic destroy (cast to StreamingSubscription_C*) */
static inline int
StreamingSubscription_Destroy(StreamingSubscription_C* psub)
{ return StreamingSubscription_Destroy_ABI(psub, 0); }


/* SUBSCRIPTION GET METHODS */

#define DECL_CSUB_GET_SERVICE_FUNC(name) \
static inline int \
name##_GetService( name##_C *psub, StreamerServiceType *service ) \
{ return StreamingSubscription_GetService_ABI( (StreamingSubscription_C*)psub, \
                                               (int*)service, 0); }

/* GetService methods for each sub type */
DECL_CSUB_GET_SERVICE_FUNC(QuotesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(OptionsSubscription);
DECL_CSUB_GET_SERVICE_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(LevelOneForexSubscription);
DECL_CSUB_GET_SERVICE_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_GET_SERVICE_FUNC(NewsHeadlineSubscription);
DECL_CSUB_GET_SERVICE_FUNC(ChartEquitySubscription);
DECL_CSUB_GET_SERVICE_FUNC(ChartFuturesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(ChartOptionsSubscription);
DECL_CSUB_GET_SERVICE_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(TimesaleEquitySubscription);
DECL_CSUB_GET_SERVICE_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_GET_SERVICE_FUNC(NasdaqActivesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(NYSEActivesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(OTCBBActivesSubscription);
DECL_CSUB_GET_SERVICE_FUNC(OptionActivesSubscription);
/* GetService generic method (cast to StreamerSubscription_C*) */
DECL_CSUB_GET_SERVICE_FUNC(StreamingSubscription);
#undef DECL_CSUB_GET_SERVICE_FUNC


#define DECL_CSUB_GET_COMMAND_FUNC(name) \
static inline int \
name##_GetCommand( name##_C *psub, CommandType *command ) \
{ return StreamingSubscription_GetCommand_ABI( (StreamingSubscription_C*)psub, \
                                               (int*)command, 0); }

/* GetCommand methods for each sub type */
DECL_CSUB_GET_COMMAND_FUNC(QuotesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(OptionsSubscription);
DECL_CSUB_GET_COMMAND_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(LevelOneForexSubscription);
DECL_CSUB_GET_COMMAND_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_GET_COMMAND_FUNC(NewsHeadlineSubscription);
DECL_CSUB_GET_COMMAND_FUNC(ChartEquitySubscription);
DECL_CSUB_GET_COMMAND_FUNC(ChartFuturesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(ChartOptionsSubscription);
DECL_CSUB_GET_COMMAND_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(TimesaleEquitySubscription);
DECL_CSUB_GET_COMMAND_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_GET_COMMAND_FUNC(NasdaqActivesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(NYSEActivesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(OTCBBActivesSubscription);
DECL_CSUB_GET_COMMAND_FUNC(OptionActivesSubscription);
/* GetCommand generic method (cast to StreamerSubscription_C*) */
DECL_CSUB_GET_COMMAND_FUNC(StreamingSubscription);
#undef DECL_CSUB_GET_COMMAND_FUNC


#define DECL_CSUB_GET_SYMBOLS_FUNC(name) \
static inline int \
name##_GetSymbols( name##_C *psub, char***buffers, size_t *n) \
{ return SubscriptionBySymbolBase_GetSymbols_ABI( \
    (StreamingSubscription_C*)psub, buffers, n, 0 ); }

/* GetSymbols methods for each SubscriptionBySymbolBase descendant*/
/* GetCommand methods for each sub type */
DECL_CSUB_GET_SYMBOLS_FUNC(QuotesSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(OptionsSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(LevelOneForexSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(ChartEquitySubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(ChartFuturesSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(ChartOptionsSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(TimesaleEquitySubscription);
DECL_CSUB_GET_SYMBOLS_FUNC(TimesaleOptionsSubscription);


#define DECL_CSUB_GET_FIELDS_FUNC(name) \
static inline int \
name##_GetFields(name##_C *psub, name##Field **fields, size_t *n) \
{ return name##_GetFields_ABI(psub, (int**)fields, n, 0); }

#define DECL_CSUB_GET_FIELDS_BASE_FUNC(name, base) \
static inline int \
name##_GetFields(name##_C *psub, base##Field **fields, size_t *n) \
{ return base##Base_GetFields_ABI((StreamingSubscription_C*)psub, \
                                   (int**)fields, n, 0); }

/* GetFields methods in derived */
DECL_CSUB_GET_FIELDS_FUNC(QuotesSubscription);
DECL_CSUB_GET_FIELDS_FUNC(OptionsSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneForexSubscription);
DECL_CSUB_GET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_GET_FIELDS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_GET_FIELDS_FUNC(ChartEquitySubscription);
#undef DECL_CSUB_GET_FIELDS_FUNC

/* GeFields methods in base */
DECL_CSUB_GET_FIELDS_BASE_FUNC(ChartFuturesSubscription, ChartSubscription);
DECL_CSUB_GET_FIELDS_BASE_FUNC(ChartOptionsSubscription, ChartSubscription);
DECL_CSUB_GET_FIELDS_BASE_FUNC(TimesaleFuturesSubscription, TimesaleSubscription);
DECL_CSUB_GET_FIELDS_BASE_FUNC(TimesaleEquitySubscription, TimesaleSubscription);
DECL_CSUB_GET_FIELDS_BASE_FUNC(TimesaleOptionsSubscription, TimesaleSubscription);
#undef DECL_CSUB_GET_FIELDS_BASE_FUNC

#define DECL_CSUB_GET_DURATION(name, base) \
static inline int \
name##_GetDuration(name##_C *psub, DurationType *duration) \
{ return base##Base_GetDuration_ABI( \
    (StreamingSubscription_C*)psub, (int*)duration, 0); }

/* GetDuration in base */
DECL_CSUB_GET_DURATION(NasdaqActivesSubscription, ActivesSubscription)
DECL_CSUB_GET_DURATION(NYSEActivesSubscription, ActivesSubscription)
DECL_CSUB_GET_DURATION(OTCBBActivesSubscription, ActivesSubscription)
DECL_CSUB_GET_DURATION(OptionActivesSubscription, ActivesSubscription)
#undef DECL_CSUB_GET_DURATION

/* GetVenue */
static inline int
OptionActivesSubscription_GetVenue( OptionActivesSubscription_C *psub,
                                    VenueType *venue )
{ return OptionActivesSubscription_GetVenue_ABI(psub, (int*)venue, 0); }


/* SUBSCRIPTION SET METHODS */

#define DECL_CSUB_SET_COMMAND_FUNC(name) \
static inline int \
name##_SetCommand( name##_C *psub, CommandType command ) \
{ return StreamingSubscription_SetCommand_ABI( (StreamingSubscription_C*)psub, \
                                               (int)command, 0); }

/* GetCommand methods for each sub type */
DECL_CSUB_SET_COMMAND_FUNC(QuotesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(OptionsSubscription);
DECL_CSUB_SET_COMMAND_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(LevelOneForexSubscription);
DECL_CSUB_SET_COMMAND_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_SET_COMMAND_FUNC(NewsHeadlineSubscription);
DECL_CSUB_SET_COMMAND_FUNC(ChartEquitySubscription);
DECL_CSUB_SET_COMMAND_FUNC(ChartFuturesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(ChartOptionsSubscription);
DECL_CSUB_SET_COMMAND_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(TimesaleEquitySubscription);
DECL_CSUB_SET_COMMAND_FUNC(TimesaleOptionsSubscription);
DECL_CSUB_SET_COMMAND_FUNC(NasdaqActivesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(NYSEActivesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(OTCBBActivesSubscription);
DECL_CSUB_SET_COMMAND_FUNC(OptionActivesSubscription);
/* GetCommand generic method (cast to StreamerSubscription_C*) */
DECL_CSUB_SET_COMMAND_FUNC(StreamingSubscription);
#undef DECL_CSUB_SET_COMMAND_FUNC


#define DECL_CSUB_SET_SYMBOLS_FUNC(name) \
static inline int \
name##_SetSymbols( name##_C *psub, const char** buffers, size_t n) \
{ return SubscriptionBySymbolBase_SetSymbols_ABI( \
    (StreamingSubscription_C*)psub, buffers, n, 0 ); }

/* SetSymbols methods for each SubscriptionBySymbolBase descendant*/
DECL_CSUB_SET_SYMBOLS_FUNC(QuotesSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(OptionsSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(LevelOneForexSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(ChartEquitySubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(ChartFuturesSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(ChartOptionsSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(TimesaleFuturesSubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(TimesaleEquitySubscription);
DECL_CSUB_SET_SYMBOLS_FUNC(TimesaleOptionsSubscription);


#define DECL_CSUB_SET_FIELDS_FUNC(name) \
static inline int \
name##_SetFields(name##_C *psub, name##Field *fields, size_t n) \
{ return name##_SetFields_ABI(psub, (int*)fields, n, 0); }

#define DECL_CSUB_SET_FIELDS_BASE_FUNC(name, base) \
static inline int \
name##_SetFields(name##_C *psub, base##Field *fields, size_t n) \
{ return base##Base_SetFields_ABI((StreamingSubscription_C*)psub, \
                                   (int*)fields, n, 0); }

/* SetFields methods in derived */
DECL_CSUB_SET_FIELDS_FUNC(QuotesSubscription);
DECL_CSUB_SET_FIELDS_FUNC(OptionsSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneFuturesSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneForexSubscription);
DECL_CSUB_SET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription);
DECL_CSUB_SET_FIELDS_FUNC(NewsHeadlineSubscription);
DECL_CSUB_SET_FIELDS_FUNC(ChartEquitySubscription);
#undef DECL_CSUB_SET_FIELDS_FUNC

/* SetFields methods in base */
DECL_CSUB_SET_FIELDS_BASE_FUNC(ChartFuturesSubscription, ChartSubscription);
DECL_CSUB_SET_FIELDS_BASE_FUNC(ChartOptionsSubscription, ChartSubscription);
DECL_CSUB_SET_FIELDS_BASE_FUNC(TimesaleFuturesSubscription, TimesaleSubscription);
DECL_CSUB_SET_FIELDS_BASE_FUNC(TimesaleEquitySubscription, TimesaleSubscription);
DECL_CSUB_SET_FIELDS_BASE_FUNC(TimesaleOptionsSubscription, TimesaleSubscription);
#undef DECL_CSUB_SET_FIELDS_BASE_FUNC

#define DECL_CSUB_SET_DURATION(name, base) \
static inline int \
name##_SetDuration(name##_C *psub, DurationType duration) \
{ return base##Base_SetDuration_ABI( \
    (StreamingSubscription_C*)psub, (int)duration, 0); }

/* GetDuration in base */
DECL_CSUB_SET_DURATION(NasdaqActivesSubscription, ActivesSubscription)
DECL_CSUB_SET_DURATION(NYSEActivesSubscription, ActivesSubscription)
DECL_CSUB_SET_DURATION(OTCBBActivesSubscription, ActivesSubscription)
DECL_CSUB_SET_DURATION(OptionActivesSubscription, ActivesSubscription)
#undef DECL_CSUB_SET_DURATION

/* GetVenue */
static inline int
OptionActivesSubscription_SetVenue( OptionActivesSubscription_C *psub,
                                    VenueType venue )
{ return OptionActivesSubscription_SetVenue_ABI(psub, (int)venue, 0); }


#else
/* C++ Interface */

namespace tdma{

struct StreamerCredentials{
    std::string user_id;
    std::string token;
    std::string company;
    std::string segment;
    std::string cd_domain;
    std::string user_group;
    std::string access_level;
    bool authorized;
    long long timestamp;
    std::string app_id;
    std::string acl;
};

struct StreamerInfo{
    StreamerCredentials credentials;
    std::string credentials_encoded;
    std::string url;
    std::string primary_acct_id;

    void
    encode_credentials();
};


class StreamingSubscription{
public:
    friend class StreamingSession;
    typedef StreamingSubscription_C CType;

private:
    std::unique_ptr<CType, CProxyDestroyer<CType>> _csub;

protected:
    template<typename CTy=CType>
    CTy*
    csub() const
    { return reinterpret_cast<CTy*>( const_cast<CType*>(_csub.get()) ); }

    template<typename CTy, typename F, typename... Args>
    StreamingSubscription(
            CTy _,
            F create_func,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type *v,
            Args... args )
        :
            _csub( new StreamingSubscription_C{0,0},
                   CProxyDestroyer<CType>(StreamingSubscription_Destroy_ABI) )
        {
            /* IF WE THROW BEFORE HERE WE MAY LEAK IN DERIVED */
            if( create_func )
                call_abi( create_func, args..., csub<CTy>() );
        }

     // lazy constructor - just create the proxy
    template<typename CTy >
    StreamingSubscription(
            CTy _,
            typename std::enable_if<IsValidCProxy<CTy, CType>::value>::type*v
                = nullptr )
        :
            _csub( new StreamingSubscription_C{0,0},
                   CProxyDestroyer<CType>(StreamingSubscription_Destroy_ABI) )
        {
        }

public:
    StreamingSubscription( const StreamingSubscription& sub )
        :
            _csub( new StreamingSubscription_C{0,0},
                    CProxyDestroyer<CType>(StreamingSubscription_Destroy_ABI) )
        {
            call_abi( StreamingSubscription_Copy_ABI, sub.csub(), csub());
        }

    StreamingSubscription&
    operator=( const StreamingSubscription& sub )
    {
        if( *this != sub ){
            _csub.reset( new CType{0,0} );
            call_abi( StreamingSubscription_Copy_ABI, sub._csub.get(), _csub.get() );
        }
        return *this;
    }

    bool
    operator==( const StreamingSubscription& sub ) const
    {
        int b;
        call_abi( StreamingSubscription_IsSame_ABI, sub._csub.get(), _csub.get(), &b );
        return b;
    }

    bool
    operator!=( const StreamingSubscription& sub ) const
    { return !(*this == sub); }

    StreamerServiceType
    get_service() const
    {
        int ss;
        call_abi( StreamingSubscription_GetService_ABI, _csub.get(), &ss );
        return static_cast<StreamerServiceType>(ss);
    }

    CommandType
    get_command() const
    {
        int c;
        call_abi( StreamingSubscription_GetCommand_ABI, _csub.get(), &c );
        return static_cast<CommandType>(c);
    }

    void
    set_command( CommandType command )
    {
        call_abi( StreamingSubscription_SetCommand_ABI, _csub.get(),
                  static_cast<int>(command) );
    }
};


class SubscriptionBySymbolBase
        : public StreamingSubscription {
protected:
    template<typename FieldTy, typename CTy>
    std::set<FieldTy>
    fields_from_abi( int(*abicall)(CTy*, int**, size_t*,int) ) const
    { return set_of_fields_from_abi<FieldTy>(abicall, csub<CTy>()); }

    template<typename FieldTy, typename CTy>
    void
    fields_to_abi( int(*abicall)(CTy*, int*, size_t,int),
                   const std::set<FieldTy>& fields )
    { set_of_fields_to_abi(abicall, csub<CTy>(), fields); }

    template<typename CTy, typename F, typename FTy>
    SubscriptionBySymbolBase( CTy _,
                              F create_func,
                              const std::set<std::string>& symbols,
                              const std::set<FTy>& fields,
                              CommandType command )
        :
            StreamingSubscription(_)
        {
            const char** s = nullptr;
            int *i = nullptr;
            try{
                s = set_to_new_cstrs(symbols);
                i = set_to_new_int_array(fields);
                call_abi( create_func, s, symbols.size(), i, fields.size(),
                          static_cast<int>(command), csub<CTy>() );
                if( s ) delete[] s;
                if( i ) delete[] i;
            }catch(...){
                if( s ) delete[] s;
                if( i ) delete[] i;
                throw;
            }
        }


public:
    std::set<std::string>
    get_symbols() const
    {
        return set_of_strs_from_abi( SubscriptionBySymbolBase_GetSymbols_ABI,
                                     csub() );
    }

    void
    set_symbols( const std::set<std::string>& symbols )
    {
        set_of_strs_to_abi( SubscriptionBySymbolBase_SetSymbols_ABI, csub(),
                            symbols );
    }
};


class QuotesSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = QuotesSubscriptionField;
    typedef QuotesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::QUOTE;

    QuotesSubscription( const std::set<std::string>& symbols,
                        const std::set<FieldType>& fields,
                        CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( QuotesSubscription_C{},
                                      QuotesSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

    std::set<FieldType>
    get_fields() const
    { return fields_from_abi<FieldType>(QuotesSubscription_GetFields_ABI); }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(QuotesSubscription_SetFields_ABI, fields); }
};


class OptionsSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = OptionsSubscriptionField;
    typedef OptionsSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::OPTION;

    OptionsSubscription( const std::set<std::string>& symbols,
                         const std::set<FieldType>& fields,
                         CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( OptionsSubscription_C{},
                                      OptionsSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }


    std::set<FieldType>
    get_fields() const
    { return fields_from_abi<FieldType>(OptionsSubscription_GetFields_ABI); }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(OptionsSubscription_SetFields_ABI, fields); }
};


class LevelOneFuturesSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = LevelOneFuturesSubscriptionField;
    typedef LevelOneFuturesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::LEVELONE_FUTURES;

    LevelOneFuturesSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields,
                                 CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( LevelOneFuturesSubscription_C{},
                                      LevelOneFuturesSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            LevelOneFuturesSubscription_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(LevelOneFuturesSubscription_SetFields_ABI, fields); }
};


class LevelOneForexSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = LevelOneForexSubscriptionField;
    typedef LevelOneForexSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::LEVELONE_FOREX;

    LevelOneForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields,
                               CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( LevelOneForexSubscription_C{},
                                      LevelOneForexSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            LevelOneForexSubscription_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(LevelOneForexSubscription_SetFields_ABI, fields); }
};


class LevelOneFuturesOptionsSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = LevelOneFuturesOptionsSubscriptionField;
    typedef LevelOneFuturesOptionsSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::LEVELONE_FUTURES_OPTIONS;

    LevelOneFuturesOptionsSubscription( const std::set<std::string>& symbols,
                                        const std::set<FieldType>& fields,
                                        CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase(
                LevelOneFuturesOptionsSubscription_C{},
                LevelOneFuturesOptionsSubscription_Create_ABI,
                symbols,
                fields,
                command
                )
        {
        }

    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            LevelOneFuturesOptionsSubscription_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(LevelOneFuturesOptionsSubscription_SetFields_ABI, fields); }
};


class NewsHeadlineSubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = NewsHeadlineSubscriptionField;
    typedef NewsHeadlineSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::NEWS_HEADLINE;

    NewsHeadlineSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields,
                              CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( NewsHeadlineSubscription_C{},
                                      NewsHeadlineSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            NewsHeadlineSubscription_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(NewsHeadlineSubscription_SetFields_ABI, fields); }
};


// TODO implement ADD command
class ChartEquitySubscription
        : public SubscriptionBySymbolBase {
public:
    using FieldType = ChartEquitySubscriptionField;
    typedef ChartEquitySubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::CHART_EQUITY;

    ChartEquitySubscription( const std::set<std::string>& symbols,
                             const std::set<FieldType>& fields,
                             CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBase( ChartEquitySubscription_C{},
                                      ChartEquitySubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            ChartEquitySubscription_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(ChartEquitySubscription_SetFields_ABI, fields); }
};


class ChartSubscriptionBase
        : public SubscriptionBySymbolBase {
public:
    using FieldType = ChartSubscriptionField;

protected:
    template<typename CTy, typename F>
    ChartSubscriptionBase( CTy _,
                           F func,
                           const std::set<std::string>& symbols,
                           const std::set<FieldType>& fields,
                           CommandType command  )
        :
            SubscriptionBySymbolBase( _, func, symbols, fields, command )
        {
        }

public:
    std::set<FieldType>
    get_fields() const
    { return fields_from_abi<FieldType>(ChartSubscriptionBase_GetFields_ABI); }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(ChartSubscriptionBase_SetFields_ABI, fields); }
};


/*
 * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
 *
class ChartForexSubscription
        : public ChartSubscriptionBase {   
public:
    typedef ChartFuturesSubscription_C CType;


    ChartForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : ChartSubscriptionBase( StreamerServiceType::CHART_FOREX,
                                 symbols, fields )
    {}
};
*/

class ChartFuturesSubscription
        : public ChartSubscriptionBase {
public:
    typedef ChartFuturesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::CHART_FUTURES;

    ChartFuturesSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields,
                              CommandType command = CommandType::SUBS )
        :
            ChartSubscriptionBase( ChartFuturesSubscription_C{},
                                   ChartFuturesSubscription_Create_ABI,
                                   symbols,
                                   fields,
                                   command )
        {
        }

};


class ChartOptionsSubscription
        : public ChartSubscriptionBase {
public:
    typedef ChartOptionsSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::CHART_OPTIONS;

    ChartOptionsSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields,
                              CommandType command = CommandType::SUBS )
        :
            ChartSubscriptionBase( ChartOptionsSubscription_C{},
                                   ChartOptionsSubscription_Create_ABI,
                                   symbols,
                                   fields,
                                   command )
        {
        }

};


class TimesaleSubscriptionBase
        : public SubscriptionBySymbolBase {
public:
    using FieldType = TimesaleSubscriptionField;

protected:
    template<typename CTy, typename F>
    TimesaleSubscriptionBase( CTy _,
                              F func,
                              const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields,
                              CommandType command )
        :
            SubscriptionBySymbolBase( _, func, symbols, fields, command )
        {
        }

public:
    std::set<FieldType>
    get_fields() const
    {
        return fields_from_abi<FieldType>(
            TimesaleSubscriptionBase_GetFields_ABI
            );
    }

    void
    set_fields(const std::set<FieldType>& fields)
    { fields_to_abi(TimesaleSubscriptionBase_SetFields_ABI, fields); }
};


class TimesaleEquitySubscription
        : public TimesaleSubscriptionBase {
public:
    typedef TimesaleEquitySubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::TIMESALE_EQUITY;

    TimesaleEquitySubscription( const std::set<std::string>& symbols,
                                const std::set<FieldType>& fields,
                                CommandType command = CommandType::SUBS )
        :
            TimesaleSubscriptionBase( TimesaleEquitySubscription_C{},
                                      TimesaleEquitySubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

};

/*
 *  * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
class TimesaleForexSubscription
        : public TimesaleSubscriptionBase {
public:
    typedef TimesaleForexSubscription_C CType;

    TimesaleForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : TimesaleSubscriptionBase( StreamerServiceType::TIMESALE_FOREX,
                                    symbols, fields)
    {}
};
*/

class TimesaleFuturesSubscription
        : public TimesaleSubscriptionBase {
public:
    typedef TimesaleFuturesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::TIMESALE_FUTURES;

    TimesaleFuturesSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields,
                                 CommandType command = CommandType::SUBS )
        :
            TimesaleSubscriptionBase( TimesaleFuturesSubscription_C{},
                                      TimesaleFuturesSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

};

class TimesaleOptionsSubscription
        : public TimesaleSubscriptionBase {
public:
    typedef TimesaleOptionsSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::TIMESALE_OPTIONS;

    TimesaleOptionsSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields,
                                 CommandType command = CommandType::SUBS )
        :
            TimesaleSubscriptionBase( TimesaleOptionsSubscription_C{},
                                      TimesaleOptionsSubscription_Create_ABI,
                                      symbols,
                                      fields,
                                      command )
        {
        }

};


class ActivesSubscriptionBase
        : public StreamingSubscription {
protected:
    template<typename CTy, typename F, typename... Args>
    ActivesSubscriptionBase( CTy _, F func, Args... args )
        :
            StreamingSubscription(_, func, nullptr, args...)
        {
        }

public:
    DurationType
    get_duration() const
    {
        int d;
        call_abi( ActivesSubscriptionBase_GetDuration_ABI, csub(), &d );
        return static_cast<DurationType>(d);
    }

    void
    set_duration( DurationType duration )
    {
        call_abi( ActivesSubscriptionBase_SetDuration_ABI, csub(),
                  static_cast<int>(duration) );
    }
};


class NasdaqActivesSubscription
        : public ActivesSubscriptionBase {
public:
    typedef NasdaqActivesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::ACTIVES_NASDAQ;

    NasdaqActivesSubscription( DurationType duration,
                               CommandType command = CommandType::SUBS )
        :
            ActivesSubscriptionBase( NasdaqActivesSubscription_C{},
                                     NasdaqActivesSubscription_Create_ABI,
                                     static_cast<int>(duration),
                                     static_cast<int>(command) )
        {
        }

};

class NYSEActivesSubscription
        : public ActivesSubscriptionBase {
public:
    typedef NYSEActivesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::ACTIVES_NYSE;

    NYSEActivesSubscription( DurationType duration,
                             CommandType command = CommandType::SUBS )
        :
            ActivesSubscriptionBase( NYSEActivesSubscription_C{},
                                     NYSEActivesSubscription_Create_ABI,
                                     static_cast<int>(duration),
                                     static_cast<int>(command) )
        {
        }

};

class OTCBBActivesSubscription
        : public ActivesSubscriptionBase {
public:
    typedef OTCBBActivesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::ACTIVES_OTCBB;

    OTCBBActivesSubscription( DurationType duration,
                              CommandType command = CommandType::SUBS )
        :
            ActivesSubscriptionBase( OTCBBActivesSubscription_C{},
                                     OTCBBActivesSubscription_Create_ABI,
                                     static_cast<int>(duration),
                                     static_cast<int>(command) )
        {
        }

};

class OptionActivesSubscription
        : public ActivesSubscriptionBase {
public:
    typedef OptionActivesSubscription_C CType;

    static const StreamerServiceType STREAMER_SERVICE_TYPE =
        StreamerServiceType::ACTIVES_OPTIONS;

    OptionActivesSubscription( VenueType venue,
                               DurationType duration,
                               CommandType command = CommandType::SUBS )
        :
            ActivesSubscriptionBase( OptionActivesSubscription_C{},
                                     OptionActivesSubscription_Create_ABI,
                                     static_cast<int>(venue),
                                     static_cast<int>(duration),
                                     static_cast<int>(command))
        {
        }


    VenueType
    get_venue() const
    {
        int v;
        call_abi( OptionActivesSubscription_GetVenue_ABI, csub<CType>(), &v );
        return static_cast<VenueType>(v);
    }

    void
    set_venue( VenueType venue )
    {
        call_abi( OptionActivesSubscription_SetVenue_ABI, csub<CType>(),
                  static_cast<int>(venue) );
    }

};

} /* tdma */

#endif /* __cplusplus */

#define STREAMING_VERSION "1.0"
#define STREAMING_MIN_TIMEOUT 1000
#define STREAMING_MIN_LISTENING_TIMEOUT 10000
#define STREAMING_LOGOUT_TIMEOUT 1000
#define STREAMING_DEF_CONNECT_TIMEOUT 3000
#define STREAMING_DEF_LISTENING_TIMEOUT 30000
#define STREAMING_DEF_SUBSCRIBE_TIMEOUT 1500
#define STREAMING_MAX_SUBSCRIPTIONS 50


typedef void(*streaming_cb_ty)(int, int, unsigned long long, const char*);

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_Create_ABI( struct Credentials *pcreds,
                             streaming_cb_ty callback,
                             unsigned long connect_timeout,
                             unsigned long listening_timeout,
                             unsigned long subscribe_timeout,
                             StreamingSession_C *psession,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_Destroy_ABI( StreamingSession_C *psession,
                              int allow_exceptions );



EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_Start_ABI( StreamingSession_C *psession,
                            StreamingSubscription_C **subs,
                            size_t nsubs,
                            int *results_buffer,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_AddSubscriptions_ABI( StreamingSession_C *psession,
                                       StreamingSubscription_C **subs,
                                       size_t nsubs,
                                       int *results_buffer,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_Stop_ABI( StreamingSession_C *psession,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_IsActive_ABI( StreamingSession_C *psession,
                               int *is_active,
                               int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_SetQOS_ABI( StreamingSession_C *psession,
                             int qos,
                             int *result,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StreamingSession_GetQOS_ABI( StreamingSession_C *psession,
                             int *qos,
                             int allow_exceptions );

#ifndef __cplusplus

/* C Interface */

static inline int
StreamingSession_Create( struct Credentials *pcreds,
                         streaming_cb_ty callback,
                         StreamingSession_C *psession )
{
    return StreamingSession_Create_ABI(pcreds, callback,
                                       STREAMING_DEF_CONNECT_TIMEOUT,
                                       STREAMING_DEF_LISTENING_TIMEOUT,
                                       STREAMING_DEF_SUBSCRIBE_TIMEOUT,
                                       psession, 0);
}

static inline int
StreamingSession_CreateEx( struct Credentials *pcreds,
                           streaming_cb_ty callback,
                           unsigned long connect_timeout,
                           unsigned long listening_timeout,
                           unsigned long subscribe_timeout,
                           StreamingSession_C *psession )
{
    return StreamingSession_Create_ABI(pcreds, callback,
                                       connect_timeout, listening_timeout,
                                       subscribe_timeout, psession, 0);
}

static inline int
StreamingSession_Destroy( StreamingSession_C *psession )
{ return StreamingSession_Destroy_ABI(psession, 0); }


static inline int
StreamingSession_Start( StreamingSession_C *psession,
                        StreamingSubscription_C **subs,
                        size_t nsubs,
                        int *results_buffer)
{ return StreamingSession_Start_ABI(psession, subs, nsubs, results_buffer, 0); }


static inline int
StreamingSession_AddSubscriptions( StreamingSession_C *psession,
                                   StreamingSubscription_C **subs,
                                   size_t nsubs,
                                   int *results_buffer )
{ return StreamingSession_AddSubscriptions_ABI(psession, subs, nsubs,
                                               results_buffer, 0); }

static inline int
StreamingSession_Stop( StreamingSession_C *psession )
{ return StreamingSession_Stop_ABI(psession, 0); }

static inline int
StreamingSession_IsActive( StreamingSession_C *psession, int *is_active )
{ return StreamingSession_IsActive_ABI( psession, is_active, 0 ); }

static inline int
StreamingSession_SetQOS(StreamingSession_C *psession, QOSType qos, int *result)
{ return StreamingSession_SetQOS_ABI(psession, (int)qos, result, 0); }

static inline int
StreamingSession_GetQOS( StreamingSession_C *psession, QOSType *qos)
{ return StreamingSession_GetQOS_ABI(psession, (int*)qos, 0); }

#else

/* C++ Interface */

namespace tdma{

class DLL_SPEC_ StreamingSession{
public:
    static const std::string VERSION; // = "1.0"
    static const std::chrono::milliseconds MIN_TIMEOUT; // 1000;
    static const std::chrono::milliseconds MIN_LISTENING_TIMEOUT; // 10000;
    static const std::chrono::milliseconds LOGOUT_TIMEOUT; // 1000
    static const std::chrono::milliseconds DEF_CONNECT_TIMEOUT; // 3000
    static const std::chrono::milliseconds DEF_LISTENING_TIMEOUT; // 30000
    static const std::chrono::milliseconds DEF_SUBSCRIBE_TIMEOUT; // 1500
    static const int MAX_SUBSCRIPTIONS = STREAMING_MAX_SUBSCRIPTIONS; // 50

    typedef StreamingSession_C CType;

private:
    std::unique_ptr<CType, CProxyDestroyer<CType>> _obj;

    StreamingSession()
        :
            _obj( new CType{0,0,0},
                  CProxyDestroyer<CType>(StreamingSession_Destroy_ABI) )
        {}

    std::deque<bool>
    _call_abi_with_subs(
        int(*abicall)(CType*, StreamingSubscription_C**, size_t, int*, int),
        const std::vector<StreamingSubscription>& subscriptions
        )
    {
        std::deque<bool> cpp_results;
        StreamingSubscription_C **buffer = nullptr;
        int *results = nullptr;
        size_t sz = subscriptions.size();
        try{
            buffer = new StreamingSubscription_C*[sz];
            std::transform( subscriptions.begin(), subscriptions.end(), buffer,
                            [](const StreamingSubscription& s){
                                return s.csub();
                            });

            results = new int[sz];
            call_abi( abicall, _obj.get(), buffer, sz, results );
            while( sz-- ){
                cpp_results.push_front(static_cast<bool>(results[sz]));
            }
            delete[] buffer;
            delete[] results;
        }catch(...){
            if( buffer ) delete[] buffer;
            if( results ) delete[] results;
            throw;
        }
        return cpp_results;
    }

public:
    static std::shared_ptr<StreamingSession>
    Create( Credentials& creds,
             streaming_cb_ty callback,
             std::chrono::milliseconds connect_timeout=DEF_CONNECT_TIMEOUT,
             std::chrono::milliseconds listening_timeout=DEF_LISTENING_TIMEOUT,
             std::chrono::milliseconds subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT
             )
    {
        StreamingSession *ss = nullptr;
        try{
            ss = new StreamingSession;
            call_abi( StreamingSession_Create_ABI, &creds, callback,
                      connect_timeout.count(), listening_timeout.count(),
                      subscribe_timeout.count(), ss->_obj.get() );
        }catch(...){
            if( ss ) delete ss;
            throw;
        }
        return std::shared_ptr<StreamingSession>(ss);
    }

    StreamingSession( const StreamingSession& ) = delete;

    StreamingSession&
    operator=( const StreamingSession& ) = delete;

    std::deque<bool> // success/fails in the order passed
    start( const std::vector<StreamingSubscription>& subscriptions )
    { return _call_abi_with_subs(StreamingSession_Start_ABI,subscriptions); }

    bool 
    start( const StreamingSubscription& subscription )
    { return start(std::vector<StreamingSubscription>{subscription})[0]; }

    void
    stop()
    { call_abi( StreamingSession_Stop_ABI, _obj.get() ); }

    bool
    is_active() const
    {
        int active;
        call_abi( StreamingSession_IsActive_ABI, _obj.get(), &active );
        return static_cast<bool>(active);
    }

    std::deque<bool> // success/fails in the order passed
    add_subscriptions(const std::vector<StreamingSubscription>& subscriptions)
    { return _call_abi_with_subs( StreamingSession_AddSubscriptions_ABI,
                                  subscriptions ); }

    bool 
    add_subscription(const StreamingSubscription& subscription)
    {
        return add_subscriptions(
            std::vector<StreamingSubscription>{subscription})[0];
    }


    QOSType
    get_qos() const
    {
        int q;
        call_abi( StreamingSession_GetQOS_ABI, _obj.get(), &q );
        return static_cast<QOSType>(q);
    }

    bool
    set_qos(const QOSType& qos)
    {
        int result;
        call_abi( StreamingSession_SetQOS_ABI, _obj.get(),
                  static_cast<int>(qos), &result );
        return static_cast<bool>(result);
    }
};

} /* tdma */


#endif /* __cplusplus */

#endif // TDMA_API_STREAMING_H
