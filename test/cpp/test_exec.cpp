
#include <iostream>
#include <array>

#include "test.h"

#include "tdma_api_execute.h"

using namespace tdma;
using namespace std;


string
get_exec_obj_title(OrderType ot, OrderInstruction oi, OrderAssetType oa)
{
    return to_string(ot) + "-" + to_string(oi) + "-" + to_string(oa);
}

void
test_simple_exec_obj_json(json j, OrderType ot, OrderInstruction oi,
                             OrderAssetType oa, string os, size_t q,
                             double limit_price=0, double stop_price=0,
                             bool has_children=false,
                             bool is_single = true,
                             bool to_stdout=true)
{
    string s;
    string e = get_exec_obj_title(ot, oi, oa);
    if(to_stdout)
        cout << e << endl << j.dump(4) << endl;

    s = j["duration"];
    if( s != to_string(OrderDuration::DAY) )
        throw runtime_error("invalid order duration(" + s + ") " + e);

    if( is_single){
        s = j["orderStrategyType"];
        if( s != to_string(OrderStrategyType::SINGLE) )
            throw runtime_error("invalid order strategy_type(" + s + ") " + e);
    }

    s = j["orderType"];
    if( s != to_string(ot) )
        throw runtime_error("invalid order type(" + s + ") " + e);

    s = j["session"];
    if( s != to_string(OrderSession::NORMAL) )
        throw runtime_error("invalid order session(" + s + ") " + e);

    size_t sz = j["orderLegCollection"].size();
    if( sz > 1 )
        throw runtime_error("invalid number of legs(" + to_string(sz) + ") " + e);

    if( !has_children ){
        sz = j.count("childOrderStrategies");
        if( sz )
            throw runtime_error("contains child orders(" + to_string(sz) + ") " + e);
    }

    if( limit_price ){
        s = j["price"];
        if( stod(s) != limit_price )
            throw runtime_error("invalid limit price(" + s + ") " + e);
    }

    if( stop_price ){
        s = j["stopPrice"];
        if( stod(s) != stop_price )
            throw runtime_error("invalid stop price(" + s + ") " + e);
    }

    json jj = j["orderLegCollection"][0];
    s = jj["instruction"];
    if( s != to_string(oi))
        throw runtime_error("invalid instruction(" + s + ") " + e);

    sz = jj["quantity"];
    if( sz != q)
        throw runtime_error("invalid quantity(" + to_string(sz) + ") " + e);

    s = jj["instrument"]["assetType"];
    if( s != to_string(oa))
        throw runtime_error("invalid asset type(" + s + ") " + e);

    s = jj["instrument"]["symbol"];
    if( s != os)
        throw runtime_error("invalid symbol(" + s + ") " + e);
};


void
test_simple_exec_equity_market()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Equity;
    OrderType ot = OrderType::MARKET;
    OrderAssetType oa = OrderAssetType::EQUITY;

    order = BUILDER::Build("SPY", 100, true, true);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY, oa, "SPY", 100);

    order = BUILDER::Build("QQQ", 200, false, false);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL, oa, "QQQ", 200);

    order = BUILDER::Build("SPY", 50, false, true);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_SHORT, oa, "SPY",
                              50);

    order = BUILDER::Build("QQQ", 199, true, false);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_COVER, oa,  "QQQ",
                              199);
}

void
test_simple_exec_equity_limit()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Equity;
    OrderType ot = OrderType::LIMIT;
    OrderAssetType oa = OrderAssetType::EQUITY;

    order = BUILDER::Build("SPY", 100, true, true, 275.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY, oa, "SPY", 100);

    order = BUILDER::Build("SPY", 1, false, false, 285.9999);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL, oa, "SPY", 1);

    order = BUILDER::Build("QQQ", 1000, false, true, 150.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_SHORT, oa,  "QQQ",
                              1000);

    order = BUILDER::Build("QQQ", 1000, true, false, 150.01);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_COVER, oa, "QQQ",
                              1000);
}

void
test_simple_exec_equity_stop()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Equity::Stop;
    OrderType ot = OrderType::STOP;
    OrderAssetType oa = OrderAssetType::EQUITY;

    order = BUILDER::Build("SPY", 100, true, true, 275.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY, oa, "SPY", 100,
                              0, 275.00);

    order = BUILDER::Build("SPY", 1, false, false, 285.9999);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL, oa, "SPY", 1,
                              0, 285.9999);

    order = BUILDER::Build("QQQ", 1000, false, true, 150.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_SHORT, oa,  "QQQ",
                              1000, 0, 150.00);

    order = BUILDER::Build("QQQ", 1000, true, false, 150.01);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_COVER, oa, "QQQ",
                              1000, 0, 150.01);
}

void
test_simple_exec_equity_stop_limit()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Equity::Stop;
    OrderType ot = OrderType::STOP_LIMIT;
    OrderAssetType oa = OrderAssetType::EQUITY;

    order = BUILDER::Build("SPY", 100, true, true, 275.00, 275.10);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY, oa, "SPY", 100,
                              275.10, 275.00);

    order = BUILDER::Build("SPY", 1, false, false, 285.9999, 285.9998);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL, oa, "SPY", 1,
                              285.9998, 285.9999);

    order = BUILDER::Build("QQQ", 1000, false, true, 150.00, 150.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_SHORT, oa,  "QQQ",
                              1000, 150.00, 150);

    order = BUILDER::Build("QQQ", 1000, true, false, 150.01, 150.00);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_COVER, oa, "QQQ",
                              1000, 150.00, 150.01);
}

