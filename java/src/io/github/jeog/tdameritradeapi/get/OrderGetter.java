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

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class OrderGetter extends AccountGetterBase {

    public OrderGetter( Credentials creds, String accountID, String orderID ) throws CLibException {
        super( create(creds, accountID, orderID) );
    }
    
    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public String
    getOrderID() throws CLibException{
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OrderGetter_GetOrderId_ABI);
    }
    
    public void
    setOrderID(String orderID) throws CLibException{
        CLib.Helpers.setString( getProxy(), orderID,
                TDAmeritradeAPI.getCLib()::OrderGetter_SetOrderId_ABI);
    }   
    
    protected CLib._OrderGetter_C
    getProxy() {
        return (CLib._OrderGetter_C)super.getProxy();
    }
    
    private static CLib._OrderGetter_C
    create( Credentials creds, String accountID, String orderID ) throws CLibException{
        CLib._OrderGetter_C pGetter = new CLib._OrderGetter_C();        
        int err = TDAmeritradeAPI.getCLib().OrderGetter_Create_ABI( creds.getNativeCredentials(), 
                accountID, orderID, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
    
}
