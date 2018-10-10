### Execute Interface
- - -
- [Overview](#overview)
- [Using OrderTicket Objects](#using-orderticket-objects)
   - [Raw Orders](#raw-orders)
   - [Managed Orders](#managed-orders)
      - [Equity](#equity)
      - [Option Spreads](#option-spreads)
- [Design Ideas](#design-ideas)
   - [Higher Level Features](#higher-level-features)
   - [Order Hub](#order-hub)
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

This interface is waiting on a mechanism from Ameritrade to test execution without having to send live dummy orders. There has been no actual testing of execution but you can build ```OrderTicket``` objects whose underlying JSON can be submitted for execution. Currently we DO NOT provide a means to send/manage orders.

Below we provide some basics of the proposed interface. Feel free to comment via [issues](https://github.com/jeog/TDAmeritradeAPI/issues/1) or email: jeog.dev@gmail.com


### Using OrderTicket Objects

**IF YOU CHOOSE TO SEND THESE ORDERS BE VERY VERY CAREFUL - MAKE SURE YOU KNOW EXACTLY WHAT IS BEING SENT!** 

1. Build an ```OrderTicket``` (see tdma_api_execute.h, tdma_api.execute.py, or examples below)
2. Check that the ```OrderTicket``` is accurately representing the JSON you intend to POST.
    - because of the number of builders and accessors expect bugs 
    - triple check quantities, prices, order types etc. **of the raw json** 
    - be sure you understand how the Builders handle price and OrderType fields for spreads (+/- prices vis-a-vis NET_CREDIT/NET_DEBIT order types)
3. Review the [offical docs](https://developer.tdameritrade.com/account-access/apis/post/accounts/{accountId}/orders-0).
4. Build your own mechanism to send the order - take a look at ```HttpsPostConnection``` in curl_connect.h/cpp

#### Raw Orders

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


#### Managed Orders

Most users will probably only need certain basic orders, most of the time. To help (safely) build, use the nested static builders/factories:

##### Equity 
```
[C++]

/* long 100 SPY @ 285.05 or better */

OrderTicket order1 = SimpleOrderBuilder::Equity::Build("SPY", 100, true, true, 285.05);
```

```
[C]

OrderTicket order1 = {0,0}

int err = BuildOrder_Equity_Limit("SPY", 100, 1, 1, 285.05, &order1);
if( err ){
    // 
}
```

```
[Python]

order1 = execute.SimpleOrderBuilder.Equity.Build("SPY", 100, True, True, 285.05)
```

Once the user has the order they can add to it as necessary e.g:
```
[C++]

// defaults to OrderSession::DAY so...
order1.set_sesion(OrderSession::GOOD_TILL_CANCEL).set_cancel_time("2019-01-01");
```
```
[C]

err = OrderTicket_SetSession(&order1, OrderSession_GOOD_TILL_CANCEL);
if( err ){
    //
}

err = OrderTicket_SetCancelTime(&order1, "2019-01-01");
if( err ){
    //
}
```
```
[Python]

order1.set_session(execute.ORDER_SESSION_GOOD_TILL_CANCEL) \
      .set_cancel_time("2019-01-01") 

```

In C you need to manually destroy the object when done.
```
[C]

err = OrderTicket_Destroy(&order1);
if( err ){
    //
}
```

##### Option Spreads
Option Spread builders are currently only available for C and C++.
```
[C++]

/* 
 * buy/open 3 Jan-20 300/350 vertical SPY call spreads @ a 10.10 debit or better 
 *
 * NOTE - negative(-) prices represent CREDITS, positive prices DEBITS 
 *        *** FOR SPREADS ONLY ***
 */

// the builder/factory
using VB = SpreadOrderBuilder::Vertical;

// add the exact options symbols
Order order2 = VB::Build("SPY_011720C300", "SPY_011720C350", 3, true, 10.10);

// OR have the builder construct the options(s) 
// (safer, but still assumes valid date, strikes etc.)
order2 = VB::Build("SPY", 1, 17, 2020, true, 300, 350, 3, true, 10.10);


/* sell/close 3 Jan-20 300/350 vertical SPY call spreads @ a 9.91 credit or better */

// with the exact options
Order order3 = VB::Build("SPY_011720C350", "SPY_011720C300", 3, false, -9.91);

// OR
order3 = VB::Build("SPY", 1, 17, 2020, true, 350, 300, 3, false, -9.91);
```
```
[C]

Order_Ticket_C order2 = {0,0};

int err = BuildOrder_Spread_Vertical_Limit("SPY_011720C300", "SPY_011720C350",
                                           3, 1, 10.10, &order2);
if( err ){
    //
}

err = OrderTicket_Destroy(&order2);
if( err ){
    //
}

// OR
err = BuildOrder_Spread_Vertical_LimitEx("SPY", 1, 17, 2020, 1, 300, 350, 3, 1, 
                                         10.10, &order2);
if( err ){
    //
}

err = OrderTicket_Desgtroy(&order2);
if( err ){
    //
}
```

### Design Ideas

#### Higher Level Features

- Automatic checking for valid symbols via the 'Get' interface
- Automatic strike combination and credit/debit value checks/warnings vis-a-vis spread type
- ```Instrument``` objects that represent tradable instruments(w/ real-time bid/ask/last data) that can be passed to the builders
- Build related close, roll and leg-adjust orders from ```Order``` instances

...

#### Order Hub

Some type of object/interface that manages all order flow from the client side:
- receive Order objects, serialize, send to TDMA
- handle return info, errors etc.
- alert client, log 
- store/curate past and future orders for reference
- globally control/enforce execution and position limits 
- kill switch
- batch order control

...




