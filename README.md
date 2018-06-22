
### TDAmeritradeAPI
- - -

A C++ front-end for the recently expanded TDAmeritrade API that will eventually include Python, and potentially other, bindings. It provides object-oriented access to the simple HTTPS/JSON interface using 
*[libcurl](https://curl.haxx.se/libcurl)* and to the Streaming interface using *[uWebSockets](https://github.com/uNetworking/uWebSockets)*.

The library is designed to abstract away many of the lower level details of 
accessing the API while still providing almost complete control over authentication,
access, data handling, and order execution.

- It does not provide complete OAuth2 authentication management, allowing users to 
customize for their particular needs. The user retrieves an access code(see below), 
and then uses the library to request an access token, which is refreshed automatically.
- It does not parse returned data, returning [json](https://github.com/nlohmann/json) objects for the user to handle as they see fit.

If you have an Ameritrade account you *should* be able to gain access to the API by following the instructions in the Authentication section below. A few things to keep in mind:

- This is a new library, built by a single developer, for an API that is still in flux. As such you should expect plenty of bumps along the way, with changes to both the interface and the implementation.       
    - *constructive* criticism is always welcome
    - please report bugs via issues: be desciptive and try to replicate if possible
    - if you're capable of and interested in contributing please communicate your intentions first
    - feel free to share samples, tests, bindings, wrappers, extensions, ideas, whatever

- Communicating w/ 3rd party servers, accessing financial accounts, and automating trade execution are all operations that present risk and require responsibility. ***By using this software you agree that you understand and accept these risks and responsibilities. You accept sole responsibility for the results of said use. You completely absolve the author of any damages, financial or otherwise, incurred personally or by 3rd parties, directly or indirectly, by using this software - even those resulting from the gross negligence of the author. All communication with TDAmeritrade servers, access of accounts, and execution of orders must adhere to their policies and terms of service and is your repsonsibility, and yours alone.***

 
#### Dependencies
- - -

This project would not be possible without some of the great open-source projects listed below.

- [libcurl](https://curl.haxx.se/libcurl) - Client-side C library supporting a ton of transfer protocols
- [openssl](https://github.com/openssl/openssl) - C library for tls/ssl and crypto 
- [uWebSockets](https://github.com/uNetworking/uWebSockets) - A simple and efficient C++ WebSocket library. The source is included, compiled and archived with our library to limit dependency issues.
- [nlohmann::json](https://github.com/nlohmann/json) : - An extensive C++ json library that only requires adding a single header file. ***You'll need to review their documentation for handling returned data from this library.***
- dl, util, pthread 

#### Coming Soon
- - -
- cross platform support
- account update and trade execution functionality
- Python bindings

#### Getting Started
- - -

##### Build

Currently only available on unix-like systems with C++11 compiler support. The Eclipse/CDT generated makefiles are in the  'Debug' and 'Release' subdirectories. You may need to tweek for non-GNU/Linux environments.

```user@host:~/dev/TDAmeritradeAPI/Release$ make```

A more portable version w/ a more robust build system should be available shortly.

##### Using the library

1. include "tdma_api_get.h" for the 'HTTPS Get' interface and/or "tdma_api_streaming.h" for streaming; be sure the compiler can find them (headers/source use relative include links, don't change the directory structure)
2. use the Library/API calls(see below) in your code
3. compile 
4. link with libTDAmeritradeAPI.so (move the file to a place the dynamic linker can find
or indicate its location to the compiler/linker)
5. run 


#### Namespaces
- - -

All front-end library code is in namespace ```tdma```. We mostly exclude it in the docs.

#### Errors & Exceptions
- - -

Before discussing authentication and access it's important to understand how the library 
handles errors and exceptional states. Almost all exceptional/error states will cause exceptions
to be thrown:

- Library Exceptions:
    - ```APIExcetion``` : base class and generic exceptions
        - ```LocalCredentialException``` : an issue creating/loading/storing/using credentials
        - ```ValueException``` : bad/invalid argument to a function or constructor (checked locally)
        - ```APIExecutionException``` : a general error connecting/communicating with the server, these
exceptions tend to be the result of some low(er) level failure with the connection        
            - ```AuthenticationException``` : an error authenticating with the server
            - ```InvalidRequest``` : user made an invalid/malformed request to the server
            - ```ServerError``` : server has returned some type of error status
        - ```StreamingException``` : a general error connecting/communicating via StreamingSession            

- 3rd Party Exceptions:
    - ```json::exception``` : base class for exceptions from the json library (review the documentation
for the derived classes)

- There are no guarantees of exception safety.             


#### Authentication
- - -

Authentication is done through OAuth2 using your account login information. 

1. [Follow Ameritrades' Getting Started guide](https://developer.tdameritrade.com/content/getting-started) 
to setup a developer account.
    
2. Get an access code using your account info:
    - [use your browser and a localhost redirect uri](https://developer.tdameritrade.com/content/simple-auth-local-apps) -or-
    - [use your own server](https://developer.tdameritrade.com/content/web-server-authentication-python-3) -or-
    - use a 3rd party solution e.g [auth0](https://auth0.com)

3. use ```RequestAccessToken``` to get an access token stored in a ```Credentials``` struct (**only has to be done once, until the refresh token expires in 3 months**)
```
    Credentials 
    RequestAccessToken(string code, string client_id, string redirect_uri="127.0.0.1");
    
       code          ::  the code from #2
       client_id     ::  the client id from #1
       redirect_uri  ::  the redirect uri from #1    
       
       
    struct Credentials{
        string access_token;
        string refresh_token;
        long long epoch_sec_token_expiration;
        string client_id;
    };     
```

The ```Credentials``` object is used throughout for accessing the API so keep it 
available. It will be updated internally as the access token is refreshed. When done, 
securely store your credentials:
```
    void 
    StoreCredentials(string path, string password, const Credentials& creds)
    
        path      ::  the full path of the file to store in
        password  ::  the password used for AES256_CBC encryption
        creds     ::  the Credentials struct returned from 'RequestAccessToken'
```        
    
In the future construct a new Credentials struct from the saved credentials file:
```
    Credentials
    LoadCredentials(string path, string password)
    
        path      ::  the full path of the file previously stored in
        password  ::  the password used above
```        
    
To avoid having to manually load and save each time your code runs use ```CredentialsManager```
to automatically load and store on construction and destruction.  
```
    struct CredentialsManager{
        Credentials credentials;
        string path;
        string password;
        CredentialsManager(string path, string password)
            : credentials( LoadCredentials(path, password) ),
              path( path ),
              password( password )
        {}
        virtual ~CredentialsManager()
        { StoreCredentials(path, password, credentials);}
    };
```   
  
You can, for instance, create a static or global ```CredentialsManager``` instance that will exist over the 
lifetime of the program, storing the credentials on exit. Just use the 
```.credentials``` member as an argument for the following API calls, where required. Keep in mind, with this approach the password will be stored in memory, in plain-text, for 
the life of the ```CredentialsManager``` object.
    
#### Access
- - - 

##### *HTTPS Get*

For queries, (non-streaming) real-time data, and account information you'll make HTTPS Get requests through 'Getter' objects and convenience functions that internally use libcurl and return json objects. [Please review the full documentation](README_GET.md).

##### *Streaming*

For real-time, low(er)-latency streaming data you'll establish a long-lived WebSocket connection through the StreamingSession class that will callback with json objects. *StreamingSession is currently a work in progress; you should anticipate bugs and substantive changes to both the interface and implementation going forward.* [Please review the full documentation](README_STREAMING.md).

##### *HTTPS Update/Execute* 

For updating your account and executing trades you'll make HTTPS Put/Post/Delete requests that have yet to be implemented. *As soon as the get calls are determined stable and there's a means to test execution outside of live trading they will be added.*


### LICENSING & WARRANTY
- - -

*TDAmeritradeAPI is released under the GNU General Public License(GPL); a copy (LICENSE.txt) should be included. If not, see http://www.gnu.org/licenses. The author reserves the right to issue current and/or future versions of TDAmeritradeAPI under other licensing agreements. Any party that wishes to use TDAmeritradeAPI, in whole or in part, in any way not explicitly stipulated by the GPL - including, but not limited to, commercial use - is thereby required to obtain a separate license from the author. The author reserves all other rights.*

*TDAmeritradeAPI includes 3rd party material operating under different licensing agreements which, although requiring adherence, do not to subsume or subordinate this agreement.*

*This software/program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. By choosing to use this software/program - under the broadest interpretation of the term 'use' - you absolve the author of ANY and ALL responsibility, for ANY and ALL damages incurred; even damages resulting from the author's gross negligence; damages including, but not limited to, those arising from the accuracy, timeliness, responsiveness, and general operation of the aformentioned software/program.*

























 
