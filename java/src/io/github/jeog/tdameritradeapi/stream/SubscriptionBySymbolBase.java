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

import java.util.HashSet;
import java.util.Set;

import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;
import io.github.jeog.tdameritradeapi.CLib;

public abstract class SubscriptionBySymbolBase extends ManagedSubscriptionBase {

    protected <O extends CLib._StreamingSubscription_C, F extends CLib.ConvertibleEnum> 
    SubscriptionBySymbolBase( Set<String> symbols, Set<F> fields, CommandType command, 
            O pSub, SymbolAndFieldCreatable<O> createFunc ) throws CLibException{ 
        super( create(symbols, fields, command, pSub, createFunc) );
    }
    
    public void
    setSymbols(Set<String> symbols) throws CLibException {    
        CLib.Helpers.setStrings(getProxy(), symbols,
                TDAmeritradeAPI.getCLib()::SubscriptionBySymbolBase_SetSymbols_ABI);        
    }
    
    public Set<String>
    getSymbols() throws CLibException {    
        return CLib.Helpers.getStrings(getProxy(),
                TDAmeritradeAPI.getCLib()::SubscriptionBySymbolBase_GetSymbols_ABI);        
    }
    
    public Set<Integer>
    getRawFields() throws CLibException{
        int[] ints = getFieldsAsInts();
        Set<Integer> fields = new HashSet<Integer>();
        for(int i = 0; i < ints.length; ++i) 
            fields.add( ints[i] );
        return fields;   
    }
    
    public void
    setRawFields( Set<Integer> fields ) throws CLibException{
        setFieldsAsInts( fields.stream().mapToInt(Integer::intValue).toArray() ); 
    }
    
    protected abstract int[]
    getFieldsAsInts() throws CLibException;
    
    protected abstract void
    setFieldsAsInts(int[] ints) throws CLibException;
        
        
    protected static <T extends CLib.ConvertibleEnum> int[]
    fieldsToInts( Set<T> fields ) {
        int n = fields.size();
        int ints[] = new int[n];
        for( CLib.ConvertibleEnum f : fields )
            ints[--n] = f.toInt();
        return ints;
    }
    
    protected static interface SymbolAndFieldCreatable <O extends CLib._StreamingSubscription_C>{
        int create(String[] symbols, CLib.size_t nSymbols, int[] fields, CLib.size_t nFields,
                int command, O pSubscription, int exc);
    }
    
    private static <O extends CLib._StreamingSubscription_C, F extends CLib.ConvertibleEnum> O
    create( Set<String> symbols, Set<F> fields, CommandType command, O pSub, 
            SymbolAndFieldCreatable<O> createFunc ) throws CLibException{                     
        String[] cSymbols = CLib.Helpers.symbolsToStrings(symbols);                        
        int[] cFields = fieldsToInts(fields);                        
        int err = createFunc.create(cSymbols, new CLib.size_t(cSymbols.length), cFields, 
                new CLib.size_t(cFields.length), command.toInt(), pSub, 0);                
        if( err != 0 )
            throw new CLibException(err);                 
        return pSub;           
    }
     
}
