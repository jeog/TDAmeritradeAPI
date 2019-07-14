package io.github.jeog.tdameritradeapi.stream;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class OptionsSubscription extends SubscriptionBySymbolBase {
    
    public enum FieldType implements CLib.ConvertibleEnum{
        SYMBOL(0),
        DESCRIPTION(1),
        BID_PRICE(2),
        ASK_PRICE(3),
        LAST_PRICE(4),    
        HIGH_PRICE(5),
        LOW_PRICE(6),
        CLOSE_PRICE(7),
        TOTAL_VOLUME(8),
        OPEN_INTEREST(9),
        VOLATILITY(10),
        QUOTE_TIME(11),
        TRADE_TIME(12),
        MONEY_INTRINSIC_VALUE(13),
        QUOTE_DAY(14),
        TRADE_DAY(15),
        EXPIRATION_YEAR(16),
        MULTIPLIER(17),
        DIGITS(18),
        OPEN_PRICE(19),
        BID_SIZE(20),
        ASK_SIZE(21),
        LAST_SIZE(22),
        NET_CHANGE(23),
        STRIKE_PRICE(24),
        CONTRACT_TYPE(25),
        UNDERLYING(26),
        EXPIRATION_MONTH(27),
        DELIVERABLES(28),
        TIME_VALUE(29),
        EXPIRATION_DAY(30),
        DAYS_TO_EXPIRATION(31),
        DELTA(32),
        GAMMA(33),
        FIELD_THETA(34),
        VEGA(35),
        RHO(36),
        SECURITY_STATUS(37),
        THEORETICAL_OPTION_VALUE(38),
        UNDERLYING_PRICE(39),
        UV_EXPIRATION_TYPE(40),
        MARK(41);
                
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
            return CLib.Helpers.convertibleEnumToString(this, 
                    TDAmeritradeAPI.getCLib()::OptionsSubscriptionField_to_string_ABI);
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
    
    public OptionsSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._OptionsSubscription_C(),
                (SymbolAndFieldCreatable<CLib._OptionsSubscription_C>)
                    TDAmeritradeAPI.getCLib()::OptionsSubscription_Create_ABI );      
    }
    
    public OptionsSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
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
    protected CLib._OptionsSubscription_C
    getProxy(){
        return (CLib._OptionsSubscription_C)super.getProxy();
    }

    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionsSubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::OptionsSubscription_SetFields_ABI);         
    }


}