void
test_simple_exec_option_market()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Option;
    OrderType ot = OrderType::MARKET;
    OrderAssetType oa = OrderAssetType::OPTION;
    string sym("SPY_011720C300");

    order = BUILDER::Build(sym, 1, true, true);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_OPEN, oa, sym, 1);
    if( order != BUILDER::Build("SPY",1,17,2020,true,300, 1, true, true) )
        throw runtime_error("BTO option market orders did not match: " + sym);

    order = BUILDER::Build(sym, 1, false, true);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_TO_OPEN, oa, sym, 1);
    if( order != BUILDER::Build("SPY",1,17,2020,true,300, 1, false, true) )
        throw runtime_error("STO option market orders did not match: " + sym);

    order = BUILDER::Build("SPY_011720C300", 1, true, false);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_CLOSE, oa, sym, 1);
    if( order != BUILDER::Build("SPY",1,17,2020,true,300, 1, true, false) )
        throw runtime_error("BTC option market orders did not match: " + sym);

    order = BUILDER::Build(sym, 1, false, false);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_TO_CLOSE, oa, sym, 1);
    if( order != BUILDER::Build("SPY",1,17,2020,true,300, 1, false ,false) )
        throw runtime_error("STC option market orders did not match: " + sym);
}


void
test_simple_exec_option_limit()
{
    json j;
    OrderTicket order;

    using BUILDER = SimpleOrderBuilder::Option;
    OrderType ot = OrderType::LIMIT;
    OrderAssetType oa = OrderAssetType::OPTION;
    string sym("SPY_011720P200");

    order = BUILDER::Build(sym, 999, true, true, .99);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_OPEN, oa, sym, 999);
    if( order != BUILDER::Build("SPY",1,17,2020,false,200, 999, true, true, .99) )
        throw runtime_error("BTO option limit orders did not match: " + sym);

    order = BUILDER::Build(sym, 999, false, true, .99);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_TO_OPEN, oa, sym, 999);
    if( order != BUILDER::Build("SPY",1,17,2020,false,200, 999, false, true, .99) )
        throw runtime_error("STO option limit orders did not match: " + sym);

    order = BUILDER::Build(sym, 999, true, false, .99);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::BUY_TO_CLOSE, oa, sym, 999);
    if( order != BUILDER::Build("SPY",1,17,2020,false,200, 999, true, false, .99) )
        throw runtime_error("BTC option limit orders did not match: " + sym);

    order = BUILDER::Build(sym, 999, false, false, .99);
    j = order.as_json();
    test_simple_exec_obj_json(j, ot, OrderInstruction::SELL_TO_CLOSE, oa, sym, 999);
    if( order != BUILDER::Build("SPY",1,17,2020,false,200, 999, false, false, .99) )
        throw runtime_error("STC option limit orders did not match: " + sym);
}

string
get_exec_obj_title(OrderType ot, ComplexOrderStrategyType cmplx,
                      const vector<OrderLeg>& legs)
{
    stringstream ss;
    ss << to_string(cmplx) << "-" << to_string(ot) << "-(";
    for(auto l : legs){
        ss << to_string(l.get_instruction()) << "/";
    }
    string e = ss.str();
    e = string(e.begin(), e.end()-1);
    e += ")";
    return e;
}

void
test_spread_exec_obj_json(json j, OrderType ot, ComplexOrderStrategyType cmplx,
                             const vector<OrderLeg>& legs, double p = 0,
                             bool has_children=false,
                             bool is_single=true,
                             bool to_stdout=true)

{
    string s;
    string e = get_exec_obj_title(ot, cmplx, legs);
    if(to_stdout)
        cout << e << endl << j.dump(4) << endl;

    s = j["duration"];
    if( s != to_string(OrderDuration::DAY) )
        throw runtime_error("invalid order duration(" + s + ") " + e);

    s = j["complexOrderStrategyType"];
    if( s != to_string(cmplx) )
        throw runtime_error("invalid complex order strategy type(" + s + ") " + e);

    if( is_single ){
        s = j["orderStrategyType"];
        if( s != to_string(OrderStrategyType::SINGLE) )
            throw runtime_error("invalid order strategy_type(" + s + ") + e");
    }

    s = j["orderType"];
    if( s != to_string(ot) )
        throw runtime_error("invalid order type(" + s + ") " + e);

    s = j["session"];
    if( s != to_string(OrderSession::NORMAL) )
        throw runtime_error("invalid order session(" + s + ") " + e);

    if( p ){
        s = j["price"];
        if( stod(s) != p )
            throw runtime_error("invalid limit price(" + s + ") " + e);
    }

    size_t sz = j["orderLegCollection"].size();
    if( sz!= legs.size() )
        throw runtime_error("invalid number of legs(" + to_string(sz) + ") " + e);

    if( !has_children ){
        sz = j.count("childOrderStrategies");
        if( sz )
            throw runtime_error("contains child orders(" + to_string(sz) + ") " + e );
    }

    for(size_t i = 0; i < legs.size(); ++i ){
        json jj = j["orderLegCollection"][i];
        OrderLeg l = legs[i];

        s = jj["instruction"];
        if( s  != to_string(l.get_instruction()))
            throw runtime_error("invalid instruction(" + s + ") - "
                                + to_string(i) + " - " + e);

        sz = jj["quantity"];
        if( sz != l.get_quantity() )
            throw runtime_error("invalid quantity(" + to_string(sz) + ") - "
                                + to_string(i) + " - " + e);

        s = jj["instrument"]["assetType"];
        if( s != to_string(l.get_asset_type()))
            throw runtime_error("invalid asset type(" + s + ") - "
                                 + to_string(i) + " - " + e);

        s = jj["instrument"]["symbol"];
        if( s != l.get_symbol() )
            throw runtime_error("invalid symbol(" + s + ") - "
                                + to_string(i) + " - " + e);
    }
};


