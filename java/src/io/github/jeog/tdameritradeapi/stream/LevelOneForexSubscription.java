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

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class LevelOneForexSubscription extends SubscriptionBySymbolBase {

    public enum FieldType implements CLib.ConvertibleEnum {
        SYMBOL(0), 
        BID_PRICE(1), 
        ASK_PRICE(2),
        LAST_PRICE(3),
        BID_SIZE(4),
        ASK_SIZE(5),
        TOTAL_VOLUME(6), 
        LAST_SIZE(7),
        QUOTE_TIME(8),
        TRADE_TIME(9),
        HIGH_PRICE(10),
        LOW_PRICE(11),
        CLOSE_PRICE(12),
        EXCHANGE_ID(13),
        DESCRIPTION(14),
        OPEN_PRICE(15),
        NET_CHANGE(16),
        FIELD_PERCENT_CHANGE(17),
        EXCHANGE_NAME(18),
        DIGITS(19),
        SECURITY_STATUS(20),
        TICK(21),
        TICK_AMOUNT(22),
        PRODUCT(23),
        TRADING_HOURS(24),
        IS_TRADABLE(25),
        MARKET_MAKER(26),
        HIGH_52_WEEK(27),
        LOW_52_WEEK(28),
        MARK(29);
                
        private int value;
        
        FieldType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static FieldType
        fromInt(int i) {
            for(FieldType ss : FieldType.values()) {
                if(ss.toInt() == i)
                    return ss;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::LevelOneForexSubscriptionField_to_string_ABI);
        }
        
        public static Set<FieldType>
        buildSet( Integer... rawFields ){
            Set<FieldType> fields = new HashSet<FieldType>();
            for( Integer i : rawFields ) {
                FieldType f = fromInt(i);
                if( f == null )
                    throw new IndexOutOfBoundsException("integer out of field range");
                fields.add(f);
            }
            return fields;
        }
        
        public static Set<FieldType>
        buildSet( FieldType... enumFields ){
            return new HashSet<FieldType>( Arrays.asList(enumFields) );       
        }       
        
    };
    
    public LevelOneForexSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._LevelOneForexSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._LevelOneForexSubscription_C>)
                   TDAmeritradeAPI.getCLib()::LevelOneForexSubscription_Create_ABI );      
    }
    
    public LevelOneForexSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public void
    setFields( Set<FieldType> fields ) throws CLibException {        
        setFieldsAsInts( fieldsToInts(fields) );    
    }
    
    public Set<FieldType>
    getFields() throws CLibException {
        int ints[] = getFieldsAsInts();
        Set<FieldType> fields = new HashSet<FieldType>();
        for(int i = 0; i < ints.length; ++i) 
            fields.add( FieldType.fromInt(ints[i]) );
        return fields;               
    }
        
    @Override
    protected CLib._LevelOneForexSubscription_C
    getProxy(){
        return (CLib._LevelOneForexSubscription_C)super.getProxy();
    }
    
    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::LevelOneForexSubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::LevelOneForexSubscription_SetFields_ABI);         
    }
    


}
