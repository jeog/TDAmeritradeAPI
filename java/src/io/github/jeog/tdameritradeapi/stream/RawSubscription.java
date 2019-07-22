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

import java.util.HashMap;
import java.util.Map;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.Error;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.CLib.KeyValPair;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class RawSubscription extends StreamingSubscription {
    
    public RawSubscription( String service, String command, Map<String,String> parameters) throws CLibException{  
        super( create(service,command,parameters) );
    } 
    
    public String
    getServiceString() throws CLibException {
        return CLib.Helpers.getString(getProxy(), 
                TDAmeritradeAPI.getCLib()::RawSubscription_GetServiceStr_ABI);
    }
    
    public void
    setServiceString(String service) throws CLibException {
        CLib.Helpers.setString(getProxy(), service,
                TDAmeritradeAPI.getCLib()::RawSubscription_SetServiceStr_ABI);
    }
    
    public String
    getCommandString() throws CLibException {
        return CLib.Helpers.getString(getProxy(), 
                TDAmeritradeAPI.getCLib()::RawSubscription_GetCommandStr_ABI);
    }
    
    public void
    setCommandString(String Command) throws CLibException {
        CLib.Helpers.setString(getProxy(), Command,
                TDAmeritradeAPI.getCLib()::RawSubscription_SetCommandStr_ABI);
    }
    
    public Map<String, String>
    getParameters() throws CLibException {
        PointerByReference p = new PointerByReference(); 
        CLib.size_t[] n = new CLib.size_t[1];  
        
        int err = TDAmeritradeAPI.getCLib().RawSubscription_GetParameters_ABI(getProxy(), p, n, 0);
        if( err != 0 )
            throw new CLibException(err);
        
        Pointer ptr = p.getValue(); 
        Map<String, String> params = null;
        try {
            params = new HashMap<String, String>();
            int i = n[0].intValue();            
            KeyValPair[] kvPairs = (KeyValPair[])(new KeyValPair(ptr).toArray(i) );            
            while( i-- > 0 )
                params.put(kvPairs[i].key, kvPairs[i].val);         
        }finally {
            err = TDAmeritradeAPI.getCLib().FreeKeyValBuffer_ABI(ptr, n[0], 0);
            if( err != 0 )
                System.err.println("Failed to free KeyVal memory in RawSubscription.getParameters" 
                        + Error.getErrorName(err));                       
        }
        
        return params;   
    }    
    
    public void
    setParameters(Map<String, String> parameters) throws CLibException {   
        KeyValPair[] kv = javaMapToKeyValPairs(parameters);
        int err = TDAmeritradeAPI.getCLib().RawSubscription_SetParameters_ABI(getProxy(), kv, 
                new CLib.size_t(kv.length), 0);
        if( err != 0 )
            throw new CLibException(err);                        
    }
        
    @Override
    protected CLib._RawSubscription_C
    getProxy(){
        return (CLib._RawSubscription_C)super.getProxy();
    }

    private static CLib._RawSubscription_C
    create(String service, String command, Map<String,String> parameters) throws CLibException{
        KeyValPair[] kv = javaMapToKeyValPairs(parameters);
        CLib._RawSubscription_C pSub = new CLib._RawSubscription_C();
        int err = TDAmeritradeAPI.getCLib().RawSubscription_Create_ABI(service, command, kv, 
                new CLib.size_t(kv.length), pSub, 0);
        if( err != 0 )
            throw new CLibException(err);
        
        return pSub;
    }
    
    static private KeyValPair[]
    javaMapToKeyValPairs(Map<String, String> parameters) {    
        CLib.KeyValPair tmp = new CLib.KeyValPair();
        CLib.KeyValPair[] kv = (CLib.KeyValPair[])tmp.toArray(parameters.size());
        int i = 0;
        for( String k : parameters.keySet() ) {
            kv[i].key = k;
            kv[i++].val = parameters.get(k);               
        }    
        return kv;
    }
    
    
}
