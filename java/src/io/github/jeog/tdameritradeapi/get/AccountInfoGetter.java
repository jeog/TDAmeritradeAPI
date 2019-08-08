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

import io.github.jeog.tdameritradeapi.Auth.Credentials;

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class AccountInfoGetter extends AccountGetterBase {

    public AccountInfoGetter( Credentials creds, String accountID, boolean positions, boolean orders ) 
            throws CLibException {
        super( create(creds, accountID, positions, orders) );
    }
    
    
    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public boolean
    returnsPositions() throws CLibException {
        return CLib.Helpers.getInt(getProxy(),
                TDAmeritradeAPI.getCLib()::AccountInfoGetter_ReturnsPositions_ABI) == 1;
    }
    
    public void
    returnPositions( boolean returnPositions ) throws CLibException {
        CLib.Helpers.setInt(getProxy(), returnPositions ? 1 : 0, 
                TDAmeritradeAPI.getCLib()::AccountInfoGetter_ReturnPositions_ABI);
    }
    
    public boolean
    returnsOrders() throws CLibException {
        return CLib.Helpers.getInt(getProxy(),
                TDAmeritradeAPI.getCLib()::AccountInfoGetter_ReturnsOrders_ABI) == 1;
    }
    
    public void
    returnOrders( boolean returnOrders ) throws CLibException {
        CLib.Helpers.setInt(getProxy(), returnOrders ? 1 : 0, 
                TDAmeritradeAPI.getCLib()::AccountInfoGetter_ReturnOrders_ABI);
    }
    
    
    protected CLib._AccountInfoGetter_C
    getProxy() {
        return (CLib._AccountInfoGetter_C)super.getProxy();
    }
    
    private static CLib._AccountInfoGetter_C
    create( Credentials creds, String accountID, boolean positions, boolean orders) throws CLibException{
        CLib._AccountInfoGetter_C pGetter = new CLib._AccountInfoGetter_C();        
        int err = TDAmeritradeAPI.getCLib().AccountInfoGetter_Create_ABI( creds.getNativeCredentials(), accountID, 
                positions ? 1 : 0, orders ? 1 : 0, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
}
