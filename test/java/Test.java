/*
Copyright (C) 2019 Jonathon Ogden <jeog.dev@gmail.com>

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

import java.text.SimpleDateFormat;
import java.time.Instant;
import java.time.Year;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.function.Function;
import java.util.function.Supplier;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.Auth;
import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.LibraryNotLoaded;
import io.github.jeog.tdameritradeapi.get.HistoricalGetterBase.FrequencyType;
import io.github.jeog.tdameritradeapi.get.HistoricalPeriodGetter.PeriodType;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.InvalidOptionStrikesType;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionContractType;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionExpMonth;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionRangeType;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionStrikes;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionStrikesType;
import io.github.jeog.tdameritradeapi.get.OptionChainGetter.OptionType;
import io.github.jeog.tdameritradeapi.get.OptionChainStrategyGetter.OptionStrategy;
import io.github.jeog.tdameritradeapi.get.OptionChainStrategyGetter.OptionStrategyType;
import io.github.jeog.tdameritradeapi.get.*;
import io.github.jeog.tdameritradeapi.stream.ActivesSubscriptionBase.DurationType;
import io.github.jeog.tdameritradeapi.stream.OptionActivesSubscription.VenueType;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CallbackType;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.QOSType;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.ServiceType;
import io.github.jeog.tdameritradeapi.stream.*;

public class Test {

    public static void
    main(String[] args) {                    
        int nArgs = args.length;
        if( nArgs != 0 && nArgs != 1 && nArgs != 3 && nArgs != 4 ) {
            System.err.println("Usage:");
            System.err.println("  Test "); 
            System.err.println("    - tests not involving Credentials or Live Connection; look for library on default path");
            System.err.println("  Test <library path>"); 
            System.err.println("    - tests not involving Credentials or Live Connection");
            System.err.println("  Test <library path> <credentials path> <credentials password>");
            System.err.println("    - tests involving Credentials but NOT Live Connection");
            System.err.println("  Test <library path> <credentials path> <credentials password> <account_id>");
            System.err.println("    - tests involving Credentials AND Live Connection");
            return;
        }    
    
        String libPath =  null;
        String accountID = null;
        String credsPath = null;
        String credsPassword = null;
        boolean liveConnect = false;
        
        if( nArgs > 0 )
            libPath = args[0];
        
        if( nArgs > 1 ) {
            credsPath = args[1];
            credsPassword = args[2];    
            if( nArgs == 4 ) {
                liveConnect = true;
                accountID = args[3];
            }
        }
    
        System.out.println( "* START TEST"); 
        System.out.println( "*");
        System.out.println( "*  DateTime: " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()) );
        System.out.println( "*  Library Path: " + (libPath == null ? "N/A" : libPath));
        System.out.println( "*  Credentials Path: " + (credsPath == null ? "N/A" : credsPath));
        System.out.println( "*  Credentials Password: " + (credsPassword == null ? "N/A" : secureString(credsPassword,2)));
        System.out.println( "*  Account ID: " + (accountID == null ? "N/A" : secureString(accountID,3)));
        System.out.println( "*");
        
        if( credsPath == null ) {     
            System.out.println( "*  NO CREDENTIAL CHECKS (pass 'credentials path' and "
                    +"'credentials password' args to allow)");
            System.out.println( "*");            
        }
        if( !liveConnect ) {    
            System.out.println( "*  NO LIVE CONNECTION (pass account ID as arg to allow)");
            System.out.println( "*");
        }
        
        // TODO check args
        
        boolean success = false;
        Credentials creds = null;
        
        try {            
            System.out.println( "*  INIT: " + libPath);
            if( libPath != null ) {
                if ( !TDAmeritradeAPI.init(libPath) ) {
                    throw new Exception("TDAmeritrade.init failed");                                
                }
            }else {
                if( !TDAmeritradeAPI.libraryIsLoaded() )
                    throw new Exception("Library was not loaded automatically(move to resource path, or pass in location)");
            }
            
            System.out.println( "*  TEST OPTION UTILITIES");
            testOptionUtils();    
            
            System.out.println("*  TEST OPTION GETTER OBJECTS");
            testOptionGetterObjects();
                   
            if( credsPath != null) {
                
                System.out.println( "*  TEST CREDENTIAL MANAGER");
                try( Auth.CredentialsManager cm = new Auth.CredentialsManager(credsPath, credsPassword) ){
                    System.out.println( String.format("*   CREDENTIALS(%s)", credsPath) );
                    System.out.println( "*     accessToken: " + secureString(cm.getCredentials().getAccessToken(), 10) );
                    System.out.println( "*     refreshToken: " + secureString(cm.getCredentials().getRefreshToken(), 10) );
                    System.out.println( "*     epochSecExp: " + String.valueOf(cm.getCredentials().getEpochSecTokenExpiration()) );
                    System.out.println( "*     clientID: " + cm.getCredentials().getClientID() );                                                             
                }                
                
                System.out.println( "*  LOAD CREDENTIALS"); 
                creds = Auth.loadCredentials(credsPath, credsPassword);
               
                System.out.println( "*  TEST (RANDOMIZED) CREDENTIALS");
                testCredentials(1000);                               
                
                System.out.println( "*  TEST C QUOTE GETTER ALLOC/DEALLOC");
                testQuoteGetterAllocs(creds, 4, 100);
         
                System.out.println( "*  TEST QUOTE GETTER ");
                testQuoteGetter(creds, liveConnect);
                    
                System.out.println( "*  TEST QUOTES GETTER ");
                testQuotesGetter(creds, liveConnect);
                
                System.out.println( "*  TEST HISTORICAL PERIOD GETTER ");
                testHistoricalPeriodGetter(creds, liveConnect);
                
                System.out.println("*  TEST HISTORICAL RANGE GETTER ");
                testHistoricalRangeGetter(creds, liveConnect);
             
                System.out.println("*  TEST OPTION CHAIN GETTER ");
                testOptionChainGetter(creds, liveConnect);
                
                System.out.println("*  TEST OPTION CHAIN STRATEGY GETTER ");
                testOptionChainStrategyGetter(creds, liveConnect);
                
                System.out.println("*  TEST OPTION CHAIN ANALYTICAL GETTER ");
                testOptionChainAnalyticalGetter(creds, liveConnect);
                
                System.out.println("*  TEST ACCOUNT INFO GETTER ");
                testAccountInfoGetter(creds, accountID, liveConnect);
                
                System.out.println("*  TEST TRANSACTION HISTORY GETTER ");
                testTransactionHistoryGetter(creds, accountID, liveConnect);
                
                System.out.println("*  TEST INDIVIDUAL TRANSACTION HISTORY GETTER ");
                testIndividualTransactionHistoryGetter(creds, accountID, liveConnect);
                
                System.out.println("*  TEST STREAMER SUBSCRIPTION KEYS GETTER ");
                testStreamerSubscriptionKeysGetter(creds, accountID, liveConnect);
                
                System.out.println("*  TEST PREFERENCES GETTER ");
                testPreferencesGetter(creds, accountID, liveConnect);
                
                System.out.println("*  USER PRINCIPALS GETTER ");
                testUserPrincipalsGetter(creds, liveConnect);
                
                System.out.println("*  INSTRUMENT INFO GETTER ");
                testInstrumentInfoGetter(creds,liveConnect);
                
                System.out.println("*  TEST MARKET HOURS GETTER ");
                testMarketHoursGetter(creds,  liveConnect);
           
                System.out.println("*  TEST MOVERS GETTER ");
                testMoversGetter(creds, liveConnect);
                
                System.out.println("*  TEST ORDER GETTER ");
                testOrderGetter(creds, accountID, liveConnect);
                
                System.out.println("*  TEST ORDERS GETTER ");
                testOrdersGetter(creds, accountID, liveConnect);
            }                               
            
            System.out.println("*  TEST HISTORICAL GETTER TYPES ");
            testHistoricalGetterTypes();
                    
            System.out.println("*  TEST STREAMING SUBSCRIPTION FIELDS");
            testSubscriptionFields();
            
            System.out.println("*  TEST STREAMING SUBSCRIPTIONS");
            testRawSubscription();
            testQuotesSubscription();
            testOptionsSubscription();
            testChartEquitySubscription();
            testChartFuturesSubscription();
            testChartOptionsSubscription();
            testNewsHeadlineSubscription();
            testNasdaqActivesSubscription();
            testOptionActivesSubscription();            
          
            System.out.println("*  TEST STREAMING:");
            testStreaming(creds, liveConnect, 5000, 30000);            
         
            success = true;    
            
        } catch (Throwable t) {        
            String msg = String.format("*   ERROR(%s): %s", t.getClass().getName(), t.getMessage());
            System.err.println(msg);
            t.printStackTrace(System.err);
        }
        
        if( creds != null ) {
            try {
                Auth.storeCredentials(credsPath, credsPassword, creds);
            } catch (LibraryNotLoaded | CLibException e) {                    
                System.err.println("*   ERROR: failed to store credentials");
            }
        }
        
        System.out.println("* END TEST - " + (success ? "SUCCESS" : "FAIL"));

                        
    }
    
    // issue calling back into lib ????
    static class StreamingCallback implements StreamingSession.Callback{        
        public void 
        call(int callbackType, int serviceType, long timestamp, String data) {
            ServiceType sType = ServiceType.fromInt(serviceType);
            CallbackType cbType = CallbackType.fromInt(callbackType);
        
            String msg = String.format("[service=%d, callback=%d, timestamp=%d]", 
                    sType.toInt(), cbType.toInt(), timestamp);                        
            System.out.println("CALLBACK " + msg);
            
            if( cbType.equals(CallbackType.DATA) ) 
            {
                try {
                    String d = data.trim();
                    if( d.startsWith("[") ) {
                        JSONArray j = new JSONArray(d);
                        for( int i = 0; i < j.length(); ++i ) {
                            System.out.println( j.getJSONObject(i).toString() );
                        }
                    }else if( d.startsWith("{") ) {
                        JSONObject j = new JSONObject(d);
                        System.out.println( j.toString(4) );
                    }else {
                        System.err.println("* unrecognizable json *");
                    }
                  
                    System.out.println("* * *");
                }catch( JSONException exc ) {
                    System.err.println("JSON exc: " + exc.getMessage());
                    System.err.println("  " + data);
                }
            }
        }        
    }
    
    private static <T extends CLib.ConvertibleEnum> void
    testEnum(T[] vals, List<String> strings, String name) throws Exception{    
        if( strings.size() != vals.length )
            throw new Exception(name + " enum has invalid size");
        
        for( int i = 0; i < vals.length; ++i ) {
            testEnumString(vals[i], strings.get(i), name);             
        }       
        
    }
    

    
    private static void
    testInstrumentInfoGetter(Credentials creds, boolean liveConnect) throws Exception {
        
        testEnum(InstrumentInfoGetter.InstrumentSearchType.values(), 
                Arrays.asList("fundamental", "symbol-search", "symbol-regex", "desc-search",
                        "desc-regex", "cusip"), "InstrumentSearchType");
        
        try( InstrumentInfoGetter getter = new InstrumentInfoGetter( creds,
                InstrumentInfoGetter.InstrumentSearchType.CUSIP, "BAD_CUSIP") ){
            
            InstrumentInfoGetter.InstrumentSearchType searchType = getter.getSearchType();
            if( searchType != InstrumentInfoGetter.InstrumentSearchType.CUSIP )
                throw new Exception("InstrumentInfoGetter 'searchType' doesn't match(1)");
            
            String queryString = getter.getQueryString();
            if( !queryString.equals("BAD_CUSIP") )
                throw new Exception("InstrumentInfoGetter 'queryString' doesn't match(1)");
            
            getter.setQuery(InstrumentInfoGetter.InstrumentSearchType.SYMBOL_EXACT, "SPY");
            
            searchType = getter.getSearchType();
            if( searchType != InstrumentInfoGetter.InstrumentSearchType.SYMBOL_EXACT )
                throw new Exception("InstrumentInfoGetter 'searchType' doesn't match(2)");
                   
            queryString = getter.getQueryString();
            if( !queryString.equals("SPY") )
                throw new Exception("InstrumentInfoGetter 'queryString' doesn't match(2)");
                
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("InstrumentInfoGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }
        
    }
    
    
    private static void
    testMarketHoursGetter(Credentials creds, boolean liveConnect) throws Exception {
     
        testEnum(MarketHoursGetter.MarketType.values(), 
                Arrays.asList("EQUITY", "OPTION","FUTURE", "BOND", "FOREX"), "MarketType");
        
        Calendar c = Calendar.getInstance();        
        int month = c.get(Calendar.MONTH) + 1;
        int year = c.get(Calendar.YEAR);
        int day = c.get(Calendar.DAY_OF_MONTH);
        
        String date;
        if( month > 7 || (month == 7 && day >= 5) ) {
            date = String.format("%d-%02d-%02d", year+1, 7, 5);
        }else {
            date = String.format("%d-%02d-%02d", year, 7, 5);
        }        
        
        try( MarketHoursGetter getter = new MarketHoursGetter(creds, MarketHoursGetter.MarketType.FOREX, date) ){
            
            String date2 = getter.getDate();
            if( !date2.equals(date) )
                throw new Exception("MarketHoursGetter 'date' doesn't match(1)");
            
            MarketHoursGetter.MarketType marketType = getter.getMarketType();
            if( marketType != MarketHoursGetter.MarketType.FOREX )
                throw new Exception("MarketHoursGetter 'marketType' doesn't match(1)");
            
            if( month > 7 || (month == 7 && day >= 4) ) {
                date = String.format("%d-%02d-%02d", year+1, 7, 4);
            }else {
                date = String.format("%d-%02d-%02d", year, 7, 4);
            }    
            
            getter.setDate(date);
            date2 = getter.getDate();
            if( !date2.equals(date) )
                throw new Exception("MarketHoursGetter 'date' doesn't match(2)");
            
            getter.setMarketType(MarketHoursGetter.MarketType.EQUITY);
            marketType = getter.getMarketType();
            if( marketType != MarketHoursGetter.MarketType.EQUITY )
                throw new Exception("MarketHoursGetter 'marketType' doesn't match(2)");
            
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("MarketHoursGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }
    
    }
    
    
    private static void
    testMoversGetter(Credentials creds, boolean liveConnect) throws Exception {
        
        testEnum(MoversGetter.MoversIndexType.values(), Arrays.asList("$COMPX", "$DJI", "$SPX.X"),
                "MoversIndexType");
        
        testEnum(MoversGetter.MoversDirectionType.values(), Arrays.asList("up", "down", "up_and_down"),
                "MoversDirectionType");
        
        testEnum(MoversGetter.MoversChangeType.values(), Arrays.asList("value", "percent"), 
                "MoversChangeType");
        
        try( MoversGetter getter = new MoversGetter(creds, MoversGetter.MoversIndexType.COMPX,
                MoversGetter.MoversDirectionType.UP, MoversGetter.MoversChangeType.VALUE) ){
                        
            MoversGetter.MoversIndexType index = getter.getIndex();
            if( index != MoversGetter.MoversIndexType.COMPX )
                throw new Exception("MoversGetter 'index' doesn't match(1)");
                        
            MoversGetter.MoversDirectionType directionType = getter.getDirectionType();
            if( directionType != MoversGetter.MoversDirectionType.UP )
                throw new Exception("MoversGetter 'directionType' doesn't match(1)");
                        
            MoversGetter.MoversChangeType changeType = getter.getChangeType();
            if( changeType != MoversGetter.MoversChangeType.VALUE )
                throw new Exception("MoversGetter 'changeType' doesn't match(1)");
                        
            getter.setIndex(MoversGetter.MoversIndexType.SPX);
            index = getter.getIndex();
            if( index != MoversGetter.MoversIndexType.SPX )
                throw new Exception("MoversGetter 'index' doesn't match(2)");
                        
            getter.setDirectionType(MoversGetter.MoversDirectionType.UP_AND_DOWN);
            directionType = getter.getDirectionType();
            if( directionType != MoversGetter.MoversDirectionType.UP_AND_DOWN )
                throw new Exception("MoversGetter 'directionType' doesn't match(2)");
                        
            getter.setChangeType(MoversGetter.MoversChangeType.PERCENT);
            changeType = getter.getChangeType();
            if( changeType != MoversGetter.MoversChangeType.PERCENT )
                throw new Exception("MoversGetter 'changeType' doesn't match(2)");
            
            if( liveConnect ) {
                JSONArray j = getter.get();    
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }
    }
    
    
    private static void
    testOrderGetter(Credentials creds, String accountID, boolean liveConnect) throws Exception{
        
        try( OrderGetter getter = new OrderGetter(creds, "BAD_ACCOUNT_ID", "BAD_ORDER_ID") ){
          
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("OrderGetter accountID doesn't match (1)");
            
            String orderID = getter.getOrderID();
            if( !orderID.equals("BAD_ORDER_ID") )
                throw new Exception("OrderGetter 'toEnteredTime' doesn't match(1)");
       
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("OrderGetter accountID doesn't match (2)");
       
            getter.setOrderID("123456789");
            orderID = getter.getOrderID();
            if( !orderID.equals("123456789") )
                throw new Exception("OrderGetter orderID doesn't match (2)");
            
            if( liveConnect ) {
                try {
                    JSONObject j = getter.get();
                    System.out.println("*   JSON: " + j.toString());
                }catch(CLibException exc) {                    
                }                
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }                                   
        }   
    }
    
    
    private static void
    testOrdersGetter(Credentials creds, String accountID, boolean liveConnect) throws Exception{
        
        testEnum(OrdersGetter.OrderStatusType.values(), 
                Arrays.asList( "AWAITING_PARENT_ORDER", "AWAITING_CONDITION", "AWAITING_MANUAL_REVIEW", 
                        "ACCEPTED", "AWAITING_UR_OUT", "PENDING_ACTIVATION", "QUEUED", "WORKING",
                        "REJECTED", "PENDING_CANCEL", "CANCELED", "PENDING_REPLACE", "REPLACED", "FILLED", 
                        "EXPIRED"), "OrderStatusType");
        
        Calendar c = Calendar.getInstance();        
        int month = c.get(Calendar.MONTH) + 1;
        int year = c.get(Calendar.YEAR);
        int day = c.get(Calendar.DAY_OF_MONTH);
        
        String startDate = String.format("%d-%02d-%02d", year-1, month, day);
        String endDate = String.format("%d-%02d-%02d", year, month, day);
        
        try( OrdersGetter getter = new OrdersGetter(creds, "BAD_ACCOUNT_ID", 10, startDate, endDate,
                OrdersGetter.OrderStatusType.EXPIRED) ){
            
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("OrdersGetter accountID doesn't match (1)");
            
            int maxResults = getter.getNMaxResults();
            if( maxResults != 10 )
                throw new Exception("OrdersGetter 'nMaxResults' doesn't match(1)");
            
            String fromTime = getter.getFromEnteredTime();
            if( !fromTime.equals(startDate) )
                throw new Exception("OrdersGetter 'fromEnteredTime' doesn't match(1)");
            
            String toTime = getter.getToEnteredTime();
            if( !toTime.equals(endDate) )
                throw new Exception("OrdersGetter 'toEnteredTime' doesn't match(1)");
            
            OrdersGetter.OrderStatusType orderStatus = getter.getOrderStatusType();
            if( orderStatus != OrdersGetter.OrderStatusType.EXPIRED )
                throw new Exception("OrdersGetter 'OrderStatusType' doesn't match(1)");
            
            if( month <= 4 ) {
                startDate = String.format("%d-%02d-%02d", year-1, month + 8, day);
            }else {
                startDate = String.format("%d-%02d-%02d", year, month - 4, day);
            }
            
            if( month <= 1 ) {
                endDate = String.format("%d-%02d-%02d", year-1, month + 11, day);
            }else {
                endDate = String.format("%d-%02d-%02d", year, month - 1, day);
            }
            
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("OrdersGetter accountID doesn't match (2)");
            
            getter.setNMaxResults(1);
            maxResults = getter.getNMaxResults();
            if( maxResults != 1 )
                throw new Exception("OrdersGetter 'nMaxResults' doesn't match(2)");
            
            getter.setFromEnteredTime(startDate);
            fromTime = getter.getFromEnteredTime();
            if( !fromTime.equals(startDate) )
                throw new Exception("OrdersGetter 'fromEnteredTime' doesn't match(2)");
            
            getter.setToEnteredTime(endDate);
            toTime = getter.getToEnteredTime();
            if( !toTime.equals(endDate) )
                throw new Exception("OrdersGetter 'toEnteredTime' doesn't match(2)");
            
            getter.setOrderStatusType(OrdersGetter.OrderStatusType.FILLED);
            orderStatus = getter.getOrderStatusType();
            if( orderStatus != OrdersGetter.OrderStatusType.FILLED )
                throw new Exception("OrdersGetter 'OrderStatusType' doesn't match(2)");
            
            if( liveConnect ) {
                try {
                    JSONArray j = getter.get();
                    System.out.println("*   JSON: " + j.toString());
                }catch(CLibException exc) {
                    System.out.println("*   NO FILLED ORDERS: " + exc.getMessage());
                }                
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }                                   
        }        
    }
    
    private static void
    testUserPrincipalsGetter(Credentials creds, boolean liveConnect) throws Exception{
        
        try( UserPrincipalsGetter getter = new UserPrincipalsGetter(creds, false, false, false, false) ){
            
            boolean b = getter.returnsSubscriptionKeys();
            if( b )
                throw new Exception("UserPrincipalsGetter 'returnsSubscriptionKeys' doesn't match(1)");
                
            b = getter.returnsConnectionInfo();
            if( b )
                throw new Exception("UserPrincipalsGetter 'returnsConnectionInfo' doesn't match(1)");
            
            b = getter.returnsPreferences();
            if( b )
                throw new Exception("UserPrincipalsGetter 'returnsPreferences' doesn't match(1)");
            
            b = getter.returnsSurrogateIDs();
            if( b )
                throw new Exception("UserPrincipalsGetter 'returnsSurrogateIDs' doesn't match(1)");
            
            getter.returnSubscriptionKeys(true);
            if( b )
                throw new Exception("UserPrincipalsGetter 'returnsSubscriptionKeys' doesn't match(2)");
                
            getter.returnConnectionInfo(true);
            b = getter.returnsConnectionInfo();
            if( !b )
                throw new Exception("UserPrincipalsGetter 'returnsConnectionInfo' doesn't match(2)");
            
            getter.returnPreferences(true);
            b = getter.returnsPreferences();
            if( !b )
                throw new Exception("UserPrincipalsGetter 'returnsPreferences' doesn't match(2)");
            
            getter.returnSurrogateIDs(true);
            b = getter.returnsSurrogateIDs();
            if( !b )
                throw new Exception("UserPrincipalsGetter 'returnsSurrogateIDs' doesn't match(2)");
            
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("UserPrincipalsGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }           
            
        }
    }
   
    private static void
    testPreferencesGetter(Credentials creds, String accountID, boolean liveConnect) 
            throws Exception{
   
        try( PreferencesGetter getter = new PreferencesGetter(creds, "BAD_ACCOUNT_ID") ){ 
                        
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("PreferencesGetter accountID doesn't match (1)");
       
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("PreferencesGetter accountID doesn't match (2)");
  
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("PreferencesGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }                                                                         
    }
    
    private static void
    testStreamerSubscriptionKeysGetter(Credentials creds, String accountID, boolean liveConnect) 
            throws Exception{
   
        try( StreamerSubscriptionKeysGetter getter = new StreamerSubscriptionKeysGetter(creds, "BAD_ACCOUNT_ID") ){ 
                        
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("StreamerSubscriptionKeysGetter accountID doesn't match (1)");
       
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("StreamerSubscriptionKeysGetter accountID doesn't match (2)");
  
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("StreamerSubscriptionKeysGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }                                                                         
    }
    
    private static void
    testIndividualTransactionHistoryGetter(Credentials creds, String accountID, boolean liveConnect) 
            throws Exception{
   
        try( IndividualTransactionHistoryGetter getter = new IndividualTransactionHistoryGetter(creds, 
                "BAD_ACCOUNT_ID", "BAD_TRANSACTION_ID") ){ 
                        
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("IndividualTransactionHistoryGetter accountID doesn't match (1)");
        
            String transactionID = getter.getTransactionID();
            if( !transactionID.equals("BAD_TRANSACTION_ID") )
                throw new Exception("IndividualTransactionHistoryGetter transactionIDdoesn't match (1)");
      
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("IndividualTransactionHistoryGetter accountID doesn't match (2)");
      
            String transactionID2 = "22345678999";
            getter.setTransactionID(transactionID2);
            transactionID = getter.getTransactionID();
            if( !transactionID.equals(transactionID2) )
                throw new Exception("IndividualTransactionHistoryGetter transactionIDdoesn't match (2)");
            
            if( liveConnect ) {
                try {
                    JSONObject j = getter.get();                    
                    System.out.println("*   JSON: " + j.toString());
                }catch( CLibException exc ) {
                        System.out.println("transaction ID not found: " + transactionID2);
                    }
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }                                                                         
    }
    
    private static void
    testTransactionHistoryGetter(Credentials creds, String accountID, boolean liveConnect) throws Exception{
        
        testEnum(TransactionHistoryGetter.TransactionType.values(), 
                Arrays.asList("ALL", "TRADE", "BUY_ONLY", "SELL_ONLY", "CASH_IN_OR_CASH_OUT",
                "CHECKING", "DIVIDEND", "INTEREST", "OTHER", "ADVISOR_FEES"), "TransactionType" );
        
        Calendar c = Calendar.getInstance();        
        int month = c.get(Calendar.MONTH) + 1;
        int year = c.get(Calendar.YEAR);
        int day = c.get(Calendar.DAY_OF_MONTH);
        
        String startDate = String.format("%d-%02d-%02d", year-1, month, day);
        String endDate = String.format("%d-%02d-%02d", year, month, day);
        
        try( TransactionHistoryGetter getter = new TransactionHistoryGetter(creds, "BAD_ACCOUNT_ID", 
                TransactionHistoryGetter.TransactionType.ADVISOR_FEES, "BAD_SYMBOL", startDate, endDate) ){
            
            String symbol = getter.getSymbol();
            if( !symbol.equals("BAD_SYMBOL") )
                throw new Exception("TransactionHistoryGetter symbol doesn't match (1)");
            
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("TransactionHistoryGetter accountID doesn't match (1)");
            
            TransactionHistoryGetter.TransactionType transactionType = getter.getTransactionType();
            if( transactionType != TransactionHistoryGetter.TransactionType.ADVISOR_FEES)
                throw new Exception("TransactionHistoryGetter TransactionType doesn't match (1)");
            
            String startDate2 = getter.getStartDate();
            if( !startDate.equals(startDate2) )
                throw new Exception("TransactionHistoryGetter startDate doesn't match (1)");
            
            String endDate2 = getter.getEndDate();
            if( !endDate.equals(endDate2) )
                throw new Exception("TransactionHistoryGetter endDate doesn't match (1)");
            
            getter.setSymbol("SPY");
            symbol = getter.getSymbol();
            if( !symbol.equals("SPY") )
                throw new Exception("TransactionHistoryGetter symbol doesn't match (2)");
            
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            
            getter.setAccountId(accountID);
            accountID2 = getter.getAccountId();
            if( !accountID2.equals(accountID) )
                throw new Exception("TransactionHistoryGetter accountID doesn't match (2)");
            
            getter.setTransactionType(TransactionHistoryGetter.TransactionType.ALL);
            transactionType = getter.getTransactionType();
            if( transactionType != TransactionHistoryGetter.TransactionType.ALL)
                throw new Exception("TransactionHistoryGetter TransactionType doesn't match (2)");
            
            if( month <= 6 ) {
                startDate = String.format("%d-%02d-%02d", year-1, month + 6, day);
            }else {
                startDate = String.format("%d-%02d-%02d", year, month - 6, day);
            }
       
            if( month <= 3 ) {
                endDate = String.format("%d-%02d-%02d", year-1, month + 9, day);
            }else {
                endDate = String.format("%d-%02d-%02d", year, month - 3, day);
            }
          
            getter.setStartDate(startDate);
            startDate2 = getter.getStartDate();
            if( !startDate.equals(startDate2) )
                throw new Exception("TransactionHistoryGetter startDate doesn't match (2)");
            
            getter.setEndDate(endDate);
            endDate2 = getter.getEndDate();
            if( !endDate.equals(endDate2) )
                throw new Exception("TransactionHistoryGetter endDate doesn't match (2)");
            
            if( liveConnect ) {
                JSONArray j = getter.get();           
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }                                                                  
        
    }
    
    
    private static void
    testAccountInfoGetter(Credentials creds, String accountID, boolean liveConnect ) throws Exception {
        
        try( AccountInfoGetter getter = new AccountInfoGetter(creds, "BAD_ACCOUNT_ID", false, false) ){
            
            String accountID2 = getter.getAccountId();
            if( !accountID2.equals("BAD_ACCOUNT_ID") )
                throw new Exception("AccountInfoGetter accountID doesn't match (1)");
            
            if( getter.returnsPositions() )
                throw new Exception("AccountInfoGetter returnsPositions doesn't match (1)");
            
            if( getter.returnsOrders() )
                throw new Exception("AccountInfoGetter returnsOrders doesn't match (1)");
            
            accountID = (accountID == null) ? "BAD_ACCOUNT_ID2" : accountID;
            
            getter.setAccountId(accountID);
            accountID2= getter.getAccountId();
            if( !accountID.equals(accountID2) )
                throw new Exception("AccountInfoGetter accountID doesn't match (2)");
            
            getter.returnPositions(true);
            if( !getter.returnsPositions() )
                throw new Exception("AccountInfoGetter returnsPositions doesn't match (2)");
            
            getter.returnOrders(true);
            if( !getter.returnsOrders() )
                throw new Exception("AccountInfoGetter returnsOrders doesn't match (2)");           
            
            if( liveConnect ) {
                JSONObject j = getter.get();
                if( j.isEmpty() )
                    throw new Exception("AccountInfoGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }
        
    }
    
    private static void
    testStrategyObj(Function<Double, OptionStrategy> method, OptionStrategyType strategyType, double spread) throws Exception{
        OptionStrategy o1 = method.apply(spread);
        OptionStrategyType type1 = o1.getStrategy();
        if( type1 != strategyType )
            throw new Exception(method.toString() + " OptionStrategy does not have type: " + strategyType.toString());
        double spread1 = o1.getSpreadInterval();
        if( spread1 != spread )
            throw new Exception(method.toString() + " OptionStrategy does not have spread: " + String.valueOf(spread));
         
        OptionStrategy o2 = new OptionStrategy(strategyType, spread);
        if( o2.getStrategy() != type1 || o2.getSpreadInterval() != spread )
            throw new Exception(method.toString() + " OptionStrategy does not match manually constructed object");
    }
    
    private  static void
    testStrategyObj(Supplier<OptionStrategy> method, OptionStrategyType strategyType) throws Exception{
        Function<Double, OptionStrategy> wrapper = new Function<Double,OptionStrategy>(){
            @Override
            public OptionStrategy apply(Double spread){
                return method.get();
            }
        };
        double spread = strategyType == OptionStrategyType.COVERED 
                || strategyType == OptionStrategyType.CALENDAR ? 0 : 1.0;
        testStrategyObj(wrapper, strategyType, spread);      
    }
    
    private static void
    testOptionGetterObjects() throws Exception {
        testEnum(OptionStrikesType.values(), Arrays.asList("n_atm", "single", "range"), 
                "OptionStrikesType");
        
        testEnum(OptionRangeType.values(), Arrays.asList("ITM","NTM","OTM","SAK","SBK","SNK","ALL"),
                "OptionRangeType");
        
        testEnum(OptionContractType.values(), Arrays.asList("CALL","PUT","ALL"), "OptionContractType");
        
        testEnum(OptionExpMonth.values(), Arrays.asList("JAN","FEB","MAR","APR","MAY","JUN","JUL",
                "AUG","SEP","OCT","NOV","DEC","ALL"), "OptionExpMonth");
        
        testEnum(OptionType.values(), Arrays.asList("S","NS","ALL"), "OptionType");
        
        testEnum(OptionStrategyType.values(), Arrays.asList("COVERED","VERTICAL","CALENDAR","STRANGLE",
                "STRADDLE","BUTTERFLY","CONDOR","DIAGONAL","COLLAR","ROLL"), "OptionStrategyType");
        
        OptionStrikes strikes1 = OptionStrikes.buildNAtm(10);
        if( strikes1.getType() != OptionStrikesType.N_ATM )
            throw new Exception("strikes1 != N_ATM");
        if( strikes1.getNAtm() != 10 )
            throw new Exception("strikes1 value != 10");
        try {
            strikes1.getSingle();
            throw new Exception("failed to catch strikes1 InvalidOptionStrikesType");
        }catch(InvalidOptionStrikesType exc) {            
        }
                
        OptionStrikes strikes2 = OptionStrikes.buildRange(OptionRangeType.ITM);
        if( strikes2.getType() != OptionStrikesType.RANGE )
            throw new Exception("strikes2 != RANGE");
        if( strikes2.getRange() != OptionRangeType.ITM )
            throw new Exception("strikes2 value !+ OptionRangeType.ITM");
        try {
            strikes2.getNAtm();
            throw new Exception("failed to ctch strikes2 InvalidOptionStrikesType");
        }catch(InvalidOptionStrikesType exc) {            
        }        
        
        OptionStrikes strikes3 = OptionStrikes.buildSingle(5.55);
        if( strikes3.getType() != OptionStrikesType.SINGLE )
            throw new Exception("strikes3 != SINGLE");
        if( strikes3.getSingle() != 5.55 )
            throw new Exception("strikes3 value != 5.55");
        try {
            strikes3.getRange();
            throw new Exception("failed to ctch strikes3 InvalidOptionStrikesType");
        }catch(InvalidOptionStrikesType exc) {            
        }  
        
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCovered, OptionStrategyType.COVERED);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCalendar, OptionStrategyType.CALENDAR);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildVertical, OptionStrategyType.VERTICAL);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildVertical, OptionStrategyType.VERTICAL, 1.0);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildStrangle, OptionStrategyType.STRANGLE);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildStrangle, OptionStrategyType.STRANGLE, .1);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildStraddle, OptionStrategyType.STRADDLE);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildStraddle, OptionStrategyType.STRADDLE, .001);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildButterfly, OptionStrategyType.BUTTERFLY);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildButterfly, OptionStrategyType.BUTTERFLY, 10.01);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCondor, OptionStrategyType.CONDOR);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCondor, OptionStrategyType.CONDOR, 99999.99);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildDiagonal, OptionStrategyType.DIAGONAL);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildDiagonal, OptionStrategyType.DIAGONAL, 1.0);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCollar, OptionStrategyType.COLLAR);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildCollar, OptionStrategyType.COLLAR, 1.0);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildRoll, OptionStrategyType.ROLL);
        testStrategyObj(OptionChainStrategyGetter.OptionStrategy::buildRoll, OptionStrategyType.ROLL, 1.0);
    }
    
    private static String
    iso8601DateNMonthsAhead(int nMonths, int dayOfMonth) {
        Calendar c = Calendar.getInstance();        
        int month = c.get(Calendar.MONTH) + nMonths;
        int year = c.get(Calendar.YEAR);
                
        if( month > 11 ) {
            int y = month / 12;
            month %= 12;
            year += y;
        }
        month += 1;
        
        return String.format("%d-%02d-%02d", year, month, dayOfMonth); 
    }
    
    private static void
    testOptionChainGetter(Credentials creds, boolean liveConnect) throws Exception {
        
        String fromDate = iso8601DateNMonthsAhead(1, 1);
        String toDate = iso8601DateNMonthsAhead(2, 1);    
        OptionStrikes strikes = OptionStrikes.buildNAtm(5);
        
        try( OptionChainGetter oGetter = new OptionChainGetter(creds, "SPY", strikes, 
                OptionContractType.CALL,
                true, fromDate, toDate, OptionExpMonth.ALL, OptionType.S) ){
            
            String symbol2 = oGetter.getSymbol();
            if( !symbol2.equals("SPY") )
                throw new Exception("OptionChainGetter symbol doesn't match (1)");
            
            OptionStrikes strikes2 = oGetter.getStrikes();
            if( strikes2.getType() != strikes.getType()
                    || strikes2.getNAtm() != strikes.getNAtm() )
                throw new Exception("OptionChainGetter strikes doesn't match (1)");
            
            /* double check we set the union up to be read correctly */
            oGetter.setStrikes(strikes2);
            OptionStrikes strikes3 = oGetter.getStrikes();
            if( strikes3.getType() != OptionStrikesType.N_ATM
                    || strikes3.getNAtm() != 5 )
                throw new Exception("OptionChainGetter strikes doesn't match (1b)");
                            
            OptionContractType contractType2 = oGetter.getContractType();
            if( contractType2 != OptionContractType.CALL )
                throw new Exception("OptionChainGetter contractType doesn't match (1)");
            
            if( !oGetter.includesQuotes() )
                throw new Exception("OptionChainGetter includesQuotes doesn't match (1)");
            
            String fromDate2 = oGetter.getFromDate();
            if( !fromDate2.equals(fromDate) )
                throw new Exception("OptionChainGetter fromDate doesn't match (1)");
    
            String toDate2 = oGetter.getToDate();
            if( !toDate2.equals(toDate) )
                throw new Exception("OptionChainGetter toDate doesn't match (1)");
            
            OptionExpMonth exp2 = oGetter.getExpMonth();
            if( exp2 != OptionExpMonth.ALL )
                throw new Exception("OptionChainGetter expMonth doesn't match (1)");
            
            OptionType otype2 = oGetter.getOptionType();
            if( otype2 != OptionType.S )
                throw new Exception("OptionChainGetter optionType doesn't match (1)");
            
        
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
            
            
            oGetter.setSymbol("QQQ");
            if( !oGetter.getSymbol().equals("QQQ") )
                throw new Exception("OptionChainGetter symbol doesn't match (2)");
        
            strikes = OptionStrikes.buildRange(OptionRangeType.ITM);
            oGetter.setStrikes(strikes);
            if( oGetter.getStrikes().getRange() != OptionRangeType.ITM )
                throw new Exception("OptionChainGetter strikes doesn't match (2)");
            
            oGetter.setContractType(OptionContractType.ALL);
            contractType2 = oGetter.getContractType();
            if( contractType2 != OptionContractType.ALL )
                throw new Exception("OptionChainGetter contractType doesn't match (2)");     
            
            oGetter.includeQuotes(false);
            if( oGetter.includesQuotes() )
                throw new Exception("OptionChainGetter includesQuotes doesn't match (2)");
            
            fromDate = iso8601DateNMonthsAhead(12, 1);
            toDate = iso8601DateNMonthsAhead(24, 1);
            
            oGetter.setFromDate(fromDate);
            if( !oGetter.getFromDate().equals(fromDate) )
                throw new Exception("OptionChainGetter fromDate doesn't match (2)");
            
            oGetter.setToDate(toDate);
            if( !oGetter.getToDate().equals(toDate) )
                throw new Exception("OptionChainGetter toDate doesn't match (2)");
            
            oGetter.setExpMonth(OptionExpMonth.JAN);
            if( oGetter.getExpMonth() != OptionExpMonth.JAN )
                throw new Exception("OptionChainGetter expMonth doesn't match (2)");
            
            oGetter.setOptionType(OptionType.ALL);
            if( oGetter.getOptionType() != OptionType.ALL )
                throw new Exception("OptionChainGetter optionType doesn't match (2)");    
            
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }
 
    }
    
    private static void
    testOptionChainStrategyGetter(Credentials creds, boolean liveConnect) throws Exception {
        
        String fromDate = iso8601DateNMonthsAhead(1, 1);
        String toDate = iso8601DateNMonthsAhead(23, 28);
              
        OptionStrikes strikes = OptionStrikes.buildSingle(200.00);
        OptionStrategy strategy = OptionStrategy.buildCovered();  
        
        try( OptionChainStrategyGetter oGetter = new OptionChainStrategyGetter(creds, "QQQ", 
                strategy, strikes, OptionContractType.ALL, false, fromDate, toDate, 
                OptionExpMonth.DEC, OptionType.ALL) ){
            
            String symbol2 = oGetter.getSymbol();
            if( !symbol2.equals("QQQ") )
                throw new Exception("OptionChainStrategyGetter symbol doesn't match (1)");
            
            OptionStrategy strategy2 = oGetter.getStrategy();
            if( strategy.getStrategy() != strategy2.getStrategy() 
                    || strategy.getSpreadInterval() != strategy2.getSpreadInterval()
                    || strategy.getSpreadInterval() != 0.0 )
                throw new Exception("OptionChainStrategyGetter strategy doesn't match (1)");
            
            OptionStrikes strikes2 = oGetter.getStrikes();
            if( strikes2.getType() != strikes.getType()
                    || strikes2.getSingle() != strikes.getSingle() )
                throw new Exception("OptionChainStrategyGetter strikes doesn't match (1)");
    
            OptionContractType contractType2 = oGetter.getContractType();
            if( contractType2 != OptionContractType.ALL )
                throw new Exception("OptionChainStrategyGetter contractType doesn't match (1)");
            
            if( oGetter.includesQuotes() )
                throw new Exception("OptionChainStrategyGetter includesQuotes doesn't match (1)");
            
            String fromDate2 = oGetter.getFromDate();
            if( !fromDate2.equals(fromDate) )
                throw new Exception("OptionChainStrategyGetter fromDate doesn't match (1)");
    
            String toDate2 = oGetter.getToDate();
            if( !toDate2.equals(toDate) )
                throw new Exception("OptionChainStrategyGetter toDate doesn't match (1)");
            
            OptionExpMonth exp2 = oGetter.getExpMonth();
            if( exp2 != OptionExpMonth.DEC )
                throw new Exception("OptionChainStrategyGetter expMonth doesn't match (1)");
            
            OptionType otype2 = oGetter.getOptionType();
            if( otype2 != OptionType.ALL )
                throw new Exception("OptionChainStrategyGetter optionType doesn't match (1)");
                    
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainStrategyGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
                        
            oGetter.setSymbol("GLD");
            if( !oGetter.getSymbol().equals("GLD") )
                throw new Exception("OptionChainStrategyGetter symbol doesn't match (2)");
        
            strategy2 = OptionStrategy.buildButterfly();
            oGetter.setStrategy(strategy2);
            strategy = oGetter.getStrategy();
            if( strategy.getStrategy() != strategy2.getStrategy() 
                    || strategy.getSpreadInterval() != strategy2.getSpreadInterval()
                    || strategy.getSpreadInterval() != 1.0 )
                throw new Exception("OptionChainStrategyGetter strategy doesn't match (2a)");
            
            strategy2 = OptionStrategy.buildDiagonal(2.5);
            oGetter.setStrategy(strategy2);
            strategy = oGetter.getStrategy();
            if( strategy.getStrategy() != strategy2.getStrategy() 
                    || strategy.getSpreadInterval() != strategy2.getSpreadInterval()
                    || strategy.getSpreadInterval() != 2.5 )
                throw new Exception("OptionChainStrategyGetter strategy doesn't match (2b)");      
            
            strategy2 = OptionStrategy.buildVertical(1.0);
            oGetter.setStrategy(strategy2);
            strategy = oGetter.getStrategy();
            if( strategy.getStrategy() != strategy2.getStrategy() 
                    || strategy.getSpreadInterval() != strategy2.getSpreadInterval()
                    || strategy.getSpreadInterval() != 1.0 )
                throw new Exception("OptionChainStrategyGetter strategy doesn't match (2c)");  
            
            strikes = OptionStrikes.buildRange(OptionRangeType.NTM);
            oGetter.setStrikes(strikes);
            if( oGetter.getStrikes().getRange() != OptionRangeType.NTM )
                throw new Exception("OptionChainStrategyGetter strikes doesn't match (2)");
            
            oGetter.setContractType(OptionContractType.PUT);
            contractType2 = oGetter.getContractType();
            if( contractType2 != OptionContractType.PUT )
                throw new Exception("OptionChainStrategyGetter contractType doesn't match (2)");     
            
            oGetter.includeQuotes(false);
            if( oGetter.includesQuotes() )
                throw new Exception("OptionChainStrategyGetter includesQuotes doesn't match (2)");
            
            fromDate = iso8601DateNMonthsAhead(1, 1);
            toDate = iso8601DateNMonthsAhead(1, 28);
            
            oGetter.setFromDate(fromDate);
            if( !oGetter.getFromDate().equals(fromDate) )
                throw new Exception("OptionChainStrategyGetter fromDate doesn't match (2)");
            
            oGetter.setToDate(toDate);
            if( !oGetter.getToDate().equals(toDate) )
                throw new Exception("OptionChainStrategyGetter toDate doesn't match (2)");
            
            oGetter.setExpMonth(OptionExpMonth.ALL);
            if( oGetter.getExpMonth() != OptionExpMonth.ALL )
                throw new Exception("OptionChainStrategyGetter expMonth doesn't match (2)");
            
            oGetter.setOptionType(OptionType.NS);
            if( oGetter.getOptionType() != OptionType.NS )
                throw new Exception("OptionChainStrategyGetter optionType doesn't match (2)");    
            
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainStrategyGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }
        }          
    }
    
    private static void
    testOptionChainAnalyticalGetter(Credentials creds, boolean liveConnect) throws Exception {
        String fromDate = iso8601DateNMonthsAhead(1, 14);
        String toDate = iso8601DateNMonthsAhead(1, 22);
              
        OptionStrikes strikes = OptionStrikes.buildRange(OptionRangeType.SNK);          
        
        try( OptionChainAnalyticalGetter oGetter = new OptionChainAnalyticalGetter(creds, "QQQ", 
                20.00, 200.00, 2.50, 90, strikes, OptionContractType.ALL, false, fromDate, toDate, 
                OptionExpMonth.ALL, OptionType.ALL) ){
            
            String symbol2 = oGetter.getSymbol(); 
            if( !symbol2.equals("QQQ") )
                throw new Exception("OptionChainAnalyticalGetter symbol doesn't match (1)");
            
            double v = oGetter.getVolatility();
            if( v != 20.00 )
                throw new Exception("OptionChainAnalyticalGetter volatility doesn't match (1)");
            
            v = oGetter.getUnderlyingPrice();
            if( v != 200.00 )
                throw new Exception("OptionChainAnalyticalGetter underlyingPrice doesn't match (1)");               
                
            v = oGetter.getInterestRate();
            if( v != 2.5 )
                throw new Exception("OptionChainAnalyticalGetter interestRate doesn't match (1)");     
            
            int d = oGetter.getDaysToExp();
            if( d != 90 )
                throw new Exception("OptionChainAnalyticalGetter daysToExp doesn't match (1)");
            
            OptionStrikes strikes2 = oGetter.getStrikes();
            if( strikes2.getType() != strikes.getType()
                    || strikes2.getRange() != strikes.getRange() )
                throw new Exception("OptionChainAnalyticalGetter strikes doesn't match (1)");
    
            OptionContractType contractType2 = oGetter.getContractType();
            if( contractType2 != OptionContractType.ALL )
                throw new Exception("OptionChainAnalyticalGetter contractType doesn't match (1)");
            
            if( oGetter.includesQuotes() )
                throw new Exception("OptionChainAnalyticalGetter includesQuotes doesn't match (1)");
            
            String fromDate2 = oGetter.getFromDate();
            if( !fromDate2.equals(fromDate) )
                throw new Exception("OptionChainAnalyticalGetter fromDate doesn't match (1)");
    
            String toDate2 = oGetter.getToDate();
            if( !toDate2.equals(toDate) )
                throw new Exception("OptionChainAnalyticalGetter toDate doesn't match (1)");
            
            OptionExpMonth exp2 = oGetter.getExpMonth();
            if( exp2 != OptionExpMonth.ALL )
                throw new Exception("OptionChainAnalyticalGetter expMonth doesn't match (1)");
            
            OptionType otype2 = oGetter.getOptionType();
            if( otype2 != OptionType.ALL )
                throw new Exception("OptionChainAnalyticalGetter optionType doesn't match (1)");
                    
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainAnalyticalGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }       
            
            oGetter.setVolatility(30.33);
            v = oGetter.getVolatility();
            if( v != 30.33 )
                throw new Exception("OptionChainAnalyticalGetter volatility doesn't match (2)");
            
            oGetter.setUnderlyingPrice(100.01);
            v = oGetter.getUnderlyingPrice();
            if( v != 100.01 )
                throw new Exception("OptionChainAnalyticalGetter underlyingPrice doesn't match (2)"); 
            
            oGetter.setInterestRate(9.999);
            v = oGetter.getInterestRate();
            if( v != 9.999 )
                throw new Exception("OptionChainAnalyticalGetter interestRate doesn't match (2)");   
            
            oGetter.setDaysToExp(720);      
            d = oGetter.getDaysToExp();
            if( d != 720 )
                throw new Exception("OptionChainAnalyticalGetter daysToExp doesn't match (2)");
            
            if( liveConnect ) {
                JSONObject j = oGetter.get();
                if( j.isEmpty() )
                    throw new Exception("OptionChainAnalyticalGetter.get() returned empty string");
                System.out.println("*   JSON: " + j.toString());
            }else {
                System.out.println("*   Can't get(), pass 'account_id' to run live");
            }       
        }    
    }
    
    
    private static <T extends CLib.ConvertibleEnum> void
    testEnumString(T e, String s, String name) throws Exception{
        if( !e.toString().equals(s) )
            throw new Exception(
                    String.format("%s enum string doesn't match(%s,%s)", name, e.toString(),s) );
    }
    
    private static void
    testHistoricalGetterTypes() throws Exception {
        testEnum( HistoricalPeriodGetter.PeriodType.values(),
                Arrays.asList("day", "month","year","ytd"), "PeriodType");
   
        testEnum( HistoricalRangeGetter.FrequencyType.values(),
                Arrays.asList("minute","daily","weekly","monthly"), "FrequencyType");
      
        if( !HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,1) 
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,5)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,10) 
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.MONTH,1)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.MONTH,6)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,1)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,10)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,20)
                || !HistoricalPeriodGetter.isValidPeriod(PeriodType.YTD,1)
              ) 
        {
            throw new Exception("HistoricalPeriodGetter.isValidPeriod not allow valid entries");
        }
        
        if( HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,6) 
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,-1)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.DAY,20) 
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.MONTH,5)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.MONTH,10)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,0)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,4)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.YEAR,30)
                || HistoricalPeriodGetter.isValidPeriod(PeriodType.YTD,3)
              ) 
        {
            throw new Exception("HistoricalPeriodGetter.isValidPeriod allowing invalid entries");
        } 
                
       if( !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.DAY,FrequencyType.MINUTE) 
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.MONTH,FrequencyType.DAILY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.MONTH,FrequencyType.WEEKLY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YEAR,FrequencyType.DAILY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YEAR,FrequencyType.WEEKLY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YEAR,FrequencyType.MONTHLY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YTD,FrequencyType.DAILY)
                || !HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YTD,FrequencyType.WEEKLY)
         
              ) 
        {
            throw new Exception("HistoricalPeriodGetter.isValidFrequencyType not allowing valid entries");
        }
    
       if( HistoricalPeriodGetter.isValidFrequencyType(PeriodType.DAY,FrequencyType.DAILY)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.DAY,FrequencyType.WEEKLY)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.DAY,FrequencyType.MONTHLY)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.MONTH,FrequencyType.MINUTE)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.MONTH,FrequencyType.MONTHLY)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YEAR,FrequencyType.MINUTE)   
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YTD,FrequencyType.MONTHLY)
               || HistoricalPeriodGetter.isValidFrequencyType(PeriodType.YTD,FrequencyType.MINUTE)
        
             ) 
       {
           throw new Exception("HistoricalPeriodGetter.isValidFrequencyType allowing invalid entries");
       }      
       
       if( !HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,1) 
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,5)
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,10) 
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,30) 
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.DAILY,1)
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.WEEKLY,1)
               || !HistoricalRangeGetter.isValidFrequency(FrequencyType.MONTHLY,1)                       
             ) 
       {
           throw new Exception("HistoricalRangeGetter.isValidFrequency not allow valid entries");
       }
       
       if( HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,0) 
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,2)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,3)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MINUTE,20)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.DAILY,0)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.DAILY,3)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.DAILY,5)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.DAILY,10)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.WEEKLY,0)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.WEEKLY,3)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.WEEKLY,5)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.WEEKLY,10)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MONTHLY,0)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MONTHLY,3)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MONTHLY,5)
               || HistoricalRangeGetter.isValidFrequency(FrequencyType.MONTHLY,10)
             ) 
       {
           throw new Exception("HistoricalRangeGetter.isValidFrequency allowing invalid entries");
       } 
        
    }
    
    
    @SuppressWarnings("resource")
    private static void
    testHistoricalPeriodGetter(Credentials creds, boolean liveConnect) throws Exception {
     
        List<Long> MSECs = new ArrayList<Long>();
        MSECs.add((long)0); MSECs.add(Long.MAX_VALUE);
        
        HistoricalPeriodGetter hpg = null;
        
        for( PeriodType pType : PeriodType.values() ) {
            
            for( int period : HistoricalPeriodGetter.VALID_PERIODS_BY_PERIOD_TYPE.get(pType) ) {
                
                for( FrequencyType fType : HistoricalPeriodGetter.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.get(pType) ) {
                    
                    for( int frequency : HistoricalPeriodGetter.VALID_FREQUENCIES_BY_FREQUENCY_TYPE.get(fType) ) {
                        
                        for( boolean extHours : Arrays.asList(true,false) ) {
                            
                            for( long msec : MSECs) {
                                
                                for( String symbol : Arrays.asList("SPY","QQQ") ) {
                                  
                                    hpg = new HistoricalPeriodGetter(creds, symbol, pType, period, fType, frequency,
                                            extHours, msec);
                                 
                                    if( !hpg.getSymbol().equals(symbol) )
                                        throw new Exception("HISTORICAL PERIOD invalid symbol");
                                    
                                    if( hpg.getFrequency() != (frequency) )
                                        throw new Exception("HISTORICAL PERIOD invalid frequency");
                                        
                                    if( !hpg.getFrequencyType().equals(fType) )
                                        throw new Exception("HISTORICAL PERIOD invalid frequency type");
                                    
                                    if( hpg.isExtendedHours() != extHours )
                                        throw new Exception("HISTORICAL PERIOD invalid extended hours");
                                    
                                    if( hpg.getPeriod() != period )
                                        throw new Exception("HISTORICAL PERIOD invalid period");
                                        
                                    if( !hpg.getPeriodType().equals(pType) )
                                        throw new Exception("HISTORICAL PERIOD invalid period type");
                                    
                                    if( hpg.getMSecSinceEpoch() != msec )
                                        throw new Exception("HISTORICAL PERIOD invalid msec since epoch");
                                        
                                    hpg = null;
                                    System.gc();
                                }
                            }
                        }                                            
                    }                                                        
                }
            }                    
        }
        
        for( PeriodType pType : PeriodType.values() ) {
            
            for( int period : HistoricalPeriodGetter.VALID_PERIODS_BY_PERIOD_TYPE.get(pType) ) {
                
                for( FrequencyType fType : HistoricalPeriodGetter.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.get(pType) ) {
                    
                    for( int frequency : HistoricalPeriodGetter.VALID_FREQUENCIES_BY_FREQUENCY_TYPE.get(fType) ) {
                        
                        for( boolean extHours : Arrays.asList(true,false) ) {
                            
                            for( long msec : MSECs) { 
                                
                                for( String symbol : Arrays.asList("SPY","QQQ") ) {
                                  
                                    if( hpg == null ) {
                                        hpg = new HistoricalPeriodGetter(creds, symbol, pType, period, fType, frequency,
                                                extHours, msec);
                                    }else {
                                        hpg.setPeriod(pType, period);
                                        hpg.setFrequency(fType, frequency);
                                        hpg.setExtendedHours(extHours);
                                        hpg.setMSecSinceEpoch(msec);
                                        hpg.setSymbol(symbol);
                                    }
                                 
                                    if( !hpg.getSymbol().equals(symbol) )
                                        throw new Exception("HISTORICAL PERIOD invalid symbol");
                                    
                                    if( hpg.getFrequency() != (frequency) )
                                        throw new Exception("HISTORICAL PERIOD invalid frequency"); 
                                        
                                    if( !hpg.getFrequencyType().equals(fType) )
                                        throw new Exception("HISTORICAL PERIOD invalid frequency type");
                                    
                                    if( hpg.isExtendedHours() != extHours )
                                        throw new Exception("HISTORICAL PERIOD invalid extended hours");
                                    
                                    if( hpg.getPeriod() != period )
                                        throw new Exception("HISTORICAL PERIOD invalid period");
                                        
                                    if( !hpg.getPeriodType().equals(pType) )
                                        throw new Exception("HISTORICAL PERIOD invalid period type");
                                    
                                    if( hpg.getMSecSinceEpoch() != msec )
                                        throw new Exception("HISTORICAL PERIOD invalid msec since epoch");
                                   
                                }
                            }
                        }                                           
                    }                                                       
                }
            }                   
        }        
        
        hpg.setFrequency(FrequencyType.WEEKLY, 1);
        hpg.setPeriod(PeriodType.YTD, 1);
        FrequencyType freq = hpg.getFrequencyType();
        PeriodType per = hpg.getPeriodType();
        // last period type YTD, last frequency type WEEKLY
        if( !freq.equals(FrequencyType.WEEKLY) || !per.equals(PeriodType.YTD))
            throw new Exception("HISTORICAL PERIOD tests not in expected state("
                    + freq.toString() + "," + per.toString() + ")");              
   
        try {
            hpg.setFrequency(FrequencyType.DAILY, 5);
            throw new Exception("HISTORICAL PERIOD failed to catch bad frequency");
        }catch(CLibException exc) {}
                
        try {
            hpg.setPeriod(PeriodType.DAY, 20);
            throw new Exception("HISTORICAL PERIOD failed catch bad periodType");
        }catch(CLibException exc) {}
        
        try {
            hpg.setPeriod(PeriodType.MONTH, 5);
            throw new Exception("HISTORICAL PERIOD failed to catch bad period");
        }catch(CLibException exc) {}
                
                
        if( liveConnect ) {
            JSONObject j = hpg.get();
            if( j.isEmpty() )
                throw new Exception("HistoricalPeriod.get() returned empty string");
            System.out.println("*   JSON: " + j.toString(4));
        }else {
            System.out.println("*   Can't get(), pass 'account_id' to run live");
        }
        
        hpg.close();
        if( !hpg.isClosed() )
            throw new Exception("HistoricalPeriodGetter wasn't closed");
    }
    
    @SuppressWarnings("resource")
    private static void
    testHistoricalRangeGetter(Credentials creds, boolean liveConnect) throws Exception {

        List<Long> startMSECs = new ArrayList<Long>();
        startMSECs.add((long)0); startMSECs.add((long)100); startMSECs.add((long)1000000);
        
        List<Long> endMSECs = new ArrayList<Long>();
        endMSECs.add((long)1000001); endMSECs.add((long)10000000); startMSECs.add((long)Long.MAX_VALUE);
        
        
        
        HistoricalRangeGetter hpg = null;

        for( FrequencyType fType : HistoricalRangeGetter.FrequencyType.values() ) {
            
            for( int frequency : HistoricalRangeGetter.VALID_FREQUENCIES_BY_FREQUENCY_TYPE.get(fType) ) {
                
                for( boolean extHours : Arrays.asList(true,false) ) {
                    
                    for( long startMsec : startMSECs) {
                        
                        for( long endMsec : endMSECs) {
                        
                            for( String symbol : Arrays.asList("SPY","QQQ") ) {
                              
                                hpg = new HistoricalRangeGetter(creds, symbol, fType, frequency,
                                        startMsec, endMsec ,extHours);
                             
                                if( !hpg.getSymbol().equals(symbol) )
                                    throw new Exception("HISTORICAL RANGE invalid symbol");
                                
                                if( hpg.getFrequency() != (frequency) )
                                    throw new Exception("HISTORICAL RANGE invalid frequency");
                                    
                                if( !hpg.getFrequencyType().equals(fType) )
                                    throw new Exception("HISTORICAL RANGE invalid frequency type");
                                
                                if( hpg.isExtendedHours() != extHours )
                                    throw new Exception("HISTORICAL RANGE invalid extended hours");
                                                                   
                                if( hpg.getStartMSecSinceEpoch() != startMsec )
                                    throw new Exception("HISTORICAL RANGE invalid start msec since epoch");
                                
                                if( hpg.getEndMSecSinceEpoch() != endMsec )
                                    throw new Exception("HISTORICAL RANGE invalid end msec since epoch");
                                            
                                hpg = null;
                                System.gc();
                            }
                        }
                    }
                }
            }
        }

     
        for( FrequencyType fType : HistoricalRangeGetter.FrequencyType.values() ) {
            
            for( int frequency : HistoricalRangeGetter.VALID_FREQUENCIES_BY_FREQUENCY_TYPE.get(fType) ) {
                
                for( boolean extHours : Arrays.asList(true,false) ) {
                    
                    for( long startMsec : startMSECs) {
                        
                        for( long endMsec : endMSECs) {
                        
                            for( String symbol : Arrays.asList("SPY","QQQ") ) {
                          
                                if( hpg == null ) {
                                    hpg = new HistoricalRangeGetter(creds, symbol, fType, frequency,
                                            startMsec, endMsec ,extHours);
                                }else {                                     
                                    hpg.setFrequency(fType, frequency);
                                    hpg.setExtendedHours(extHours);
                                    hpg.setStartMSecSinceEpoch(startMsec);
                                    hpg.setEndMSecSinceEpoch(endMsec);
                                    hpg.setSymbol(symbol);
                                }
                             
                                if( !hpg.getSymbol().equals(symbol) )
                                    throw new Exception("HISTORICAL RANGE invalid symbol");
                                
                                if( hpg.getFrequency() != (frequency) )
                                    throw new Exception("HISTORICAL RANGE invalid frequency"); 
                                    
                                if( !hpg.getFrequencyType().equals(fType) )
                                    throw new Exception("HISTORICAL RANGE invalid frequency type");
                                
                                if( hpg.isExtendedHours() != extHours )
                                    throw new Exception("HISTORICAL RANGE invalid extended hours");
                             
                                if( hpg.getStartMSecSinceEpoch() != startMsec )
                                    throw new Exception("HISTORICAL RANGE invalid start msec since epoch");
                                
                                if( hpg.getEndMSecSinceEpoch() != endMsec )
                                    throw new Exception("HISTORICAL RANGE invalid end msec since epoch");
                            }
                        }
                    }
                }                                           
            }                                                       
        }
   
        try {
            hpg.setSymbol("");
            throw new Exception("HISTORICAL RANGE failed to catch empty symbol");
        }catch(CLibException exc) {}
         
        long utcToday = Instant.now().toEpochMilli();
        long utc5daysAgo = utcToday - 5*(24 * 60 * 60 * 1000); 
        
        hpg.setFrequency(FrequencyType.MINUTE, 30);
        hpg.setStartMSecSinceEpoch(utc5daysAgo);
        hpg.setEndMSecSinceEpoch(utcToday);
        hpg.setExtendedHours(false);
                
        if( liveConnect ) {
            JSONObject j = hpg.get();
            if( j.isEmpty() )
                throw new Exception("HistoricalRange.get() returned empty string");
            System.out.println("*   JSON: " + j.toString(4));
        }else {
            System.out.println("*   Can't get(), pass 'account_id' to run live");
        }
        
        hpg.close();
        if( !hpg.isClosed() )
            throw new Exception("HistoricalRangeGetter wasn't closed");
    
    
    }
    
    private static void
    testSubscriptionFields() throws Exception {
        testEnum(ServiceType.values(), Arrays.asList("NONE",  "QUOTE", "OPTION", "LEVELONE_FUTURES", 
                "LEVELONE_FOREX", "LEVELONE_FUTURES_OPTIONS", "NEWS_HEADLINE", "CHART_EQUITY", 
                "CHART_FOREX",  "CHART_FUTURES", "CHART_OPTIONS",  "TIMESALE_EQUITY", 
                "TIMESALE_FOREX",  "TIMESALE_FUTURES", "TIMESALE_OPTIONS", "ACTIVES_NASDAQ", 
                "ACTIVES_NYSE", "ACTIVES_OTCBB", "ACTIVES_OPTIONS",  "ADMIN",  "ACCT_ACTIVITY",  
                "CHART_HISTORY_FUTURES",  "FOREX_BOOK",  "FUTURES_BOOK",  "LISTED_BOOK",  
                "NASDAQ_BOOK",  "OPTIONS_BOOK",  "FUTURES_OPTIONS_BOOK",  "NEWS_STORY",  
                "NEWS_HEADLINE_LIST",  "UNKNOWN"), "ServiceType");
       
        testEnum(CommandType.values(), Arrays.asList("SUBS", "UNSUBS", "ADD", "VIEW"), "CommandType"); 
  
        testEnum(QOSType.values(), Arrays.asList("express","real-time", "fast", "moderate", "slow", "delayed"),
                "QOSTYpe");
        
        testEnum(CallbackType.values(), Arrays.asList("listening_start", "listening_stop", "data", 
                "request_response", "notify", "timeout", "error"), "CallbackType"); 
              
        testEnumString(QuotesSubscription.FieldType.SYMBOL, "QuotesSubscriptionField-0", "QuotesSubscription");
        testEnumString(QuotesSubscription.FieldType.REGULAR_MARKET_TRADE_TIME_AS_LONG, "QuotesSubscriptionField-52", "QuotesSubscription");

        testEnumString(OptionsSubscription.FieldType.SYMBOL, "OptionsSubscriptionField-0", "OptionsSubscription");
        testEnumString(OptionsSubscription.FieldType.MARK, "OptionsSubscriptionField-41", "OptionsSubscription");

        testEnumString(LevelOneFuturesSubscription.FieldType.SYMBOL, "LevelOneFuturesSubscriptionField-0", "LevelOneFuturesSubscription");
        testEnumString(LevelOneFuturesSubscription.FieldType.FUTURE_EXPIRATION_DATE, "LevelOneFuturesSubscriptionField-35", "LevelOneFuturesSubscription");

        testEnumString(LevelOneForexSubscription.FieldType.SYMBOL, "LevelOneForexSubscriptionField-0", "LevelOneForexSubscription");
        testEnumString(LevelOneForexSubscription.FieldType.MARK, "LevelOneForexSubscriptionField-29", "LevelOneForexSubscription");

        testEnumString(LevelOneFuturesOptionsSubscription.FieldType.SYMBOL, "LevelOneFuturesOptionsSubscriptionField-0", "LevelOneFuturesOptionsSubscription");
        testEnumString(LevelOneFuturesOptionsSubscription.FieldType.FUTURE_EXPIRATION_DATE, "LevelOneFuturesOptionsSubscriptionField-35", "LevelOneFuturesOptionsSubscription");

        testEnumString(ChartEquitySubscription.FieldType.SYMBOL, "ChartEquitySubscriptionField-0", "ChartEquitySubscription");
        testEnumString(ChartEquitySubscription.FieldType.CHART_DAY, "ChartEquitySubscriptionField-8", "ChartEquitySubscription");

        testEnumString(ChartFuturesSubscription.FieldType.SYMBOL, "ChartSubscriptionField-0", "ChartFuturesSubscription");
        testEnumString(ChartFuturesSubscription.FieldType.VOLUME, "ChartSubscriptionField-6", "ChartFuturesSubscription");

        testEnumString(ChartOptionsSubscription.FieldType.CHART_TIME, "ChartSubscriptionField-1", "ChartOptionsSubscription");
        testEnumString(ChartOptionsSubscription.FieldType.CLOSE_PRICE, "ChartSubscriptionField-5", "ChartOptionsSubscription");

        testEnumString(TimesaleEquitySubscription.FieldType.SYMBOL, "TimesaleSubscriptionField-0", "TimesaleEquitySubscription");
        testEnumString(TimesaleFuturesSubscription.FieldType.LAST_SIZE, "TimesaleSubscriptionField-3", "TimesaleFuturesSubscription");
        testEnumString(TimesaleOptionsSubscription.FieldType.LAST_SEQUENCE, "TimesaleSubscriptionField-4", "TimesaleOptionsSubscription");

        testEnumString(NewsHeadlineSubscription.FieldType.SYMBOL, "NewsHeadlineSubscriptionField-0", "NewsHeadlineSubscription");
        testEnumString(NewsHeadlineSubscription.FieldType.STORY_SOURCE, "NewsHeadlineSubscriptionField-10", "NewsHeadlineSubscription");

        testEnumString(NasdaqActivesSubscription.DurationType.ALL_DAY, "ALL", "DurationType");
        testEnumString(NasdaqActivesSubscription.DurationType.MIN_1, "60", "DurationType");

        testEnumString(OptionActivesSubscription.VenueType.OPTS, "OPTS", "VenueType");
        testEnumString(OptionActivesSubscription.VenueType.PUTS_DESC, "PUTS-DESC", "VenueType");
    }
    
    
    private static <F extends CLib.ConvertibleEnum> void
    displayTestSymbolFieldInfo( Set<String> s, Set<F> f, CommandType cmd, ServiceType srv, String name) {
        StringBuilder sb = new StringBuilder();
        sb.append("*    TestSubscription: ").append(name).append(", Service: ")
          .append(srv.toString()).append(", Command: ").append(cmd.toString())
          .append(", Symbols: [");
        
        if( s.size() > 0 ) {
            for(String ss : s)
                sb.append(ss+",");     
            sb.setLength(sb.length()-1);
        }        
        sb.append("], Fields: [");
        
        if( f.size() > 0 ) {
            for(F ff : f)
                sb.append(ff.toString() + ",");
            sb.setLength(sb.length()-1);
        }        
        sb.append("]");
        System.out.println(sb.toString());
    }
    
    
    private static <F extends CLib.ConvertibleEnum> Set<Integer>
    fieldsToRaw( Set<F> f ){
        Set<Integer> fields = new HashSet<Integer>();
        for( F ff : f )
            fields.add( ff.toInt() );
        return fields;
    }
    
    
    private static <T extends SubscriptionBySymbolBase, F extends CLib.ConvertibleEnum> void
    testSymbolFieldSub(T sub, Set<String> s1, Set<String> s2, Set<F> f1, Set<F> f2,
            CommandType cmd1, CommandType cmd2, ServiceType srv, String name ) throws Exception {
     
        displayTestSymbolFieldInfo(s1, f1, cmd1, srv, name);
        
        String srvName = srv.toString();
        if( !srvName.equals(name) )
            throw new Exception(
                    String.format("service strings don't match [%s,%s]", srvName, name) );
                    
        Set<String> symbols = sub.getSymbols();
        if( !symbols.equals(s1) )
            throw new Exception(name + " sub symbols don't match (1)");
               
        sub.setSymbols(s2);
        symbols = sub.getSymbols();
        if( !symbols.equals(s2) )
            throw new Exception(name + " sub symbols don't match (2)");
        
        Set<Integer> rFields1 = fieldsToRaw(f1);
        Set<Integer> rFields = sub.getRawFields();
        if( !rFields.equals(rFields1) )
            throw new Exception(name + " sub fields don't match (1)");
        
        Set<Integer> rFields2 = fieldsToRaw(f2);
        sub.setRawFields(rFields2);
        rFields = sub.getRawFields();
        if( !rFields.equals(rFields2) )
            throw new Exception(name + " sub fields don't match (2)");       
 
        
        CommandType c = sub.getCommand();
        if( !c.equals(cmd1) )        
            throw new Exception(name + " sub command doesn't match (1)");
        
        sub.setCommand(cmd2);
        c = sub.getCommand(); 
        if( !c.equals(cmd2) )        
            throw new Exception(name + " sub command doesn't match (2)");           
        
    }
    
    private static void
    testNasdaqActivesSubscription() throws Exception{        
        DurationType dur1 = DurationType.ALL_DAY;
        NasdaqActivesSubscription sub = new NasdaqActivesSubscription(dur1);
                
        StringBuilder sb = new StringBuilder();
        sb.append("*    TestSubscription: ").append("ACTIVES_NASDAQ").append(", Service: ")
          .append(ServiceType.ACTIVES_NASDAQ.toString()).append(", Command: ")
          .append(CommandType.SUBS.toString()).append(", Duration: ")
          .append(dur1).append("]");
        System.out.println(sb.toString());        
        
        ServiceType srv = sub.getService();
        CommandType c2 = sub.getCommand();
        DurationType dur2 = sub.getDuration();
        
        if( !srv.equals(ServiceType.ACTIVES_NASDAQ))
            throw new Exception("ACTIVES_NASDAQ has invalid service type ");
                
        if( !dur1.equals(dur2) )
            throw new Exception("ACTIVES_NASDAQ durations don't match (1)");
                
        if( !c2.equals(CommandType.SUBS) )
            throw new Exception("ACTIVES_NASDAQ commands(" + c2.toString() + ") don't match (1)");
        
        for( DurationType d : DurationType.values() ) {
            for( CommandType c : CommandType.values() ) {
                sub.setDuration(d);
                sub.setCommand(c);
                dur2 = sub.getDuration();
                c2 = sub.getCommand();
                if( !dur2.equals(d) )
                    throw new Exception("ACTIVES_NASDAQ durations(" + d.toString() + ") don't match (2)");
                if( !c2.equals(c) )
                    throw new Exception("ACTIVES_NASDAQ commands(" + c.toString() + ") don't match (2)");
            }
        }

        if( DurationType.fromInt(6) != null )      
            throw new Exception("DurationType built from invalid int");
                 
    }
       
    private static void
    testOptionActivesSubscription() throws Exception {
        DurationType dur1 = DurationType.ALL_DAY;
        VenueType ven1 = VenueType.CALLS;
        OptionActivesSubscription sub = new OptionActivesSubscription(ven1, dur1);
        
        StringBuilder sb = new StringBuilder();
        sb.append("*    TestSubscription: ").append("ACTIVES_OPTION").append(", Service: ")
          .append(ServiceType.ACTIVES_OPTIONS.toString()).append(", Command: ")
          .append(CommandType.SUBS.toString()).append(", Duration: ")
          .append(dur1.toString()).append(", Venue: ").append(ven1.toString()).append("]");
        System.out.println(sb.toString());        
        
        ServiceType srv = sub.getService();
        CommandType c2 = sub.getCommand();
        DurationType dur2 = sub.getDuration();
        VenueType ven2 = sub.getVenue();
        
        if( !srv.equals(ServiceType.ACTIVES_OPTIONS))
            throw new Exception("ACTIVES_OPTION has invalid service type ");
        
        if( !ven1.equals(ven2) )
            throw new Exception("ACTIVES_OPTION durations don't match (1");
 
        if( !dur1.equals(dur2) )
            throw new Exception("ACTIVES_OPTION durations don't match (1)");
      
        if( !c2.equals(CommandType.SUBS) )
            throw new Exception("ACTIVES_OPTION commands(" + c2.toString() + ") don't match (1)");
        
        for( VenueType v : VenueType.values() ) {
            for( DurationType d : DurationType.values() ) {
                for( CommandType c : CommandType.values() ) {
                    sub.setVenue(v);
                    sub.setDuration(d);
                    sub.setCommand(c);
                    dur2 = sub.getDuration();
                    c2 = sub.getCommand();
                    ven2 = sub.getVenue();
                    if( !dur2.equals(d) )
                        throw new Exception("ACTIVES_OPTION durations(" + d.toString() + ")don't match (2)");
                    if( !c2.equals(c) )
                        throw new Exception("ACTIVES_OPTION commands(" + c.toString() + ") don't match (2)");
                    if( !ven2.equals(v) )
                        throw new Exception("ACTIVES_OPTION venue(" + v.toString() + ") dont't match (2)");
                }
            }
        }

        if( VenueType.fromInt(6) != null )
            throw new Exception("VenueTYpe build from invalid int");
      
    }
    
    
    private static void
    testNewsHeadlineSubscription() throws Exception {
        Set<String> symbols = new HashSet<String>( Arrays.asList("MSFT", "INTC") );
        Set<NewsHeadlineSubscription.FieldType> qFields = new HashSet<NewsHeadlineSubscription.FieldType>();
        
        try {        
            new NewsHeadlineSubscription( symbols, qFields, CommandType.ADD );
            throw new Exception("NEWS_HEADLINE allow ADD with no fields");
        }catch( CLibException exc ) {}
        
        qFields.add( NewsHeadlineSubscription.FieldType.fromInt(1) );     
        NewsHeadlineSubscription qSub = new NewsHeadlineSubscription( symbols, qFields, CommandType.ADD );
        
        Set<String> symbols2 = new HashSet<String>( Arrays.asList("AMZN") ) ;
        Set<NewsHeadlineSubscription.FieldType> qFields2 = 
                NewsHeadlineSubscription.FieldType.ALL;
        
        testSymbolFieldSub(qSub, symbols, symbols2, qFields, qFields2, CommandType.ADD,
                CommandType.SUBS, ServiceType.NEWS_HEADLINE, "NEWS_HEADLINE");
        
        try {
            qSub.setFields( NewsHeadlineSubscription.FieldType.buildSet(11) );
            throw new Exception("NEWS_HEADLINE sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            qSub.setRawFields( new HashSet<Integer>(Arrays.asList(11)) );
            throw new Exception("NEWS_HEADLINE sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}
        
        for( CommandType c : CommandType.values() ) {
            CommandType cc = new NewsHeadlineSubscription( symbols, qFields, c).getCommand();
            if( !cc.equals(c) ) {
                String msg =String.format("NEWS_HEADLINE sub command construction resulted in bad command (%s,%s)", c, cc);
                    throw new Exception(msg);            
            }
        }                        
    }
    

    private static void
    testChartOptionsSubscription() throws Exception {
        Set<String> symbols = new HashSet<String>( Arrays.asList("OPTION1", "OPTION2", "OPTION3") );
        Set<ChartOptionsSubscription.FieldType> qFields = new HashSet<ChartOptionsSubscription.FieldType>();
        
        try {        
            new ChartOptionsSubscription( symbols, qFields, CommandType.ADD );
            throw new Exception(" CHART_OPTIONS allow ADD with no fields");
        }catch( CLibException exc ) {}
        
        qFields.add( ChartOptionsSubscription.FieldType.fromInt(1) );     
        ChartOptionsSubscription qSub = new ChartOptionsSubscription( symbols, qFields, CommandType.ADD );
        
        Set<String> symbols2 = new HashSet<String>( Arrays.asList("OPTION4") ) ;
        Set<ChartOptionsSubscription.FieldType> qFields2 = 
                ChartOptionsSubscription.FieldType.ALL;
        
        testSymbolFieldSub(qSub, symbols, symbols2, qFields, qFields2, CommandType.ADD,
                CommandType.SUBS, ServiceType.CHART_OPTIONS, "CHART_OPTIONS");
        
        try {
            qSub.setFields( ChartOptionsSubscription.FieldType.buildSet(7) );
            throw new Exception("CHART_OPTIONS sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            qSub.setRawFields( new HashSet<Integer>(Arrays.asList(7)) );
            throw new Exception("CHART_OPTIONS sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}
        
        for( CommandType c : CommandType.values() ) {
            CommandType cc = new ChartOptionsSubscription( symbols, qFields, c).getCommand();
            if( !cc.equals(c) ) {
                String msg =String.format("CHART_OPTIONS sub command construction resulted in bad command (%s,%s)", c, cc);
                    throw new Exception(msg);            
            }
        }                   
    }
       
    private static void
    testChartFuturesSubscription() throws Exception {
        Set<String> symbols = new HashSet<String>( );
        Set<ChartFuturesSubscription.FieldType> qFields = ChartFuturesSubscription.FieldType.ALL;
        ChartFuturesSubscription qSub = new ChartFuturesSubscription( symbols, qFields, CommandType.VIEW );
        
        Set<String> symbols2 = new HashSet<String>( Arrays.asList("/ES", "/NQ", "/YM") );
        Set<ChartFuturesSubscription.FieldType> qFields2 = 
                ChartFuturesSubscription.FieldType.buildSet(2,3,4);
        
        testSymbolFieldSub(qSub, symbols, symbols2, qFields, qFields2, CommandType.VIEW,
                CommandType.SUBS, ServiceType.CHART_FUTURES, "CHART_FUTURES");
        
        try {
            qSub.setFields( ChartFuturesSubscription.FieldType.buildSet(-1) );
            throw new Exception("CHART_FUTURES sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            qSub.setRawFields( new HashSet<Integer>(Arrays.asList(-1)) );
            throw new Exception("CHART_FUTURES sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}
        
        try {
            new ChartFuturesSubscription( symbols );
            throw new Exception("CHART_FUTURES allowed SUBS with no symbols");
        }catch(CLibException exc) {}
        
        for( CommandType c : CommandType.values() ) {
            CommandType cc = new ChartFuturesSubscription( symbols2, qFields, c).getCommand();
            if( !cc.equals(c) ) {
                String msg =String.format("CHART_FUTURES sub command construction resulted in bad command (%s,%s)", c, cc);
                throw new Exception(msg);            
            }
        }                   
    }
    
    private static void
    testChartEquitySubscription() throws Exception {
        Set<String> symbols = new HashSet<String>( Arrays.asList("SPY","QQQ") );
        Set<ChartEquitySubscription.FieldType> qFields = 
                ChartEquitySubscription.FieldType.buildSet(0);
        ChartEquitySubscription qSub = new ChartEquitySubscription( symbols, qFields, CommandType.VIEW );
        
        Set<String> symbols2 = new HashSet<String>( Arrays.asList("AAPL") );
        Set<ChartEquitySubscription.FieldType> qFields2 = 
                ChartEquitySubscription.FieldType.buildSet(7,8);
        
        testSymbolFieldSub(qSub, symbols, symbols2, qFields, qFields2, CommandType.VIEW,
                CommandType.ADD, ServiceType.CHART_EQUITY, "CHART_EQUITY");
        
        try {
            qSub.setFields( ChartEquitySubscription.FieldType.buildSet(7,8,9) );
            throw new Exception("CHART_EQUITY sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            qSub.setRawFields( new HashSet<Integer>(Arrays.asList(1,9)) );
            throw new Exception("CHART_EQUITY sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}
        
        for( CommandType c : CommandType.values() ) {
            CommandType cc = new ChartEquitySubscription( symbols, qFields, c).getCommand();
            if( !cc.equals(c) ) {
                String msg =String.format("CHART_EQUITY sub command construction resulted in bad command (%s,%s)", c, cc);
                throw new Exception(msg);            
            }
        }                        
    }
    
    private static void
    testOptionsSubscription() throws Exception {
        String op1 = buildFutureOptionSymbol("SPY", 1,1,true,300);        
        Set<String> symbols = new HashSet<String>( Arrays.asList(op1) );
        Set<OptionsSubscription.FieldType> fields = 
                OptionsSubscription.FieldType.buildSet(OptionsSubscription.FieldType.BID_PRICE);
        OptionsSubscription oSub = new OptionsSubscription(symbols, fields, CommandType.VIEW);
        
        String op2 = buildFutureOptionSymbol("QQQ", 12,0,false,300);
        String op3 = buildFutureOptionSymbol("QQQ", 12,1,true,305);
        Set<String> symbols2 = new HashSet<String>( Arrays.asList(op2,op3) );
        Set<OptionsSubscription.FieldType> fields2 = 
                OptionsSubscription.FieldType.buildSet( 
                        OptionsSubscription.FieldType.BID_PRICE.toInt(),
                        OptionsSubscription.FieldType.ASK_PRICE.toInt() );
        
        testSymbolFieldSub(oSub, symbols, symbols2, fields, fields2, CommandType.VIEW,
                CommandType.ADD, ServiceType.OPTION, "OPTION");
                
        try {
            oSub.setFields( OptionsSubscription.FieldType.buildSet(43) );
            throw new Exception("OPTION sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            oSub.setRawFields( new HashSet<Integer>(Arrays.asList(43)) );
            throw new Exception("OPTION sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}                   
        
    }
    
    
    private static void
    testQuotesSubscription() throws Exception {        
        Set<String> symbols = new HashSet<String>( Arrays.asList("SPY","QQQ") );
        Set<QuotesSubscription.FieldType> qFields = 
                QuotesSubscription.FieldType.buildSet(1,2,3); 
        QuotesSubscription qSub = new QuotesSubscription( symbols, qFields );
        
        Set<String> symbols2 = new HashSet<String>( Arrays.asList("SPY","QQQ", "IWM") );
        Set<QuotesSubscription.FieldType> qFields2 = 
                QuotesSubscription.FieldType.buildSet(3,4,5);
        
        testSymbolFieldSub(qSub, symbols, symbols2, qFields, qFields2, CommandType.SUBS,
                CommandType.UNSUBS, ServiceType.QUOTE, "QUOTE");
        
        try {
            qSub.setFields( QuotesSubscription.FieldType.buildSet(53) );
            throw new Exception("QUOTE sub failed to catch index out of bounds");
        }catch(IndexOutOfBoundsException exc) {}
        
        try {
            qSub.setRawFields( new HashSet<Integer>(Arrays.asList(53)) );
            throw new Exception("QUOTE sub failed to catch invalid enum from c lib exception");
        }catch(CLibException exc) {}
        
        for( CommandType c : CommandType.values() ) {
            CommandType cc = new QuotesSubscription( symbols, qFields, c).getCommand();
            if( !cc.equals(c) ) {
                String msg =String.format("QUOTE sub command construction resulted in bad command (%s,%s)", c, cc);
                throw new Exception(msg);            
            }
        }                           
    }
    
    private static void
    testRawSubscription() throws Exception {                
        Map<String,String> subParams = new HashMap<String, String>();
        subParams.put("keys", "BAD_KEYS");
        subParams.put("fields", "BAD_FIELDS");     
        RawSubscription rSub = new RawSubscription("BAD_SERVICE", "BAD_COMMAND", subParams);
            
        String ss = rSub.getServiceString();
        if( !ss.equals("BAD_SERVICE") )
            throw new Exception("service != 'BAD_SERVICE'");
                
        rSub.setServiceString("NASDAQ_BOOK");
        ss = rSub.getServiceString();
        if( !ss.equals("NASDAQ_BOOK") )
            throw new Exception("service != 'NASDAQ_BOOK'");
        
        String cs = rSub.getCommandString();
        if( !cs.equals("BAD_COMMAND") )
            throw new Exception("service != 'BAD_COMMAND'");
                
        rSub.setCommandString("SUBS");
        cs = rSub.getCommandString();
        if( !cs.equals("SUBS") )
            throw new Exception("service != 'SUBS'");        
        
        subParams = rSub.getParameters();
        if( !subParams.get("keys").equals("BAD_KEYS") )
            throw new Exception("keys parameter != 'BAD_KEYS'");
        if( !subParams.get("fields").equals("BAD_FIELDS") )
            throw new Exception("fields parameter != 'BAD_FIELDS'");
        
        subParams.clear();
        subParams.put("keys", "GOOG,APPL");
        subParams.put("fields", "0,1,2");
        rSub.setParameters(subParams);
        
        subParams = rSub.getParameters();
        if( !subParams.get("keys").equals("GOOG,APPL") )
            throw new Exception("keys parameter != 'GOOG,APPL'");
        if( !subParams.get("fields").equals("0,1,2") )
            throw new Exception("fields parameter != '0,1,2'");   
    }
    

    
    private static void
    testStreaming(Credentials creds, boolean liveConnect, long runMSec1, long runMSec2) throws Exception {    
    
        TimesaleEquitySubscription tse = new TimesaleEquitySubscription( 
                new HashSet<String>(Arrays.asList("SPY")), 
                new HashSet<TimesaleEquitySubscription.FieldType>(
                        Arrays.asList(TimesaleEquitySubscription.FieldType.LAST_PRICE)
                        )
                );
        
        TimesaleOptionsSubscription tso = new TimesaleOptionsSubscription( 
                new HashSet<String>(Arrays.asList(
                        buildFutureOptionSymbol("SPY", 1,0,true,300),
                        buildFutureOptionSymbol("SPY", 1,0,false,300)
                        )), 
                new HashSet<TimesaleOptionsSubscription.FieldType>(
                        Arrays.asList(
                                TimesaleOptionsSubscription.FieldType.LAST_PRICE,
                                TimesaleOptionsSubscription.FieldType.LAST_SIZE
                                )
                        )
                );

        TimesaleFuturesSubscription tsf = new TimesaleFuturesSubscription( 
               new HashSet<String>(Arrays.asList("/CL","/GC"))             
               );
        
        NewsHeadlineSubscription nhl = new NewsHeadlineSubscription( 
                new HashSet<String>(Arrays.asList("SPY")),
                NewsHeadlineSubscription.FieldType.buildSet(0,5)
                );
        
        Map<String,String> subParams = new HashMap<String, String>();
        subParams.put("keys", "GOOG");
        subParams.put("fields", "0,1,2");   
        RawSubscription rSub = new RawSubscription(
                StreamingSession.ServiceType.NASDAQ_BOOK.toString(), 
                StreamingSession.CommandType.SUBS.toString() , 
                subParams);
        
        List<StreamingSubscription> subs = new ArrayList<StreamingSubscription>();
        subs.add(tse);
        subs.add(tso);
        subs.add(tsf);
        subs.add(nhl);
        subs.add(rSub);
            
        if( !liveConnect ) {
            System.out.println("*   Can't test streaming session, pass 'account_id' to run live");
            return;
        }
        
        try( StreamingSession session = new StreamingSession(creds, new StreamingCallback() ) ){
            if( session.isActive() )
                throw new Exception("session should not be active");
            
            QOSType qos = session.getQOS();
            if ( !qos.equals(QOSType.FAST) )
                throw new Exception("qos type != FAST");
            
            try {
                session.add( subs );
                throw new Exception("failed to catch exception when calling .add on stopped session");
            }catch(CLibException exc) {            
            }
            
            /* START SESION */
            
            System.out.print("*   Start Session: ");
            List<Boolean> results = session.start( subs );
            if( results.size() != subs.size() )
                throw new Exception(String.format("session.start didn't return %d reults", subs.size()));
            
            int badResults = 0;
            for( Boolean b : results )
                badResults += (b ? 0 : 1);
            if( badResults > 0)
                throw new Exception(String.format("session.start failed with %d bad results", badResults));        

            
            if( !session.isActive() )
                throw new Exception("session isn't active be active");
               
            boolean qosResult = session.setQOS(QOSType.REAL_TIME);
            if( !qosResult )
                throw new Exception("setQOS failed");
            
            qos = session.getQOS();
            if( !qos.equals(QOSType.REAL_TIME) )
                throw new Exception("qos type != 'REAL_TIME' (2)");
            
            ///
            Thread.sleep(runMSec1);
            ///
            
            // update timesale subs
            tse.setCommand(CommandType.ADD);
            tse.setSymbols(           
                    new HashSet<String>(Arrays.asList("AAPL", "ABBV","ABT", "ACN", "ADBE", "AGN", "AIG", 
                           "ALL", "AMGN", "AMZN", "AXP", "BA", "BAC", "BIIB", "BK", "BKNG", "BLK", 
                           "BMY", "BRK.B", "C", "CAT", "CELG", "CHTR", "CL", "CMCSA", "COF", "COP", 
                           "COST", "CSCO", "CVS", "CVX", "DHR", "DIS", "DOW", "DUK", "EMR", "EXC", 
                           "F", "FB", "FDX", "GD", "GE", "GILD", "GM", "GOOG", "GOOGL", "GS", "HD", 
                           "HON", "IBM", "INTC", "JNJ", "JPM", "KHC", "KMI", "KO", "LLY", "LMT", "LOW", 
                           "MA", "MCD", "MDLZ", "MDT", "MET", "MMM", "MO", "MRK", "MS", "MSFT", "NEE", 
                           "NFLX", "NKE", "NVDA", "ORCL", "OXY", "PEP", "PFE", "PG", "PM", "PYPL", 
                           "QCOM", "RTN", "SBUX", "SLB", "SO", "SPG", "T", "TGT", "TXN", "UNH", "UNP", 
                           "UPS", "USB", "UTX", "V", "VZ", "WBA", "WFC", "WMT", "XOM") )
                   );
            
            tso.setCommand(CommandType.UNSUBS);   
            
            tsf.setCommand(CommandType.UNSUBS);
            
            nhl.setCommand(CommandType.VIEW);
            nhl.setFields( NewsHeadlineSubscription.FieldType.buildSet(2,8,10));
            
            subs.remove(subs.size() -1);
                    
            results = session.add(subs);
            if( results.size() != subs.size() )
                throw new Exception(String.format("session.add didn't return %d reults", subs.size()));
            
            badResults = 0;
            for( Boolean b : results )
                badResults += (b ? 0 : 1);
            if( badResults > 0)
                throw new Exception(String.format("session.add failed with %d bad results", badResults));       
            
            rSub.setCommandString(StreamingSession.CommandType.UNSUBS.toString());
            boolean result = session.add(rSub);
            if( !result )
                throw new Exception("session.add failed for Raw Subscription ");
            
            Thread.sleep(runMSec2);
            
            session.stop();
            
            if( session.isActive() )
                throw new Exception("session should not be active");
            
            try {
                session.add( subs );
                throw new Exception("failed to catch exception when calling .add on stopped session");
            }catch(CLibException exc) {         
            }                                                 
        }
        
        System.gc(); // TODO
  
    }


    private static void
    testQuotesGetter(Credentials creds, boolean liveConnect) throws Exception {
        
        System.out.println("*  CREATE QuoteGetter");
        
        Set<String> symbols = new HashSet<String>( Arrays.asList("SPY", "QQQ", "IWM") );
        
        
        try( QuotesGetter qg = new QuotesGetter(creds, symbols) ){
            Set<String> symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");
            
            symbols.clear();
            symbols.add("GLD");
            symbols.add("TLT");
            
            qg.setSymbols(symbols);
            
            symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");        
            
            symbols.add("GOOG");
            qg.addSymbol("GOOG");
            symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");
            
            symbols.remove("GOOG");
            qg.removeSymbol("GOOG");
            symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");        
            
            symbols.add("AMZN");
            symbols.add("MSFT");
            qg.addSymbols(symbols);
            symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");         
                 
            symbols.remove("MSFT");
            qg.removeSymbols(symbols);
            symbols.clear();
            symbols.add("MSFT");
            symbols2 = qg.getSymbols();
            if( !symbols.equals(symbols2) )
                throw new Exception("*   symbols don't match");         
            
            symbols.clear();
            try {
                qg.setSymbols(symbols);
                throw new Exception("failed to raise exception for empty symbols");
            }catch( CLibException e) {            
            }                       
        }                      
    }
    
    
    private static void
    testCredentials(int nAllocs) throws Exception{
        Random R = new Random(Calendar.getInstance().getTimeInMillis());
        
        System.out.println("*   Credential Alloc 1");    
        List<Credentials> C = new ArrayList<Credentials>();
        for(int i = 0; i < nAllocs; ++i ) {
            C.add( testCredentialAccessors(R, 100) );
        }        
        System.out.println("*   Credential DeAlloc 1");    
        C.clear();
        C = null;
        
        System.gc(); 
        Thread.sleep(2000);
        
        System.out.println("*   Credential Alloc 2");
        C = new ArrayList<Credentials>();
        for(int i = 0; i < nAllocs; ++i ) {
            C.add( testCredentialAccessors(R, 100) );
        }    
        
        System.out.println("*   Credential DeAlloc 2");    
        C.clear();
        C = null;
    
        System.gc();            
    }

    private static String
    generateRandomString(Random rand, int length) {
        String CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        
        StringBuilder sb = new StringBuilder();
        for( int i = 0; i < length; ++i ) {
            int index = (int) (rand.nextFloat() * CHARS.length());
            sb.append(CHARS.charAt(index));
        }
        return sb.toString();
    }
    
    private static Credentials
    testCredentialAccessors(Random rand, int ssz) throws Exception {

        Credentials c = new Auth.Credentials(
                generateRandomString(rand, rand.nextInt(ssz-1) + 1),
                generateRandomString(rand, rand.nextInt(ssz-1) + 1),
                Math.abs( Math.max(rand.nextLong(), Long.MAX_VALUE - 1) ),
                generateRandomString(rand, rand.nextInt(ssz-1) + 1)
                );                

        String s = c.getAccessToken(); 
        s = shuffle(s);
        c.setAccessToken(s);
        if( !s.equals(c.getAccessToken()) )
            throw new Exception("failed to reset accessToken");
        
        s = c.getRefreshToken();
        s = shuffle(s);
        c.setRefreshToken(s);
        if( !s.equals(c.getRefreshToken()) )
            throw new Exception("failed to reset refreshToken");
        
        long l = c.getEpochSecTokenExpiration();
        l += 1;        
        c.setEpochSecTokenExpiration(l);
        if( l != c.getEpochSecTokenExpiration() )
            throw new Exception("failed to reset epochSecTokenExpiration");
        
        s = c.getClientID();
        s = shuffle(s);
        c.setClientID(s);
        if( !s.equals(c.getClientID()) )
            throw new Exception("failed to reset clientID");
    
        return c;
    }
    
    static private String
    shuffle(String s) {
        List<Character> C = new ArrayList<Character>();
        for(char c : s.toCharArray() ) {
            C.add(c);
        }
        Collections.shuffle(C);
        StringBuilder sb = new StringBuilder();        
        for(char c : C) {
            sb.append(c);
        }
        return sb.toString();
    }
    
    
    private static void
    testQuoteGetterAllocs(Credentials creds, int nRuns, int nAllocs) throws  CLibException, InterruptedException {
        System.out.print("*   ");   
        for( int i = 0; i < nRuns; ++i ) {
            System.out.print( String.valueOf(i) + " ");
            List<QuoteGetter> C = new ArrayList<QuoteGetter>();
            for(int ii = 0; ii < nAllocs; ++ii ) {
                C.add( new QuoteGetter(creds, "SPY") );
            }
            C.clear();
            C = null;
            
            System.gc(); 
            Thread.sleep(1000);
            C = new ArrayList<QuoteGetter>();
            for(int ii = 0; ii < nAllocs; ++ii ) {
                C.add( new QuoteGetter(creds, "SPY"));
            }
            C.clear();
            C = null;
        
            System.gc();        
            System.gc();
        }
        System.out.print(System.lineSeparator());
    }
    
    private static void
    testQuoteGetter(Credentials creds, boolean liveConnect) throws Exception {            
        System.out.println("*  CREATE QuoteGetter");
        QuoteGetter qg = new QuoteGetter(creds, "SPY");    
    
        String s= qg.getSymbol();
        if( !s.equals("SPY") ) 
            throw new Exception("invalid symbol in QuoteGetter: SPY, " + s);                
        
        if( liveConnect ) {
            JSONObject j = qg.get(); 
            if( j.isEmpty() )
                throw new Exception("QuoteGetter.get() returned empty string");
            System.out.println("*   JSON: " + j.toString(4));
        }else {
            System.out.println("*   Can't get(), pass 'account_id' to run live");
        }

        long w = APIGetter.getWaitMSec();
        System.out.println("*   Wait MSec: " + String.valueOf(w));
        System.out.println("*   Default Wait MSec: " + String.valueOf(APIGetter.getDefaultWaitMSec()));
        System.out.println("*   Wait Remaining: " + String.valueOf(APIGetter.waitRemaining()));
                
        w *= 2;
        APIGetter.setWaitMSec(w);
        long wNew = APIGetter.getWaitMSec();
        System.out.println("*   Wait MSec: " + String.valueOf(wNew));        
        System.out.println("*   Wait Remaining: " + String.valueOf(APIGetter.waitRemaining()));
        
        if( w != wNew )
            throw new Exception("invalid wait msec");
        
        qg.setSymbol("QQQ");
        s = qg.getSymbol();
        if( !s.equals("QQQ") ) 
            throw new Exception("invalid symbol in QuoteGetter: QQQ, " + s);        
        
        if( liveConnect ) {
            JSONObject j = qg.get();
            if( j.isEmpty() )
                throw new Exception("QuoteGetter.get() returned empty string");
            System.out.println("*   JSON: " + j.toString(4));
        }else {
            System.out.println("*   Can't get(), pass 'account_id' to run live");
        }
        
        qg.close();
        if( !qg.isClosed() )
            throw new Exception("QuoteGetter wasn't closed");
    }
        
    
    private static void
    testOptionUtils() throws Exception {        
        Calendar exp = getNextJanExpDate();
        int day = exp.get(Calendar.DAY_OF_MONTH);
        int year = exp.get(Calendar.YEAR);
        
        String symbol;
        System.out.println("*  BUILD (VALID) OPTION SYMBOLS:");
        
        symbol = TDAmeritradeAPI.buildOptionSymbol("SPY", 1, day, year, true, 300);
        System.out.println("*   SYMBOL: " + symbol);                
        System.out.println("*  CHECK OPTION SYMBOL: " + symbol);
        TDAmeritradeAPI.checkOptionSymbol(symbol);    
        
        symbol = TDAmeritradeAPI.buildOptionSymbol("T", 1, day, year, false, 10.25);
        System.out.println("*   SYMBOL: " + symbol);                
        System.out.println("*  CHECK OPTION SYMBOL: " + symbol);
        TDAmeritradeAPI.checkOptionSymbol(symbol);    
        
        System.out.println("*  BUILD (INVALID) OPTION SYMBOLS:");
        try {
            symbol = TDAmeritradeAPI.buildOptionSymbol("SPY", 0, day, year, true, 300);
            throw new Exception("buildOptionSymbol failed to throw");
        }catch( CLibException e ) {
            System.out.println("*   SUCCESSFULLY CAUGHT EXCEPTION: " + e.getMessage());
        }
        try {
            symbol = TDAmeritradeAPI.buildOptionSymbol("SPY", 1, 32, year, true, 300);
            throw new Exception("buildOptionSymbol failed to throw");
        }catch( CLibException e ) {
            System.out.println("*   SUCCESSFULLY CAUGHT EXCEPTION: " + e.getMessage());
        }
        try {
            symbol = TDAmeritradeAPI.buildOptionSymbol("SPY", 1, day, year, true, -300);
            throw new Exception("buildOptionSymbol failed to throw");
        }catch( CLibException e ) {
            System.out.println("*   SUCCESSFULLY CAUGHT EXCEPTION: " + e.getMessage());
        }
        try {
            symbol = TDAmeritradeAPI.buildOptionSymbol("_SPY", 0, day, year, true, 300);
            throw new Exception("buildOptionSymbol failed to throw");
        }catch( CLibException e ) {
            System.out.println("*   SUCCESSFULLY CAUGHT EXCEPTION: " + e.getMessage());
        }
    }
    
    private static String
    buildFutureOptionSymbol(String symbol,int month, int yearsAhead, boolean isCall, double strike) 
            throws CLibException {
        Calendar c = Calendar.getInstance();
        int currentYear = c.get(Calendar.YEAR);
        int currentMonth = c.get(Calendar.MONTH) + 1; // 1 -12
        int currentDay = c.get(Calendar.DAY_OF_MONTH); // 1 - n        
        
        if( yearsAhead < 0 )
            throw new IllegalArgumentException("negative yearsAhead");
        int futureYear = currentYear + yearsAhead;
     
        if( month < currentMonth ) {
            ++futureYear;
        }else if( month == currentMonth ) {
            Calendar dayOfExp = nthWeekOfMonth(Calendar.FRIDAY, month-1, currentYear, 3);
            if( currentDay >= dayOfExp.get(Calendar.DAY_OF_MONTH) ){
                ++futureYear;
            }                
        }        
        
        Calendar futureDayOfExp = nthWeekOfMonth(Calendar.FRIDAY, month-1, futureYear, 3);
        
        return TDAmeritradeAPI.buildOptionSymbol(symbol, month, futureDayOfExp.get(Calendar.DAY_OF_MONTH),
                futureYear, isCall, strike);
        
    }
    
    
    /* { month 1 - 12, week 1-5} */
    private static Calendar 
    nthWeekOfMonth(int dayOfWeek, int month, int year, int week) {
        Calendar c = Calendar.getInstance();
        c.set(Calendar.DAY_OF_WEEK,  dayOfWeek); 
        c.set(Calendar.MONTH, month-1);
        c.set(Calendar.YEAR, year);
        c.set(Calendar.WEEK_OF_MONTH,  week);
        return c;        
    }
    
    private static Calendar
    getNextJanExpDate() {
        int year = Year.now().getValue();
        return nthWeekOfMonth(Calendar.FRIDAY, 1, year+1, 3);        
    }
    
    private static String
    secureString( String s, int nChars ) {
        int n = s.length();
        int b = Math.min(nChars, n);
        int e = n - b;
        return s.substring(0, b) + new String(new char[e]).replace('\0', '*'); 
    }
}
