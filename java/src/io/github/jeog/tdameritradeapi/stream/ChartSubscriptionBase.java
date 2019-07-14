package io.github.jeog.tdameritradeapi.stream;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public abstract class ChartSubscriptionBase extends SubscriptionBySymbolBase {

    public enum FieldType implements CLib.ConvertibleEnum{
        SYMBOL(0),
        CHART_TIME(1),
        OPEN_PRICE(2), 
        HIGH_PRICE(3),
        LOW_PRICE(4),
        CLOSE_PRICE(5),
        VOLUME(6);        
        
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
                    TDAmeritradeAPI.getCLib()::ChartSubscriptionField_to_string_ABI);
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
    
    protected <O extends CLib._StreamingSubscription_C, F extends CLib.ConvertibleEnum> 
    ChartSubscriptionBase( Set<String> symbols, Set<F> fields, CommandType command, 
            O pSub, SymbolAndFieldCreatable<O> createFunc ) throws CLibException{ 
        super( symbols, fields, command, pSub, createFunc );
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
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::ChartSubscriptionBase_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::ChartSubscriptionBase_SetFields_ABI);         
    }


}
