package io.github.jeog.tdameritradeapi.get;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class QuotesGetter extends APIGetter{

    static private CLib._QuotesGetter_C
    create( Credentials creds, Set<String> symbols ) throws  CLibException {    
        CLib._QuotesGetter_C getter = new CLib._QuotesGetter_C();        
        String[] cSymbols = new String[symbols.size()];
        cSymbols = symbols.toArray(cSymbols);
        int err = TDAmeritradeAPI.getCLib().QuotesGetter_Create_ABI( creds.getNativeCredentials(), 
                cSymbols, new CLib.size_t(cSymbols.length), getter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return getter;
    }
    
    public QuotesGetter( Credentials creds, Set<String> symbols ) throws  CLibException {
        super( create(creds, symbols) ) ;
    }

    public Set<String>
    getSymbols() throws  CLibException {
        return CLib.Helpers.getStrings(getProxy(), TDAmeritradeAPI.getCLib()::QuotesGetter_GetSymbols_ABI );             
    }
    
    public void
    setSymbols(Set<String> symbols) throws  CLibException {
        CLib.Helpers.setStrings(getProxy(), symbols, TDAmeritradeAPI.getCLib()::QuotesGetter_SetSymbols_ABI );
    }
    
    public void
    addSymbol(String symbol) throws CLibException {
        CLib.Helpers.setString(getProxy(), symbol, TDAmeritradeAPI.getCLib()::QuotesGetter_AddSymbol_ABI );
    }
    
    public void
    removeSymbol(String symbol) throws CLibException {
        CLib.Helpers.setString(getProxy(), symbol, TDAmeritradeAPI.getCLib()::QuotesGetter_RemoveSymbol_ABI );
    }
    
    public void
    addSymbols(Set<String> symbols) throws CLibException {
        CLib.Helpers.setStrings(getProxy(), symbols, TDAmeritradeAPI.getCLib()::QuotesGetter_AddSymbols_ABI );
    }
    
    public void
    removeSymbols(Set<String> symbols) throws CLibException {
        CLib.Helpers.setStrings(getProxy(), symbols, TDAmeritradeAPI.getCLib()::QuotesGetter_RemoveSymbols_ABI );
    }
    
    protected CLib._QuotesGetter_C
    getProxy(){
        return (CLib._QuotesGetter_C)super.getProxy();
    }
}
