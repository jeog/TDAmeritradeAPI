### Execute Interface
- - -
- [Overview](#overview)
- [WARNING](#warning-please-read)
- [Using OrderTicket Objects](#using-orderticket-objects)
- [Raw Orders](#raw-orders)
- [Managed Orders](#managed-orders)
   - [SimpleOrderBuilder](#simpleorderbuilder)
   - [SpreadOrderBuilder](#spreadorderbuilder)
   - [ConditionalOrderBuilder](#conditionalorderbuilder)
- [Execute](#execute)
   - [Send Order](#send-order)
   - [Cancel Order](#cancel-order)
   - [Replace Order](#replace-order)
- [Order & Position Information](#order--position-information)
- - -

### Overview

```
[C, C++]
#include "tdma_api_execute.h"

[C++]
using namespace tdma;

[Python]
from tdma_api import execute
```

Order execution and management interface. 

This interface is waiting on a mechanism from Ameritrade to test execution without having to send live dummy orders. There has only been limited testing of execution.

Feel free to comment and make suggestsions via [issues](https://github.com/jeog/TDAmeritradeAPI/issues/1) or email: jeog.dev@gmail.com

[TDAmeritrade docs.](https://developer.tdameritrade.com/account-access/apis/post/accounts/{accountId}/orders-0)

## WARNING (PLEASE READ)

- This interface allows you to build and execute **live** orders.
- This interface has undergone very **limited testing.**
- It's recommended you wait for Ameritrade to release a better testing environment before using.
- You should **assume bugs** in Order Builders, Tickets, Legs, etc that may affect order quantity, price, type etc. 
- **REVIEW ALL THE RELEVANT CODE** and **DOUBLE CHECK THE RAW JSON BEFORE SENDING.**

**BY USING THIS CODE/LIBRARY TO BUILD AND/OR SEND LIVE ORDERS YOU AGREE TO TAKE FULL RESPONSIBILITY FOR ANY LOSSES INCURRED - INCLUDING, BUT NOT LIMITED TO, LOSSES RESULTING FROM ERRORS IN THE CODE/LIBRARY AND/OR THE GROSS NEGLIGENCE OF THE AUTHOR(S).**


### Using OrderTicket Objects

1. Build an ```OrderTicket``` directly or use one of the builders. (see below)
2. Check that the ```OrderTicket``` is accurately representing the JSON you intend to POST.
    - triple check quantities, prices, order types etc. **of the raw json** 
    - be sure you understand how the Builders handle price and OrderType fields for spreads (+/- prices vis-a-vis NET_CREDIT/NET_DEBIT order types)
3. Use ```Execute_SendOrder``` or create your own mechanism to send the order.

### Raw Orders

The full order schema will be represented by ```OrderTicket``` and ```OrderLeg``` objects the user has complete control over. The user is responsible for building the order from scratch i.e setting the order type, adding the order legs etc.

Like the 'Get' and 'Streaming' interfaces a proxy object is used that calls through the ABI.

```
[C++]

OrderLeg leg1(OrderAssetType::OPTION, "SPY_011720C300", 
              OrderInstruction::BUY_TO_OPEN, 10);

OrderTicket order1; 

order1.add_leg(leg1).set_type(OrderType::LIMIT).set_price(9.99);

// the raw json (object)
cout<< order1.as_json() << endl;
```
```
[C]

OrderLeg_C leg1 = {0,0};
OrderTicket_C order1 = {0,0};

int err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720C300",   
                          OrderInstruction_BUY_TO_OPEN, 10, &leg1);
if( err ){
    //
}

err = OrderTicket_Create(&order1);
if( err ){
    //
}

err = OrderTicket_AddLegs(&order1, &leg1, 1);
if( err ){
    //
}

err = OrderTicket_SetType(&order1, OrderType_LIMIT);
if( err ){
    //
}

err = OrderTicket_SetPrice(&order1, 9.99);
if( err ){
    // 
}

char *buf;
size_t n;
err = OrderTicket_AsJsonString(&order1, &buf, &n);
if( err ){
    //
}

// the raw json (string)
printf( "%s\n", buf);

FreeBuffer(buf);
```
```
[Python]
leg1 = execute.OrderLeg(execute.ORDER_ASSET_TYPE_OPTION, "SPY_011720C300",
                        execute.ORDER_INSTRUCTION_BUY, 10)
order1 = execute.OrderTicket()
order1.add_legs(leg1).set_type(execute.ORDER_TYPE_LIMIT).set_price(9.99)

// json as builtin object
print( str(order1.as_json()) )
```

In C you need to manually destroy the objects when done.
```
[C]

err = OrderTicket_Destroy(&order1);
if( err ){
    //
}

err = OrderLeg_Destroy(&leg1);
if( err ) {
    //
}
```

If a specific order type is allowed by TDMA it *should* be possible for a motivated user to build it this way.


### Managed Orders

Most users will probably only need certain basic orders, most of the time. To help (safely) build, use the nested static builders/factories. Once you have the order ticket you can add to it using the 'set' methods described above.

#### SimpleOrderBuilder 

Equity and Option OrderTickets can be constructed with the static builders of the relevant nested classes inside the SimpleOrderBuilder class(C++, Python) or the similarly name static methods(C) for:

- Equities
    - Buy / Sell / Short / Cover
    - Market 
    - Limit
    - Stop
    - Stop-Limit

- Options
    - Buy / Sell
    - Open / Close
    - Market 
    - Limit

**Example - BUY LONG (TO OPEN) 100 SPY @ 285.05 or better**

```
[C++]

OrderTicket order = SimpleOrderBuilder::Equity::Build("SPY", 100, true, true, 285.05);
```
```
[C]

OrderTicket order = {0,0};

int err = BuildOrder_Equity_Limit("SPY", 100, 1, 1, 285.05, &order);
if( err ){
    // 
}

OrderTicket_Destroy(&order);
```
```
[Python]

order = execute.SimpleOrderBuilder.Equity.Build("SPY", 100, True, True, 285.05)
```

#### SpreadOrderBuilder

Option spread OrderTickets can be constructed with the static builders of the relevant nested classes inside the SpreadOrderBuilder class(C++, Python) or the similarly name static methods(C) for:

- Vertical
- Vertical Roll
- Unbalanced Vertical Roll
- Butterfly
- Unbalanced Butterfly
- BackRatio
- Calendar
- Diagonal
- Straddle
- Strangle
- Synthetic Collar
- Collar With Stock
- Condor
- Unbalanced Condor
- Iron Condor
- Unbalanced Iron Condor
- Double Diagonal

IMPORTANT - negative(-) prices for NET_CREDIT, positive prices for NET_DEBIT


**Example - BUY (TO OPEN) 3 Jan-20 300/350 VERTICAL SPY CALL spreads @ a 10.10 DEBIT or better**

```
[C++]

// the builder/factory
using VB = SpreadOrderBuilder::Vertical;

// add the exact options symbols
Order order = VB::Build("SPY_011720C300", "SPY_011720C350", 3, true, 10.10);

// OR have the builder construct the options(s) 
order = VB::Build("SPY", 1, 17, 2020, true, 300, 350, 3, true, 10.10);
```
```
[C]

Order_Ticket_C order = {0,0};

int err = BuildOrder_Spread_Vertical_Limit("SPY_011720C300", "SPY_011720C350",
                                           3, 1, 10.10, &order);
if( err ){
    //
}

OrderTicket_Destroy(&order);

// OR
err = BuildOrder_Spread_Vertical_LimitEx("SPY", 1, 17, 2020, 1, 300, 350, 3, 1, 
                                         10.10, &order);
if( err ){
    //
}

OrderTicket_Desgtroy(&order);
```
```
[Python]

VB = execute.SpreadOrderBuilder.Vertical

order = VB.Build1("SPY_011720C300", "SPY_011720C350", 3, True, 10.10)

# OR
order = VB.Build2("SPY", 1, 17, 2020, 1, 300, 350, 3, 1, 10.10)
```

**Example - SELL (TO CLOSE) 10 1-3-2 UNBALANCED Jan-20 300/325/350 SPY CALL BUTTERFLIES @ 1.05 CREDIT or better**
```
[C++]

using BB = SpreadOrderBuilder::Butterfly::Unbalanced;

Order order = BB::Build("SPY_011720C300", "SPY_011720C325", "SPY_011720C350", 
                         10, 20, false, false, -1.05);

// OR 
order = BB::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 10, 20, 
                  false, false, -1.05);
```
```
[C]

Order_Ticket_C order = {0,0};
int err;

err = BuildOrder_Spread_ButterflyUnbalanced_Limit(
        "SPY_011720C300", "SPY_011720C325", "SPY_011720C350", 10, 20, 0, 0, -1.05, &order
        );
if( err ){
    //
}

OrderTicket_Destroy(&order);

// OR
err = BuildOrder_Spread_ButterflyUnbalanced_LimitEx(
        "SPY", 1, 17, 2020, 1, 300, 325, 350, 10, 20, 0, 0, -1.05, &order
        );
if( err ){
    //
}

OrderTicket_Desgtroy(&order);
```
```
[Python]

BB = execute.SpreadOrderBuilder.Butterfly.Unbalanced

order = BB.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350", 
                  10, 20, False, False, -1.05)

# OR
order = BB.Build2("SPY", 1, 17, 2020, True, 300, 325, 350, 10, 20, 
                  False, False, -1.05)
```

#### ConditionalOrderBuilder

One-Cancels-Other(OCO) and One-Triggers-Other(OTO) OrderTickets can be constructed with the static builders of the relevant nested classes inside the ConditionalOrderBuilder class(C++, Python) or the similarly name static methods(C).

**Example - OCO Exit Bracket: SELL (TO CLOSE) 100 SPY @ 299.95 or better -OR- SELL (TO CLOSE) 100 SPY on trade below 289.95 @ 289.45 or better**
```
[C++]

using SB = SimpleOrderBuilder::Equity;

OrderTicket order1 = SB::Build("SPY", 100, false, false, 299.95);
OrderTicket order2 = SB::Stop::Build("SPY", 100, false, false, 289.95, 289.45);
OrderTicket order3 = ConditionalOrderBuilder::OCO(order1, order2);
```
```
[C]

OrderTicket order1 = {0,0};
OrderTicket order2 = {0,0};
OrderTicket order3 = {0,0};

int err = BuildOrder_Equity_Limit("SPY", 100, 0, 0, 299.95, &order1);
if( err ){
    // 
}

err = BuildOrder_Equity_StopLimit("SPY", 100, 0, 0, 289.95, 289.45, &order2);
if( err ){
    //
}

err = BuildOrder_OneCancelsOther(&order1, &order2, &order3);
if( err ){
    //
}

OrderTicket_Destroy(&order1);
OrderTicket_Destroy(&order2);
OrderTicket_Destroy(&order3);
```
```
[Python]

SB = execute.SimpleOrderBuilder.Equity

order1 = SB.Build("SPY", 100, false, false, 299.95)
order2 = SB.Stop.Build("SPY", 100, false, false, 289.95, 289.45)
order3 = execute.ConditionalOrderBuilder.OCO(order1, order2);
```

### Execute

***CAUTION* - The following functionality has undergone very limited testing (basic equity limit orders in C++ only)**

#### Send Order

```Execute_SendOrder``` attempts to take an ```OrderTicket```, convert it to JSON and make a HTTPS/POST connection in order to place an order for account ```account_id```. If the order is successfully recieved an order ID string will be returned; if not an exception will be thrown(C++) or an error code returned(C).
```
[C++]
inline std::string
Execute_SendOrder( Credentials& creds,
                   const std::string& account_id,
                   const OrderTicket& order );

[C]
static inline int
Execute_SendOrder( struct Credentials *creds,
                   const char* account_id,
                   OrderTicket_C *porder,
                   char** buf,
                   size_t *n );
```

#### Cancel Order

```Execute_CancelOrder``` attempts to take an ```order_id``` string (of an active order) for account ```account_id``` and make a HTTPS/DELETE connection to cancel that order. If the order is active and successfully canceled ```true``` will be returned(C++) or ```*success``` will be set to non-zero(C); if not an exception will be thrown(C++) or an error code returned(C). 

(At some point in the future we may catch certain exceptions and return a fail state.)
```
[C++]
inline bool
Execute_CancelOrder( Credentials& creds,
                     const std::string& account_id,
                     const std::string& order_id );

[C]
static inline int
Execute_CancelOrder( struct Credentials *creds,
                     const char* account_id,
                     const char* order_id,
                     int *success );
```

#### Replace Order

// TODO

### Order & Position Information

To get order and position information for an account review the following 'Getter' objects:
   - [OrderGetter](README_GET.md#ordergetter) - order by id
   - [OrdersGetter](README_GET.md#ordersgetter) - orders by status/range 
   - [AccountInfoGetter](README_GET.md#accountinfogetter) - orders and/or positions 


