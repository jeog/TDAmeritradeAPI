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

public class NewsHeadlineSubscription extends SubscriptionBySymbolBase {

    public enum FieldType implements CLib.ConvertibleEnum {
        SYMBOL(0),
        ERROR_CODE(1), 
        STORY_DATETIME(2),
        HEADLINE_ID(3),
        STATUS(4),
        HEADLINE(5), 
        STORY_ID(6),
        COUNT_FOR_KEYWORD(7),
        KEYWORD_ARRAY(8),
        IS_HOST(9),
        STORY_SOURCE(10);
        
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
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::NewsHeadlineSubscriptionField_to_string_ABI);
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
    
    public NewsHeadlineSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException {
        super( symbols, fields, command, new CLib._NewsHeadlineSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._NewsHeadlineSubscription_C>)
                   TDAmeritradeAPI.getCLib()::NewsHeadlineSubscription_Create_ABI );      
    }
    
    public NewsHeadlineSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException {
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
    protected CLib._NewsHeadlineSubscription_C
    getProxy(){
        return (CLib._NewsHeadlineSubscription_C)super.getProxy();
    }
    
    @Override
    protected int[] 
    getFieldsAsInts() throws CLibException {
        return CLib.Helpers.getFields( getProxy(), 
                TDAmeritradeAPI.getCLib()::NewsHeadlineSubscription_GetFields_ABI);         
    }
    
    @Override
    protected void
    setFieldsAsInts(int[] ints) throws CLibException{
        CLib.Helpers.setFields( getProxy(), ints, 
                TDAmeritradeAPI.getCLib()::NewsHeadlineSubscription_SetFields_ABI);         
    }
    


}
