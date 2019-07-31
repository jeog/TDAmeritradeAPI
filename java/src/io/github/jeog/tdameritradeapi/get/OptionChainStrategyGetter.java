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
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class OptionChainStrategyGetter extends OptionChainGetter {

    public enum OptionStrategyType implements CLib.ConvertibleEnum {
        COVERED(0),
        VERTICAL(1),
        CALENDAR(2),
        STRANGLE(3),
        STRADDLE(4),
        BUTTERFLY(5),
        CONDOR(6),
        DIAGONAL(7),
        COLLAR(8),
        ROLL(9);

        private int value;
        
        OptionStrategyType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OptionStrategyType
        fromInt(int i) {
            for(OptionStrategyType o : OptionStrategyType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OptionStrategyType_to_string_ABI);
        }    
    };
    
    public static class OptionStrategy{
        private OptionStrategyType strategyType;
        private double spreadInterval;
        
        public OptionStrategyType
        getStrategy() {
            return strategyType;
        }
        
        public double
        getSpreadInterval() {
            return spreadInterval;
        }
        
        public OptionStrategy(OptionStrategyType strategyType, double spreadInterval) {
            this.strategyType = strategyType;
            this.spreadInterval = spreadInterval;
        }
        
        public static OptionStrategy
        buildCovered() { 
            return new OptionStrategy(OptionStrategyType.COVERED, 0); 
        }
        
        public static OptionStrategy
        buildCalendar() { 
            return new OptionStrategy(OptionStrategyType.CALENDAR, 0); 
        }
        
        public static OptionStrategy
        buildVertical( double spreadInterval ) { 
            return new OptionStrategy(OptionStrategyType.VERTICAL, spreadInterval); 
        }
        
        public static OptionStrategy
        buildVertical() { 
            return new OptionStrategy(OptionStrategyType.VERTICAL, 1.0); 
        }
        
        public static OptionStrategy
        buildStrangle(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.STRANGLE, spreadInterval); 
        }
        
        public static OptionStrategy
        buildStrangle() { 
            return new OptionStrategy(OptionStrategyType.STRANGLE, 1.0); 
        }
        
        public static OptionStrategy
        buildStraddle(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.STRADDLE ,spreadInterval); 
        }
        
        public static OptionStrategy
        buildStraddle() { 
            return new OptionStrategy(OptionStrategyType.STRADDLE, 1.0); 
        }
        
        public static OptionStrategy
        buildButterfly(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.BUTTERFLY ,spreadInterval); 
        }
        
        public static OptionStrategy
        buildButterfly() { 
            return new OptionStrategy(OptionStrategyType.BUTTERFLY, 1.0); 
        }
        
        public static OptionStrategy
        buildCondor(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.CONDOR ,spreadInterval); 
        }
        
        public static OptionStrategy
        buildCondor() { 
            return new OptionStrategy(OptionStrategyType.CONDOR, 1.0); 
        }
        
        public static OptionStrategy
        buildDiagonal(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.DIAGONAL, spreadInterval); 
        }
        
        public static OptionStrategy
        buildDiagonal() { 
            return new OptionStrategy(OptionStrategyType.DIAGONAL, 1.0); 
        }
        
        public static OptionStrategy
        buildCollar(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.COLLAR ,spreadInterval); 
        }
        
        public static OptionStrategy
        buildCollar() { 
            return new OptionStrategy(OptionStrategyType.COLLAR, 1.0); 
        }
        
        public static OptionStrategy
        buildRoll(double spreadInterval) { 
            return new OptionStrategy(OptionStrategyType.ROLL,spreadInterval); 
        }
        
        public static OptionStrategy
        buildRoll() { 
            return new OptionStrategy(OptionStrategyType.ROLL, 1.0); 
        }        
    }
    
    
    public OptionChainStrategyGetter(Credentials creds, String symbol, OptionStrategy strategy, 
            OptionStrikes strikes,OptionContractType contractType, boolean includeQuotes, String fromDate, 
            String toDate, OptionExpMonth expMonth, OptionType optionType) throws CLibException {
        super( create(creds, symbol, strategy, strikes, contractType, includeQuotes, 
                fromDate, toDate, expMonth, optionType) );        
    } 
    
    public OptionStrategy 
    getStrategy() throws CLibException {
        int i[] = {0};
        double d[] = {.0};
        int err = TDAmeritradeAPI.getCLib().OptionChainStrategyGetter_GetStrategy_ABI(getProxy(), i, d, 0);
        if( err != 0 )
            throw new CLibException(err);
        return new OptionStrategy( OptionStrategyType.fromInt(i[0]), d[0]);
    }
    
    public void
    setStrategy( OptionStrategy strategy ) throws CLibException{
        int err = TDAmeritradeAPI.getCLib().OptionChainStrategyGetter_SetStrategy_ABI(getProxy(), 
                strategy.getStrategy().toInt(), strategy.getSpreadInterval(), 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    protected CLib._OptionChainStrategyGetter_C
    getProxy(){
        return (CLib._OptionChainStrategyGetter_C)super.getProxy(); 
    }   
    
    static private CLib._OptionChainStrategyGetter_C
    create( Credentials creds, String symbol, OptionStrategy strategy, OptionStrikes strikes, 
            OptionContractType contractType, boolean includeQuotes, String fromDate, 
            String toDate, OptionExpMonth expMonth, OptionType optionType) throws CLibException 
    {
        CLib._OptionChainStrategyGetter_C pGetter = new CLib._OptionChainStrategyGetter_C();
        
        int err = TDAmeritradeAPI.getCLib().OptionChainStrategyGetter_Create_ABI(creds.getNativeCredentials(), 
                symbol, strategy.getStrategy().toInt(), strategy.getSpreadInterval(), strikes.getType().toInt(), 
                buildOptionStrikesValueByValue( strikes), contractType.toInt(), (includeQuotes ? 1 : 0), 
                fromDate, toDate, expMonth.toInt(), optionType.toInt(), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);
        
        return pGetter;
    }
    

}
