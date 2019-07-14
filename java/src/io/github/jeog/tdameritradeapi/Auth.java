package io.github.jeog.tdameritradeapi;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Memory;
import com.sun.jna.Structure;

import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class Auth {
    
    public static final String DEF_REDIRECT_URI = "https://127.0.0.1";
    
    public static class Credentials {
        
        // class needs to be public for jna access
        public static class _Credentials extends Structure {        
            public String accessToken;
            public String refreshToken;
            public long epochSecTokenExpiration;
            public String clientID;    
    
            private _Credentials() { 
                super();                        
            }            
                    
            @Override
            protected List<String> 
            getFieldOrder() {
                return Arrays.asList("accessToken", "refreshToken", "epochSecTokenExpiration", "clientID");
            }
    
            protected Memory autoAllocate(int size) {
                return new Memory(size) {
                    protected void finalize() {
                        try {
                            if( accessToken != null ) {                                                    
                                int err = TDAmeritradeAPI.getCLib().CloseCredentials_ABI(this, 0);
                                if( err != 0 )            
                                    throw new CLibException(err);                
                            }            
                        }catch( Throwable t ) {            
                            System.err.println("Credentials finalize() error: " + t.getMessage());
                        }finally {
                            super.finalize();
                        }                    
                    }
                };
            }
        }
        
        private _Credentials _credentials;    
        
        private Credentials() {
            _credentials = new _Credentials();
        }    
        
        public Credentials( String accessToken, String refreshToken, 
                long epochSecTokenExpiration, String clientID) throws  CLibException {
            this();        
            int err = TDAmeritradeAPI.getCLib().CreateCredentials_ABI(accessToken, 
                    refreshToken, epochSecTokenExpiration, clientID, _credentials, 0);
            if( err != 0 ) 
                throw new CLibException(err);        
                                
        }                
        
        /* NOTE - shouldn't be used directly by client code */
        public _Credentials
        getNativeCredentials() {
            return _credentials;     
        }
        
        public String
        getAccessToken() {
            return _credentials.accessToken;
        }
        
        public void
        setAccessToken(String accessToken) throws  CLibException {        
            resetCredentials(accessToken, new String(_credentials.refreshToken), 
                    _credentials.epochSecTokenExpiration, new String(_credentials.clientID) );        
        }
        
        public String
        getRefreshToken() {
            return _credentials.refreshToken;
        }
        
        public void
        setRefreshToken(String refreshToken) throws  CLibException {        
            resetCredentials(new String(_credentials.accessToken), refreshToken, 
                    _credentials.epochSecTokenExpiration, new String(_credentials.clientID) );        
        }
        
        public long
        getEpochSecTokenExpiration() {
            return _credentials.epochSecTokenExpiration;
        }
        
        public void
        setEpochSecTokenExpiration( long epochSecTokenExpiration ) throws  CLibException {        
            resetCredentials(new String(_credentials.accessToken), new String(_credentials.refreshToken), 
                    epochSecTokenExpiration, new String(_credentials.clientID) );        
        }
        
        public String
        getClientID() {
            return _credentials.clientID;
        }
         
        public void
        setClientID( String clientID ) throws  CLibException {        
            resetCredentials(new String(_credentials.accessToken), new String(_credentials.refreshToken), 
                    _credentials.epochSecTokenExpiration, clientID );                
        }    
        
        private void
        resetCredentials(String accessToken, String refreshToken, long epochSecTokenExpiration, 
                String clientID) throws  CLibException {            
            int err = TDAmeritradeAPI.getCLib().CloseCredentials_ABI(_credentials, 0);
            if( err != 0 )
                throw new CLibException(err);
        
            err = TDAmeritradeAPI.getCLib().CreateCredentials_ABI(accessToken, 
                    refreshToken, epochSecTokenExpiration, clientID, _credentials, 0);
            if( err != 0 )
                throw new CLibException(err);
        }
    };
    
    
    public static class CredentialsManager implements AutoCloseable {
        private Credentials credentials;        
        private String path;
        private String password;
        
        public CredentialsManager(String path, String password) throws CLibException { 
            this.path = path;
            this.password = password;
            this.credentials = loadCredentials(path, password);
        }
        
        @Override
        public void close() throws CLibException {
            storeCredentials(path, password, credentials);            
        }
        
        public Credentials getCredentials() { return credentials; }
        public String getPath() { return path; }
        public void setPath(String path) { this.path = path; }
        public String getPassword() { return password; }
        public void setPassword(String password) { this.password = password; }                
    }
    

    public static Credentials
    loadCredentials(String path, String password) throws CLibException {            
        Credentials creds = new Credentials();         
        int err = TDAmeritradeAPI.getCLib().LoadCredentials_ABI(path, password, creds._credentials, 0);
        if( err != 0 )
            throw new CLibException(err);    
        return creds;
    }
    
    public static void
    storeCredentials(String path, String password, Credentials creds) throws  CLibException {
        int err = TDAmeritradeAPI.getCLib().StoreCredentials_ABI(path, password, creds._credentials, 0);
        if( err != 0 )
            throw new CLibException(err);                
    }
    
    public static Credentials
    requestAccessToken(String code, String clientID, String redirectURI) throws  CLibException {        
        Credentials creds = new Credentials();    
        int err = TDAmeritradeAPI.getCLib().RequestAccessToken_ABI(code, clientID, redirectURI, creds._credentials, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return creds;    
    }
    
    public static Credentials
    requestAccessToken(String code, String clientID) throws  CLibException {
        return requestAccessToken(code, clientID, DEF_REDIRECT_URI);        
    }
    
    public static void
    setCertificateBundlePath(String path) throws CLibException {
        int err = TDAmeritradeAPI.getCLib().SetCertificateBundlePath_ABI(path, 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    public static String
    getCertificateBundlePath() throws CLibException {
        return CLib.Helpers.getString( TDAmeritradeAPI.getCLib()::GetCertificateBundlePath_ABI);   
    }
    
    public static String
    getDefaultCertificateBundlePath() throws CLibException {
        return CLib.Helpers.getString( TDAmeritradeAPI.getCLib()::GetDefaultCertificateBundlePath_ABI);   
    }
    
}
