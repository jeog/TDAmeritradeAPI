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

public class MarketHoursGetter extends APIGetter {

    public enum MarketType implements CLib.ConvertibleEnum {
        EQUITY(0),
        OPTION(1),
        FUTURE(2),
        BOND(3),
        FOREX(4);        
            
        private int value;
        
        MarketType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static MarketType
        fromInt(int i) {
            for(MarketType d : MarketType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::MarketType_to_string_ABI);
        }    
    };
    
    public MarketHoursGetter( Credentials creds, MarketType marketType, String date) throws CLibException {
        super( create(creds, marketType, date) );
    }
    
    
    public String
    getDate() throws CLibException{
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::MarketHoursGetter_GetDate_ABI);
    }
    
    public void
    setDate(String date) throws CLibException{
        CLib.Helpers.setString( getProxy(), date,
                TDAmeritradeAPI.getCLib()::MarketHoursGetter_SetDate_ABI);
    }
    
    public MarketType
    getMarketType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::MarketHoursGetter_GetMarketType_ABI);
        return MarketType.fromInt(i);
    }
    
    public void
    setMarketType( MarketType orderStatusType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), orderStatusType.toInt(),
                TDAmeritradeAPI.getCLib()::MarketHoursGetter_SetMarketType_ABI);        
    }
    
    protected CLib._MarketHoursGetter_C
    getProxy() {
        return (CLib._MarketHoursGetter_C)super.getProxy();
    }
    
    private static CLib._MarketHoursGetter_C
    create( Credentials creds, MarketType marketType, String date) throws CLibException{
        CLib._MarketHoursGetter_C pGetter = new CLib._MarketHoursGetter_C();        
        int err = TDAmeritradeAPI.getCLib().MarketHoursGetter_Create_ABI( creds.getNativeCredentials(), 
                marketType.toInt(), date, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
}
