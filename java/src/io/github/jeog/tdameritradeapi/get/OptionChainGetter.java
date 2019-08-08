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

package io.github.jeog.tdameritradeapi.get;

import io.github.jeog.tdameritradeapi.Auth.Credentials;

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.CLib.OptionStrikesValue;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;


public class OptionChainGetter extends APIGetter {

    public enum OptionStrikesType implements CLib.ConvertibleEnum {
        N_ATM(0),
        SINGLE(1),
        RANGE(2);
            
        private int value;
        
        OptionStrikesType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionStrikesType
        fromInt(int i) {
            for(OptionStrikesType o : OptionStrikesType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionStrikesType_to_string_ABI);
        }    
    };
    
    
    public enum OptionRangeType implements CLib.ConvertibleEnum {
        ITM(1),
        NTM(2),
        OTM(3),
        SAK(4),
        SBK(5),
        SNK(6),
        ALL(7);        
            
        private int value;
        
        OptionRangeType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionRangeType
        fromInt(int i) {
            for(OptionRangeType o : OptionRangeType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionRangeType_to_string_ABI);
        }    
    };
    
    
    public enum OptionContractType implements CLib.ConvertibleEnum {
        CALL(0),
        PUT(1),
        ALL(2);      
            
        private int value;
        
        OptionContractType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionContractType
        fromInt(int i) {
            for(OptionContractType o : OptionContractType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionContractType_to_string_ABI);
        }    
    };
    
    public enum OptionExpMonth implements CLib.ConvertibleEnum {
        JAN(0),
        FED(1),
        MAR(2),
        APR(3),
        MAY(4),
        JUN(5),
        JUL(6),
        AUG(7),
        SEP(8),
        OCT(9),
        NOV(10),
        DEC(11),
        ALL(12);    
            
        private int value;
        
        OptionExpMonth(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionExpMonth
        fromInt(int i) {
            for(OptionExpMonth o : OptionExpMonth.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionExpMonth_to_string_ABI);
        }    
    };
    
    public enum OptionType implements CLib.ConvertibleEnum {
        S(0),
        NS(1),
        ALL(2);    
            
        private int value;
        
        OptionType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionType
        fromInt(int i) {
            for(OptionType o : OptionType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionType_to_string_ABI);
        }    
    };
    
    
    @SuppressWarnings("serial")
    public static class InvalidOptionStrikesType extends Exception{
        public InvalidOptionStrikesType(String msg) {
            super(msg);
        }            
    }
    
    public static class OptionStrikes {
        private OptionStrikesType strikesType;
        private CLib.OptionStrikesValue strikesValue;
        
        private OptionStrikes( OptionStrikesType strikesType, CLib.OptionStrikesValue strikesValue){
            this.strikesType = strikesType;
            this.strikesValue = strikesValue;
        }
                
        public OptionStrikesType
        getType() {           
            return strikesType;
        }
        
        /* package private */ CLib.OptionStrikesValue
        getValue() {
            return strikesValue;
        }
        
        public int
        getNAtm() throws InvalidOptionStrikesType {
            if( getType() != OptionStrikesType.N_ATM )
                throw new InvalidOptionStrikesType("OptionStrikes not of type 'N_ATM");
            return (int)strikesValue.nAtm;
 
        }
        
        public double
        getSingle() throws InvalidOptionStrikesType {
            if( getType() != OptionStrikesType.SINGLE)
                throw new InvalidOptionStrikesType("OptionStrikes not of type 'SINGLE");
            return strikesValue.single;
        }
        
        public OptionRangeType
        getRange() throws InvalidOptionStrikesType {
            if( getType() != OptionStrikesType.RANGE )
                throw new InvalidOptionStrikesType("OptionStrikes not of type 'RANGE");
            return OptionRangeType.fromInt((int)strikesValue.range);
        }
        
        public static OptionStrikes
        buildNAtm( int n ) {                    
            return new OptionStrikes( OptionStrikesType.N_ATM, new CLib.OptionStrikesValue(n) );
        }
        
        public static OptionStrikes
        buildSingle( double strike ) {
            return new OptionStrikes( OptionStrikesType.SINGLE,  new CLib.OptionStrikesValue(strike));            
        }
        
