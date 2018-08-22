### Execute Interface
- - -
- [Overview](#overview)
- [Preliminary Design Ideas](#preliminary-design-ideas)
   - [Raw Orders](#raw-orders)
   - [Managed Orders](#managed-orders)
   - [Order Hub](#order-hub)
- - -

### Overview

Order execution and management interface. 

This interface is waiting on a mechanism from Ameritrade to test execution without having to send live dummy orders. In the meantime we'll provide some basics of the proposed interface so prospective users can comment and provide feedback.

Here's your chance to make suggestions about the design and implentation that you would, and would not, like to see. What do you like and not like about other broker APIs? What feature(s) would be useful?

Feel free to comment via [issues](https://github.com/jeog/TDAmeritradeAPI/issues/1) or email: jeog.dev@gmail.com

### Preliminary Design Ideas

#### Raw Orders

The full order schema will be represented by an Order object that the user has complete control over. The user is responsible for building the order from scratch i.e setting the order type, adding the order legs etc.

```
OrderInstrument instrument(OrderAssetType::OPTION, "SPY_011720C300");
OrderLeg leg1(instrument, OrderInstruction::BUY_TO_OPEN, 10);

Order order1; 

order1.add_leg(leg1).set_type(OrderType::LIMIT).set_price(9.99);
```

If a specific order type is allowed by TDMA it *should* be possible for a motivated user to build it this way.

#### Managed Orders

Most users will probably only need certain basic orders most of the time e.g market/limit/stop equity, vanilla spreads. To help (safely) build they would use nested static builders that would handle all the order details, allowing for something like:
```
// long 100 SPY @ 285.05 or better
Order order1 = SimpleOrderBuilder::Equity::Limit::Buy("SPY", 100, 285.05);

// buy/open 3 Jan-20 300/350 vertical SPY call spreads @ a 10.10 debit or better
Order order2 = SpreadOrderBuilder::Vertical::Open("SPY_011720C300", "SPY_011720C350", 3, 10.10);
```

Once the user has the order they can add to it as necessary e.g:
```
// defaults to OrderSession::DAY so...
order1.set_sesion(OrderSession::GOOD_TILL_CANCEL).set_cancel_time("2019-01-01");
```

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




