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


public class OptionChainAnalyticalGetter extends OptionChainGetter {

    public OptionChainAnalyticalGetter(Credentials creds, String symbol, double volatility,
            double underlyingPrice, double interestRate, int daysToExp, OptionStrikes strikes, 
            OptionContractType contractType, boolean includeQuotes, String fromDate, String toDate, 
            OptionExpMonth expMonth, OptionType optionType) throws CLibException {
        super( create(creds, symbol, volatility, underlyingPrice, interestRate, daysToExp, strikes, 
                contractType, includeQuotes, fromDate, toDate, expMonth, optionType) );
        
    }
    
    public double 
    getVolatility() throws CLibException {
        return CLib.Helpers.getDouble(getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_GetVolatility_ABI);
    }
    
    public void
    setVolatility( double volatility ) throws CLibException {
        CLib.Helpers.setDouble(getProxy(), volatility,
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_SetVolatility_ABI);
    }
    
    public double 
    getUnderlyingPrice() throws CLibException {
        return CLib.Helpers.getDouble(getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_GetUnderlyingPrice_ABI);
    }
    
    public void
    setUnderlyingPrice( double underlyingPrice ) throws CLibException {
        CLib.Helpers.setDouble(getProxy(), underlyingPrice,
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_SetUnderlyingPrice_ABI);
    }   
    
    public double 
    getInterestRate() throws CLibException {
        return CLib.Helpers.getDouble(getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_GetInterestRate_ABI);
    }
    
    public void
    setInterestRate( double interestRate ) throws CLibException {
        CLib.Helpers.setDouble(getProxy(), interestRate,
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_SetInterestRate_ABI);
    }   
    
    public int
    getDaysToExp() throws CLibException {
        return CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_GetDaysToExp_ABI);
    }
    
    public void
    setDaysToExp( int daysToExp ) throws CLibException {
        CLib.Helpers.setInt(getProxy(), daysToExp,
                TDAmeritradeAPI.getCLib()::OptionChainAnalyticalGetter_SetDaysToExp_ABI);
    }   
    
    
    protected CLib._OptionChainAnalyticalGetter_C
    getProxy(){
        return (CLib._OptionChainAnalyticalGetter_C)super.getProxy(); 
    }      

    static private CLib._OptionChainAnalyticalGetter_C
    create( Credentials creds, String symbol, double volatility, double underlyingPrice, double interestRate, 
            int daysToExp, OptionStrikes strikes, OptionContractType contractType, boolean includeQuotes, 
            String fromDate, String toDate, OptionExpMonth expMonth, OptionType optionType) throws CLibException 
    {
        CLib._OptionChainAnalyticalGetter_C pGetter = new CLib._OptionChainAnalyticalGetter_C();  
        
        int err = TDAmeritradeAPI.getCLib().OptionChainAnalyticalGetter_Create_ABI(creds.getNativeCredentials(), 
                symbol, volatility, underlyingPrice, interestRate, daysToExp, strikes.getType().toInt(), 
                buildOptionStrikesValueByValue( strikes), contractType.toInt(), (includeQuotes ? 1 : 0), 
                fromDate, toDate, expMonth.toInt(), optionType.toInt(), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);
        
        return pGetter;
    }
    
    
}