        public static OptionStrikes
        buildRange( OptionRangeType range ) {            
            return new OptionStrikes( OptionStrikesType.RANGE, new CLib.OptionStrikesValue(range.toInt()) );
        }                
    }
     
      
    public OptionChainGetter( Credentials creds, String symbol, OptionStrikes strikes, 
            OptionContractType contractType, boolean includeQuotes, String fromDate, String toDate, 
            OptionExpMonth expMonth, OptionType optionType) throws CLibException {
        super( create(creds, symbol, strikes, contractType, includeQuotes, fromDate, toDate, 
                expMonth, optionType) );
    }
    
    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public String
    getSymbol() throws CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetSymbol_ABI);
    }
    
    public void
    setSymbol( String symbol ) throws CLibException {
        CLib.Helpers.setString( getProxy(), symbol, 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetSymbol_ABI);
    }

    public OptionStrikes
    getStrikes() throws CLibException {
        int[] i = {0};       
        OptionStrikesValue v = new OptionStrikesValue();
        int err = TDAmeritradeAPI.getCLib().OptionChainGetter_GetStrikes_ABI( getProxy(), i, v, 0);
        if( err != 0 )
            throw new CLibException(err);
        OptionStrikesType ty = OptionStrikesType.fromInt(i[0]);       
        return new OptionStrikes(ty, v);
    }
    
    public void
    setStrikes( OptionStrikes strikes ) throws CLibException {
        int err = TDAmeritradeAPI.getCLib().OptionChainGetter_SetStrikes_ABI( getProxy(), 
                strikes.getType().toInt(), buildOptionStrikesValueByValue( strikes), 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    public OptionContractType
    getContractType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetContractType_ABI);
        return OptionContractType.fromInt(i);
    }
    
    public void
    setContractType( OptionContractType contractType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), contractType.toInt(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetContractType_ABI);
    }
    
    
    public boolean
    includesQuotes() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OptionChainGetter_IncludesQuotes_ABI) == 1;   
    }
    
    public void
    includeQuotes(boolean includeQuotes ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), (includeQuotes ? 1 : 0),
                TDAmeritradeAPI.getCLib()::OptionChainGetter_IncludeQuotes_ABI);
    }
    
    public String
    getFromDate() throws CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetFromDate_ABI);
    }
    
    public void
    setFromDate( String date) throws CLibException {
        CLib.Helpers.setString( getProxy(), date,
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetFromDate_ABI);
    }
    
    public String
    getToDate() throws CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetToDate_ABI);
    }
    
    public void
    setToDate( String date) throws CLibException {
        CLib.Helpers.setString( getProxy(), date,
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetToDate_ABI);
    }    
    
    
    public OptionExpMonth
    getExpMonth() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetExpMonth_ABI);
        return OptionExpMonth.fromInt(i);
    }
    
    public void
    setExpMonth( OptionExpMonth ExpMonth ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), ExpMonth.toInt(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetExpMonth_ABI);
    }
    
    public OptionType
    getOptionType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OptionChainGetter_GetOptionType_ABI);
        return OptionType.fromInt(i);
    }
    
    public void
    setOptionType( OptionType OptionType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), OptionType.toInt(), 
                TDAmeritradeAPI.getCLib()::OptionChainGetter_SetOptionType_ABI);
    }
        
    protected static OptionStrikesValue.ByValue
    buildOptionStrikesValueByValue( OptionStrikes strikes ){
        switch(strikes.getType()) {
        case N_ATM: return  new OptionStrikesValue.ByValue(strikes.strikesValue.nAtm);                
        case SINGLE: return new OptionStrikesValue.ByValue(strikes.strikesValue.single);                
        default: return new OptionStrikesValue.ByValue(strikes.strikesValue.range);                
        }         
    }
        
    protected OptionChainGetter(CLib._OptionChainStrategyGetter_C pGetter) {
        super(pGetter);
        
    }
    
    protected OptionChainGetter(CLib._OptionChainAnalyticalGetter_C pGetter) {
        super(pGetter);
        
    }    

    protected CLib._OptionChainGetter_C
    getProxy(){
        return (CLib._OptionChainGetter_C)super.getProxy(); 
    }
    
    static private CLib._OptionChainGetter_C
    create( Credentials creds, String symbol, OptionStrikes strikes, OptionContractType contractType,
            boolean includeQuotes, String fromDate, String toDate, OptionExpMonth expMonth,
            OptionType optionType) throws CLibException {
        CLib._OptionChainGetter_C pGetter = new CLib._OptionChainGetter_C();        
        
        int err = TDAmeritradeAPI.getCLib().OptionChainGetter_Create_ABI(creds.getNativeCredentials(), symbol, 
                strikes.getType().toInt(), buildOptionStrikesValueByValue( strikes), contractType.toInt(), 
                (includeQuotes ? 1 : 0), fromDate, toDate, expMonth.toInt(), optionType.toInt(), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);
        
        return pGetter;
    }
    
    

}
