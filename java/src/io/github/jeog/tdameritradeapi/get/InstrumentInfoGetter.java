package io.github.jeog.tdameritradeapi.get;

import org.json.JSONObject;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class InstrumentInfoGetter extends APIGetter {


    public enum InstrumentSearchType implements CLib.ConvertibleEnum {
        SYMBOL_EXACT(0),
        SYMBOL_SEARCH(1),
        SYMBOL_REGEX(2),
        DESCRIPTION_SEARCH(3),
        DESCRIPTION_REGEX(4),
        CUSIP(5);
            
        private int value;
        
        InstrumentSearchType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static InstrumentSearchType
        fromInt(int i) {
            for(InstrumentSearchType d : InstrumentSearchType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::InstrumentSearchType_to_string_ABI);
        }    
    };
    
    public InstrumentInfoGetter( Credentials creds, InstrumentSearchType searchType, String queryString ) 
            throws CLibException {
        super( create(creds, searchType, queryString) );
    }

    @Override
    public JSONObject
    get() throws  CLibException {        
        return new JSONObject( getRaw() );
    }
    
    public InstrumentSearchType
    getSearchType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::InstrumentInfoGetter_GetSearchType_ABI);
        return InstrumentSearchType.fromInt(i);
    }
     
    public String
    getQueryString() throws CLibException{
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::InstrumentInfoGetter_GetQueryString_ABI);
    }
    
    public void
    setQuery(InstrumentSearchType searchType, String queryString) throws CLibException{
        int err = TDAmeritradeAPI.getCLib().InstrumentInfoGetter_SetQuery_ABI(getProxy(), 
                searchType.toInt(), queryString, 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    protected CLib._InstrumentInfoGetter_C
    getProxy() {
        return (CLib._InstrumentInfoGetter_C)super.getProxy();
    }
    
    private static CLib._InstrumentInfoGetter_C
    create( Credentials creds, InstrumentSearchType searchType, String queryString) throws CLibException{
        CLib._InstrumentInfoGetter_C pGetter = new CLib._InstrumentInfoGetter_C();        
        int err = TDAmeritradeAPI.getCLib().InstrumentInfoGetter_Create_ABI( creds.getNativeCredentials(), 
                searchType.toInt(), queryString, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
    
    
}
