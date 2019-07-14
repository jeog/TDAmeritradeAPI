package io.github.jeog.tdameritradeapi;

import java.util.HashMap;
import java.util.Map;

import com.sun.jna.ptr.PointerByReference;

import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class Error {
    private static final Map<Integer,String> NAMES;
    static{
        NAMES = new HashMap<Integer, String>();
        NAMES.put(0, "NONE");
        NAMES.put(1, "TDMA_API_ERROR");
        NAMES.put(2, "TDMA_API_CRED_ERROR");
        NAMES.put(3, "TDMA_API_VALUE_ERROR");
        NAMES.put(4, "TDMA_API_TYPE_ERROR");
        NAMES.put(5, "TDMA_API_MEMORY_ERROR");
        NAMES.put(101, "TDMA_API_CONNECT_ERROR");
        NAMES.put(102, "TDMA_API_AUTH_ERROR");
        NAMES.put(103, "TDMA_API_REQUEST_ERROR");
        NAMES.put(104, "TDMA_API_SERVER_ERROR");
        NAMES.put(201, "TDMA_API_STREAM_ERROR");
        NAMES.put(301, "TDMA_API_EXECUTE_ERROR");
        NAMES.put(501, "TDMA_API_STD_EXCEPTION");
        NAMES.put(1001, "TDMA_API_UNKNOWN_EXCEPTION");        
    };        
    
    public static String
    getErrorName(int errorCode) {
        return NAMES.get(errorCode);
    }    
    
    public static int
    lastErrorCode() throws CLibException {
        return CLib.Helpers.getInt(TDAmeritradeAPI.getCLib()::LastErrorCode_ABI);    
    }
    
    public static int
    lastErrorLineNumber() throws CLibException  {
        return CLib.Helpers.getInt(TDAmeritradeAPI.getCLib()::LastErrorLineNumber_ABI);    
    }
    
    public static String
    lastErrorMessage() throws CLibException  {
        return CLib.Helpers.getString(TDAmeritradeAPI.getCLib()::LastErrorMsg_ABI);    
    }
    
    public static String
    lastErrorFilename() throws CLibException  {
        return CLib.Helpers.getString(TDAmeritradeAPI.getCLib()::LastErrorFilename_ABI);    
    }    
    
    // DOESN'T THROW - returns null if no library, "N/A" for value of failed call
    public static Map<String, String>
    lastErrorInfo() {        
        Map<String, String> info = new HashMap<String,String>();
        CLib lib;
        try {
            lib = TDAmeritradeAPI.getCLib();
            
            int eCode = _getInt(lib::LastErrorCode_ABI);  
            info.put("code", eCode == -1 ? "N/A" : String.valueOf(eCode));
            
            int eLineno = _getInt(lib::LastErrorLineNumber_ABI);
            info.put("lineNumber", eLineno == -1 ? "N/A" : String.valueOf(eLineno));
            
            String eMessage = _getString(lib::LastErrorMsg_ABI);
            info.put("message",  eMessage == null ? "N/A" : eMessage);
            
            String eFilename = _getString(lib::LastErrorFilename_ABI);              
            info.put("filename",  eFilename == null ? "N/A" : eFilename);            
        }catch( Throwable t ) {
            System.err.println("lastErrorInfo failed: " + t.getMessage());
            return null;
        }
        return info;
    }

    private static int
    _getInt(CLib.Helpers.FunctionType.GetInt method) {
        int e[] = {0};
        int err = method.call(e, 0); 
        return (err == 0) ? e[0] : -1;                
    }        
    
    private static String 
    _getString(CLib.Helpers.FunctionType.GetString method) {
        PointerByReference p = new PointerByReference();
        CLib.size_t n[] = new CLib.size_t[1];
        int err = method.call(p, n, 0); 
        if( err != 0 )
            return null;
        String s = new String( p.getValue().getByteArray(0, n[0].intValue() - 1) );
        err = TDAmeritradeAPI.getCLib().FreeBuffer_ABI(p.getValue(), 0);
        if( err != 0 ) 
            System.err.println("Failed to free string memory in Error._getString");        
        return s;    
    }


}
