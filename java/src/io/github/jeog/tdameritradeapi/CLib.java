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

package io.github.jeog.tdameritradeapi;

import com.sun.jna.Library;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.Structure;
import com.sun.jna.Union;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import com.sun.jna.IntegerType;
import com.sun.jna.ptr.PointerByReference;

import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession;

import com.sun.jna.Pointer;

/*
 * NOTE - CLib provides a set of tools and interfaces to be used by other
 *        packages and classes. It shouldn't be used directly by client code.
 */
public interface CLib extends Library {
    
    @SuppressWarnings("serial")
    public static class size_t extends IntegerType {
        public size_t() { this(0); }
        public size_t(long value) { super(Native.SIZE_T_SIZE, value, true); }
    }

    public static interface ConvertibleEnum{
        int toInt();
        String toString();
    }

    
    public static class Helpers{
        public static class FunctionType{
            public interface SetInt{ int call(int i, int exc); }
            public interface GetInt{ int call(int[] i, int exc); }              
            public interface GetLong{ int call(long[] l, int exc); }        
            public interface SetLong{ int call(long l, int exc); }            
            public interface GetString{ int call(PointerByReference buffer, size_t[] n, int exc); }
            public interface GetStringFromInt{ int call(int i, PointerByReference buffer, size_t[] n, int exc); }
        }
        
        public static class MethodType{
            public interface GetInt< T >{ int call(T pObj, int[] i, int exc); }   
            public interface SetInt< T >{ int call(T pObj, int i, int exc); }   
            public interface GetInts< T >{ int call(T pObj, PointerByReference buffer, size_t[] n, int exc); }   
            public interface SetInts< T >{ int call(T pObj, int[] i, size_t n, int exc); }     
            public interface GetLong< T >{ int call(T pObj, long[] i, int exc); }   
            public interface SetLong< T >{ int call(T pObj, long i, int exc); } 
            public interface GetDouble< T >{ int call(T pObj, double[] d, int exc); }   
            public interface SetDouble< T >{ int call(T pObj, double d, int exc); } 
            public interface GetString< T >{ int call(T pObj, PointerByReference buffer, size_t[] n, int exc); }        
            public interface SetString< T >{ int call(T pObj, String s, int exc); }     
            public interface GetStrings< T >{ int call(T pObj, PointerByReference buffers, size_t[] n, int exc); }      
            public interface SetStrings< T >{ int call(T pObj, String[] buffers, size_t n, int exc); }                          
        }                   
   
        
        public static void
        setInt(int i, FunctionType.SetInt method) throws CLibException{    
            int err = method.call(i, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);               
        }  
        
        public static int
        getInt( FunctionType.GetInt method) throws CLibException{
            int i[] = {0};
            int err = method.call(i, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);
            return i[0];        
        }  
        
        public static long
        getLong( FunctionType.GetLong method) throws CLibException{
            long l[] = {0};
            int err = method.call(l, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);
            return l[0];        
        }   
        
        public static void
        setLong(long l, FunctionType.SetLong method) throws CLibException{    
            int err = method.call(l, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);               
        }   
            
        public static String 
        getString(FunctionType.GetString method) throws CLibException{
            PointerByReference p = new PointerByReference();
            CLib.size_t n[] = new CLib.size_t[1];
            int err = method.call(p, n, 0);
            if( err != 0 )
                throw new CLibException(err);   
            return stringFromBuffer(p.getValue(), n[0].intValue());     
        }
        
        public static String 
        getStringFromInt(int i, FunctionType.GetStringFromInt method) throws CLibException{
            PointerByReference p = new PointerByReference();
            CLib.size_t n[] = new CLib.size_t[1];
            int err = method.call(i, p, n, 0);
            if( err != 0 )
                throw new CLibException(err);   
            return stringFromBuffer(p.getValue(), n[0].intValue());     
        }
        
        public static <T> int
        getInt(T pObj, MethodType.GetInt<T> method) throws CLibException{
            int i[] = {0};
            int err = method.call(pObj, i, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);
            return i[0];        
        }               
        
        public static <T> void
        setInt(T pObj, int i, MethodType.SetInt<T> method) throws CLibException{            
            int err = method.call(pObj, i, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);                
        } 
        
        public static <T> long
        getLong(T pObj, MethodType.GetLong<T> method) throws CLibException{
            long l[] = {0};
            int err = method.call(pObj, l, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);
            return l[0];        
        }               
        
        public static <T> void
        setLong(T pObj, long l, MethodType.SetLong<T> method) throws CLibException{            
            int err = method.call(pObj, l, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);                
        } 
                
        public static <T> double
        getDouble(T pObj, MethodType.GetDouble<T> method) throws CLibException{
            double d[] = {.0};
            int err = method.call(pObj, d, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);
            return d[0];        
        }               
        
        public static <T> void
        setDouble(T pObj, double d, MethodType.SetDouble<T> method) throws CLibException{            
            int err = method.call(pObj, d, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);                
        } 
        
        public static <T> int[]
        getFields(T pObj, MethodType.GetInts<T> method) throws CLibException{
            PointerByReference p = new PointerByReference();
            CLib.size_t n[] = new CLib.size_t[1];
            int err = method.call(pObj, p, n, 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);            
            Pointer ptr = p.getValue();
            int[] ints = null;
            try {
                ints = ptr.getIntArray(0, n[0].intValue());  
            }finally {
                err = TDAmeritradeAPI.getCLib().FreeFieldsBuffer_ABI(ptr, 0);
                if( err != 0 )
                    System.err.println("Failed to free fields memory in CLib._getStrings" 
                            + Error.getErrorName(err));                    
            }            
            return ints;  
        }               
        
        public static <T> void
        setFields(T pObj, int[] i, MethodType.SetInts<T> method) throws CLibException{            
            int err = method.call(pObj, i, new CLib.size_t(i.length), 0); 
            if( err != 0 )
                throw new TDAmeritradeAPI.CLibException(err);                
        } 
        
        public static <T> String 
        getString(T pObj, MethodType.GetString<T> method) throws CLibException{
            PointerByReference p = new PointerByReference();
            CLib.size_t n[] = new CLib.size_t[1];
            int err = method.call(pObj, p, n, 0);
            if( err != 0 )
                throw new CLibException(err);  
            return stringFromBuffer(p.getValue(), n[0].intValue());              
        }

