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

public class QuotesSubscription extends SubscriptionBySymbolBase {

    public enum FieldType implements CLib.ConvertibleEnum {
        SYMBOL(0),
        BID_PRICE(1),
        ASK_PRICE(2),
        LAST_PRICE(3),
        BID_SIZE(4),
        ASK_SIZE(5),
        ASK_ID(6),
        BID_ID(7),
        TOTAL_VOLUME(8),
        LAST_SIZE(9),
        TRADE_TIME(10),
        QUOTE_TIME(11),
        HIGH_PRICE(12),
        LOW_PRICE(13),
        BID_TICK(14),
        CLOSE_PRICE(15),
        EXCHANGE_ID(16),
        MARGINABLE(17),
        SHORTABLE(18),
        ISLAND_BID(19),
        ISLAND_ASK(20),
        ISLAND_VOLUME(21),
        QUOTE_DAY(22),
        TRADE_DAY(23),
        VOLATILITY(24),
        DESCRIPTION(25),
        LAST_ID(26),
        DIGITS(27),
        OPEN_PRICE(28),
        NET_CHANGE(29),
        HIGH_52_WEEK(30),
        LOW_52_WEEK(31),
        PE_RATIO(32),
        DIVIDEND_AMOUNT(33),
        DIVIDEND_YEILD(34),
        ISLAND_BID_SIZE(35),
        ISLAND_ASK_SIZE(36),
        NAV(37),
        FUND_PRICE(38),
        EXCHANGED_NAME(39),
        DIVIDEND_DATE(40),
        REGULAR_MARKET_QUOTE(41),
        REGULAR_MARKET_TRADE(42),
        REGULAR_MARKET_LAST_PRICE(43),
        REGULAR_MARKET_LAST_SIZE(44),
        REGULAR_MARKET_TRADE_TIME(45),
        REGULAR_MARKET_TRADE_DAY(46),
        REGULAR_MARKET_NET_CHANGE(47),
        SECURITY_STATUS(48),
        MARK(49),
        QUOTE_TIME_AS_LONG(50),
        TRADE_TIME_AS_LONG(51),
        REGULAR_MARKET_TRADE_TIME_AS_LONG(52);
                
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
                    TDAmeritradeAPI.getCLib()::QuotesSubscriptionField_to_string_ABI);
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
    
    public QuotesSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._QuotesSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._QuotesSubscription_C>)
                   TDAmeritradeAPI.getCLib()::QuotesSubscription_Create_ABI );      
    }
    
    public QuotesSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
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
    protected CLib._QuotesSubscription_C
    getProxy(){
        return (CLib._QuotesSubscription_C)super.getProxy();
    }
    
    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::QuotesSubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::QuotesSubscription_SetFields_ABI);         
    }
    

}
