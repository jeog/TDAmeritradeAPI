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

package io.github.jeog.tdameritradeapi.stream;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class StreamingSession implements AutoCloseable {
    
    public static final long DEF_CONNECT_TIMEOUT = 3000;
    public static final long DEF_LISTENING_TIMEOUT = 30000;
    public static final long DEF_SUBSCRIBE_TIMEOUT = 1500;

    public static interface Callback {
        public void 
        call(int serviceType, int callbackType, long timestamp, String data);
    }
    
    public static class _CallbackWrapper implements com.sun.jna.Callback {
        private Callback callback;
        
        public _CallbackWrapper(Callback callback) {
            this.callback = callback;
        }
        
        public void 
        call(int serviceType, int callbackType, long timestamp, String data) {
            callback.call(serviceType, callbackType, timestamp, data);
        }
    } 

    
    public enum ServiceType implements CLib.ConvertibleEnum {
        NONE(0), // *DONT PASS*
        QUOTE(1),
        OPTION(2),
        LEVELONE_FUTURES(3),
        LEVELONE_FOREX(4),
        LEVELONE_FUTURES_OPTIONS(5),
        NEWS_HEADLINE(6),
        CHART_EQUITY(7),
        CHART_FOREX(8), // NOT WORKING (SERVER SIDE)
        CHART_FUTURES(9),
        CHART_OPTIONS(10), 
        TIMESALE_EQUITY(11),
        TIMESALE_FOREX(12), // NOT WORKING (SERVER SIDE)
        TIMESALE_FUTURES(13),
        TIMESALE_OPTIONS(14),
        ACTIVES_NASDAQ(15),
        ACTIVES_NYSE(16),
        ACTIVES_OTCBB(17),
        ACTIVES_OPTIONS(18), 
        ADMIN(19), // NOT MANAGED
        ACCT_ACTIVITY(20), // NOT MANAGED
        CHART_HISTORY_FUTURES(21), // NOT MANAGED
        FOREX_BOOK(22), // NOT MANAGED
        FUTURES_BOOK(23), // NOT MANAGED
        LISTED_BOOK(24), // NOT MANAGED
        NASDAQ_BOOK(25), // NOT MANAGED
        OPTIONS_BOOK(26), // NOT MANAGED
        FUTURES_OPTIONS_BOOK(27), // NOT MANAGED
        NEWS_STORY(28), // NOT MANAGED
        NEWS_HEADLINE_LIST(29), // NOT MANAGED
        UNKNOWN(30); // *DONT PASS*
                
        private int value;
        
        ServiceType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static ServiceType
        fromInt(int i) {
            for(ServiceType ss : ServiceType.values()) {
                if(ss.toInt() == i)
                    return ss;
            }
            return null;
        }       
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this,
                    TDAmeritradeAPI.getCLib()::StreamerServiceType_to_string_ABI);
        }
    };
    
    public enum CommandType implements CLib.ConvertibleEnum {
        SUBS(0),
        UNSUBS(1),
        ADD(2),
        VIEW(3);
                
        private int value;
        
        CommandType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static CommandType
        fromInt(int i) {
            for(CommandType ss : CommandType.values()) {
                if(ss.toInt() == i)
                    return ss;
            }
            return null;
        }   

        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this,
                    TDAmeritradeAPI.getCLib()::CommandType_to_string_ABI);
        }
    };
    
    public enum CallbackType implements CLib.ConvertibleEnum {
        LISTENING_START(0),
        LISTENING_STOP(1),
        DATA(2),
        REQUEST_RESPONSE(3),
        NOTIFY(4),
        TIMEOUT(5),
        ERROR(6);        
                
        private int value;
        
        CallbackType(int value){ this.value = value; }      
        
        @Override
        public int toInt() { return value; }
        
        public static CallbackType
        fromInt(int i) {
            for(CallbackType ss : CallbackType.values()) {
                if(ss.toInt() == i)
                    return ss;
            }
            return null;
        }     
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this,
                    TDAmeritradeAPI.getCLib()::StreamingCallbackType_to_string_ABI);
        }
    };
    
    public enum QOSType implements CLib.ConvertibleEnum {
        EXPRESS(0),
        REAL_TIME(1),
        FAST(2),
        MODERATE(3),
        SLOW(4),
        DELAYED(5);               
                
        private int value;
        
        QOSType(int value){ this.value = value; }   
        
        @Override
        public int toInt() { return value; }
        
        public static QOSType
        fromInt(int i) {
            for(QOSType ss : QOSType.values()) {
                if(ss.toInt() == i)
                    return ss;
            }
            return null;
        }  
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this,
                    TDAmeritradeAPI.getCLib()::QOSType_to_string_ABI);
        }
    };
    
    private CLib._StreamingSession_C pSession; 
    private _CallbackWrapper callback;
    
    public StreamingSession( Credentials creds, Callback callback, long connectTimeout,
            long listeningTimeout, long subscribeTimeout ) throws CLibException{
        this.callback = new _CallbackWrapper(callback);
        this.pSession = new CLib._StreamingSession_C();       
        int err = TDAmeritradeAPI.getCLib().StreamingSession_Create_ABI( creds.getNativeCredentials(), 
                this.callback, connectTimeout, listeningTimeout, subscribeTimeout, pSession, 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    public StreamingSession( Credentials creds, Callback callback) throws CLibException{
        this(creds,callback, DEF_CONNECT_TIMEOUT, DEF_LISTENING_TIMEOUT, DEF_SUBSCRIBE_TIMEOUT);
    }
   
    public List<Boolean>
    start( List<StreamingSubscription> subscriptions ) throws CLibException{
        CLib._StreamingSubscription_C.ByReference[] cSubs = subsToPtrArray(subscriptions);
        int[] cResults = new int[cSubs.length]; 
        int err = TDAmeritradeAPI.getCLib().StreamingSession_Start_ABI(pSession, cSubs, 
                new CLib.size_t(cSubs.length), cResults, 0);
        if(err != 0)
            throw new CLibException(err); 
        return intsToListOfBools(cResults);
    }
    
    public boolean
    start( StreamingSubscription subscription ) throws CLibException{
        return start( Arrays.asList(subscription) ).get(0);
    }
   
    public List<Boolean>
    add( List<StreamingSubscription> subscriptions ) throws CLibException{      
        CLib._StreamingSubscription_C.ByReference[] cSubs = subsToPtrArray(subscriptions);
        int[] cResults = new int[cSubs.length]; 
        int err = TDAmeritradeAPI.getCLib().StreamingSession_AddSubscriptions_ABI(pSession, 
                cSubs, new CLib.size_t(cSubs.length), cResults, 0);
        if(err != 0)
            throw new CLibException(err); 
        return intsToListOfBools(cResults);
    }
    
    public boolean
    add( StreamingSubscription subscription ) throws CLibException{
        return add( Arrays.asList(subscription) ).get(0); 
    }
    
    public void
    stop() throws CLibException {
        int err = TDAmeritradeAPI.getCLib().StreamingSession_Stop_ABI(pSession, 0);
        if(err != 0)
            throw new CLibException(err);
    }
    
    public boolean
    isActive() throws CLibException {
        return CLib.Helpers.getInt(pSession, 
                TDAmeritradeAPI.getCLib()::StreamingSession_IsActive_ABI) == 1;       
    }
    
    public QOSType
    getQOS() throws CLibException {
        int qos = CLib.Helpers.getInt(pSession, 
                TDAmeritradeAPI.getCLib()::StreamingSession_GetQOS_ABI);        
        return QOSType.fromInt(qos);        
    }
    
    public boolean
    setQOS( QOSType qos ) throws CLibException {
        int[] b = {0};
        int err = TDAmeritradeAPI.getCLib().StreamingSession_SetQOS_ABI(pSession, qos.toInt(), b, 0);
        if(err != 0)
            throw new CLibException(err);
        return (b[0] == 1);
    }
    
    @Override
    public void close() throws CLibException {
        stop();        
    }
    
    private static CLib._StreamingSubscription_C.ByReference[]
    subsToPtrArray( List<StreamingSubscription> subscriptions ){
        int n = subscriptions.size();
        CLib._StreamingSubscription_C.ByReference[] cSubs = 
                new CLib._StreamingSubscription_C.ByReference[n];
        for(int i = 0; i < n; ++i) {
            Pointer p = subscriptions.get(i).getProxy().getPointer();
            cSubs[i] = new CLib._StreamingSubscription_C.ByReference(p);
        }
        return cSubs;
    }
    
    private static List<Boolean>
    intsToListOfBools(int[] cInts) {
        List<Boolean> res = new ArrayList<Boolean>();
        for(int i = 0; i < cInts.length; ++i) 
            res.add( cInts[i] != 0 );       
        return res;
    }


    
}