void
test_spread_exec_vertical()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::VERTICAL;
    OrderAssetType oa = OrderAssetType::OPTION;
    OrderType ot = OrderType::MARKET;

    using BUILDER = SpreadOrderBuilder::Vertical;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350", 1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, ot, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 350, 1, true);
    if( order != order2 )
        throw runtime_error("vertical open market orders did not match");

    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300", 1, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, ot, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 350, 300, 1, false);
    if( order != order2 )
        throw runtime_error("vertical close market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250", 999, true, 5.99);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_OPEN, 999},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_OPEN, 999}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, 5.99);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 999, true, 5.99);
    if( order != order2 )
        throw runtime_error("vertical open limit orders did not match");

    order = BUILDER::Build("SPY_011720P250", "SPY_011720P200", 999, false, -5.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 999},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 999}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 5.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 250, 200, 999, false, -5.01);
    if( order != order2 )
        throw runtime_error("vertical close limit orders did not match");
}

void
test_spread_exec_vertical_roll()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::VERTICAL_ROLL;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Vertical::Roll;

    /* MARKET */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_032020C375", "SPY_032020C325",1);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_032020C375", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C325", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, true,
                              300, 350, 375, 325, 1);
    if( order != order2 )
        throw runtime_error("NewExp Vertical Roll market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720C375", "SPY_011720C325",1);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 350, 375, 325, 1);
    if( order != order2 )
        throw runtime_error("SameExp Vertical Roll market orders did not match");

    /* LIMIT  */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_032020C375", "SPY_032020C325",1, .0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_032020C375", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C325", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs, .0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, true,
                              300, 350, 375, 325, 1, .0);
    if( order != order2 )
        throw runtime_error("NewExp Vertical Roll limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720C375", "SPY_011720C325",1, -5.55);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 5.55);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 350, 375, 325,
                              1 ,-5.55);
    if( order != order2 )
        throw runtime_error("SameExp Vertical Roll limit orders did not match");
}

void
test_spread_exec_unbalanced_vertical_roll()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx =
        ComplexOrderStrategyType::UNBALANCED_VERTICAL_ROLL;
    OrderAssetType oa = OrderAssetType::OPTION;
    OrderType ot = OrderType::MARKET;

    using BUILDER = SpreadOrderBuilder::Vertical::Roll::Unbalanced;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                         "SPY_032020P275", "SPY_032020P200",3,2);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 3},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_032020P275", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_032020P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020,
                             false, 200, 250, 275, 200, 3, 2);
    if( order != order2)
        throw runtime_error("NewExp Unbalanced Vertical Roll market orders did "
                            "not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                         "SPY_011720P275", "SPY_011720P200",3,2);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 3},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, ot, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 275, 200, 3, 2);
    if( order != order2 )
        throw runtime_error("SameExp Unbalanced Vertical Roll market orders "
                            "did not match");

    /* LIMITS*/
    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                         "SPY_032020P275", "SPY_032020P200",3,2, .9);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 3},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_032020P275", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_032020P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, .9);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020,
                                         false, 200, 250, 275, 200, 3, 2, .9);
    if( order != order2)
        throw runtime_error("NewExp Unbalanced Vertical Roll limit orders did "
                            "not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                         "SPY_011720P275", "SPY_011720P200",3,2, -1.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 3},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 1.0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 275, 200, 3,
                              2, -1.0);
    if( order != order2 )
        throw runtime_error("SameExp Unbalanced Vertical Roll limit orders "
                            "did not match");

}

void
test_spread_exec_butterfly()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::BUTTERFLY;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Butterfly;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", 1, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 2},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1, true, true);
    if( order != order2 )
        throw runtime_error("Butterfly BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                                 "SPY_011720C350", 1, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_CLOSE, 2},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1, false, false);
    if( order != order2 )
        throw runtime_error("Butterfly STC market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 998},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, false, true);
    if( order != order2 )
        throw runtime_error("Butterfly STO market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 998},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, true, false);
    if( order != order2 )
        throw runtime_error("Butterfly BTC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", 1, true, true, 11.99 );
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 2},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 11.99);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1, true, true,11.99);
    if( order != order2 )
        throw runtime_error("Butterfly BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                                 "SPY_011720C350", 1, false, false, -.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_CLOSE, 2},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1,false, false, -.01);
    if( order != order2 )
        throw runtime_error("Butterfly STC limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, false, true, -1.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 998},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 1.0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, false, true, -1.0);
    if( order != order2 )
        throw runtime_error("Butterfly STO limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, true, false, .01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 998},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, true, false, .01);
    if( order != order2 )
        throw runtime_error("Butterfly BTC limit orders did not match");
}


void
test_spread_exec_unbalanced_butterfly()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::UNBALANCED_BUTTERFLY;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Butterfly::Unbalanced;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", 1, 2, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 3},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1, 2, true, true);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                                 "SPY_011720C350", 3, 2, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_CLOSE, 5},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 3,2, false, false);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced STC market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, 500, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 999},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 500}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, 500, false, true);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced STO market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, 1, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 500},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET, cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, 1, true, false);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced BTC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", 1, 2, true, true, 11.99);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 3},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 11.99);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 1, 2, true, true, 11.99);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                                 "SPY_011720C350", 3, 2, false, false, -.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 3},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_CLOSE, 5},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 325, 350, 3, 2, false, false, -.01);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced STC limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, 500, false, true, -1.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 999},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 500}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 1.0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499, 500, false, true, -1.0);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced STO limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P250",
                                "SPY_011720P300", 499, 1, true, false, .01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 500},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, false, 200, 250, 300, 499,1, true, false, .01);
    if( order != order2 )
        throw runtime_error("Butterfly unbalanced BTC limit orders did not match");
}


