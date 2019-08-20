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

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;


public class APIGetter implements AutoCloseable {    
    private CLib._Getter_C pGetter;
 
    public String
    getRaw() throws CLibException {
        return CLib.Helpers.getString( pGetter, TDAmeritradeAPI.getCLib()::APIGetter_Get_ABI );
    }
    
    public Object
    get() throws  CLibException {        
        String j = getRaw();
        try {
            return new JSONObject(j);
        }catch( JSONException exc ) {
            return new JSONArray(j);
        }
    }
    
    @Override
    public void
    close() throws  CLibException {
        int err = TDAmeritradeAPI.getCLib().APIGetter_Close_ABI(pGetter, 0);
        if( err != 0 )
            throw new CLibException(err); 
    }
    
    public boolean
    isClosed() throws  CLibException {
        int b = CLib.Helpers.getInt(pGetter, TDAmeritradeAPI.getCLib()::APIGetter_IsClosed_ABI);
        return b != 0;
    }
    
    static public void
    setWaitMSec( long msec ) throws  CLibException {
        CLib.Helpers.setLong(msec, TDAmeritradeAPI.getCLib()::APIGetter_SetWaitMSec_ABI);
    }
    
    static public long
    getWaitMSec() throws  CLibException {
        return CLib.Helpers.getLong( TDAmeritradeAPI.getCLib()::APIGetter_GetWaitMSec_ABI);
    }
    
    static public long
    getDefaultWaitMSec() throws  CLibException {
        return CLib.Helpers.getLong( TDAmeritradeAPI.getCLib()::APIGetter_GetDefWaitMSec_ABI);        
    }
    
    static public void
    shareConnections(boolean share) throws CLibException{
        CLib.Helpers.setInt( share ? 1 : 0,
                TDAmeritradeAPI.getCLib()::APIGetter_ShareConnections_ABI);
    }
    
    static public boolean
    isSharingConnections() throws CLibException{
        return CLib.Helpers.getInt( TDAmeritradeAPI.getCLib()::APIGetter_IsSharingConnections_ABI) == 1;
        
    }
    
    static public long
    waitRemaining() throws  CLibException {
        return CLib.Helpers.getLong( TDAmeritradeAPI.getCLib()::APIGetter_WaitRemaining_ABI);
    }
        
    protected APIGetter(CLib._Getter_C pGetter){
        this.pGetter = pGetter;    
    }

    protected CLib._Getter_C
    getProxy(){
        return pGetter;
    }
       
}