        public static <T> void
        setString(T pObj, String s, MethodType.SetString<T> method) throws CLibException { 
            int err = method.call(pObj, s, 0);
            if( err != 0 )
                throw new CLibException(err);       
        }
        
        public static <T> Set<String>
        getStrings(T pObj, MethodType.GetStrings<T> method) throws CLibException {
            PointerByReference p = new PointerByReference(); 
            CLib.size_t[] n = new CLib.size_t[1];        
            int err = method.call(pObj, p, n, 0);
            if( err != 0 )
                throw new CLibException(err);
            
            Pointer ptr = p.getValue();
            if( ptr == null )
                return new HashSet<String>();  
            Set<String> symbols = null;
            try {
                int i = n[0].intValue();            
                String[] cSymbols = ptr.getStringArray(0, i);            
                symbols = new HashSet<String>();                
                while( i-- > 0 )            
                    symbols.add( cSymbols[i] );
            }finally {
                err = TDAmeritradeAPI.getCLib().FreeBuffers_ABI(ptr, n[0], 0);
                if( err != 0 )
                    System.err.println("Failed to free string memory in CLib._getStrings" 
                            + Error.getErrorName(err));          
            }          
            
            return symbols;          
        }

        public static <T> void
        setStrings(T pObj, Set<String> symbols, MethodType.SetStrings<T> method) throws CLibException {         
            int err = method.call(pObj, symbolsToStrings(symbols), new CLib.size_t(symbols.size()), 0);                
            if( err != 0 )
                throw new CLibException(err);       
        }      
        
        public static String[]
        symbolsToStrings( Set<String> symbols ) {
            String[] s = new String[symbols.size()];
            s = symbols.toArray(s);
            return s;
        }   
        
        
        public static String
        convertibleEnumToString(ConvertibleEnum e, Helpers.FunctionType.GetStringFromInt method ) {  
            // DO NOT call e.toString() from in here
            int i = e.toInt();
            try {
                return CLib.Helpers.getStringFromInt(i, method);
            } catch (CLibException exc) {
                String msg = String.format("Failed to convert enum value '%d' for '%s' to string: %s", 
                        i, e.getClass().getName(), exc.getMessage() );
                System.err.println(msg);
            }
            return null;              
        }
        
        public static String
        stringFromBuffer(Pointer p, int n) {
            if( p == null )
                return "";
            String s = new String( p.getByteArray(0, n-1) );
            int err = TDAmeritradeAPI.getCLib().FreeBuffer_ABI(p, 0);
            if( err != 0 )
                System.err.println("Failed to free string memory in CLib._getString: " 
                        + Error.getErrorName(err)); 
            return s;              
        }
    }

    
    public static abstract class _CProxy2 extends Structure  { 
        public Pointer obj; 
        public int typeId;    
                    
        protected _CProxy2(){ super(); }
        protected _CProxy2(Pointer p){ super(p); read(); }
        
        protected Memory autoAllocate(int size) {
            return new Memory(size) {
                protected void finalize() {
                    try {                    
                        destroy();                                    
                    }catch( Throwable t ) {            
                        System.err.println("_CProxy2 finalize() error: " + t.getMessage());
                    }finally {
                        super.finalize();
                    }                    
                }
            };
        }
        
        protected abstract int
        destroyNative() throws  CLibException;
        
        public void
        destroy() throws CLibException {
            if( isNative() ) {
                int err = destroyNative();     
                if( err != 0 )            
                    throw new TDAmeritradeAPI.CLibException(err);
                obj = null; // redundant ?
            }
        }
                
        @Override
        protected List<String> 
        getFieldOrder() { return new ArrayList<String>(Arrays.asList("obj", "typeId")); }        
        
        public boolean
        isNative() { return obj != null; }            
    }
    