void
test_spread_exec_backratio()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::BACK_RATIO;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::BackRatio;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300", 2, 1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 350, 300, 2, 1, true);
    if( order != order2 )
        throw runtime_error("BackRatio ToOpen market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350", 1, 2, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 350, 1, 2, false);
    if( order != order2 )
        throw runtime_error("BackRatio ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300", 2, 1, true, -.20);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 350, 300, 2, 1, true, -.20);
    if( order != order2 )
        throw runtime_error("BackRatio ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350", 1, 2, false, .20);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_CLOSE, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, true, 300, 350, 1, 2, false, .20);
    if( order != order2 )
        throw runtime_error("BackRatio ToClose limit orders did not match");
}


void
test_spread_exec_calendar()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::CALENDAR;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Calendar;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_032020C300", 1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C300", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, true, 300, 1, true);
    if( order != order2 )
        throw runtime_error("Calendar ToOpen market orders did not match");

    order = BUILDER::Build("SPY_032020C300", "SPY_011720C300", 499, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020C300", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 3,20, 2020, 1, 17, 2020, true, 300, 499, false);
    if( order != order2 )
        throw runtime_error("Calendar ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_032020P250", "SPY_011720P250", 1, true, -.20);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 3,20, 2020, 1, 17, 2020, false, 250, 1, true, -.20);
    if( order != order2 )
        throw runtime_error("Calendar ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720P250", "SPY_032020P250", 999, false, .20);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 999},
        {oa, "SPY_032020P250", OrderInstruction::SELL_TO_CLOSE, 999}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3,20,2020, false, 250, 999, false, .20);
    if( order != order2 )
        throw runtime_error("Calendar ToClose limit orders did not match");
}


void
test_spread_exec_diagonal()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::DIAGONAL;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Diagonal;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_032020C350", 1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C350", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, true, 300, 350, 1, true);
    if( order != order2 )
        throw runtime_error("Diagonal ToOpen market orders did not match");

    order = BUILDER::Build("SPY_032020C350", "SPY_011720C300", 499, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 3,20, 2020, 1, 17, 2020, true, 350, 300, 499, false);
    if( order != order2 )
        throw runtime_error("Diagonal ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_032020P250", "SPY_011720P300", 1, true, -.20);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 3,20, 2020, 1, 17, 2020, false, 250, 300, 1, true, -.20);
    if( order != order2 )
        throw runtime_error("Diagonal ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720P300", "SPY_032020P250", 999,  false, .20);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 999},
        {oa, "SPY_032020P250", OrderInstruction::SELL_TO_CLOSE, 999}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .20);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3,20,2020, false, 300, 250, 999, false, .20);
    if( order != order2 )
        throw runtime_error("Diagonal ToClose limit orders did not match");
}


void
test_spread_exec_straddle()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::STRADDLE;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Straddle;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720P300", 1, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 1, true, true);
    if( order != order2 )
        throw runtime_error("Straddle BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P300", 1, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 1, false, true);
    if( order != order2 )
        throw runtime_error("Straddle STO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P300", 1, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 1, true, false);
    if( order != order2 )
        throw runtime_error("Straddle BTC market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P300", 1, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 1, false, false);
    if( order != order2 )
        throw runtime_error("Straddle STC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C275", "SPY_011720P275", 499, true, true, 10.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, 10.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 499, true, true, 10.01);
    if( order != order2 )
        throw runtime_error("Straddle BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P275", 499, false, true, -10.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 10.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 499, false, true, -10.01);
    if( order != order2 )
        throw runtime_error("Straddle STO limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P275", 499, true, false, .99);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .99);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 499, true, false, .99);
    if( order != order2 )
        throw runtime_error("Straddle BTC limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P275", 499, false, false, 0.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs, 0.0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 499, false, false, 0.0);
    if( order != order2 )
        throw runtime_error("Straddle STC limit orders did not match");
}


void
test_spread_exec_strangle()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::STRANGLE;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Strangle;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", 1,true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, true, true);
    if( order != order2 )
        throw runtime_error("Strangle BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", 1, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, false, true);
    if( order != order2 )
        throw runtime_error("Strangle STO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", 1, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, true, false);
    if( order != order2 )
        throw runtime_error("Strangle BTC market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", 1, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, false, false);
    if( order != order2 )
        throw runtime_error("Strangle STC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C275", "SPY_011720P250", 499, true, true, 10.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, 10.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 250, 499, true, true, 10.01);
    if( order != order2 )
        throw runtime_error("Strangle BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P250", 499, false, true, -10.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 10.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275, 250, 499, false, true, -10.01);
    if( order != order2 )
        throw runtime_error("Strangle STO limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P250", 499, true, false, .99);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .99);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275,250, 499, true, false, .99);
    if( order != order2 )
        throw runtime_error("Strangle BTC limit orders did not match");

    order = BUILDER::Build("SPY_011720C275", "SPY_011720P250", 499, false, false, 0.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C275", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs, 0.0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 275,250, 499, false, false, 0.0);
    if( order != order2 )
        throw runtime_error("Strangle STC limit orders did not match");
}

