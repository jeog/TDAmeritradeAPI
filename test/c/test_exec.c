#include <math.h>

#include "tdma_api_execute.h"

#include "test.h"

struct TestLeg{
    const char* symbol;
    OrderInstruction instruction;
    OrderAssetType asset_type;
    size_t quantity;
};

int test_order_leg(size_t nleg, OrderLeg_C* l1, struct TestLeg *l2); /*DONE*/

int test_order(OrderTicket_C* porder,
                        OrderDuration duration,
                        OrderSession session,
                        OrderType order_type,
                        OrderStrategyType strategy_type,
                        ComplexOrderStrategyType complex_type,
                        double price,
                        double stop_price ,
                        struct TestLeg* legs,
                        size_t nlegs ); /*DONE*/

int test_option_order(OrderTicket_C* porder,
                        OrderType order_type,
                        ComplexOrderStrategyType complex_type,
                        double price,
                        struct TestLeg *legs,
                        size_t nlegs ); /*DONE*/

int test_equity_order(OrderTicket_C* porder,
                        OrderType order_type,
                        double price,
                        double stop_price,
                        struct TestLeg *legs,
                        size_t nlegs ); /*DONE*/

int test_order_access(); /*DONE*/
int test_simple_equity_order_builder(); /*DONE*/
int test_simple_option_order_builder(); /*DONE*/
int test_vertical_spread_order_builder(); /*DONE*/
int test_verticalroll_spread_order_builder(); /*DONE*/
int test_verticalroll_unbalanced_spread_order_builder(); /*DONE*/
int test_butterfly_spread_order_builder(); /*DONE*/
int test_butterfly_unbalanced_spread_order_builder(); /*DONE*/
int test_backratio_spread_order_builder(); /*DONE*/
int test_calendar_spread_order_builder(); /*DONE*/
int test_diagonal_spread_order_builder(); /*DONE*/
int test_straddle_spread_order_builder(); /*DONE*/
int test_strangle_spread_order_builder(); /*DONE*/
int test_syntheticcollar_spread_order_builder(); /*DONE*/
int test_collarwithstock_spread_order_builder(); /*DONE*/
int test_condor_spread_order_builder(); /*DONE*/
int test_condor_unbalanced_spread_order_builder(); /*DONE*/
int test_ironcondor_spread_order_builder(); /*DONE*/
int test_ironcondor_unbalanced_spread_order_builder(); /*DONE*/
int test_doublediagonal_spread_order_builder(); /*DONE*/
int test_one_cancels_other_order_builder(); /*DONE*/
int test_one_triggers_other_order_builder(); /*DONE*/

int
Test_Execution_Order_Objects()
{
    int err = 0;

    err = test_order_access();
    if( err )
        return err;

    err = test_simple_equity_order_builder();
    if( err )
        return err;

    err = test_simple_option_order_builder();
    if( err )
        return err;

    err = test_vertical_spread_order_builder();
    if( err )
        return err;

    err = test_verticalroll_spread_order_builder();
    if( err )
        return err;

    err = test_verticalroll_unbalanced_spread_order_builder();
    if( err )
        return err;

    err = test_butterfly_spread_order_builder();
    if( err )
        return err;

    err = test_butterfly_unbalanced_spread_order_builder();
    if( err )
        return err;

    err = test_backratio_spread_order_builder();
    if( err )
        return err;

    err = test_calendar_spread_order_builder();
    if( err )
        return err;

    err = test_diagonal_spread_order_builder();
    if( err )
        return err;

    err = test_straddle_spread_order_builder();
    if( err )
        return err;

    err = test_strangle_spread_order_builder();
    if( err )
        return err;

    err = test_syntheticcollar_spread_order_builder();
    if( err )
        return err;

    err = test_collarwithstock_spread_order_builder();
    if( err )
        return err;

    err = test_condor_spread_order_builder();
    if( err )
        return err;

    err = test_condor_unbalanced_spread_order_builder();
    if( err )
        return err;

    err = test_ironcondor_spread_order_builder();
    if( err )
        return err;

    err = test_ironcondor_unbalanced_spread_order_builder();
    if( err )
        return err;

    err = test_doublediagonal_spread_order_builder();
    if( err )
        return err;

    err = test_one_cancels_other_order_builder();
    if( err )
        return err;

    err = test_one_triggers_other_order_builder();
    if( err )
        return err;

    return 0;
}

