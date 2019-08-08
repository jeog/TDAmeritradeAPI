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

public class UserPrincipalsGetter extends APIGetter {
    
    public UserPrincipalsGetter( Credentials creds, boolean returnSubscriptionKeys, boolean returnConnectionInfo,
            boolean returnPreferences, boolean returnSurrogateIDs ) throws CLibException {
         super( create(creds, returnSubscriptionKeys, returnConnectionInfo, returnPreferences, returnSurrogateIDs) );
    }
  
    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public boolean
    returnsSubscriptionKeys() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnsSubscriptionKeys_ABI) == 1;   
    }
    
    public void
    returnSubscriptionKeys(boolean returnSubscriptionKeys ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), (returnSubscriptionKeys ? 1 : 0),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnSubscriptionKeys_ABI);
  }
  
    public boolean
    returnsConnectionInfo() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnsConnectionInfo_ABI) == 1;   
    }
  
    public void
    returnConnectionInfo(boolean returnConnectionInfo ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), (returnConnectionInfo ? 1 : 0),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnConnectionInfo_ABI);
    }
 
    public boolean
    returnsPreferences() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnsPreferences_ABI) == 1;   
    }
  
    public void
    returnPreferences(boolean returnPreferences ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), (returnPreferences ? 1 : 0),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnPreferences_ABI);
    }
  
    public boolean
    returnsSurrogateIDs() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnsSurrogateIds_ABI) == 1;   
    }
  
    public void
    returnSurrogateIDs(boolean returnSurrogateIDs ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), (returnSurrogateIDs ? 1 : 0),
              TDAmeritradeAPI.getCLib()::UserPrincipalsGetter_ReturnSurrogateIds_ABI);
    }

    protected CLib._UserPrincipalsGetter_C
    getProxy(){
        return (CLib._UserPrincipalsGetter_C)super.getProxy(); 
    }
  
    static private CLib._UserPrincipalsGetter_C
    create( Credentials creds, boolean returnSubscriptionKeys, boolean returnConnectionInfo, 
            boolean returnPreferences, boolean returnSurrogateIDs ) throws CLibException {
        CLib._UserPrincipalsGetter_C pGetter = new CLib._UserPrincipalsGetter_C();        
      
        int err = TDAmeritradeAPI.getCLib().UserPrincipalsGetter_Create_ABI(creds.getNativeCredentials(),
                returnSubscriptionKeys ? 1 : 0, returnConnectionInfo ? 1 : 0, returnPreferences ? 1 : 0,
                        returnSurrogateIDs ? 1 : 0, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);
      
        return pGetter;
    }
  
}