void
test_spread_exec_collar_synthetic()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::COLLAR_SYNTHETIC;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::CollarSynthetic;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", 1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, true, true);
    if( order != order2 )
        throw runtime_error("Collar Synthetic ToOpen market orders did not match");

    order = BUILDER::Build("SPY_011720P300", "SPY_011720C300", 1, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 300, 1, false, false);
    if( order != order2 )
        throw runtime_error("Collar Synthetic ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720P200", "SPY_011720C300", 499, true, -.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 499, false, true,  -.01);
    if( order != order2 )
        throw runtime_error("Collar Synthetic ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720C250", "SPY_011720P200", 499, false, .0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C250", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO,cmplx, legs, .0);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 250, 200, 499, true, false, .0);
    if( order != order2 )
        throw runtime_error("Collar Synthetic ToClose limit orders did not match");
}

void
test_spread_exec_collar_with_stock()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx =
        ComplexOrderStrategyType::COLLAR_WITH_STOCK;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::CollarWithStock;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720P200", "SPY_011720C300", "SPY", 1, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 1},
        {OrderAssetType::EQUITY, "SPY", OrderInstruction::BUY, 100}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, true, true);
    if( order != order2 )
        throw runtime_error("Collar With Stock BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720P300", "SPY", 1, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P300", OrderInstruction::SELL_TO_OPEN, 1},
        {OrderAssetType::EQUITY, "SPY", OrderInstruction::SELL_SHORT, 100}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 300, 1, false, true);
    if( order != order2 )
        throw runtime_error("Collar With Stock STO market orders did not match");

    order = BUILDER::Build("SPY_011720P300", "SPY_011720C310", "SPY", 499, true, false);
     j = order.as_json();
     legs = {
         {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 499},
         {oa, "SPY_011720C310", OrderInstruction::SELL_TO_CLOSE, 499},
         {OrderAssetType::EQUITY, "SPY", OrderInstruction::BUY_TO_COVER, 49900}
     };
     test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
     order2 = BUILDER::Build("SPY", 1, 17, 2020, 310, 300, 499, true, false);
     if( order != order2 )
         throw runtime_error("Collar With Stock BTC market orders did not match");

     order = BUILDER::Build("SPY_011720C320", "SPY_011720P300", "SPY", 499, false, false);
     j = order.as_json();
     legs = {
         {oa, "SPY_011720C320", OrderInstruction::BUY_TO_CLOSE, 499},
         {oa, "SPY_011720P300", OrderInstruction::SELL_TO_CLOSE, 499},
         {OrderAssetType::EQUITY, "SPY", OrderInstruction::SELL, 49900}
     };
     test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
     order2 = BUILDER::Build("SPY", 1, 17, 2020, 320, 300, 499, false, false);
     if( order != order2 )
         throw runtime_error("Collar With Stock STC market orders did not match");

     /* LIMITS */
     order = BUILDER::Build("SPY_011720P300", "SPY_011720C350", "SPY",1, true, true, 310.11);
     j = order.as_json();
     legs = {
         {oa, "SPY_011720P300", OrderInstruction::BUY_TO_OPEN, 1},
         {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
         {OrderAssetType::EQUITY, "SPY", OrderInstruction::BUY, 100}
     };
     test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 310.11);
     order2 = BUILDER::Build("SPY", 1, 17, 2020, 350, 300, 1, true, true, 310.11);
     if( order != order2 )
         throw runtime_error("Collar With Stock BTO limit orders did not match");

     order = BUILDER::Build("SPY_011720C300", "SPY_011720P200", "SPY", 1, false, true, -330);
     j = order.as_json();
     legs = {
         {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
         {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 1},
         {OrderAssetType::EQUITY, "SPY", OrderInstruction::SELL_SHORT, 100}
     };
     test_spread_exec_obj_json(j, OrderType::NET_CREDIT, cmplx, legs, 330);
     order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 200, 1, false, true, -330);
     if( order != order2 )
         throw runtime_error("Collar With Stock STO limit orders did not match");

     order = BUILDER::Build("SPY_011720P300", "SPY_011720C300", "SPY", 499, true, false, 299.99);
      j = order.as_json();
      legs = {
          {oa, "SPY_011720P300", OrderInstruction::BUY_TO_CLOSE, 499},
          {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
          {OrderAssetType::EQUITY, "SPY", OrderInstruction::BUY_TO_COVER, 49900}
      };
      test_spread_exec_obj_json(j, OrderType::NET_DEBIT ,cmplx, legs, 299.99);
      order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 300, 499, true, false, 299.99);
      if( order != order2 )
          throw runtime_error("Collar With Stock BTC limit orders did not match");

      order = BUILDER::Build("SPY_011720C400", "SPY_011720P100", "SPY", 499, false, false, 0.);
      j = order.as_json();
      legs = {
          {oa, "SPY_011720C400", OrderInstruction::BUY_TO_CLOSE, 499},
          {oa, "SPY_011720P100", OrderInstruction::SELL_TO_CLOSE, 499},
          {OrderAssetType::EQUITY, "SPY", OrderInstruction::SELL, 49900}
      };
      test_spread_exec_obj_json(j, OrderType::NET_ZERO,cmplx, legs,  0.);
      order2 = BUILDER::Build("SPY", 1, 17, 2020, 400, 100, 499, false, false, 0.);
      if( order != order2 )
          throw runtime_error("Collar With Stock STC limit orders did not match");
}

