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

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public abstract class HistoricalGetterBase extends APIGetter {
   
    public enum FrequencyType implements CLib.ConvertibleEnum {
        MINUTE(0),
        DAILY(1),
        WEEKLY(2),
        MONTHLY(3);
            
        private int value;
        
        FrequencyType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static FrequencyType
        fromInt(int i) {
            for(FrequencyType d : FrequencyType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::FrequencyType_to_string_ABI);
        }    
    };
    
    public static final Map<FrequencyType, Set<Integer>> VALID_FREQUENCIES_BY_FREQUENCY_TYPE =
            new HashMap<FrequencyType, Set<Integer>>();
    static {      
        VALID_FREQUENCIES_BY_FREQUENCY_TYPE.put(FrequencyType.MINUTE, 
                new HashSet<Integer>( Arrays.asList(1,5,10,15,30) ) );
        VALID_FREQUENCIES_BY_FREQUENCY_TYPE.put(FrequencyType.DAILY, 
                new HashSet<Integer>( Arrays.asList(1) ) );
        VALID_FREQUENCIES_BY_FREQUENCY_TYPE.put(FrequencyType.WEEKLY, 
                new HashSet<Integer>( Arrays.asList(1) ) );
        VALID_FREQUENCIES_BY_FREQUENCY_TYPE.put(FrequencyType.MONTHLY, 
                new HashSet<Integer>( Arrays.asList(1) ) );
    }

   
    protected <T extends CLib._Getter_C>
    HistoricalGetterBase( T pGetter ) throws  CLibException {
        super( pGetter );
    }

    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public String
    getSymbol() throws  CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_GetSymbol_ABI);
    }
    
    public void
    setSymbol(String symbol) throws  CLibException {
        CLib.Helpers.setString( getProxy(), symbol, 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_SetSymbol_ABI);
    }
 
    public int
    getFrequency() throws CLibException {
        return CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_GetFrequency_ABI);        
    }    
   
    public FrequencyType
    getFrequencyType() throws CLibException {
        int f = CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_GetFrequencyType_ABI);
        return FrequencyType.fromInt(f);
    }
    
    public void
    setFrequency( FrequencyType frequencyType, int frequency ) throws CLibException {
        int err = TDAmeritradeAPI.getCLib().HistoricalGetterBase_SetFrequency_ABI(
                getProxy(), frequencyType.toInt(), frequency, 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    public boolean
    isExtendedHours() throws CLibException {
        return CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_IsExtendedHours_ABI) == 1;        
    }   
    
    public void
    setExtendedHours(boolean extendedHours) throws CLibException {
        CLib.Helpers.setInt(getProxy(), extendedHours ? 1 : 0, 
                TDAmeritradeAPI.getCLib()::HistoricalGetterBase_SetExtendedHours_ABI);
    }
 
    public static boolean
    isValidFrequency( FrequencyType frequencyType, int frequency ) {
        Set<Integer> valid = VALID_FREQUENCIES_BY_FREQUENCY_TYPE.get(frequencyType);
        return (valid != null) && valid.contains(frequency);
    }
}
