package io.github.jeog.tdameritradeapi.get;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;


public class APIGetter implements AutoCloseable {    
    private CLib._Getter_C pGetter;
    
    protected APIGetter(CLib._Getter_C pGetter){
        this.pGetter = pGetter;    
    }

    protected CLib._Getter_C
    getProxy(){
        return pGetter;
    }
        
    public Object
    get() throws  CLibException {        
        String j = CLib.Helpers.getString( pGetter, TDAmeritradeAPI.getCLib()::APIGetter_Get_ABI );
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
    
    static public long
    waitRemaining() throws  CLibException {
        return CLib.Helpers.getLong( TDAmeritradeAPI.getCLib()::APIGetter_WaitRemaining_ABI);
    }
    

}