void
test_spread_exec_condor()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::CONDOR;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Condor;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 1, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 1, true, true);
    if( order != order2 )
        throw runtime_error("Condor BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 499, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C375", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 499, false, true);
    if( order != order2 )
        throw runtime_error("Condor  STO market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                                "SPY_011720P250", "SPY_011720P275", 1, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720P225", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 1, true, false);
    if( order != order2 )
        throw runtime_error("Condor BTC market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                               "SPY_011720P250", "SPY_011720P275", 499, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P225", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 499, false, false);
    if( order != order2 )
        throw runtime_error("Condor STC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 1, true, true, 3.33);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 3.33);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 1, true, true, 3.33);
    if( order != order2 )
        throw runtime_error("Condor BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 499, false, true, -3.33);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C375", OrderInstruction::SELL_TO_OPEN, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 3.33);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 499, false, true, -3.33);
    if( order != order2 )
        throw runtime_error("Condor  STO limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                                "SPY_011720P250", "SPY_011720P275", 1, true, false, .01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720P225", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 1, true, false, .01);
    if( order != order2 )
        throw runtime_error("Condor BTC limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                               "SPY_011720P250", "SPY_011720P275", 499, false, false, 0.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P225", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs, 0.);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 499, false, false, 0.);
    if( order != order2 )
        throw runtime_error("Condor STC limit orders did not match");
}


void
test_spread_exec_unbalanced_condor()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx =
        ComplexOrderStrategyType::UNBALANCED_CONDOR;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::Condor::Unbalanced;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 1,2, true, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 2},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 1, 2, true, true);
    if( order != order2 )
        throw runtime_error("Unbalanced Condor BTO market orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 499, 498, false, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 498},
        {oa, "SPY_011720C375", OrderInstruction::SELL_TO_OPEN, 498}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 499, 498, false, true);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor  STO market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                                "SPY_011720P250", "SPY_011720P275", 10, 1, true, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 10},
        {oa, "SPY_011720P225", OrderInstruction::SELL_TO_CLOSE, 10},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 10, 1, true, false);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor BTC market orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                               "SPY_011720P250", "SPY_011720P275", 499, 998, false, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P225", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 998},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_CLOSE, 998}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 499, 998, false, false);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor STC market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 1, 2, true, true, 3.33);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C325", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 2},
        {oa, "SPY_011720C375", OrderInstruction::BUY_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 3.33);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 1, 2, true, true, 3.33);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor BTO limit orders did not match");

    order = BUILDER::Build("SPY_011720C300", "SPY_011720C325",
                               "SPY_011720C350", "SPY_011720C375", 499, 498, false, true, -3.33);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_OPEN, 499},
        {oa, "SPY_011720C325", OrderInstruction::BUY_TO_OPEN, 499},
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_OPEN, 498},
        {oa, "SPY_011720C375", OrderInstruction::SELL_TO_OPEN, 498}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 3.33);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 325, 350, 375, true, 499, 498, false, true, -3.33);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor  STO limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                                "SPY_011720P250", "SPY_011720P275", 10,1, true, false, .01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 10},
        {oa, "SPY_011720P225", OrderInstruction::SELL_TO_CLOSE, 10},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 1},
        {oa, "SPY_011720P275", OrderInstruction::BUY_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, .01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 10, 1, true, false, .01);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor BTC limit orders did not match");

    order = BUILDER::Build("SPY_011720P200", "SPY_011720P225",
                               "SPY_011720P250", "SPY_011720P275", 499, 498, false, false, 0.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P225", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_CLOSE, 498},
        {oa, "SPY_011720P275", OrderInstruction::SELL_TO_CLOSE, 498}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs, 0.);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 200, 225, 250, 275, false, 499, 498, false, false, 0.);
    if( order != order2 )
        throw runtime_error("Unabalanced Condor STC limit orders did not match");
}

void
test_spread_exec_iron_condor()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::IRON_CONDOR;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::IronCondor;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720P250", "SPY_011720P200",  1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 350, 250, 200, 1, true);
    if( order != order2 )
        throw runtime_error("IronCondor ToOpen market orders did not match");

    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300",
                          "SPY_011720P200", "SPY_011720P250",  499, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 350, 300, 200, 250, 499, false);
    if( order != order2 )
        throw runtime_error("IronCondor ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720P250", "SPY_011720P200",  1, true, 4.44);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, 4.44);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 350, 250, 200, 1, true, 4.44);
    if( order != order2 )
        throw runtime_error("IronCondor ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300",
                          "SPY_011720P200", "SPY_011720P250",  499, false, - 4.43);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 4.43);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 350, 300, 200, 250, 499, false, -4.43);
    if( order != order2 )
        throw runtime_error("IronCondor ToClose limit orders did not match");
}

