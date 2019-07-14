package io.github.jeog.tdameritradeapi.stream;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class ChartEquitySubscription extends SubscriptionBySymbolBase {
    
    public enum FieldType implements CLib.ConvertibleEnum{
        SYMBOL(0),
        OPEN_PRICE(1), 
        HIGH_PRICE(2),
        LOW_PRICE(3),
        CLOSE_PRICE(4),
        VOLUME(5),
        SEQUENCE(6), 
        CHART_TIME(7), 
        CHART_DAY(8);
        
        public static final Set<FieldType> ALL = new HashSet<FieldType>();
        static {
            for( FieldType f : FieldType.values() )
                ALL.add(f);
        }
                
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
                    TDAmeritradeAPI.getCLib()::ChartEquitySubscriptionField_to_string_ABI);
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
    
    public ChartEquitySubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._ChartEquitySubscription_C(),
                (SymbolAndFieldCreatable<CLib._ChartEquitySubscription_C>)
                    TDAmeritradeAPI.getCLib()::ChartEquitySubscription_Create_ABI );      
    }
    
    public ChartEquitySubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
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
    protected CLib._ChartEquitySubscription_C
    getProxy(){
        return (CLib._ChartEquitySubscription_C)super.getProxy();
    }

    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::ChartEquitySubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::ChartEquitySubscription_SetFields_ABI);         
    }

}
