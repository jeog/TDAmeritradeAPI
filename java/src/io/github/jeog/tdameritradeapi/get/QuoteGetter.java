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

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class QuoteGetter extends APIGetter{

    static private CLib._QuoteGetter_C
    create( Credentials creds, String symbol ) throws  CLibException {    
        CLib._QuoteGetter_C getter = new CLib._QuoteGetter_C();    
        int err = TDAmeritradeAPI.getCLib().QuoteGetter_Create_ABI( creds.getNativeCredentials(), 
                symbol, getter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return getter;
    }
    
    public QuoteGetter( Credentials creds, String symbol ) throws  CLibException {
        super( create(creds, symbol) ) ;
    }
    
    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }

    public String
    getSymbol() throws  CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::QuoteGetter_GetSymbol_ABI);
    }
    
    public void
    setSymbol(String symbol) throws  CLibException {
        CLib.Helpers.setString( getProxy(), symbol, 
                TDAmeritradeAPI.getCLib()::QuoteGetter_SetSymbol_ABI);
    }
    
    @Override
    protected CLib._QuoteGetter_C
    getProxy(){
        return (CLib._QuoteGetter_C)super.getProxy();
    }
}