void
test_spread_exec_unbalanced_iron_condor()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx =
        ComplexOrderStrategyType::UNBALANCED_IRON_CONDOR;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::IronCondor::Unbalanced;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720P250", "SPY_011720P200",  1, 2, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 350, 250, 200, 1, 2,true);
    if( order != order2 )
        throw runtime_error("Unbalanced IronCondor ToOpen market orders did not match");

    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300",
                          "SPY_011720P200", "SPY_011720P250",  499,1, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 1},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 350, 300, 200, 250, 499, 1, false);
    if( order != order2 )
        throw runtime_error("Unbalanced IronCondor ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_011720C350",
                          "SPY_011720P250", "SPY_011720P200",  1, 2, true, 4.44);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_011720C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 2},
        {oa, "SPY_011720P200", OrderInstruction::SELL_TO_OPEN, 2}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT, cmplx, legs, 4.44);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 300, 350, 250, 200, 1, 2, true, 4.44);
    if( order != order2 )
        throw runtime_error("Unbalanced IronCondor ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_011720C350", "SPY_011720C300",
                          "SPY_011720P200", "SPY_011720P250",  499, 499, false, -4.43);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_011720P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_CREDIT,cmplx, legs, 4.43);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 350, 300, 200, 250, 499, 499, false, -4.43);
    if( order != order2 )
        throw runtime_error("Unbalanced IronCondor ToClose limit orders did not match");

}