    public static abstract class _Getter_C extends _CProxy2 { 
        public _Getter_C() { 
            super(); 
        } 
    }
    
    
    public static class _QuoteGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().QuoteGetter_Destroy_ABI(this, 0);                            
        }        
    }
    
    public static class _QuotesGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().QuotesGetter_Destroy_ABI(this, 0);                            
        }        
    }
    
    public static class _MarketHoursGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().MarketHoursGetter_Destroy_ABI(this, 0);                            
        }        
    }
    
    public static class _MoversGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().MoversGetter_Destroy_ABI(this, 0);                            
        }        
    }
    
    public static class _HistoricalRangeGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().HistoricalRangeGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _HistoricalPeriodGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().HistoricalPeriodGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _OptionChainGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().OptionChainGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _OptionChainStrategyGetter_C extends _OptionChainGetter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().OptionChainStrategyGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _OptionChainAnalyticalGetter_C extends _OptionChainGetter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().OptionChainAnalyticalGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _AccountInfoGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().AccountInfoGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _PreferencesGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().PreferencesGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _StreamerSubscriptionKeysGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().StreamerSubscriptionKeysGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _TransactionHistoryGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().TransactionHistoryGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _IndividualTransactionHistoryGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().IndividualTransactionHistoryGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _OrderGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().OrderGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _OrdersGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().OrdersGetter_Destroy_ABI(this, 0);                         
        }       
    }

    public static class _InstrumentInfoGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().InstrumentInfoGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static class _UserPrincipalsGetter_C extends _Getter_C {
        protected int
        destroyNative() throws  CLibException {
            return TDAmeritradeAPI.getCLib().UserPrincipalsGetter_Destroy_ABI(this, 0);                         
        }       
    }
    
    public static abstract class _CProxy3 extends _CProxy2 {
        public Pointer ctx;
        
        protected _CProxy3(){ super(); }
        
        @Override
        protected List<String> 
        getFieldOrder() {
            List<String> fields = super.getFieldOrder();    
            fields.add("ctx");
            return fields;
        }        
    }
    
    public static class _StreamingSession_C extends _CProxy3 {
        public _StreamingSession_C() { super(); }
        
        protected int
        destroyNative() throws CLibException {
            return TDAmeritradeAPI.getCLib().StreamingSession_Destroy_ABI(this,0);
        }
    }

    public static class _StreamingSubscription_C extends _CProxy2 {
        protected int
        destroyNative() throws CLibException {
            return TDAmeritradeAPI.getCLib().StreamingSubscription_Destroy_ABI(this,0);
        }        
        
        public static class ByReference extends _StreamingSubscription_C implements Structure.ByReference {
            public ByReference() {}
            public ByReference(Pointer p) { super(p); read(); }                       
        }
        
        public _StreamingSubscription_C() { }
        public _StreamingSubscription_C(Pointer p) { super(p); read(); }
    }
    
    
    public static class _RawSubscription_C extends _StreamingSubscription_C { }     
    public static class _QuotesSubscription_C extends _StreamingSubscription_C { }               
    public static class _OptionsSubscription_C extends _StreamingSubscription_C { }
    public static class _LevelOneFuturesSubscription_C extends _StreamingSubscription_C { }
    public static class _LevelOneForexSubscription_C extends _StreamingSubscription_C { }
    public static class _LevelOneFuturesOptionsSubscription_C extends _StreamingSubscription_C { }
    public static class _ChartEquitySubscription_C extends _StreamingSubscription_C { }
    public static class _ChartFuturesSubscription_C extends _StreamingSubscription_C { }
    public static class _ChartOptionsSubscription_C extends _StreamingSubscription_C { }
    public static class _TimesaleEquitySubscription_C extends _StreamingSubscription_C { }    
    public static class _TimesaleFuturesSubscription_C extends _StreamingSubscription_C { }    
    public static class _TimesaleOptionsSubscription_C extends _StreamingSubscription_C { }   
    public static class _NewsHeadlineSubscription_C extends _StreamingSubscription_C { }
    public static class _NasdaqActivesSubscription_C extends _StreamingSubscription_C { }
    public static class _NYSEActivesSubscription_C extends _StreamingSubscription_C { }
    public static class _OTCBBActivesSubscription_C extends _StreamingSubscription_C { }
    public static class _OptionActivesSubscription_C extends _StreamingSubscription_C { }
    public static class _AcctActivitySubscription_C extends _StreamingSubscription_C { }
    
    public static class KeyValPair extends Structure {
        public String key;
        public String val;
        
        @Override
        protected List<String> 
        getFieldOrder() { return new ArrayList<String>(Arrays.asList("key", "val")); }  
   
        public KeyValPair() { super(); }
        public KeyValPair(Pointer p) { super(p); read(); }
        
    }

    
    public class OptionStrikesValue extends Union {
        /*
         * WARNING - purposely passing longs in place of int as a temporary
         *           work-around to a bug in JNA
         *           
         *           https://github.com/java-native-access/jna/issues/1118
         */
        public long nAtm;
        public double single;
        public long range;
        
        public OptionStrikesValue() {
            super();
        }
        
        public OptionStrikesValue(long l) {
            super();
            this.range = this.nAtm = l;
            setType(Long.TYPE);
        }
        
        public OptionStrikesValue(double d) {
            super();
            this.single = d;
            setType(Double.TYPE);
        }
          
        public static class ByValue extends OptionStrikesValue implements Union.ByValue {            
            public ByValue( long l ) { super(l); }
            public ByValue( double d ) { super(d); }          
        };        
    }

    
    /* ERROR */
    int LastErrorCode_ABI( int[] code, int exc );    
    int LastErrorMsg_ABI( PointerByReference buffer, size_t[] n, int exc ); // need to free    
    int LastErrorLineNumber_ABI( int[] lineno, int exc );    
    int LastErrorFilename_ABI( PointerByReference buffer, size_t[] n, int exc); // need to free    
    
    /* DE-ALLOC */
    int FreeBuffer_ABI( Pointer buffer, int exc );
    int FreeBuffers_ABI( Pointer buffers, size_t n, int exc );
    int FreeKeyValBuffer_ABI( Pointer buffer, size_t n, int exc );
    int FreeFieldsBuffer_ABI( Pointer buffer, int exc );
    
    /* TO STRING */
    int StreamerServiceType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc);
    int StreamingCallbackType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc);
    int CommandType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc);
    int QOSType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc); 
    int QuotesSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc);
    int OptionsSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc); 
    int ChartEquitySubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int ChartSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int LevelOneFuturesSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int LevelOneForexSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int LevelOneFuturesOptionsSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int TimesaleSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int NewsHeadlineSubscriptionField_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int DurationType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int VenueType_to_string_ABI( int service, PointerByReference buffer, size_t[] n, int exc );
    int FrequencyType_to_string_ABI( int frequencyType, PointerByReference buffer, size_t[] n, int exc );
    int PeriodType_to_string_ABI( int periodType, PointerByReference buffer, size_t[] n, int exc );
    int OptionStrikesType_to_string_ABI( int optionStrikesType, PointerByReference buffer, size_t[] n, int exc );
    int OptionRangeType_to_string_ABI( int optionRangeType, PointerByReference buffer, size_t[] n, int exc );
    int OptionContractType_to_string_ABI( int optionContractType, PointerByReference buffer, size_t[] n, int exc );
    int OptionExpMonth_to_string_ABI( int optionExpMonth, PointerByReference buffer, size_t[] n, int exc );
    int OptionType_to_string_ABI( int optionType, PointerByReference buffer, size_t[] n, int exc );
    int OptionStrategyType_to_string_ABI( int strategyType, PointerByReference buffer, size_t[] n, int exc );
    int TransactionType_to_string_ABI( int transactionType, PointerByReference buffer, size_t[] n, int exc );
    int OrderStatusType_to_string_ABI( int orderStatusType, PointerByReference buffer, size_t[] n, int exc );
    int InstrumentSearchType_to_string_ABI( int instrumentSearchType, PointerByReference buffer, size_t[] n, int exc );
    int MarketType_to_string_ABI( int marketType, PointerByReference buffer, size_t[] n, int exc );
    int MoversIndex_to_string_ABI( int moversIndex, PointerByReference buffer, size_t[] n, int exc );
    int MoversDirectionType_to_string_ABI( int moversDirectionType, PointerByReference buffer, size_t[] n, int exc );
    int MoversChangeType_to_string_ABI( int moversChangeType, PointerByReference buffer, size_t[] n, int exc );
    
    /* AUTH */
    int LoadCredentials_ABI( String path, String password, Credentials._Credentials pCredentials, int exc );    
    int StoreCredentials_ABI( String path, String password, Credentials._Credentials pCredentials, int exc );    
    int RequestAccessToken_ABI( String code, String clienID, String redirectURI, 
            Credentials._Credentials pCredentials, int exc);    
    int CloseCredentials_ABI( Memory m, int exc);
    int CloseCredentials_ABI( Credentials._Credentials pCredentials, int exc); 
    int CreateCredentials_ABI( String accessToken, String refreshToken, 
            long epochSecTokenExpiration, String clientID, Credentials._Credentials pCredentials, int exc);
    int SetCertificateBundlePath_ABI( String path, int exc );
    int GetCertificateBundlePath_ABI( PointerByReference buffer, size_t[] n, int exc );
    int GetDefaultCertificateBundlePath_ABI( PointerByReference buffer, size_t[] n, int exc );
        
    /* MISC */
    int BuildOptionSymbol_ABI( String underlying, int month, int day, int year, int is_call, 
            double strike, PointerByReference buffer, size_t[] n, int exc);    
    int CheckOptionSymbol_ABI( String symbol, int exc );

    
    /*
     *  NOTE - C/ABI methods of a base class use the root proxy object since in the C lib
     *         those objects are only defined for the most derived (client) objects
     *         
     *         e.g HistoricalRangeGetter derives from HistoricalGetterBase but
     *             because C only defines _HistoricalRangeGetter_C and _HistoricalPeriodGetter_C
     *             (no _HistoricalGetterBase_C) we need to cast and pass as _Getter_C when using
     *             the methods of the sub-class (HistoricalGetterBase)
     */
    
    
    /* GETTERS (BASE) */
    int APIGetter_Get_ABI( _Getter_C pGetter, PointerByReference buffer, size_t[] n, int exc );    
    int APIGetter_Close_ABI( _Getter_C pGetter, int exc );    
    int APIGetter_IsClosed_ABI( _Getter_C pGetter, int[] b, int exc );    
    int APIGetter_SetWaitMSec_ABI( long msec, int exc );
    int APIGetter_GetWaitMSec_ABI( long[] msec, int exc );
    int APIGetter_GetDefWaitMSec_ABI( long[] msec, int exc );
    int APIGetter_WaitRemaining_ABI( long[] msec, int exc );
    int APIGetter_ShareConnections_ABI( int b, int exc );
    int APIGetter_IsSharingConnections_ABI( int[] b, int exc);
    
    /* QUOTE GETTER */
    int QuoteGetter_Create_ABI( Credentials._Credentials pCredentials, String symbol, 
            _QuoteGetter_C pGetter, int exc );    
    int QuoteGetter_Destroy_ABI( _QuoteGetter_C pGetter, int exc );    
    int QuoteGetter_GetSymbol_ABI(_QuoteGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc );    
    int QuoteGetter_SetSymbol_ABI(_QuoteGetter_C pGetter, String symbol, int exc );
        
    /* QUOTES GETTERS*/
    int QuotesGetter_Create_ABI( Credentials._Credentials pCredentials, String[] symbols, size_t n, 
            _QuotesGetter_C pGetter, int exc );    
    int QuotesGetter_Destroy_ABI( _QuotesGetter_C pGetter, int exc );    
    int QuotesGetter_GetSymbols_ABI(_QuotesGetter_C pGetter, PointerByReference buffers, size_t[] n, int exc );    
    int QuotesGetter_SetSymbols_ABI(_QuotesGetter_C pGetter, String[] symbols, size_t n, int exc );
    int QuotesGetter_AddSymbol_ABI(_QuotesGetter_C pGetter, String symbol, int exc );    
    int QuotesGetter_RemoveSymbol_ABI(_QuotesGetter_C pGetter, String symbol, int exc );
    int QuotesGetter_AddSymbols_ABI(_QuotesGetter_C pGetter, String[] symbols, size_t n, int exc ); 
    int QuotesGetter_RemoveSymbols_ABI(_QuotesGetter_C pGetter, String[] symbols, size_t n, int exc );
    
    /* MARKET HOURS GETTER */
    int MarketHoursGetter_Create_ABI( Credentials._Credentials pCredentials, int marketType, String date,
            _MarketHoursGetter_C pGetter, int exc);
    int MarketHoursGetter_Destroy_ABI( _MarketHoursGetter_C pGetter, int exc);                                  
    int MarketHoursGetter_GetMarketType_ABI( _MarketHoursGetter_C pGetter, int[] marketType, int exc);
    int MarketHoursGetter_SetMarketType_ABI( _MarketHoursGetter_C pGetter, int marketType, int exc);
    int MarketHoursGetter_GetDate_ABI( _MarketHoursGetter_C pGetter, PointerByReference buffers, 
            size_t[] n, int exc );
    int MarketHoursGetter_SetDate_ABI( _MarketHoursGetter_C pGetter, String date, int exc );
    
    /* MOVERS GETTER */
    int MoversGetter_Create_ABI( Credentials._Credentials pCredentials, int index, int directionType, 
            int changeType, _MoversGetter_C pGetter, int exc );
    int MoversGetter_Destroy_ABI( _MoversGetter_C pGetter, int exc );
    int MoversGetter_GetIndex_ABI( _MoversGetter_C pGetter, int[] index, int exc );
    int MoversGetter_SetIndex_ABI( _MoversGetter_C pGetter, int index, int exc );
    int MoversGetter_GetDirectionType_ABI( _MoversGetter_C pGetter, int[] directionType, int exc );
    int MoversGetter_SetDirectionType_ABI( _MoversGetter_C pGetter, int directionType, int exc );
    int MoversGetter_GetChangeType_ABI( _MoversGetter_C pGetter, int[] changeType, int exc );
    int MoversGetter_SetChangeType_ABI( _MoversGetter_C pGetter, int changeType, int exc );
    
    /* HISTORICAL GETTER (BASE) */
    // NOTE - use the base object (see explanation above)     
    int HistoricalGetterBase_GetSymbol_ABI( _Getter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int HistoricalGetterBase_SetSymbol_ABI( _Getter_C pGetter, String symbol, int exc );
    int HistoricalGetterBase_GetFrequency_ABI( _Getter_C pGetter, int[] frequency, int exc );
    int HistoricalGetterBase_GetFrequencyType_ABI(  _Getter_C pGetter, int[] frequencyType, int exc );
    int HistoricalGetterBase_SetFrequency_ABI(  _Getter_C pGetter, int frequencyType, int frequency, int exc );
    int HistoricalGetterBase_IsExtendedHours_ABI(  _Getter_C pGetter, int[] b, int exc );
    int HistoricalGetterBase_SetExtendedHours_ABI(  _Getter_C pGetter, int b, int exc);
    
    /* HISTORICAL PERIOD GETTER */
    int HistoricalPeriodGetter_Create_ABI( Credentials._Credentials pCredentials, String symbol, 
            int periodType, int period, int frequencyType, int frequency, int extendedHours, 
            long msecSinceEpoch, _HistoricalPeriodGetter_C pGetter, int exc );
    int HistoricalPeriodGetter_Destroy_ABI( _HistoricalPeriodGetter_C pGetter, int exc);
    int HistoricalPeriodGetter_GetPeriodType_ABI( _HistoricalPeriodGetter_C pGetter, int[] periodType, int exc);
    int HistoricalPeriodGetter_GetPeriod_ABI( _HistoricalPeriodGetter_C pGetter, int[] period, int exc);
    int HistoricalPeriodGetter_SetPeriod_ABI( _HistoricalPeriodGetter_C pGetter, int periodType, int period, int exc);
    int HistoricalPeriodGetter_GetMSecSinceEpoch_ABI( _HistoricalPeriodGetter_C pGetter, long[] msecSinceEpoch, int exc );
    int HistoricalPeriodGetter_SetMSecSinceEpoch_ABI( _HistoricalPeriodGetter_C pGetter, long msecSinceEpoch, int exc);
    
    /* HISTORICAL RANGE GETTER */
    int HistoricalRangeGetter_Create_ABI( Credentials._Credentials pCredentials, 
            String symbol, int frequencyType, int frequency, long startMSecSinceEpoch, 
            long endMSecSinceEpoch, int extendedHours, _HistoricalRangeGetter_C pGetter, int exc);
    int HistoricalRangeGetter_Destroy_ABI( _HistoricalRangeGetter_C pGetter, int exc);
    int HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(_HistoricalRangeGetter_C pGetter, long[] endMSecSinceEpcoh, int exc);
    int HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(_HistoricalRangeGetter_C pGetter, long endMSecSinceEpcoh, int exc);
    int HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(_HistoricalRangeGetter_C pGetter, long[] endMSecSinceEpoch, int exc);
    int HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI( _HistoricalRangeGetter_C pGetter, long endMSecSinceEpoch, int exc);   

    /* OPTION CHAIN GETTER */
    int OptionChainGetter_Create_ABI( Credentials._Credentials pCredentials, String symbol, int strikesType,     
            OptionStrikesValue.ByValue strikesValue, int contractType, int includeQuotes, String fromDate, String toDate, 
            int expMonth, int optionType, _OptionChainGetter_C pGetter, int exc );
    int OptionChainGetter_Destroy_ABI( _OptionChainGetter_C pGetter, int exc);
    int OptionChainGetter_GetSymbol_ABI( _OptionChainGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OptionChainGetter_SetSymbol_ABI( _OptionChainGetter_C pGetter, String symbol, int exc );
    int OptionChainGetter_GetStrikes_ABI( _OptionChainGetter_C pGetter, int[] strikesType, 
            OptionStrikesValue strikesValue, int exc );
    int OptionChainGetter_SetStrikes_ABI( _OptionChainGetter_C pGetter, int strikesType, 
            OptionStrikesValue.ByValue strikesValue, int exc );
    int OptionChainGetter_GetContractType_ABI( _OptionChainGetter_C pGetter, int[] contractType, int exc );
    int OptionChainGetter_SetContractType_ABI( _OptionChainGetter_C pGetter, int contractType, int exc );
    int OptionChainGetter_IncludesQuotes_ABI( _OptionChainGetter_C pGetter, int[] includes_quotes, int exc );
    int OptionChainGetter_IncludeQuotes_ABI( _OptionChainGetter_C pGetter, int include_quotes, int exc );
    int OptionChainGetter_GetFromDate_ABI( _OptionChainGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OptionChainGetter_SetFromDate_ABI( _OptionChainGetter_C pGetter, String date, int exc );
    int OptionChainGetter_GetToDate_ABI( _OptionChainGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OptionChainGetter_SetToDate_ABI( _OptionChainGetter_C pGetter, String date, int exc );
    int OptionChainGetter_GetExpMonth_ABI( _OptionChainGetter_C pGetter, int[] expMonth, int exc );
    int OptionChainGetter_SetExpMonth_ABI( _OptionChainGetter_C pGetter, int expMonth, int exc );
    int OptionChainGetter_GetOptionType_ABI( _OptionChainGetter_C pGetter, int[] optionType, int exc );
    int OptionChainGetter_SetOptionType_ABI( _OptionChainGetter_C pGetter, int optionType, int exc );
    
    /* OPTION CHAIN STRATEGY GETTER */
    int OptionChainStrategyGetter_Destroy_ABI( _OptionChainStrategyGetter_C pGetter, int exc);
    int OptionChainStrategyGetter_Create_ABI( Credentials._Credentials pCredentials, String symbol, int strategyType,
            double spreadInterval, int strikesType, OptionStrikesValue.ByValue strikesValue, int contractType, 
            int includeQuotes, String fromDate, String toDate, int expMonth, int optionType, 
            _OptionChainStrategyGetter_C pGetter, int exc );   
    int OptionChainStrategyGetter_GetStrategy_ABI( _OptionChainStrategyGetter_C pGetter, int[] strategyType,
            double[] spreadInterval, int exc );
    int OptionChainStrategyGetter_SetStrategy_ABI( _OptionChainStrategyGetter_C pGetter, int strategyType,
            double spreadInterval, int exc );      
    
    
    /* OPTION CHAIN ANALYTICAL GETTER */
    int OptionChainAnalyticalGetter_Create_ABI( Credentials._Credentials pCredentials, String symbol, double volatility,
            double underlyingPirce, double interestRate, int daysToExp, int strikesType, 
            OptionStrikesValue.ByValue strikesValue, int contractType, int includeQuotes, String fromDate, 
            String toDate, int expMonth, int optionType, _OptionChainAnalyticalGetter_C pGetter, int exc );
    int OptionChainAnalyticalGetter_Destroy_ABI( _OptionChainAnalyticalGetter_C pGetter, int exc);
    int OptionChainAnalyticalGetter_GetVolatility_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double[] volatility, int excs );
    int OptionChainAnalyticalGetter_SetVolatility_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double volatility, int excs );
    int OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double[] underlying_price, int excs );
    int OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double underlying_price, int excs );
    int OptionChainAnalyticalGetter_GetInterestRate_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double[] interest_rate, int excs );
    int OptionChainAnalyticalGetter_SetInterestRate_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            double interest_rate, int excs );
    int OptionChainAnalyticalGetter_GetDaysToExp_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            int[] days_to_exp, int excs );
    int OptionChainAnalyticalGetter_SetDaysToExp_ABI( _OptionChainAnalyticalGetter_C pGetter, 
            int days_to_exp, int excs );
    
    /* ACCOUNT GETTER (BASE */
    int AccountGetterBase_GetAccountId_ABI( _Getter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int AccountGetterBase_SetAccountId_ABI( _Getter_C pGetter, String symbol, int exc );
    
    /* ACCOUNT INFO GETTER */
    int AccountInfoGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, int positions,
            int orders, _AccountInfoGetter_C pGetter, int exc);                              
    int AccountInfoGetter_Destroy_ABI( _AccountInfoGetter_C pgetter, int exc );                                   
    int AccountInfoGetter_ReturnsPositions_ABI( _AccountInfoGetter_C pGettretter, int[] returnsPositions, int exc);                                        
    int AccountInfoGetter_ReturnPositions_ABI( _AccountInfoGetter_C pGetter, int returnPositions, int exc);                                          
    int AccountInfoGetter_ReturnsOrders_ABI( _AccountInfoGetter_C pGetter, int[] returnsOrder, int exc);                                         
    int AccountInfoGetter_ReturnOrders_ABI( _AccountInfoGetter_C pGetter, int returnOrders, int exc);
    
    /* PREFERENCES GETTER */
    int PreferencesGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, 
            _PreferencesGetter_C pGetter, int exc); 
    int PreferencesGetter_Destroy_ABI( _PreferencesGetter_C pGetter, int exc );
    
    /* STREAMER SUBSCRIPTION KEYS GETTER */
    int StreamerSubscriptionKeysGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, 
            _StreamerSubscriptionKeysGetter_C pGetter, int exc); 
    int StreamerSubscriptionKeysGetter_Destroy_ABI( _StreamerSubscriptionKeysGetter_C pGetter, int exc );                                       
        
    /* USER PRINCIPALS GETTER */
    int UserPrincipalsGetter_Create_ABI( Credentials._Credentials pCredentials, int streamerSubscriptionKeys, 
            int streamerConnectionInfo, int preferences, int surrogateIds, _UserPrincipalsGetter_C pGetter, int exc );
    int UserPrincipalsGetter_Destroy_ABI( _UserPrincipalsGetter_C pGetter, int exc );
    int UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI( _UserPrincipalsGetter_C pGetter, 
            int[] returnsSubscriptionKeys,  int exc);
    int UserPrincipalsGetter_ReturnSubscriptionKeys_ABI( _UserPrincipalsGetter_C pGetter, 
            int returnSubscriptionKeys, int exc);
    int UserPrincipalsGetter_ReturnsConnectionInfo_ABI( _UserPrincipalsGetter_C pGetter,
            int[] returnsConnectionInfo, int exc );
    int UserPrincipalsGetter_ReturnConnectionInfo_ABI( _UserPrincipalsGetter_C pGetter,
            int returnConnectionInfo, int exc );       
    int UserPrincipalsGetter_ReturnsPreferences_ABI( _UserPrincipalsGetter_C pGetter,
            int[]  returns_preferences, int exc);
    int UserPrincipalsGetter_ReturnPreferences_ABI( _UserPrincipalsGetter_C pGetter,
            int returnPreferences, int exc);
    int UserPrincipalsGetter_ReturnsSurrogateIds_ABI( _UserPrincipalsGetter_C pGetter,
            int[] returnsSurrogateIds, int exc);       
    int UserPrincipalsGetter_ReturnSurrogateIds_ABI( _UserPrincipalsGetter_C pGetter,
        int returnSurrogateIds, int exc); 
 
    
    /* TRANSACTION HISTORY GETTER */
    int TransactionHistoryGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, 
            int transactionType, String symbol, String startDate, String endDate, 
            _TransactionHistoryGetter_C pGetter, int exc);                                      
    int TransactionHistoryGetter_Destroy_ABI( _TransactionHistoryGetter_C pGetter, int exc );
    int TransactionHistoryGetter_GetTransactionType_ABI( _TransactionHistoryGetter_C pGetter, 
            int[] transactionType, int exc );
    int TransactionHistoryGetter_SetTransactionType_ABI( _TransactionHistoryGetter_C pGetter, 
            int transactionType, int exc );
    int TransactionHistoryGetter_GetSymbol_ABI( _TransactionHistoryGetter_C pGetter, PointerByReference buffer, 
            size_t[] n, int exc);    
    int TransactionHistoryGetter_SetSymbol_ABI( _TransactionHistoryGetter_C pGetter, String symbol, int exc );
    int TransactionHistoryGetter_GetStartDate_ABI( _TransactionHistoryGetter_C pGetter, PointerByReference buffer, 
            size_t[] n, int exc);  
    int TransactionHistoryGetter_SetStartDate_ABI( _TransactionHistoryGetter_C pGetter, String startDate, int exc );
    int TransactionHistoryGetter_GetEndDate_ABI( _TransactionHistoryGetter_C pGetter, PointerByReference buffer, 
            size_t[] n, int exc); 
    int TransactionHistoryGetter_SetEndDate_ABI( _TransactionHistoryGetter_C pGetter, String endDate, int exc );
        
    /* INDIVIDUAL TRANSACTION HISTORY GETTER */
    int IndividualTransactionHistoryGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, 
            String transactionID, _IndividualTransactionHistoryGetter_C pGetter, int exc);                                      
    int IndividualTransactionHistoryGetter_Destroy_ABI( _IndividualTransactionHistoryGetter_C pGetter, int exc );
    int IndividualTransactionHistoryGetter_GetTransactionId_ABI( _IndividualTransactionHistoryGetter_C pGetter, 
            PointerByReference buffer, size_t[] n, int exc);    
    int IndividualTransactionHistoryGetter_SetTransactionId_ABI( _IndividualTransactionHistoryGetter_C pGetter, 
            String transactionID, int exc );
    
    /* ORDER GETTER */
    int OrderGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, String orderID,
            _OrderGetter_C pGetter, int exc); 
    int OrderGetter_Destroy_ABI( _OrderGetter_C pGetter, int exc );  
    int OrderGetter_GetOrderId_ABI( _OrderGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OrderGetter_SetOrderId_ABI( _OrderGetter_C pGetter, String symbol, int exc );     
    
    /* ORDERS GETTER */
    int OrdersGetter_Create_ABI( Credentials._Credentials pCredentials, String accountID, int nMaxResults,
            String fromEnteredTime, String toEnteredTime, int orderStatusType, _OrdersGetter_C pGetter, int exc); 
    int OrdersGetter_Destroy_ABI( _OrdersGetter_C pGetter, int exc );    
    int OrdersGetter_GetNMaxResults_ABI( _OrdersGetter_C pGetter, int[] nMaxResults, int exc);
    int OrdersGetter_SetNMaxResults_ABI( _OrdersGetter_C pGetter, int nMaxResults, int exc );
    int OrdersGetter_GetFromEnteredTime_ABI( _OrdersGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OrdersGetter_SetFromEnteredTime_ABI( _OrdersGetter_C pGetter, String symbol, int exc );
    int OrdersGetter_GetToEnteredTime_ABI( _OrdersGetter_C pGetter, PointerByReference buffer, size_t[] n, int exc);
    int OrdersGetter_SetToEnteredTime_ABI( _OrdersGetter_C pGetter, String symbol, int exc );
    int OrdersGetter_GetOrderStatusType_ABI( _OrdersGetter_C pGetter, int[] orderStatusType, int exc);
    int OrdersGetter_SetOrderStatusType_ABI( _OrdersGetter_C pGetter, int orderStatusType, int exc );
    
    /* INSTRUMENT INFO GETTER*/
    int InstrumentInfoGetter_Create_ABI( Credentials._Credentials pCredentials, int searchType, 
            String queryString, _InstrumentInfoGetter_C pGetter, int exc); 
    int InstrumentInfoGetter_Destroy_ABI( _InstrumentInfoGetter_C pGetter, int exc );    
    int InstrumentInfoGetter_GetSearchType_ABI( _InstrumentInfoGetter_C pGetter, int[] searchType, int exc);  
    int InstrumentInfoGetter_GetQueryString_ABI( _InstrumentInfoGetter_C pGetter, PointerByReference buffer, 
            size_t[] n, int exc);
    int InstrumentInfoGetter_SetQuery_ABI( _InstrumentInfoGetter_C pGetter, int searchType, 
            String queryString, int exc );
    
    
    /* STREAMING SESSION */
    int StreamingSession_Create_ABI( Credentials._Credentials pCredentials, 
            StreamingSession._CallbackWrapper callback, long connectTimeout, long listeningTimeout, 
            long subscribeTimeout, _StreamingSession_C pSession, int exc );
    int StreamingSession_Destroy_ABI( _StreamingSession_C pSession, int exc );
    int StreamingSession_Start_ABI( _StreamingSession_C pSession, 
            _StreamingSubscription_C.ByReference[] pSubscriptions, size_t n, int[] results, int exc);
    int StreamingSession_AddSubscriptions_ABI( _StreamingSession_C pSession, 
            _StreamingSubscription_C.ByReference[] pSubscriptions, size_t n, int[] results, int exc);
    int StreamingSession_Stop_ABI( _StreamingSession_C pSession, int exc);
    int StreamingSession_IsActive_ABI( _StreamingSession_C pSession, int[] b, int exc);
    int StreamingSession_GetQOS_ABI( _StreamingSession_C pSession, int[] qos, int exc);
    int StreamingSession_SetQOS_ABI( _StreamingSession_C pSession, int qos, int[] result, int exc);
    
    /* STREAMING SUBCRIPTION (BASE) */
    int StreamingSubscription_Destroy_ABI( _StreamingSubscription_C pSubscription, int exc );
    int StreamingSubscription_IsSame_ABI( _StreamingSubscription_C lSubscription, 
            _StreamingSubscription_C rSubscription, int[] b, int exc );
    
    /* RAW SUBSCRIPTION */
    int RawSubscription_Create_ABI( String service, String command, KeyValPair[] kvPairs, size_t n, 
            _RawSubscription_C pSubscription, int exc );
    int RawSubscription_GetServiceStr_ABI (_RawSubscription_C pSubscription, PointerByReference buffer, 
            size_t[] n, int exc);
    int RawSubscription_SetServiceStr_ABI (_RawSubscription_C pSubscription, String service, int exc);
    int RawSubscription_GetCommandStr_ABI (_RawSubscription_C pSubscription, PointerByReference buffer, 
            size_t[] n, int exc);
    int RawSubscription_SetCommandStr_ABI (_RawSubscription_C pSubscription, String command, int exc);
    int RawSubscription_GetParameters_ABI (_RawSubscription_C pSubscription, PointerByReference pKeyVals, 
            size_t[] n, int exc);
    int RawSubscription_SetParameters_ABI (_RawSubscription_C pSubscription, KeyValPair[] kvPairs, 
            size_t n, int exc);
    
    /* MANAGED SUBCRIPTION (BASE) */ 
    // NOTE - use the base object (see explanation above)        
    int StreamingSubscription_GetService_ABI( _StreamingSubscription_C pSubscription, int[] service, int exc );
    int StreamingSubscription_GetCommand_ABI( _StreamingSubscription_C pSubscription, int[] command, int exc );
    int StreamingSubscription_SetCommand_ABI( _StreamingSubscription_C pSubscription, int command, int exc );
        
    /* SUBSCRIPTION BY SYMBOL (BASE) */
    int SubscriptionBySymbolBase_GetSymbols_ABI( _StreamingSubscription_C pSubscription, PointerByReference buffers, 
            size_t[] n, int exc );
    int SubscriptionBySymbolBase_SetSymbols_ABI( _StreamingSubscription_C pSubscription, String[] buffers, 
            size_t n, int exc );
        
    /* QUOTES SUBSCRIPTION */
    int QuotesSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _QuotesSubscription_C pSubscription, int exc );
    int QuotesSubscription_GetFields_ABI( _QuotesSubscription_C pSubscription, PointerByReference fields, 
            size_t[] n, int exc);
    int QuotesSubscription_SetFields_ABI( _QuotesSubscription_C pSubscription, int[] fields, size_t n, int exc);
    
    /* OPTIONS SUBSCRIPTION */
    int OptionsSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _OptionsSubscription_C pSubscription, int exc );
    int OptionsSubscription_GetFields_ABI( _OptionsSubscription_C pSubscription, PointerByReference fields, 
            size_t[] n, int exc);
    int OptionsSubscription_SetFields_ABI( _OptionsSubscription_C pSubscription, int[] fields, size_t n, int exc);            
    
    /* LEVEL ONE FUTURES SUBSCRIPTION */
    int LevelOneFuturesSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _LevelOneFuturesSubscription_C pSubscription, int exc );
    int LevelOneFuturesSubscription_GetFields_ABI( _LevelOneFuturesSubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int LevelOneFuturesSubscription_SetFields_ABI( _LevelOneFuturesSubscription_C pSubscription, 
            int[] fields, size_t n, int exc);
    
    /* LEVEL ONE FOREX SUBSCRIPTION */
    int LevelOneForexSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _LevelOneForexSubscription_C pSubscription, int exc );
    int LevelOneForexSubscription_GetFields_ABI( _LevelOneForexSubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int LevelOneForexSubscription_SetFields_ABI( _LevelOneForexSubscription_C pSubscription, int[] fields, 
            size_t n, int exc);
    
    /* LEVEL ONE FUTURES OPTIONS SUBSCRIPTION */
    int LevelOneFuturesOptionsSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, 
            size_t nFields, int command, _LevelOneFuturesOptionsSubscription_C pSubscription, int exc );
    int LevelOneFuturesOptionsSubscription_GetFields_ABI( _LevelOneFuturesOptionsSubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int LevelOneFuturesOptionsSubscription_SetFields_ABI( _LevelOneFuturesOptionsSubscription_C pSubscription, 
            int[] fields, size_t n, int exc);  
    
    /* CHART EQUITY SUBSCRIPTION */
    int ChartEquitySubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _ChartEquitySubscription_C pSubscription, int exc );
    int ChartEquitySubscription_GetFields_ABI( _ChartEquitySubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int ChartEquitySubscription_SetFields_ABI( _ChartEquitySubscription_C pSubscription, int[] fields, 
            size_t n, int exc);  
    
    /* CHART SUBSCRIPTION (BASE) */
    // NOTE - use the base object (see explanation above)
    int ChartSubscriptionBase_GetFields_ABI( _StreamingSubscription_C pSubscription,    
            PointerByReference fields, size_t[] n, int exc);
    int ChartSubscriptionBase_SetFields_ABI( _StreamingSubscription_C pSubscription, int[] fields, 
            size_t n, int exc);    
    
    /* CHART FUTURES SUBSCRIPTION */
    int ChartFuturesSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _ChartFuturesSubscription_C pSubscription, int exc );
    
    /* CHART OPTIONS SUBSCRIPTION */
    int ChartOptionsSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _ChartOptionsSubscription_C pSubscription, int exc );   
    
    
    /* TIMESALE SUBSCRIPTION (BASE) */
    // NOTE - use the base object (see explanation above)
    int TimesaleSubscriptionBase_GetFields_ABI( _StreamingSubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int TimesaleSubscriptionBase_SetFields_ABI( _StreamingSubscription_C pSubscription, int[] fields, 
            size_t n, int exc);
      
    /* TIMESALE EQUITY SUBSCRIPTION */
    int TimesaleEquitySubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _TimesaleEquitySubscription_C pSubscription, int exc );
    
    /* TIMESALE FUTURES SUBSCRIPTION */    
    int TimesaleFuturesSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _TimesaleFuturesSubscription_C pSubscription, int exc );
        
    /* TIMESALE OPTIONS SUBSCRIPTION */
    int TimesaleOptionsSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, size_t nFields, 
            int command, _TimesaleOptionsSubscription_C pSubscription, int exc );

    
    /* NEWS HEADLINE SUBSCRIPTION */
    int NewsHeadlineSubscription_Create_ABI( String[] symbols, size_t nSymbols, int[] fields, 
            size_t nFields,  int command, _NewsHeadlineSubscription_C pSubscription, int exc );
    int NewsHeadlineSubscription_GetFields_ABI( _NewsHeadlineSubscription_C pSubscription, 
            PointerByReference fields, size_t[] n, int exc);
    int NewsHeadlineSubscription_SetFields_ABI( _NewsHeadlineSubscription_C pSubscription, 
            int[] fields, size_t n, int exc);
    
    /* ACTIVE SUBSCRIPTION (BASE) */
    // NOTE - use the base object (see explanation above)
    int ActivesSubscriptionBase_GetDuration_ABI( _StreamingSubscription_C pSubscription, int[] duration, int exc);
    int ActivesSubscriptionBase_SetDuration_ABI( _StreamingSubscription_C pSubscription, int duration, int exc);
    
    /* NASDAQ ACTIVES SUBSCRIPTION */
    int NasdaqActivesSubscription_Create_ABI( int duration, int command, _NasdaqActivesSubscription_C pSubscription, int exc);

    /* NYSE ACTIVES SUBSCRIPTION */
    int NYSEActivesSubscription_Create_ABI( int duration, int command, _NYSEActivesSubscription_C pSubscription, int exc);
    
    /* OTCBB ACTIVES SUBSCRIPTION */
    int OTCBBActivesSubscription_Create_ABI( int duration, int command, _OTCBBActivesSubscription_C pSubscription, int exc);
    
    /* Option ACTIVES SUBSCRIPTION */
    int OptionActivesSubscription_Create_ABI( int venue, int duration, int command, _OptionActivesSubscription_C pSubscription, int exc);    
    int OptionActivesSubscription_GetVenue_ABI( _OptionActivesSubscription_C pSubscription, int[] venue, int exc);
    int OptionActivesSubscription_SetVenue_ABI( _OptionActivesSubscription_C pSubscription, int venue, int exc);
    
    /* ACCOUNT ACTIVITY SUBSCRIPTION */
    int AcctActivitySubscription_Create_ABI( int command, _AcctActivitySubscription_C pSubscription, int exc );   
   
    /* EXEC */    
    

}
