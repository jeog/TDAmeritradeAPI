package io.github.jeog.tdameritradeapi.get;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
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