int test_one_cancels_other_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    OrderTicket_C o3 = {0,0};
    OrderTicket_C o4 = {0,0};
    OrderTicket_C o5 = {0,0};
    char *buf;
    size_t n;

    printf( "\nOCO-Equity-Sell-Limit-/-Equity-Sell-Stop \n");

    if( (err = BuildOrder_Equity_Limit_Sell("SPY", 100, 300, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Sell");

    if( (err = BuildOrder_Equity_Stop_Sell("SPY", 100, 270, &o2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Sell");

    if( (err = BuildOrder_OneCancelsOther(&o, &o2, &o3)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OnceCancelsOther");

    OrderTicket_Destroy(&o);
    OrderTicket_Destroy(&o2);

    if( (err = OrderTicket_AsJsonString(&o3, &buf, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }
    printf( "BEFORE: \n %s \n", buf);

    const char* S1 = "{\"childOrderStrategies\":"
        "[{\"duration\":\"DAY\",\"orderLegCollection\":"
                "[{\"instruction\":\"SELL\","
                "\"instrument\":{\"assetType\":\"EQUITY\",\"symbol\":\"SPY\"},"
                "\"quantity\":100}],"
            "\"orderStrategyType\":\"SINGLE\",\"orderType\":\"LIMIT\","
            "\"price\":\"300.000000\",\"session\":\"NORMAL\"},"
        "{\"duration\":\"DAY\",\"orderLegCollection\":"
                "[{\"instruction\":\"SELL\","
                "\"instrument\":{\"assetType\":\"EQUITY\",\"symbol\":\"SPY\"},"
                "\"quantity\":100}],"
            "\"orderStrategyType\":\"SINGLE\",\"orderType\":\"STOP\","
            "\"session\":\"NORMAL\",\"stopPrice\":\"270.000000\"}],"
        "\"orderStrategyType\":\"OCO\"}";

    if( strcmp(buf, S1) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);

    // test order

    if( test_order(&o3, OrderDuration_NONE, OrderSession_NONE, OrderType_NONE,
                    OrderStrategyType_OCO, ComplexOrderStrategyType_NONE, .0, .0,
                    NULL, 0) )
     {
         OrderTicket_Destroy(&o3);
         return -1;
     }

    // get child orders

    OrderTicket_C *kids = NULL;
    if( (err = OrderTicket_GetChildren(&o3, &kids, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetChildren");
    }

    if( n != (size_t)2 ){
        fprintf(stderr, "invalid # of children(%i,%zu)\n", 2, n);
        OrderTicket_Destroy(&o3);
        // not destroying kids
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    // test child orders
    struct TestLeg legs[] = {
        {"SPY", OrderInstruction_SELL, OrderAssetType_EQUITY, 100},
    };
    if( test_equity_order(&kids[0], OrderType_LIMIT, 300, 0., legs, 1) )
    {
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        OrderTicket_Destroy(&kids[1]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    if( test_equity_order(&kids[1], OrderType_STOP, 0., 270, legs, 1) )
    {
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        OrderTicket_Destroy(&kids[1]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    OrderTicket_Destroy(&kids[0]);
    OrderTicket_Destroy(&kids[1]);
    FreeOrderTicketBuffer(kids);
    kids = NULL;

    printf( "\nOCO-Equity-Cover-Stop-/-Equity-Cover-Limit \n");

    // create and add new child orders

    if( (err = BuildOrder_Equity_Stop_Cover("SPY", 200, 280, &o4)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Cover");
    }

    if( (err = BuildOrder_Equity_Limit_Cover("SPY", 200, 269.99, &o5)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o4);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Cover");
    }

    if( (err = OrderTicket_ClearChildren(&o3)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o4);
        OrderTicket_Destroy(&o5);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_ClearChildren");
        return -1;
    }

    if( (err = OrderTicket_AddChild(&o3, &o4)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o4);
        OrderTicket_Destroy(&o5);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_AddChild");
        return -1;
    }
    OrderTicket_Destroy(&o4);

    if( (err = OrderTicket_AddChild(&o3, &o5)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o5);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_AddChild (2)");
        return -1;
    }
    OrderTicket_Destroy(&o5);

    if( (err = OrderTicket_AsJsonString(&o3, &buf, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    printf( "AFTER: \n %s \n", buf);
    FreeBuffer(buf);

    // get new child orders

    if( (err = OrderTicket_GetChildren(&o3, &kids, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetChildren (2)");
    }

    if( n != (size_t)2 ){
        fprintf(stderr, "invalid # of children(%i,%zu) (2)\n", 1, n);
        OrderTicket_Destroy(&o3);
        // not destroying kids
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    // test new order

    if( test_order(&o3, OrderDuration_NONE, OrderSession_NONE, OrderType_NONE,
                    OrderStrategyType_OCO, ComplexOrderStrategyType_NONE, .0, .0,
                    NULL, 0) )
     {
         OrderTicket_Destroy(&o3);
         OrderTicket_Destroy(&kids[0]);
         OrderTicket_Destroy(&kids[1]);
         FreeOrderTicketBuffer(kids);
         return -1;
     }

    // test new child orders

    legs[0].instruction = OrderInstruction_BUY_TO_COVER;
    legs[0].quantity = 200;
    if( test_equity_order(&kids[0], OrderType_STOP, 0.0, 280.00, legs, 1) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        OrderTicket_Destroy(&kids[1]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    if( test_equity_order(&kids[1], OrderType_LIMIT, 269.99, 0.0, legs, 1) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        OrderTicket_Destroy(&kids[1]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    OrderTicket_Destroy(&o3);
    OrderTicket_Destroy(&kids[0]);
    OrderTicket_Destroy(&kids[1]);
    FreeOrderTicketBuffer(kids);
    return 0;
}


int test_one_triggers_other_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    OrderTicket_C o3 = {0,0};
    OrderTicket_C o4 = {0,0};
    char *buf;
    size_t n;

    printf( "\nOTO-Equity-Buy-Limit-/-Equity-Sell-Stop \n");

    if( (err = BuildOrder_Equity_Limit_Buy("SPY", 100, 275, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Buy");

    if( (err = BuildOrder_Equity_Stop_Sell("SPY", 100, 270, &o2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Sell");

    if( (err = BuildOrder_OneTriggersOther(&o, &o2, &o3)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OnceCancelsOther");

    OrderTicket_Destroy(&o);
    OrderTicket_Destroy(&o2);

    if( (err = OrderTicket_AsJsonString(&o3, &buf, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }
    printf( "BEFORE: \n %s \n", buf);

    const char* S1 = "{\"childOrderStrategies\":"
        "[{\"duration\":\"DAY\",\"orderLegCollection\":"
                "[{\"instruction\":\"SELL\","
                "\"instrument\":{\"assetType\":\"EQUITY\",\"symbol\":\"SPY\"},"
                "\"quantity\":100}],"
            "\"orderStrategyType\":\"SINGLE\",\"orderType\":\"STOP\","
            "\"session\":\"NORMAL\",\"stopPrice\":\"270.000000\"}],"
        "\"duration\":\"DAY\","
        "\"orderLegCollection\":"
            "[{\"instruction\":\"BUY\",\"instrument\":"
                "{\"assetType\":\"EQUITY\",\"symbol\":\"SPY\"},\"quantity\":100}],"
        "\"orderStrategyType\":\"TRIGGER\",\"orderType\":\"LIMIT\","
        "\"price\":\"275.000000\",\"session\":\"NORMAL\"}";

    if( strcmp(buf, S1) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);

    struct TestLeg legs[] = {
        {"SPY", OrderInstruction_BUY, OrderAssetType_EQUITY, 100},
    };
    if( test_order(&o3, OrderDuration_DAY, OrderSession_NORMAL,
                          OrderType_LIMIT, OrderStrategyType_TRIGGER,
                          ComplexOrderStrategyType_NONE, 275, .0, legs, 1) ){
        return -1;
    }

    OrderTicket_C *kids = NULL;
    if( (err = OrderTicket_GetChildren(&o3, &kids, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetChildren");
    }

    if( n != (size_t)1 ){
        fprintf(stderr, "invalid # of children(%i,%zu)\n", 1, n);
        OrderTicket_Destroy(&o3);
        // not destroying kids
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    legs[0].instruction = OrderInstruction_SELL;
    if( test_equity_order(&kids[0], OrderType_STOP, 0., 270, legs, 1) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }
    OrderTicket_Destroy(&kids[0]);
    FreeOrderTicketBuffer(kids);
    kids = NULL;

    printf( "\nOTO-Equity-Buy-Limit-/-Equity-StopLimit-Stop \n");

    if( (err = BuildOrder_Equity_StopLimit_Sell("SPY", 200, 269.99, 269.89, &o4)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_StopLimit_Sell");
    }

    if( (err = OrderTicket_ClearChildren(&o3)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o4);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_ClearChildren");
        return -1;
    }

    if( (err = OrderTicket_AddChild(&o3, &o4)) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&o4);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_AddChild");
        return -1;
    }
    OrderTicket_Destroy(&o4);

    if( (err = OrderTicket_AsJsonString(&o3, &buf, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    printf( "AFTER: \n %s \n", buf);
    FreeBuffer(buf);

    if( (err = OrderTicket_GetChildren(&o3, &kids, &n)) ){
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetChildren (2)");
    }

    if( n != (size_t)1 ){
        fprintf(stderr, "invalid # of children(%i,%zu) (2) \n", 1, n);
        OrderTicket_Destroy(&o3);
        // not destroying kids
        FreeOrderTicketBuffer(kids);
        return -1;
    }

    legs[0].instruction = OrderInstruction_SELL;
    legs[0].quantity = 200;
    if( test_equity_order(&kids[0], OrderType_STOP_LIMIT, 269.89, 269.99, legs, 1) ){
        OrderTicket_Destroy(&o3);
        OrderTicket_Destroy(&kids[0]);
        FreeOrderTicketBuffer(kids);
        return -1;
    }
    OrderTicket_Destroy(&kids[0]);
    FreeOrderTicketBuffer(kids);
    OrderTicket_Destroy(&o3);
    return 0;
}


int test_doublediagonal_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_DOUBLE_DIAGONAL;

    /* MARKET */
    printf( "Spread-Diagonal-Market (1) \n");

    if( (err = BuildOrder_Spread_DoubleDiagonal_Market("SPY_011720C300",
                "SPY_032020C325", "SPY_011720P275","SPY_032020P250", 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P275", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020P250", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_DoubleDiagonal_MarketEx("SPY", 1, 17,
            2020, 3,20,2020, 300, 325, 275, 250,  1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-DoubleDiagonal-Market (2) \n");

    if( (err = BuildOrder_Spread_DoubleDiagonal_Market("SPY_032020C325",
         "SPY_011720C300", "SPY_032020P250", "SPY_011720P275", 99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_032020C325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P275", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}

    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_DoubleDiagonal_MarketEx("SPY",  3, 20, 2020,
        1, 17,  2020, 325, 300, 250, 275, 99, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-DoubleDiagonal-Limit (1) \n");

    if( (err = BuildOrder_Spread_DoubleDiagonal_Limit("SPY_011720C300",
        "SPY_032020C325", "SPY_011720P275","SPY_032020P250",  3, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_032020C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720P275", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_032020P250", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_DoubleDiagonal_LimitEx("SPY", 1, 17,
            2020, 3, 20, 2020, 300, 325, 275, 250, 3, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-DoubleDiagonal-Limit (2) \n");

    if( (err = BuildOrder_Spread_DoubleDiagonal_Limit("SPY_032020C325",
        "SPY_011720C300", "SPY_032020P250", "SPY_011720P275", 99, 0, -.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_032020C325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P275", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, .01, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_DoubleDiagonal_LimitEx("SPY", 3, 20, 2020,
            1, 17, 2020,  325, 300, 250, 275, 99, 0, -.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_DoubleDiagonal_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}


int test_ironcondor_unbalanced_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_UNBALANCED_IRON_CONDOR;

    /* MARKET */
    printf( "Spread-IronCondorUnbalanced-Market (1) \n");

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_Market("SPY_011720C300",
                "SPY_011720C350", "SPY_011720P250", "SPY_011720P200", 1, 2, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C350", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_MarketEx("SPY", 1, 17,
            2020, 300, 350, 250, 200, 1, 2, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-IronCondorUnbalanced-Market (2) \n");

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_Market("SPY_011720C350",
        "SPY_011720C300", "SPY_011720P200", "SPY_011720P250",  99, 98,0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C350", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 98},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 98},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_MarketEx("SPY", 1, 17,
            2020, 350, 300, 200, 250, 99, 98, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-IronCondorUnbalanced-Limit (1) \n");

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_Limit("SPY_011720C300",
        "SPY_011720C350", "SPY_011720P250", "SPY_011720P200", 3, 4, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C350", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 4},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 4},
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_LimitEx("SPY", 1, 17, 2020,
                 300, 350, 250, 200, 3, 4, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-IronCondorUnbalanced-Limit (2) \n");

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_Limit("SPY_011720C350",
        "SPY_011720C300", "SPY_011720P200", "SPY_011720P250", 1, 99, 0, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C350", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondorUnbalanced_LimitEx("SPY", 1, 17,
            2020, 350, 300, 200, 250, 1, 99, 0, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondorUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;

}

int test_ironcondor_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_IRON_CONDOR;

    /* MARKET */
    printf( "Spread-IronCondor-Market (1) \n");

    if( (err = BuildOrder_Spread_IronCondor_Market("SPY_011720C300",
                "SPY_011720C350", "SPY_011720P250", "SPY_011720P200", 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C350", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondor_MarketEx("SPY", 1, 17,
            2020, 300, 350, 250, 200, 1, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-IronCondor-Market (2) \n");

    if( (err = BuildOrder_Spread_IronCondor_Market("SPY_011720C350",
        "SPY_011720C300", "SPY_011720P200", "SPY_011720P250",  99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C350", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondor_MarketEx("SPY", 1, 17,
            2020, 350, 300, 200, 250, 99, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-IronCondor-Limit (1) \n");

    if( (err = BuildOrder_Spread_IronCondor_Limit("SPY_011720C300",
        "SPY_011720C350", "SPY_011720P250", "SPY_011720P200", 3, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C350", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3},
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondor_LimitEx("SPY", 1, 17, 2020,
                 300, 350, 250, 200, 3, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-IronCondor-Limit (2) \n");

    if( (err = BuildOrder_Spread_IronCondor_Limit("SPY_011720C350",
        "SPY_011720C300", "SPY_011720P200", "SPY_011720P250", 99, 0, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C350", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_IronCondor_LimitEx("SPY", 1, 17,
            2020, 350, 300, 200, 250, 99, 0, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_IronCondor_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;

}

int test_condor_unbalanced_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_UNBALANCED_CONDOR;

    /* MARKET */
    printf( "Spread-CondorUnbalanced-Market (1) \n");

    if( (err = BuildOrder_Spread_CondorUnbalanced_Market("SPY_011720P250",
                "SPY_011720P275", "SPY_011720P300", "SPY_011720P325", 1, 2, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P275", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_011720P325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 2},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CondorUnbalanced_MarketEx("SPY", 1, 17,
            2020, 250, 275, 300, 325, 0, 1, 2, 1, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CondorUnbalanced-Market (2) \n");

    if( (err = BuildOrder_Spread_CondorUnbalanced_Market("SPY_011720C300",
        "SPY_011720C290", "SPY_011720C270", "SPY_011720C260",  99, 98, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C290", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C270", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 98},
        {"SPY_011720C260", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 98},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CondorUnbalanced_MarketEx("SPY", 1, 17,
            2020, 300, 290, 270, 260, 1, 99, 98, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-CondorUnbalanced-Limit (1) \n");

    if( (err = BuildOrder_Spread_CondorUnbalanced_Limit("SPY_011720P250",
        "SPY_011720P275", "SPY_011720P300", "SPY_011720P325", 3, 4, 1, 0, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_011720P275", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 4},
        {"SPY_011720P325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 4},
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CondorUnbalanced_LimitEx("SPY", 1, 17, 2020,
                 250, 275, 300, 325, 0, 3, 4, 1, 0, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CondorUnbalanced-Limit (2) \n");

    if( (err = BuildOrder_Spread_CondorUnbalanced_Limit("SPY_011720C300",
        "SPY_011720C290", "SPY_011720C270", "SPY_011720C260", 99,100, 0, 1, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720C290", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720C270", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 100},
        {"SPY_011720C260", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 100},
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CondorUnbalanced_LimitEx("SPY", 1, 17,
            2020, 300, 290, 270, 260,1, 99, 100, 0, 1, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CondorUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;

}

int test_condor_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_CONDOR;

    /* MARKET */
    printf( "Spread-Condor-Market (1) \n");

    if( (err = BuildOrder_Spread_Condor_Market("SPY_011720P250",
                "SPY_011720P275", "SPY_011720P300", "SPY_011720P325", 1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P275", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Condor_MarketEx("SPY", 1, 17,
            2020, 250, 275, 300, 325, 0, 1, 1, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Condor-Market (2) \n");

    if( (err = BuildOrder_Spread_Condor_Market("SPY_011720C300",
        "SPY_011720C290", "SPY_011720C270", "SPY_011720C260",  99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C290", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C270", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C260", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Condor_MarketEx("SPY", 1, 17,
            2020, 300, 290, 270, 260, 1, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Condor-Limit (1) \n");

    if( (err = BuildOrder_Spread_Condor_Limit("SPY_011720P250",
        "SPY_011720P275", "SPY_011720P300", "SPY_011720P325", 3, 1, 0, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_011720P275", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_011720P325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Condor_LimitEx("SPY", 1, 17, 2020,
                 250, 275, 300, 325, 0, 3, 1, 0, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Condor-Limit (2) \n");

    if( (err = BuildOrder_Spread_Condor_Limit("SPY_011720C300",
        "SPY_011720C290", "SPY_011720C270", "SPY_011720C260", 99, 0, 1, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720C290", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720C270", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720C260", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Condor_LimitEx("SPY", 1, 17,
            2020, 300, 290, 270, 260,1, 99, 0, 1, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Condor_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;

}


int test_collarwithstock_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_COLLAR_WITH_STOCK;

    /* MARKET */
    printf( "Spread-CollarWithStock-Market (1) \n");

    if( (err = BuildOrder_Spread_CollarWithStock_Market("SPY_011720P250",
                "SPY_011720C250", "SPY",  1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C250", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
        {"SPY", OrderInstruction_BUY, OrderAssetType_EQUITY, 100}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarWithStock_MarketEx("SPY", 1, 17,
            2020, 250, 250,  1, 1, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CollarWithStock-Market (2) \n");

    if( (err = BuildOrder_Spread_CollarWithStock_Market("SPY_011720C300",
                "SPY_011720P200", "SPY",  99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY", OrderInstruction_SELL, OrderAssetType_EQUITY, 9900}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarWithStock_MarketEx("SPY", 1, 17,
            2020, 300, 200, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-CollarWithStock-Limit (1) \n");

    if( (err = BuildOrder_Spread_CollarWithStock_Limit("SPY_032020P305",
                "SPY_032020C315", "SPY", 3, 1, 0, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020P305", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_032020C315", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY", OrderInstruction_BUY_TO_COVER, OrderAssetType_EQUITY, 300}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarWithStock_LimitEx("SPY", 3, 20, 2020,
                 315, 305, 3, 1, 0, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CollarWithStock-Limit (2) \n");

    if( (err = BuildOrder_Spread_CollarWithStock_Limit("SPY_011720C300",
                "SPY_011720P295", "SPY", 99, 0, 1, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720P295", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
        {"SPY", OrderInstruction_SELL_SHORT, OrderAssetType_EQUITY, 9900}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarWithStock_LimitEx("SPY", 1, 17,
            2020, 300, 295, 99, 0, 1, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarWithStock_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}

int test_syntheticcollar_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_COLLAR_SYNTHETIC;

    /* MARKET */
    printf( "Spread-CollarSynthetic-Market (1) \n");

    if( (err = BuildOrder_Spread_CollarSynthetic_Market("SPY_011720C300",
                "SPY_011720P200",  1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarSynthetic_MarketEx("SPY", 1, 17,
            2020, 300, 200,  1, 1, 1,  &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CollarSynthetic-Market (2) \n");

    if( (err = BuildOrder_Spread_CollarSynthetic_Market("SPY_011720P200",
                "SPY_011720C200",  99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarSynthetic_MarketEx("SPY", 1, 17,
            2020, 200, 200, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-CollarSynthetic-Limit (1) \n");

    if( (err = BuildOrder_Spread_CollarSynthetic_Limit("SPY_032020C305",
                "SPY_032020P295",  3, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020C305", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_032020P295", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarSynthetic_LimitEx("SPY", 3, 20, 2020,
                 305, 295, 3, 1, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-CollarSynthetic-Limit (2) \n");

    if( (err = BuildOrder_Spread_CollarSynthetic_Limit("SPY_011720P300",
                "SPY_011720C295", 99, 0, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720C295", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_CollarSynthetic_LimitEx("SPY", 1, 17,
            2020, 295, 300, 99, 0, 0, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_CollarSynthetic_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}


int test_strangle_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;

    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_STRANGLE;

    /* MARKET */
    printf( "Spread-Strangle-Market (1) \n");

    if( (err = BuildOrder_Spread_Strangle_Market("SPY_011720C300",
                "SPY_011720P200",  1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P200", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Strangle_MarketEx("SPY", 1, 17,
            2020, 300, 200,  1, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Strangle-Market (2) \n");

    if( (err = BuildOrder_Spread_Strangle_Market("SPY_011720C350",
                "SPY_011720P250",  99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C350", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Strangle_MarketEx("SPY", 1, 17,
            2020, 350, 250, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Strangle-Limit (1) \n");

    if( (err = BuildOrder_Spread_Strangle_Limit("SPY_032020C305",
                "SPY_032020P300",  3, 1, 0, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020C305", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_032020P300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Strangle_LimitEx("SPY", 3, 20, 2020,
                 305, 300, 3, 1, 0, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Strangle-Limit (2) \n");

    if( (err = BuildOrder_Spread_Strangle_Limit("SPY_011720C300",
                "SPY_011720P290", 99, 0, 1, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720P290", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Strangle_LimitEx("SPY", 1, 17,
            2020, 300, 290, 99, 0, 1, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Strangle_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}


int test_straddle_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_STRADDLE;

    /* MARKET */
    printf( "Spread-Straddle-Market (1) \n");

    if( (err = BuildOrder_Spread_Straddle_Market("SPY_011720C300",
                "SPY_011720P300",  1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720P300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Straddle_MarketEx("SPY", 1, 17,
            2020, 300,  1, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Straddle-Market (2) \n");

    if( (err = BuildOrder_Spread_Straddle_Market("SPY_011720C250",
                "SPY_011720P250",  99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720C250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Straddle_MarketEx("SPY", 1, 17,
            2020, 250, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Straddle-Limit (1) \n");

    if( (err = BuildOrder_Spread_Straddle_Limit("SPY_032020C325",
                "SPY_032020P325",  3, 1, 0, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020C325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3},
        {"SPY_032020P325", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Straddle_LimitEx("SPY", 3, 20, 2020,
                 325, 3, 1, 0, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Straddle-Limit (2) \n");

    if( (err = BuildOrder_Spread_Straddle_Limit("SPY_011720C200",
                "SPY_011720P200", 99, 0, 1, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720C200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Straddle_LimitEx("SPY", 1, 17,
            2020, 200, 99, 0, 1, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Straddle_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}


int test_diagonal_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_DIAGONAL;

    /* MARKET */
    printf( "Spread-Diagonal-Market (1) \n");

    if( (err = BuildOrder_Spread_Diagonal_Market("SPY_011720C300",
                "SPY_032020C325",  1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Diagonal_MarketEx("SPY", 1, 17,
            2020, 3,20,2020,1, 300, 325,  1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Diagonal-Market (2) \n");

    if( (err = BuildOrder_Spread_Diagonal_Market("SPY_011720P250",
                "SPY_032020P225",  99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P225", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Diagonal_MarketEx("SPY", 1, 17,
            2020, 3, 20, 2020, 0, 250, 225, 99, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Diagonal-Limit (1) \n");

    if( (err = BuildOrder_Spread_Diagonal_Limit("SPY_032020C325",
                "SPY_011720C300",  3, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Diagonal_LimitEx("SPY", 3, 20, 2020, 1, 17,
            2020, 1, 325, 300, 3, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Diagonal-Limit (2) \n");

    if( (err = BuildOrder_Spread_Diagonal_Limit("SPY_011720P200",
                "SPY_032020P175", 99, 0, -.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P175", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, .01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Diagonal_LimitEx("SPY", 1, 17,
            2020, 3, 20, 2020, 0, 200, 175, 99, 0, -.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Diagonal_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}

int test_calendar_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_CALENDAR;

    /* MARKET */
    printf( "Spread-Calendar-Market (1) \n");

    if( (err = BuildOrder_Spread_Calendar_Market("SPY_011720C300",
                "SPY_032020C300",  1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Calendar_MarketEx("SPY", 1, 17,
            2020, 3,20,2020,1, 300,  1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Calendar-Market (2) \n");

    if( (err = BuildOrder_Spread_Calendar_Market("SPY_032020P250",
                "SPY_011720P250",  99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_032020P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Calendar_MarketEx("SPY", 3, 20, 2020, 1, 17,
            2020, 0, 250, 99, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Calendar-Limit (1) \n");

    if( (err = BuildOrder_Spread_Calendar_Limit("SPY_032020C325",
                "SPY_011720C325",  3, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Calendar_LimitEx("SPY", 3, 20, 2020, 1, 17,
            2020, 1, 325, 3, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Calendar-Limit (2) \n");

    if( (err = BuildOrder_Spread_Calendar_Limit("SPY_011720P200",
                "SPY_032020P200", 99, 0, -.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, .01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Calendar_LimitEx("SPY", 1, 17,
            2020, 3, 20, 2020, 0, 200, 99, 0, -.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Calendar_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}

int test_backratio_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_BACK_RATIO;

    /* MARKET */
    printf( "Spread-BackRatio-Market (1) \n");

    if( (err = BuildOrder_Spread_BackRatio_Market("SPY_011720C300",
               "SPY_011720C325", 1, 2, 1, &o)) )
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
       OrderTicket_Destroy(&o);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_BackRatio_MarketEx("SPY", 1, 17,
           2020, 1, 300, 325,  1,2, 1, &o2)) ){
       FreeBuffer(buf);
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
       FreeBuffer(buf);
       OrderTicket_Destroy(&o2);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
       FreeBuffer(buf);
       FreeBuffer(buf2);
       fprintf(stderr, "json of raw and ex build orders don't match \n");
       return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-BackRatio-Market (2) \n");

    if( (err = BuildOrder_Spread_BackRatio_Market("SPY_011720P250",
               "SPY_011720P200",  99, 98, 0, &o)) )
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
       OrderTicket_Destroy(&o);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 98}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_BackRatio_MarketEx("SPY", 1, 17,
           2020, 0, 250, 200, 99, 98, 0, &o2)) ){
       FreeBuffer(buf);
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
       FreeBuffer(buf);
       OrderTicket_Destroy(&o2);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
       FreeBuffer(buf);
       FreeBuffer(buf2);
       fprintf(stderr, "json of raw and ex build orders don't match \n");
       return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-BackRatio-Limit (1) \n");

    if( (err = BuildOrder_Spread_BackRatio_Limit("SPY_011720C300",
               "SPY_011720C325", 3, 1, 1, 9.99, &o)) )
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
       OrderTicket_Destroy(&o);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_BackRatio_LimitEx("SPY", 1, 17,
           2020, 1, 300, 325, 3, 1, 1, 9.99, &o2)) ){
       FreeBuffer(buf);
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
       FreeBuffer(buf);
       OrderTicket_Destroy(&o2);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
       FreeBuffer(buf);
       FreeBuffer(buf2);
       fprintf(stderr, "json of raw and ex build orders don't match \n");
       return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-BackRatio-Limit (2) \n");

    if( (err = BuildOrder_Spread_BackRatio_Limit("SPY_011720P200",
               "SPY_011720P250", 1, 99, 0, .0, &o)) )
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
       OrderTicket_Destroy(&o);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_ZERO, COMPLEX, .0, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_BackRatio_LimitEx("SPY", 1, 17,
           2020, 0, 200, 250, 1,99, 0, .0, &o2)) ){
       FreeBuffer(buf);
       CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_BackRatio_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
       FreeBuffer(buf);
       OrderTicket_Destroy(&o2);
       CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
       FreeBuffer(buf);
       FreeBuffer(buf2);
       fprintf(stderr, "json of raw and ex build orders don't match \n");
       return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;

}


int test_butterfly_unbalanced_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_UNBALANCED_BUTTERFLY;

    /* MARKET */
    printf( "Spread-ButterflyUnbalanced-Market (1) \n");

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_Market("SPY_011720C300",
                "SPY_011720C325", "SPY_011720C350", 1,2, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C350", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 2}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_MarketEx("SPY", 1, 17,
            2020, 1, 300, 325, 350,  1,2, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-ButterflyUnbalanced-Market (2) \n");

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_Market("SPY_011720P200",
                "SPY_011720P250", "SPY_011720P300", 99, 98, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 197},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 98}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_MarketEx("SPY", 1, 17,
            2020, 0, 200, 250, 300, 99, 98, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-ButterflyUnbalanced-Limit (1) \n");

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_Limit("SPY_011720C300",
                "SPY_011720C325", "SPY_011720C350", 3, 1, 1, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 3},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 4},
        {"SPY_011720C350", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_LimitEx("SPY", 1, 17,
            2020, 1, 300, 325, 350,  3, 1, 1, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-ButterflyUnbalanced-Limit (2) \n");

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_Limit("SPY_011720P200",
                "SPY_011720P250", "SPY_011720P300", 1, 99, 0, 0, -.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 100},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, .01, legs4, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_ButterflyUnbalanced_LimitEx("SPY", 1, 17,
            2020, 0, 200, 250, 300, 1,99, 0, 0, -.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_ButterflyUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}

int test_butterfly_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_BUTTERFLY;

    /* MARKET */
    printf( "Spread-Butterfly-Market (1) \n");

    if( (err = BuildOrder_Spread_Butterfly_Market("SPY_011720C300",
                "SPY_011720C325", "SPY_011720C350", 1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Buttefly_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_011720C350", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Butterfly_MarketEx("SPY", 1, 17,
            2020, 1, 300, 325, 350,  1, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Butterfly_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Butterfly-Market (2) \n");

    if( (err = BuildOrder_Spread_Butterfly_Market("SPY_011720P200",
                "SPY_011720P250", "SPY_011720P300", 99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Buttefly_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 198},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, .0, legs2, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Butterfly_MarketEx("SPY", 1, 17,
            2020, 0, 200, 250, 300, 99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Butterfly_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-Butterfly-Limit (1) \n");

    if( (err = BuildOrder_Spread_Butterfly_Limit("SPY_011720C300",
                "SPY_011720C325", "SPY_011720C350", 1, 1, 1, 9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Buttefly_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_011720C350", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 9.99, legs3, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Butterfly_LimitEx("SPY", 1, 17,
            2020, 1, 300, 325, 350,  1, 1, 1, 9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Butterfly_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Butterfly-Limit (2) \n");

    if( (err = BuildOrder_Spread_Butterfly_Limit("SPY_011720P200",
                "SPY_011720P250", "SPY_011720P300", 49, 0, 0, -.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Buttefly_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 49},
        {"SPY_011720P250", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 98},
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 49}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, .01, legs4, 3) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Butterfly_LimitEx("SPY", 1, 17,
            2020, 0, 200, 250, 300, 49, 0, 0, -.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Butterfly_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    return 0;
}

int test_verticalroll_unbalanced_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_UNBALANCED_VERTICAL_ROLL;

    /* MARKET */
    printf( "Spread-VerticalRollUnbalanced-Market (1) \n");

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_Market("SPY_011720C300",
                "SPY_011720C325", "SPY_032020C325", "SPY_032020C300", 1, 2, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs1[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_032020C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs1, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_MarketEx("SPY", 1, 17,
            2020, 3, 20,  2020, 1, 300, 325, 325, 300,  1, 2, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-VerticalRollUnbalanced-Market (2) \n");

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_Market("SPY_011720P200",
        "SPY_011720P250", "SPY_032020P225", "SPY_032020P200", 50, 49, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 50},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 50},
        {"SPY_032020P225", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 49},
        {"SPY_032020P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 49}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_MarketEx("SPY", 1, 17,
            2020, 3, 20,  2020, 0, 200, 250, 225, 200, 50, 49, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    /* LIMIT */
    printf( "Spread-VerticalRollUnbalanced-Limit (1) \n");

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_Limit("SPY_011720C300",
                "SPY_011720C325", "SPY_032020C325", "SPY_032020C300", 1, 2,
                10.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 2},
        {"SPY_032020C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 2}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 10.01, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_LimitEx("SPY", 1, 17,
            2020, 3, 20,  2020, 1, 300, 325, 325, 300,  1, 2, 10.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-VerticalRollUnbalanced-Limit (2) \n");

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_Limit("SPY_011720P200",
        "SPY_011720P250", "SPY_032020P225", "SPY_032020P200", 50, 49,
                0.0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 50},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 50},
        {"SPY_032020P225", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 49},
        {"SPY_032020P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 49}
    };
    if( test_option_order(&o, OrderType_NET_ZERO, COMPLEX, 0., legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRollUnbalanced_LimitEx("SPY", 1, 17,
            2020, 3, 20,  2020, 0, 200, 250, 225, 200,  50,49, 0.0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err,
            "BuildOrder_Spread_VerticalRollUnbalanced_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    /* MANUAL */
    char *buf3;
    size_t n3;
    OrderTicket_C o3;
    OrderLeg_C legs[] = {{0,0},{0,0},{0,0},{0,0}};

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P200",
                          OrderInstruction_BUY_TO_CLOSE, 50, &legs[0]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P250",
                          OrderInstruction_SELL_TO_CLOSE, 50, &legs[1]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (2)");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_032020P225",
                          OrderInstruction_BUY_TO_OPEN, 49, &legs[2]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        OrderLeg_Destroy(&legs[1]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (3)");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_032020P200",
                          OrderInstruction_SELL_TO_OPEN, 49, &legs[3]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        OrderLeg_Destroy(&legs[1]);
        OrderLeg_Destroy(&legs[2]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (4)");
    }

    err = BuildOrder_Spread(ComplexOrderStrategyType_UNBALANCED_VERTICAL_ROLL,
                            legs, 4, 0, .0, &o3);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        OrderLeg_Destroy(&legs[1]);
        OrderLeg_Destroy(&legs[2]);
        OrderLeg_Destroy(&legs[3]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread (Vertical Roll)");
    }
    OrderLeg_Destroy(&legs[0]);
    OrderLeg_Destroy(&legs[1]);
    OrderLeg_Destroy(&legs[2]);
    OrderLeg_Destroy(&legs[3]);

    if( (err = OrderTicket_AsJsonString(&o3, &buf3, &n3)) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (3)" );
    }
    OrderTicket_Destroy(&o3);

    printf( "MANUAL: \n %s \n", buf3);

    if( n2 != n3 || strcmp(buf2, buf3) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        FreeBuffer(buf3);
        fprintf(stderr, "json of ex and manual build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    FreeBuffer(buf3);
    buf = buf2 = buf3 = NULL;


    return 0;
}

int test_verticalroll_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX =
        ComplexOrderStrategyType_VERTICAL_ROLL;

    /* MARKET */
    printf( "Spread-VerticalRoll-Market (1) \n");

    if( (err = BuildOrder_Spread_VerticalRoll_Market("SPY_011720C300",
                "SPY_011720C325", "SPY_032020C325", "SPY_032020C300", 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs1[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs1, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRoll_MarketEx("SPY", 1, 17, 2020, 3, 20,
            2020, 1, 300, 325, 325, 300, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-VerticalRoll-Market (2) \n");

    if( (err = BuildOrder_Spread_VerticalRoll_Market("SPY_011720P200",
                "SPY_011720P250", "SPY_032020P225", "SPY_032020P200", 99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P225", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_032020P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRoll_MarketEx("SPY", 1, 17, 2020, 3, 20,
            2020, 0, 200, 250, 225, 200, 99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    /* LIMIT */
    printf( "Spread-VerticalRoll-Limit (1) \n");

    if( (err = BuildOrder_Spread_VerticalRoll_Limit("SPY_011720C300",
                  "SPY_011720C325", "SPY_032020C325", "SPY_032020C300", 1,
                  1.11, &o)) )
         CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
         OrderTicket_Destroy(&o);
         CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 1},
        {"SPY_032020C325", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_032020C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 1.11, legs3, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRoll_LimitEx("SPY", 1, 17, 2020, 3, 20,
             2020, 1, 300, 325, 325, 300, 1, 1.11, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
         FreeBuffer(buf);
         OrderTicket_Destroy(&o2);
         CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-VerticalRoll-Limit (2) \n");

    if( (err = BuildOrder_Spread_VerticalRoll_Limit("SPY_011720P200",
        "SPY_011720P250", "SPY_032020P225", "SPY_032020P200", 99, -1.11, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_032020P225", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 99},
        {"SPY_032020P200", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.11, legs4, 4) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_VerticalRoll_LimitEx("SPY", 1, 17, 2020, 3, 20,
                2020, 0, 200, 250, 225, 200, 99, -1.11, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_VerticalRoll_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    /* MANUAL */

    char *buf3;
    size_t n3;
    OrderTicket_C o3;
    OrderLeg_C legs[] = {{0,0},{0,0},{0,0},{0,0}};

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P200",
                          OrderInstruction_BUY_TO_CLOSE, 99, &legs[0]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P250",
                          OrderInstruction_SELL_TO_CLOSE, 99, &legs[1]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (2)");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_032020P225",
                          OrderInstruction_BUY_TO_OPEN, 99, &legs[2]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[1]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (3)");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_032020P200",
                          OrderInstruction_SELL_TO_OPEN, 99, &legs[3]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[2]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (4)");
    }

    err = BuildOrder_Spread(ComplexOrderStrategyType_VERTICAL_ROLL, legs, 4, 0,
                            -1.11, &o3);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        OrderLeg_Destroy(&legs[1]);
        OrderLeg_Destroy(&legs[2]);
        OrderLeg_Destroy(&legs[3]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread (Vertical Roll)");
    }
    OrderLeg_Destroy(&legs[0]);
    OrderLeg_Destroy(&legs[1]);
    OrderLeg_Destroy(&legs[2]);
    OrderLeg_Destroy(&legs[3]);

    if( (err = OrderTicket_AsJsonString(&o3, &buf3, &n3)) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (3)" );
    }
    OrderTicket_Destroy(&o3);

    printf( "MANUAL: \n %s \n", buf3);

    if( n2 != n3 || strcmp(buf2, buf3) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        FreeBuffer(buf3);
        fprintf(stderr, "json of ex and manual build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    FreeBuffer(buf3);
    buf = buf2 = buf3 = NULL;


    return 0;
}


int test_vertical_spread_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_VERTICAL;

    /* MARKET */
    printf( "Spread-Vertical-Market (1) \n");

    if( (err = BuildOrder_Spread_Vertical_Market("SPY_011720C300",
                "SPY_011720C325", 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs1[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs1, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Vertical_MarketEx("SPY", 1, 17, 2020, 1, 300,
                                                  325, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)");
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Vertical-Market (2) \n");

    if( (err = BuildOrder_Spread_Vertical_Market("SPY_011720P200",
                "SPY_011720P250", 99, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Vertical_MarketEx("SPY", 1, 17, 2020, 0, 200,
                                                  250, 99, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    /* LIMIT */
    printf( "Spread-Vertical-Limit (1) \n");

    if( (err = BuildOrder_Spread_Vertical_Limit("SPY_011720C300",
                "SPY_011720C325", 1, 1, 4.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1},
        {"SPY_011720C325", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1},
    };
    if( test_option_order(&o, OrderType_NET_DEBIT, COMPLEX, 4.99, legs3, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Vertical_LimitEx("SPY", 1, 17, 2020, 1, 300,
                                                  325, 1, 1, 4.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_LimitEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    printf( "Spread-Vertical-Limit (2) \n");

    if( (err = BuildOrder_Spread_Vertical_Limit("SPY_011720P200",
                "SPY_011720P250", 99, 0, -1.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_Limit (2)");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99},
    };
    if( test_option_order(&o, OrderType_NET_CREDIT, COMPLEX, 1.01, legs4, 2) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "RAW: \n %s \n", buf);

    if( (err = BuildOrder_Spread_Vertical_LimitEx("SPY", 1, 17, 2020, 0, 200,
                                                  250, 99, 0, -1.01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread_Vertical_LimitEx (2)");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "EX: \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    /* manual */
    char *buf3;
    size_t n3;
    OrderTicket_C o3;
    OrderLeg_C legs[] = {{0,0},{0,0}};

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P200",
                          OrderInstruction_BUY_TO_CLOSE, 99, &legs[0]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create");
    }

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P250",
                          OrderInstruction_SELL_TO_CLOSE, 99, &legs[1]);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (2)");
    }

    err = BuildOrder_Spread(ComplexOrderStrategyType_VERTICAL, legs, 2, 0, -1.01, &o3);
    if( err ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderLeg_Destroy(&legs[0]);
        OrderLeg_Destroy(&legs[1]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Spread (Vertical)");
    }
    OrderLeg_Destroy(&legs[0]);
    OrderLeg_Destroy(&legs[1]);

    if( (err = OrderTicket_AsJsonString(&o3, &buf3, &n3)) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        OrderTicket_Destroy(&o3);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (3)" );
    }
    OrderTicket_Destroy(&o3);

    printf( "MANUAL: \n %s \n", buf3);

    if( n2 != n3 || strcmp(buf2, buf3) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        FreeBuffer(buf3);
        fprintf(stderr, "json of ex and manual build orders don't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    FreeBuffer(buf3);
    buf = buf2 = buf3 = NULL;
    /* */

    return 0;
}

int
test_simple_option_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    OrderTicket_C o2 = {0,0};
    char* buf = NULL;
    char* buf2 = NULL;
    size_t n, n2;
    const OrderAssetType ASSET_TYPE = OrderAssetType_OPTION;
    const ComplexOrderStrategyType COMPLEX = ComplexOrderStrategyType_NONE;

    /* MARKET */
    if( (err = BuildOrder_Option_Market("SPY_011720C300", 1, 1, 1, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_Market");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs1[] = {
        {"SPY_011720C300", OrderInstruction_BUY_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs1, 1) ){
        return -1;
    }

    OrderTicket_Destroy(&o);

    printf( "Option-Market-Buy-ToOpen (1): \n %s \n", buf);

    if( (err = BuildOrder_Option_MarketEx("SPY", 1, 17, 2020, 1, 300,
                                          1, 1, 1, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_MarketEx");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "Option-Market-Buy-ToOpen (2): \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    const char* S1 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_OPEN\",\"instrument\":{\"assetType\":\"OPTION\","
        "\"symbol\":\"SPY_011720C300\"},\"quantity\":1}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S1) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw option doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    if( (err = BuildOrder_Option_Market("SPY_011720P300", 99, 0, 0, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_Market (2)");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY_011720P300", OrderInstruction_SELL_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_MARKET, COMPLEX, 0., legs2, 1) ){
        return -1;
    }

    OrderTicket_Destroy(&o);

    printf( "Option-Market-Sell-ToClose (1): \n %s \n", buf);

    if( (err = BuildOrder_Option_MarketEx("SPY", 1, 17, 2020, 0, 300,
                                          99, 0, 0, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_MarketEx (2)");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "Option-Market-Sell-ToClose (2): \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    const char* S2 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_TO_CLOSE\",\"instrument\":{\"assetType\":\"OPTION\","
        "\"symbol\":\"SPY_011720P300\"},\"quantity\":99}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S2) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw option doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    /* LIMIT */

    if( (err = BuildOrder_Option_Limit("SPY_011720C300", 1, 0, 1, -9.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_Limit");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY_011720C300", OrderInstruction_SELL_TO_OPEN, ASSET_TYPE, 1}
    };
    if( test_option_order(&o, OrderType_LIMIT, COMPLEX, -9.99, legs3, 1) ){
        return -1;
    }

    OrderTicket_Destroy(&o);

    printf( "Option-Limit-Sell-ToOpen (1): \n %s \n", buf);

    if( (err = BuildOrder_Option_LimitEx("SPY", 1, 17, 2020, 1, 300,
                                          1, 0, 1, -9.99, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_LimitEx ");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "Option-Limit-Sell-ToOpen (2): \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    const char* S3 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_TO_OPEN\",\"instrument\":{\"assetType\":\"OPTION\","
        "\"symbol\":\"SPY_011720C300\"},\"quantity\":1}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"-9.990000\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S3) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw option doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;
    /* */

    if( (err = BuildOrder_Option_Limit("SPY_011720P200", 99, 1, 0, .01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_Limit (2)");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, ASSET_TYPE, 99}
    };
    if( test_option_order(&o, OrderType_LIMIT, COMPLEX, .01, legs4, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Option-Limit-Buy-ToClose (1): \n %s \n", buf);

    if( (err = BuildOrder_Option_LimitEx("SPY", 1, 17, 2020, 0, 200,
                                          99, 1, 0, .01, &o2)) ){
        FreeBuffer(buf);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Option_LimitEx (2)");
    }

    if( (err = OrderTicket_AsJsonString(&o2, &buf2, &n2)) ){
        FreeBuffer(buf);
        OrderTicket_Destroy(&o2);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString (2)" );
    }
    OrderTicket_Destroy(&o2);

    printf( "Option-Limit-Buy-ToClose (2): \n %s \n", buf2);

    if( n != n2 || strcmp(buf, buf2) ){
        FreeBuffer(buf);
        FreeBuffer(buf2);
        fprintf(stderr, "json of raw and ex build orders don't match \n");
        return -1;
    }

    const char* S4 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_CLOSE\",\"instrument\":{\"assetType\":\"OPTION\","
        "\"symbol\":\"SPY_011720P200\"},\"quantity\":99}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"0.010000\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S4) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw option doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    FreeBuffer(buf2);
    buf = buf2 = NULL;

    return 0;
}

int
test_simple_equity_order_builder()
{
    int err = 0;
    OrderTicket_C o = {0,0};
    char* buf = NULL;
    size_t n;
    const OrderAssetType ASSET_TYPE = OrderAssetType_EQUITY;

    /* MARKET */
    if( (err = BuildOrder_Equity_Market_Buy("SPY", 100, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Market_Buy");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs1[] = {
        {"SPY", OrderInstruction_BUY, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_MARKET, .0, .0, legs1, 1) ){
        return -1;
    }

    OrderTicket_Destroy(&o);

    printf( "Equity-Market-Buy: \n %s \n", buf);

    const char* S1 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S1) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Market_Sell("SPY", 100, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Market_Sell");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs2[] = {
        {"SPY", OrderInstruction_SELL, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_MARKET,  .0, .0, legs2, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Market-Sell: \n %s \n", buf);

    const char* S2 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S2) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Market_Short("SPY", 100, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Market_Short");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs3[] = {
        {"SPY", OrderInstruction_SELL_SHORT, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_MARKET,  .0, .0, legs3, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);


    printf( "Equity-Market-Sell-Short: \n %s \n", buf);

    const char* S3 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_SHORT\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S3) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Market_Cover("SPY", 100, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Market_Cover");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs4[] = {
        {"SPY", OrderInstruction_BUY_TO_COVER, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_MARKET,  .0, .0, legs4, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Market-Cover: \n %s \n", buf);

    const char* S4 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_COVER\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"MARKET\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S4) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    /* LIMIT */
    if( (err = BuildOrder_Equity_Limit_Buy("SPY", 100, 275, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Buy");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs5[] = {
        {"SPY", OrderInstruction_BUY, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_LIMIT,  275, .0, legs5, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Limit-Buy: \n %s \n", buf);

    const char* S5 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"275.000000\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S5) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Limit_Sell("SPY", 100, 274.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Sell");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs6[] = {
        {"SPY", OrderInstruction_SELL, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_LIMIT,  274.99, .0, legs6, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Limit-Sell: \n %s \n", buf);

    const char* S6 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"274.990000\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S6) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Limit_Short("SPY", 100, 275.0001, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Short");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs7[] = {
        {"SPY", OrderInstruction_SELL_SHORT, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_LIMIT,  275.0001, .0, legs7, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Limit-Short: \n %s \n", buf);

    const char* S7 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_SHORT\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"275.000100\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S7) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Limit_Cover("SPY", 100, .01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Limit_Cover");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs8[] = {
        {"SPY", OrderInstruction_BUY_TO_COVER, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_LIMIT,  .01, .0, legs8, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Limit-Cover: \n %s \n", buf);

    const char* S8 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_COVER\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"LIMIT\",\"price\":\"0.010000\",\"session\":\"NORMAL\"}";
    if( strcmp(buf, S8) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    /* STOP */
    if( (err = BuildOrder_Equity_Stop_Buy("SPY", 100, 275, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Buy");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs9[] = {
        {"SPY", OrderInstruction_BUY, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP,  0., 275, legs9, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Stop-Buy: \n %s \n", buf);

    const char* S9 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP\",\"session\":\"NORMAL\",\"stopPrice\":\"275.000000\"}";
    if( strcmp(buf, S9) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Stop_Sell("SPY", 100, 274.99, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Sell");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs10[] = {
        {"SPY", OrderInstruction_SELL, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP,  0., 274.99, legs10, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Stop-Sell: \n %s \n", buf);

    const char* S10 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP\",\"session\":\"NORMAL\",\"stopPrice\":\"274.990000\"}";
    if( strcmp(buf, S10) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Stop_Short("SPY", 100, 275.0001, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Short");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs11[] = {
        {"SPY", OrderInstruction_SELL_SHORT, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP,  0., 275.0001, legs11, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Stop-Short: \n %s \n", buf);

    const char* S11 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_SHORT\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP\",\"session\":\"NORMAL\",\"stopPrice\":\"275.000100\"}";
    if( strcmp(buf, S11) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_Stop_Cover("SPY", 100, .01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_Stop_Cover");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs12[] = {
        {"SPY", OrderInstruction_BUY_TO_COVER, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP,  .0, 0.01, legs12, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-Stop-Cover: \n %s \n", buf);

    const char* S12 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_COVER\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP\",\"session\":\"NORMAL\",\"stopPrice\":\"0.010000\"}";
    if( strcmp(buf, S12) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    /* STOP-LIMIT */
    if( (err = BuildOrder_Equity_StopLimit_Buy("SPY", 100, 275, 276.01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_StopLimit_Buy");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs13[] = {
        {"SPY", OrderInstruction_BUY, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP_LIMIT,  276.01, 275, legs13, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-StopLimit-Buy: \n %s \n", buf);

    const char* S13 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP_LIMIT\",\"price\":\"276.010000\","
        "\"session\":\"NORMAL\",\"stopPrice\":\"275.000000\"}";
    if( strcmp(buf, S13) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_StopLimit_Sell("SPY", 100, 274.99, 274, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_StopLimit_Sell");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs14[] = {
        {"SPY", OrderInstruction_SELL, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP_LIMIT,  274, 274.99, legs14, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-StopLimit-Sell: \n %s \n", buf);

    const char* S14 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP_LIMIT\",\"price\":\"274.000000\","
        "\"session\":\"NORMAL\",\"stopPrice\":\"274.990000\"}";
    if( strcmp(buf, S14) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_StopLimit_Short("SPY", 100, 275.0001, 274.999, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_StopLimit_Short");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs15[] = {
        {"SPY", OrderInstruction_SELL_SHORT, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP_LIMIT,  274.999, 275.0001,
                          legs15, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-StopLimit-Short: \n %s \n", buf);

    const char* S15 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"SELL_SHORT\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP_LIMIT\",\"price\":\"274.999000\","
        "\"session\":\"NORMAL\",\"stopPrice\":\"275.000100\"}";
    if( strcmp(buf, S15) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;

    if( (err = BuildOrder_Equity_StopLimit_Cover("SPY", 100, .01, .01, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_StopLimit_Cover");

    if( (err = OrderTicket_AsJsonString(&o, &buf, &n)) ){
        OrderTicket_Destroy(&o);
        CHECK_AND_RETURN_ON_ERROR(err, "Order_AsJsonString");
    }

    struct TestLeg legs16[] = {
        {"SPY", OrderInstruction_BUY_TO_COVER, ASSET_TYPE, 100}
    };
    if( test_equity_order(&o, OrderType_STOP_LIMIT,  .01, .01, legs16, 1) ){
        return -1;
    }
    OrderTicket_Destroy(&o);

    printf( "Equity-StopLimit-Cover: \n %s \n", buf);

    const char* S16 = "{\"duration\":\"DAY\",\"orderLegCollection\""
        ":[{\"instruction\":\"BUY_TO_COVER\",\"instrument\":{\"assetType\":\"EQUITY\","
        "\"symbol\":\"SPY\"},\"quantity\":100}],\"orderStrategyType\":\"SINGLE\","
        "\"orderType\":\"STOP_LIMIT\",\"price\":\"0.010000\","
        "\"session\":\"NORMAL\",\"stopPrice\":\"0.010000\"}";
    if( strcmp(buf, S16) ){
        FreeBuffer(buf);
        fprintf(stderr, "json of raw equity doesn't match \n");
        return -1;
    }

    FreeBuffer(buf);
    buf = NULL;
    return 0;
}


int test_order_access()
{
    int err = 0;
    char *buf;
    size_t n;
    OrderTicket_C o = {0,0};

    if( (err = BuildOrder_Equity_Market_Buy("SPY", 100, &o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_Equity_MarketBuy");

    struct TestLeg legs[] = {
        {"SPY", OrderInstruction_BUY, OrderAssetType_EQUITY, 100}
    };
    if( test_equity_order(&o, OrderType_MARKET,  .0, .0, legs, 1) ){
        return -1;
    }

    if( (err = OrderTicket_SetSession(&o, OrderSession_PM)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetSession");
    if( (err = OrderTicket_SetDuration(&o, OrderDuration_GOOD_TILL_CANCEL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetDuration");

    if( test_order(&o, OrderDuration_GOOD_TILL_CANCEL, OrderSession_PM,
                          OrderType_MARKET, OrderStrategyType_SINGLE,
                          ComplexOrderStrategyType_NONE,
                          .0, .0, legs, 1) ){
        return -1;
    }

    const char* new_cancel_time = "2019-01-01";
    if( (err = OrderTicket_SetCancelTime(&o, new_cancel_time)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetSession");

    if( (err = OrderTicket_GetCancelTime(&o, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetCancelTime");

    if( strcmp(new_cancel_time, buf) ){
        fprintf(stderr, "new cancel time doesn't match(%s,%s)\n", buf,
                 new_cancel_time);
        FreeBuffer(buf);
        return -1;
    }
    FreeBuffer(buf);

    if( (err = OrderTicket_SetType(&o, OrderType_STOP_LIMIT)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetType");

    if( (err = OrderTicket_SetPrice(&o, 349.9999)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetPrice");

    if( (err = OrderTicket_SetStopPrice(&o, 300.01)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetStopPrice");

    if( test_order(&o, OrderDuration_GOOD_TILL_CANCEL, OrderSession_PM,
                          OrderType_STOP_LIMIT, OrderStrategyType_SINGLE,
                          ComplexOrderStrategyType_NONE,
                          349.9999, 300.01, legs, 1) ){
        return -1;
    }

    OrderLeg_C legs2[] = {{0,0},{0,0}};

    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P200",
                          OrderInstruction_BUY_TO_CLOSE, 99, &legs2[0]);
    if( err ) CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create");


    err = OrderLeg_Create(OrderAssetType_OPTION, "SPY_011720P250",
                          OrderInstruction_SELL_TO_CLOSE, 99, &legs2[1]);
    if( err ){
        OrderLeg_Destroy(&legs2[0]);
        CHECK_AND_RETURN_ON_ERROR(err, "BuildOrder_OrderLeg_Create (2)");
    }

    if( (err = OrderTicket_SetType(&o, OrderType_NET_CREDIT)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetType");

    if( (err = OrderTicket_SetStopPrice(&o, 0.0)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetStopPrice");

    if( (err = OrderTicket_SetPrice(&o, 4.99)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetPrice");

    if( (err = OrderTicket_SetComplexStrategyType(&o,
            ComplexOrderStrategyType_VERTICAL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_SetComplexStrategyTYpe");

    if( (err = OrderTicket_ReplaceLeg(&o, 0, &legs2[0])) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_ReplaceLeg");

    if( (err = OrderTicket_AddLegs(&o, &legs2[1], 1)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_AddLegs");

    OrderLeg_Destroy(&legs2[0]);
    OrderLeg_Destroy(&legs2[1]);

    struct TestLeg legs3[] = {
        {"SPY_011720P200", OrderInstruction_BUY_TO_CLOSE, OrderAssetType_OPTION, 99},
        {"SPY_011720P250", OrderInstruction_SELL_TO_CLOSE, OrderAssetType_OPTION, 99}
    };

    if( test_order(&o, OrderDuration_GOOD_TILL_CANCEL, OrderSession_PM,
                          OrderType_NET_CREDIT, OrderStrategyType_SINGLE,
                          ComplexOrderStrategyType_VERTICAL,
                          4.99, .0, legs3, 2) ){
        return -1;
    }

    if( (err = OrderTicket_Destroy(&o)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_Destroy");

    // TEST OCO/OTO

    return 0;
}


int test_order_leg(size_t nleg, OrderLeg_C* l1, struct TestLeg *l2)
{
    int err = 0;
    char *buf;
    size_t n;

    OrderInstruction oi;
    if( (err = OrderLeg_GetInstruction(l1, &oi)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderLeg_GetInstruction");
    if( oi != l2->instruction ){
        fprintf(stderr, "leg #%zu: invalid instruction(%i,%i) \n", nleg, oi,
                 l2->instruction);
        return -1;
    }

    OrderAssetType oat;
    if( (err = OrderLeg_GetAssetType(l1, &oat)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderLeg_GetAssetType");
    if( oat != l2->asset_type ){
        fprintf(stderr, "leg #%zu: invalid asset type(%i,%i) \n", nleg, oi,
                 l2->instruction);
        return -1;
    }

    size_t quantity;
    if( (err = OrderLeg_GetQuantity(l1, &quantity)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderLeg_GetQuantity");
    if( quantity != l2->quantity ){
        fprintf(stderr, "leg #%zu: invalid quantity(%zu,%zu) \n", nleg, quantity,
                 l2->quantity);
        return -1;
    }

    if( (err = OrderLeg_GetSymbol(l1, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderLeg_GetSymbol");
    if( strcmp(buf, l2->symbol) ){
        fprintf(stderr, "leg #%zu: invalid symbol(%s,%s) \n", nleg, buf,
                 l2->symbol);
        FreeBuffer(buf);
        return -1;
    }

    FreeBuffer(buf);
    return 0;
}



int test_order(OrderTicket_C* porder,
                        OrderDuration duration,
                        OrderSession session,
                        OrderType order_type,
                        OrderStrategyType strategy_type,
                        ComplexOrderStrategyType complex_type,
                        double price,
                        double stop_price ,
                        struct TestLeg* legs,
                        size_t nlegs )
{
    int err = 0;

    OrderDuration d;
    if( (err = OrderTicket_GetDuration(porder, &d)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetDuration");
    if( d != duration ){
        fprintf(stderr, "invalid order duration(%i,%i)\n", d, duration);
        return -1;
    }

    OrderSession s;
    if( (err = OrderTicket_GetSession(porder, &s)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetSession");
    if( s != session ){
        fprintf(stderr, "invalid order session(%i,%i)\n", s, session);
        return -1;
    }

    OrderStrategyType st;
    if( (err = OrderTicket_GetStrategyType(porder, &st)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetStrategyType");
    if( st != strategy_type ){
        fprintf(stderr, "invalid order strategy type(%i,%i)\n", st, strategy_type);
        return -1;
    }

    OrderType ot;
    if( (err = OrderTicket_GetType(porder, &ot)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetType");
    if( ot != order_type ){
        fprintf(stderr, "invalid order type(%i,%i)\n", ot, order_type);
        return -1;
    }

    double p;
    if( (err = OrderTicket_GetPrice(porder, &p)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetPrice");
    if( fabs(p - price) > .00000001 ){
        fprintf(stderr, "invalid price(%f,%f)\n", p, price);
        return -1;
    }

    double sp;
    if( (err = OrderTicket_GetStopPrice(porder, &sp)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetStopPrice");
    if( fabs(sp - stop_price) > .00000001 ){
        fprintf(stderr, "invalid stop price(%f,%f)\n", sp, stop_price);
        return -1;
    }

    size_t n;
    OrderLeg_C *l;
    if( (err = OrderTicket_GetLegs(porder, &l, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrderTicket_GetType");
    if( nlegs != n ){
        fprintf(stderr, "invalid # of legs(%zu,%zu)\n", n, nlegs );
        err = -1;
        goto cleanup_and_exit;
    }

    size_t i;
    for(i = 0; i < n; ++i){
        if( test_order_leg(i, l + i, legs + i) ){
            err = -1;
            goto cleanup_and_exit;
        }
    }

    // TODO child orders

cleanup_and_exit:
    for( i = 0; i < n; ++i ){
        OrderLeg_Destroy(l + i);
    }
    FreeOrderLegBuffer(l);
    return err;
}

int test_option_order(OrderTicket_C* porder,
                        OrderType order_type,
                        ComplexOrderStrategyType complex_type,
                        double price,
                        struct TestLeg *legs,
                        size_t nlegs )
{
    return test_order(porder, OrderDuration_DAY, OrderSession_NORMAL,
                             order_type, OrderStrategyType_SINGLE, complex_type,
                             price, 0., legs, nlegs);
}

int test_equity_order(OrderTicket_C* porder,
                        OrderType order_type,
                        double price,
                        double stop_price,
                        struct TestLeg *legs,
                        size_t nlegs )
{
    return test_order(porder, OrderDuration_DAY, OrderSession_NORMAL,
                             order_type, OrderStrategyType_SINGLE,
                             ComplexOrderStrategyType_NONE,
                             price, stop_price, legs, nlegs);
}






