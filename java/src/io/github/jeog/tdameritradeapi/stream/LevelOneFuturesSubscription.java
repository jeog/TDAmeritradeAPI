package io.github.jeog.tdameritradeapi.stream;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class LevelOneFuturesSubscription extends SubscriptionBySymbolBase {

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
        QUOTE_TIME(10),
        TRADE_TIME(11),
        HIGH_PRICE(12),
        LOW_PRICE(13),
        CLOSE_PRICE(14),
        EXCHANGE_ID(15),
        DESCRIPTION(16),
        LAST_ID(17),
        OPEN_PRICE(18), 
        NET_CHANGE(19),
        FUTURE_PERCENT_CHANGE(20),
        EXCHANGE_NAME(21),
        SECURITY_STATUS(22),
        OPEN_INTEREST(23),
        MARK(24),
        TICK(25),
        TICK_AMOUNT(26),
        PRODUCT(27),
        FUTURE_PRICE_FORMAT(28),
        FUTURE_TRADING_HOURS(29),
        FUTURE_IS_TRADABLE(30),
        FUTURE_MULTIPLIER(31),
        FUTURE_IS_ACTIVE(32),
        FUTURE_SETTLEMENT_PRICE(33),
        FUTURE_ACTIVE_SYMBOL(34),
        FUTURE_EXPIRATION_DATE(35);
                
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
                    TDAmeritradeAPI.getCLib()::LevelOneFuturesSubscriptionField_to_string_ABI);
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
    
    public LevelOneFuturesSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._LevelOneFuturesSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._LevelOneFuturesSubscription_C>)
                   TDAmeritradeAPI.getCLib()::LevelOneFuturesSubscription_Create_ABI );      
    }
    
    public LevelOneFuturesSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
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
    protected CLib._LevelOneFuturesSubscription_C
    getProxy(){
        return (CLib._LevelOneFuturesSubscription_C)super.getProxy();
    }
    
    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::LevelOneFuturesSubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::LevelOneFuturesSubscription_SetFields_ABI);         
    }
    

}