void
test_spread_exec_double_diagonal()
{
    json j;
    OrderTicket order, order2;
    vector<OrderLeg> legs;
    ComplexOrderStrategyType cmplx = ComplexOrderStrategyType::DOUBLE_DIAGONAL;
    OrderAssetType oa = OrderAssetType::OPTION;

    using BUILDER = SpreadOrderBuilder::DoubleDiagonal;

    /* MARKETS */
    order = BUILDER::Build("SPY_011720C300", "SPY_032020C350",
                          "SPY_011720P250", "SPY_032020P200",  1, true);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, 300, 350, 250, 200, 1, true);
    if( order != order2 )
        throw runtime_error("DoubleDiagonal ToOpen market orders did not match");

    order = BUILDER::Build("SPY_032020C350", "SPY_011720C300",
                           "SPY_032020P200", "SPY_011720P250",  499, false);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_032020P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::MARKET,cmplx, legs);
    order2 = BUILDER::Build("SPY", 3, 20, 2020, 1, 17, 2020, 350, 300, 200, 250, 499, false);
    if( order != order2 )
        throw runtime_error("DoubleDiagonal ToClose market orders did not match");

    /* LIMITS */
    order = BUILDER::Build("SPY_011720C300", "SPY_032020C350",
                          "SPY_011720P250", "SPY_032020P200",  1, true, 10.01);
    j = order.as_json();
    legs = {
        {oa, "SPY_011720C300", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020C350", OrderInstruction::SELL_TO_OPEN, 1},
        {oa, "SPY_011720P250", OrderInstruction::BUY_TO_OPEN, 1},
        {oa, "SPY_032020P200", OrderInstruction::SELL_TO_OPEN, 1}
    };
    test_spread_exec_obj_json(j, OrderType::NET_DEBIT,cmplx, legs, 10.01);
    order2 = BUILDER::Build("SPY", 1, 17, 2020, 3, 20, 2020, 300, 350, 250, 200, 1, true, 10.01);
    if( order != order2 )
        throw runtime_error("DoubleDiagonal ToOpen limit orders did not match");

    order = BUILDER::Build("SPY_032020C350", "SPY_011720C300",
                           "SPY_032020P200", "SPY_011720P250",  499, false, 0.0);
    j = order.as_json();
    legs = {
        {oa, "SPY_032020C350", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720C300", OrderInstruction::SELL_TO_CLOSE, 499},
        {oa, "SPY_032020P200", OrderInstruction::BUY_TO_CLOSE, 499},
        {oa, "SPY_011720P250", OrderInstruction::SELL_TO_CLOSE, 499}
    };
    test_spread_exec_obj_json(j, OrderType::NET_ZERO, cmplx, legs);
    order2 = BUILDER::Build("SPY", 3, 20, 2020, 1, 17, 2020, 350, 300, 200, 250, 499, false, 0.0);
    if( order != order2 )
        throw runtime_error("DoubleDiagonal ToClose limit orders did not match");
}


void
test_oco_exec_obj_json(json j, string e, const vector<OrderTicket>& children)

{
    cout << e << endl << j.dump(4) << endl;

    if( j["orderStrategyType"] != to_string(OrderStrategyType::OCO) )
        throw runtime_error("invalid order strategy_type(" + e + ")");

    if( j.count("childOrderStrategies") != 1 )
        throw runtime_error("doesn't contain child order strategies(" + e + ")");

    if( j["childOrderStrategies"].size() != 2 )
        throw runtime_error("doesn't contain 2 child order strategies(" + e + ")");

    for(size_t i = 0; i < children.size(); ++i ){
        json jj = j["childOrderStrategies"][i];
        OrderTicket c = children[i];
        if( c.get_complex_strategy_type() == ComplexOrderStrategyType::NONE )
        {
            test_simple_exec_obj_json(jj, c.get_type(),
                                      c.get_legs()[0].get_instruction(),
                                      c.get_legs()[0].get_asset_type(),
                                      c.get_legs()[0].get_symbol(),
                                      c.get_legs()[0].get_quantity(),
                                      c.get_price(), c.get_stop_price(),
                                      false, true, false);
        }else{
            test_spread_exec_obj_json(jj, c.get_type(),
                                      c.get_complex_strategy_type(),
                                      c.get_legs(), c.get_price(), false,
                                      true, false);
        }
    }
};

void
test_conditional_exec_oco()
{
    json j;

    OrderTicket order1 =
        SimpleOrderBuilder::Equity::Build("SPY", 100, true, true, 289.91);
    OrderTicket order2 =
        SimpleOrderBuilder::Equity::Stop::Build("SPY", 100, false, true, 282.01);
    OrderTicket order3 = ConditionalOrderBuilder::OCO(order1, order2);
    j = order3.as_json();
    string title("OCO-[[LIMIT-BUY-EQUITY], [STOP-SELL_SHORT-EQUITY]]");
    test_oco_exec_obj_json(j, title, {order1, order2});

    OrderTicket order4 = SpreadOrderBuilder::Vertical::Build("SPY_011720C300",
        "SPY_011720C325", 1, true, 11.99);
    OrderTicket order5 = ConditionalOrderBuilder::OCO(order2, order4);
    j = order5.as_json();
    title = "OCO-[[STOP-SELL_SHORT-EQUITY], "
            "[VERTICAL-NET_DEBIT-(BUY_TO_OPEN/SELL_TO_OPEN)]]";
    test_oco_exec_obj_json(j, title, {order2, order4});
}


void
test_oto_exec_obj_json(json j, string e, OrderTicket& primary, OrderTicket& child)
{
    cout << e << endl << j.dump(4) << endl;

    if( j["orderStrategyType"] != to_string(OrderStrategyType::TRIGGER) )
        throw runtime_error("invalid order strategy_type(" + e + ")");

    if( primary.get_complex_strategy_type() == ComplexOrderStrategyType::NONE ){
        test_simple_exec_obj_json(j, primary.get_type(),
                                  primary.get_legs()[0].get_instruction(),
                                  primary.get_legs()[0].get_asset_type(),
                                  primary.get_legs()[0].get_symbol(),
                                  primary.get_legs()[0].get_quantity(),
                                  primary.get_price(),
                                  primary.get_stop_price(),
                                  true, false, false);
    }else{
        test_spread_exec_obj_json(j, primary.get_type(),
                                  primary.get_complex_strategy_type(),
                                  primary.get_legs(), primary.get_price(),
                                  true, false, false);
    }

    if( j.count("childOrderStrategies") != 1 )
        throw runtime_error("doesn't contain child order strategies(" + e + ")");

    if( j["childOrderStrategies"].size() != 1 )
        throw runtime_error("doesn't contain 1 child order strategies(" + e + ")");

    json jj = j["childOrderStrategies"][0];

    if( child.get_complex_strategy_type() == ComplexOrderStrategyType::NONE )
    {
        test_simple_exec_obj_json(jj, child.get_type(),
                                  child.get_legs()[0].get_instruction(),
                                  child.get_legs()[0].get_asset_type(),
                                  child.get_legs()[0].get_symbol(),
                                  child.get_legs()[0].get_quantity(),
                                  child.get_price(), child.get_stop_price(),
                                  false, true, false);
    }else{
        test_spread_exec_obj_json(jj, child.get_type(),
                                  child.get_complex_strategy_type(),
                                  child.get_legs(), child.get_price(), false,
                                  true, false);
    }

};

void
test_conditional_exec_oto()
{
    json j;

    OrderTicket order1 =
        SimpleOrderBuilder::Equity::Build("SPY", 100, true, true, 289.91);
    OrderTicket order2 =
        SimpleOrderBuilder::Equity::Stop::Build("SPY", 100, false, false, 282.01);
    OrderTicket order3 = ConditionalOrderBuilder::OTO(order1, order2);
    j = order3.as_json();
    string title("OTO-[[LIMIT-BUY-EQUITY], [STOP-SELL-EQUITY]]");
    test_oto_exec_obj_json(j, title, order1, order2);

    order2 = SimpleOrderBuilder::Equity::Stop::Build("SPY", 100, false, true,
                                                     282.01);
    OrderTicket order4 = SpreadOrderBuilder::Vertical::Build("SPY_011720C300",
        "SPY_011720C325", 1, true, 11.99);
    OrderTicket order5 = ConditionalOrderBuilder::OTO(order2, order4);
    j = order5.as_json();
    title = "OTO-[[STOP-SELL_SHORT-EQUITY], "
            "[VERTICAL-NET_DEBIT-(BUY_TO_OPEN/SELL_TO_OPEN)]]";
    test_oto_exec_obj_json(j, title, order2, order4);
}


void
test_execution_order_objects()
{    
    test_simple_exec_equity_market();
    test_simple_exec_equity_limit();
    test_simple_exec_equity_stop();
    test_simple_exec_equity_stop_limit();
    test_simple_exec_option_market();
    test_simple_exec_option_limit();
    test_spread_exec_vertical();
    test_spread_exec_vertical_roll();
    test_spread_exec_unbalanced_vertical_roll();
    test_spread_exec_butterfly();
    test_spread_exec_unbalanced_butterfly();
    test_spread_exec_calendar();
    test_spread_exec_diagonal();
    test_spread_exec_backratio();
    test_spread_exec_straddle();
    test_spread_exec_strangle();
    test_spread_exec_collar_synthetic();
    test_spread_exec_collar_with_stock();
    test_spread_exec_condor();
    test_spread_exec_unbalanced_condor();
    test_spread_exec_iron_condor();
    test_spread_exec_unbalanced_iron_condor();
    test_spread_exec_double_diagonal();
    test_conditional_exec_oco();
    test_conditional_exec_oto();
}

/* LIVE ORDERS! */
void
test_execute_transactions(const std::string& account_id, Credentials& creds)
{
    auto order1 = SimpleOrderBuilder::Equity::Build("XLF", 1, true, true, 1.99);
    /*
    string oid = Execute_SendOrder(creds, account_id, order1);
    std::cout<< "Order ID: " << oid << std::endl;
    bool success = Execute_CancelOrder(creds, account_id, oid);
    std::cout<< "Cancel: " << std::boolalpha << success << std::endl;
    */
}
