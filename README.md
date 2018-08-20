
### TDAmeritradeAPI
- - -

C++ front-end library, with C and Python bindings, for the recently expanded TDAmeritrade API. It provides object-oriented access to the simple HTTPS/JSON interface using *[libcurl](https://curl.haxx.se/libcurl)* and to the Streaming interface using *[uWebSockets](https://github.com/uNetworking/uWebSockets)*.

If you have an Ameritrade account you *should* be able to gain access to the API by following the instructions in the Authentication section below. 

The library is designed to abstract away many of the lower level details of 
accessing the API while still providing almost complete control over authentication,
access, data handling, and order execution.

- It does not provide complete OAuth2 authentication management, allowing users to 
customize for their particular needs. The user retrieves an access code(see below), 
and then uses the library to request an access token, which is refreshed automatically.

- It does not completely parse returned data, allowing users to handle it as they see fit. 
    - C++ interface returns [json](https://github.com/nlohmann/json) objects
    - C interface populates char buffers w/ un-parsed json strings
    - Python interface returns builtin objects(list, dict etc.) via json.loads().

- This is a new library, built by a single developer, for an API that is still in flux. As such you should expect plenty of bumps along the way, with changes to both the interface and the implementation.       

- Please report bugs via issues: be desciptive and try to replicate if possible. 

- If you're capable of and interested in contributing please communicate your intentions first.

*Communicating w/ 3rd party servers, accessing financial accounts, and automating trade execution are all operations that present risk and require responsibility. **By using this software you agree that you understand and accept these risks and responsibilities. You accept sole responsibility for the results of said use. You completely absolve the author of any damages, financial or otherwise, incurred personally or by 3rd parties, directly or indirectly, by using this software - even those resulting from the gross negligence of the author. All communication with TDAmeritrade servers, access of accounts, and execution of orders must adhere to their policies and terms of service and is your repsonsibility, and yours alone.***


### Index
- - -
- [Dependencies](#dependencies)
- [Status](#status)
- [Structure](#structure)
- [Getting Started](#getting-started)
    - [Unix-Like Systems](#unix-like)
    - [Windows](#windows)
    - [Managing the Libraries](#managing-the-libraries)
    - [Using the TDAmeritradeAPI Library ](#using-the-tdameritradeapi-library)
    - [Python3](#python3)
- [Namespaces](#namespaces)
- [Conventions](#conventions)
- [Errors & Exceptions](#errors--exceptions)
- [Authentication](#authentication)
    - [Access Code](#access-code)
        - [Simple Approach for Personal Use](#simple-approach-for-personal-use)
        - [Other Options](#other-options)
    - [Credentials](#credentials)
- [Access](#access)
    - [HTTPS Get](#https-get)
    - [Streaming](#streaming)
    - [HTTPS Update/Execute](#https-updateexecute)
- [Licensing & Warranty](#licensing--warranty)

<br>

 
### Dependencies
- - -

This project would not be possible without some of the great open-source projects listed below.

- [libcurl](https://curl.haxx.se/libcurl) - Client-side C library supporting a ton of transfer protocols
- [openssl](https://www.openssl.org) - C library for tls/ssl and crypto 
- [zlib](https://zlib.net) - Compression library
- [libuv](https://libuv.org) - Cross-platform asynchronous I/O
- [uWebSockets](https://github.com/uNetworking/uWebSockets) - A simple and efficient C++ WebSocket library. The source is included, compiled and archived with our library to limit dependency issues.
- [nlohmann::json](https://github.com/nlohmann/json) : - An extensive C++ json library that only requires adding a single header file. ***You'll need to review their documentation for handling returned data from the C++ version of this library.***

### Status
- - -
| | Get Interface  |  Streaming Interface  |  Execute Interface 
-------------------|---------------|---------------------|--------------------
**C**              | *Working*     | *Working*       | *Coming Soon*
**C++**            | *Working (incomplete ABI)* | *Working (incomplete ABI)* | *Coming Soon*
**Python**         | *Working*     | *Working*       | *Coming Soon*

*Note: 'Working' does not necessarily mean 'Stable'*

### Structure
- - -

    |--------------------------------------|-----------------|------------|
    |      client python, java etc.        |   client C++    |  client C  |
    |--------------------------------------|                 |            | 
    |          extension layer             |                 |            |
    |--------------------------------------|-----------------|            |
    | C-lib interface (e.g ctypes.py, JNA) | C++ Proxy Layer |            |
    |=====================================================================|
    |                   (binary compatible) C interface                   |
    |---------------------------------------------------------------------|
    |       (non binary compatible) C++ interface to TDAmeritradeAPI      |
    |---------------------------------------------------------------------|

**IMPORTANT** - *There are certain binary compatibility issues when exporting C++ code accross compilations(from name mangling, differing runtimes, changes to STL implementations etc.). If, for instance, we return an std::vector in an earlier version of a library, its implementation changes, and code that imports the library is compiled against a new version of the STL, there can be an issue.*

*The C++ interfaces still throw exceptions across the library boundary which can create issues.*

***Until we implement a better way to deal w/ this either 1) compile your code and the library using the same compiler/settings and link to the same libraries to avoid ABI incompatibility or 2) use the C or Python bindings and corresponding error codes.***


### Getting Started
- - -

The current build setup is a bit messy. Hopefully things will get a little simpler in the future.
You can currently build on Unix-like systems and Windows. For Mac OS/X you'll have to download/install the necessary libraries and adjust the makefiles. (If you build sucessfully on Mac feel free to share and we can 
include it in the docs.)

- The library is implemented in C++ and needs to be built accordingly. It exports a C/ABI layer which is wrapped by header defined C calls, allowing for access from pure C and Python via ctypes.py.
```
  FunctionImpl(string)                [C++ implementation code defined in lib]
  Funtion_ABI(const char*)            [C ABI code defined in lib and exported]
    /\    /\   /\
====||====||===||=====================[lib boundary]==========================
    ||    ||   ||
    ||    ||   ||   ---------------------[headers]----------------------------
    ||    ||   ||      #ifdef __cplusplus
    ||    ||   ||<===  Function(string)        [C++ wrapper defined in header]
    ||    ||           #else
    ||    ||<=======   Function(const char *)  [C wrapper defined in header]
    ||                 #endif
    ||              --------/\------------------------------------------------
    ||              --------||-------------------------------------------------
    ||                 Function("foo")         [Client C/C++]
    ||              ----------------------------------------------------------
    ||   
    ||<===  ctypes.CDll.Function_ABI("foo")    [ctypes.py access to library] 
           
```

- Compilers need to support C++11.

- As mentioned above the C++ interfaces still have binary compatibility issues so you'll need to compile/link your C++ code the same way you compile/link this library.

- If you have a build issue file an issue or send an email.

- Define ```DEBUG_VERBOSE_1_``` to send verbose logging/debug info to stdout. (Debug builds do this automatically.)

#### Unix-Like

##### Install Dependencies

If using a package manager like apt installing libcurl should install libssl and libz*:  

        user@host:~$ sudo apt-get install libcurl4-openssl-dev

*...or* you can clone the github projects **  

        user@host:~$ git clone https://github.com/openssl/openssl.git
        user@host:~$ git clone https://github.com/curl/curl.git   
        user@host:~$ git clone https://github.com/madler/zlib.git

*...or* download them directly: [openssl](https://www.openssl.org/source), [libcurl](https://curl.haxx.se/download.html), [zlib](https://zlib.net) ** 

\* *[libuv](https://github/com/libuv/libuv.git) is not necessary if epoll is available (linux)*  
\*\* *you'll need to follow the build/install instructions in the README/INSTALL files*

##### Build

The Eclipse/CDT generated makefiles are in the  'Debug' and 'Release' subdirectories. *(The makefiles may need some tweaks for non-linux systems.)*

    user@host:~/dev/TDAmeritradeAPI/Release$ make

\* *Tested on GNU/Linux Debian-8 using 'libcurl4-openssl-dev' package via apt and stock packages.*

#### Windows

##### Install Dependencies

Dealing with dependencies on Windows can be more complicated. Pre-built binaries(x86 and x64 release builds of openssl, curl, zlib, and uv) are included in the *vsbuild/deps/libs* directory to make things easier. 

If you'd still like to download and build them yourselves visit the links from the unix-like build 
section above.

##### Build
The solution provided was created in VisualStudio2017 using toolset v141. If it doesn't work in 
your version of VS you'll need to tweak the project settings and/or source.

1. Open vsbuild/vsbuild.sln
2. Select an x64 or Win32 Release build configuration. *(If you want a debug configuration 
you'll have to edit the 'Additional Library Directories' in Properties->Linker->General for
the TDAmeritradeAPI project so it looks for Release builds of the dependencies. In the path strings change '$(Configuration)' to 'Release\\' .)*
3. Build->Build Solution. 
 
If sucessful the TDAmeritradeAPI library files AND the pre-built 
dependencies will be in vsbuild\\x64 or vsbuild\\Win32. (If you built/installed your own dependencies you can ignore these.)

\* *The 'test' project is just a hodge-podge for checking basic functionality and linking. 
The executable it produces takes three args: account id, path to credentials file, and a password to decrypt credentials. It requires a valid credentials file which you would need to create via
the instructions in the Authorization section below BEFORE using.*

\*\* *Tested on 32 and 64 bit Windows7 using pre-built dependencies.*

#### Managing the Libraries

Once you've built all the dependencies *AND* TDAmeritradeAPI you'll need to make sure they
 are in a location the linker can find, or tell the linker where to look.

If you've installed the dependencies to the default location you shouldn't need to do anything 
else except deal with TDAmeritradeAPI. If not you'll to have deal with ALL the libraries.

- move them to a folder where the linker will automatically look
	- e.g. if moving a 32bit(Win32) DLL to C:\Windows\SysWow64\, copy there and run:  
	```(Admin) C:Windows\SysWow64\regsvr32.exe <dll name>```
	- e.g. if moving a 64bit(x64) DLL to C:\Windows\System32, copy there and run:  
	```(Admin) C:Windows\System32\regsvr32.exe <dll name>```
- *-or-* indicate to the linker where they can be found 
    - e.g. for GCC '-L/path/to/lib -Wl,-rpath,/path/to/lib'    
- *-or-* keep them in the same folder as your executable 
- *-or-* add their folder to your PATH(windows) or LD_LIBRARY_PATH(linux).

#### Using the TDAmeritradeAPI Library

1. include headers:
    - "tdma_api_get.h" for the 'HTTPS Get' interface 
    - "tdma_api_streaming.h" for the 'Streaming' interface  
    - *(headers/source use relative include links, don't change the directory structure)*
2. add Library/API calls to your code
3. compile *(read the message above on binary compatibility)*
4. link your code with libTDAmeritradeAPI *(be sure the linker can find it)*
5. run 

#### Python3

1. Be sure to have built/installed the shared library(above)
2. Be sure the library build(32 vs 64 bit) matches the python build
3. ```user@host:~/dev/TDAmeritradeAPI/python$ python setup.py install```
    - if your ```python``` links to ```python2``` run ```python3 setup.py install``` instead
4. Import the package or module(s):
	``` 
	import tdma_api # -or-
	from tdma_api import auth    # authorization methods and objects
	from tdma_api import get     # 'getter' objects and utilities
	from tdma_api import stream  # 'streaming' class and subscriptions
	```
    - the python package will try to load the library automatically
    - if it can't it will output an error message on package import 
        - the most common issue is the library not being installed in the default library search path
        - to load it manually: ```>>> tdma_api.clib.init("path/to/lib.so")```
    - if you get an error message concerning the dependencies you'll need to
      move them to a location the dynamic linker can find.


### Namespaces
- - -

All front-end C++ library code is in namespace ```tdma```. We mostly exclude it in the docs.

### Conventions
- - -

- Enums are defined for both C and C++ code using MACROS. Python mimics these enums by defining constant values. For example:
    ```
    DECL_C_CPP_TDMA_ENUM(AdminCommandType, 0, 2,
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, LOGIN),
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, LOGOUT),
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, QOS)
        );
    ```
    ... indicates values from index 0(LOGIN) to 2(QOS) are valid to pass to theAPI. The macros expand to:
    ```
    [C++]
    enum class AdminCommandType : int {
        LOGIN,
        LOGOUT,
        QOS
    };

    [C]
    enum AdminCommandType {
        AdminCommandType_LOGIN,
        AdminCommandType_LOGOUT,
        AdminCommandType_QOS
    };
    ```
    In python they would look like:
    ```
    ADMIN_COMMAND_TYPE_LOGIN = 0
    ADMIN_COMMAND_TYPE_LOGOUT = 1
    ADMIN_COMMAND_TYPE_QOS = 2
    ```

- C++ methods return values/objects and throw exceptions
- C functions populate pointers/buffers and return error codes
    - Most buffers require the client to Free the underlying memory using the appropriate library calls:
        - ```FreeBuffer(char *buf)```
        - ```FreeBuffers(char **bufers, size_t n)```
        - ```FreeFieldsBuffer(int* fields)```
    - All buffers are accompanied w/ a size_t* arg to be filled w/ the size of the populated buffer. Buffers of type char* return the size of the string + 1 for the null term.
    - Arrays passed by caller have to pass a size_t value of the number of elements

- Only ABI functions(those ending in '_ABI') are directly exported from the library. The C interface calls and C++ classes wrap these as header-defined inlines.

### Errors & Exceptions
- - -
All exceptional/error states from the C++ interface will cause exceptions to be thrown.  ***Currently C++ exceptions are passed across the library boundary and may break the ABI***

- Library Exceptions:
    - ```APIExcetion``` : base class and generic exceptions
        - ```LocalCredentialException``` : an issue creating/loading/storing/using credentials
        - ```ValueException``` : bad/invalid argument to a function or constructor (checked locally)
        - ```TypeException``` : type inconsistency of a proxy object
        - ```MemoryError``` : error allocating memory within the ABI layer
        - ```APIExecutionException``` : a general error connecting/communicating with the server, these
exceptions tend to be the result of some low(er) level failure with the connection        
            - ```AuthenticationException``` : an error authenticating with the server
            - ```InvalidRequest``` : user made an invalid/malformed request to the server
            - ```ServerError``` : server has returned some type of error status
        - ```StreamingException``` : a general error connecting/communicating via StreamingSession            

- 3rd Party Exceptions:
    - ```json::exception``` : base class for exceptions from the json library (review the documentation
for the derived classes)

- The C interface has corresponding error codes:
    ```
    #define TDMA_API_ERROR 1
    #define TDMA_API_CRED_ERROR 2
    #define TDMA_API_VALUE_ERROR 3
    #define TDMA_API_TYPE_ERROR 4
    #define TDMA_API_MEMORY_ERROR 5

    #define TDMA_API_EXEC_ERROR 101
    #define TDMA_API_AUTH_ERROR 102
    #define TDMA_API_REQUEST_ERROR 103
    #define TDMA_API_SERVER_ERROR 104

    #define TDMA_API_STREAM_ERROR 201
    ```
- The Python interface throws ```tdma_api.clib.LibraryNotLoaded``` and ```tdma_api.clib.CLibException``` w/ the error code, name, and message returned from the library.         

- ```LastErrorMsg``` and ```LastErrorCode``` can be used to get the last error message and code, respectively.
### Authentication
- - -
Authentication is done through OAuth2 using your account login information.

#### Access Code

First, you need to get an access code.

##### Simple Approach For Personal Use

1. [Set up a developer account.](https://developer.tdameritrade.com/user/register)
    - Set the 'redirect_uri' field to the localhost: https://127.0.0.1
    - Set a 'client_id' and remember the full name (e.g MY_ID@AMER.OAUTHAP)
2. Open the ```tools/get-access-code.html``` file with your web broswer and follow instructions.
    - Temporarily disable any browser add-ons that block pop-ups/redirects/JavaScript
    - If this doesn't work [follow these instructions](https://developer.tdameritrade.com/content/simple-auth-local-apps).
3. **Securely** record the access code it gives you. You'll only need to pass this to the API **once**. Once you use this code to create and successfully store a Credentials object(see below) you can dispose of it.

##### Other Options
  
These options provide for more robust authentication e.g writing an app for 3rd party users. You'll need to [set up a developer account.](https://developer.tdameritrade.com/user/register) (The 'redirect_uri' field to use depends on what you want to do.)
  
1. [Set up your own server](https://developer.tdameritrade.com/content/web-server-authentication-python-3)
2. Use a 3rd party solution e.g [auth0](https://auth0.com)

#### Credentials

Once you have an access code use ```RequestAccessToken``` to get a ```Credentials``` object that will retrieve and store the access and refresh tokens. **This only has to be done once, until the refresh token expires in 3 months.**
```
    [C++]
    Credentials 
    RequestAccessToken(string code, string client_id, string redirect_uri="127.0.0.1");
    
       code          ::  the access code retrieved
       client_id     ::  the client id used to set up the account
       redirect_uri  ::  the redirect uri used to set up the account
       
    [C]
    inline int
    RequestAccessToken( const char* code,
                        const char* client_id,
                        const char* redirect_uri,
                        struct Credentials* pcreds );

        ...
        pcreds :: a pointer to a Credentials struct to be populated
        returns -> 0 on success, error code on failure

    [C, C++]
    struct Credentials{
        char *access_token;
        char *refresh_token;
        long long epoch_sec_token_expiration;
        char *client_id;
    };

    [Python]    
    def auth.request_access_token(code, client, redirect_uri="https://127.0.0.1"):
        ...
        returns -> Credentials class
        throws CLibException on error

    [Python]
    class auth.Credentials(_Structure):
        _fields = [
            ("access_token", c_char_p),
            ("refresh_token", c_char_p),
            ("epoch_sec_token_expiration", c_longlong),
            ("client_id", c_char_p)
        ]

```

The ```Credentials``` object is used throughout for accessing the API so keep it 
available. It will be updated internally as the access token is refreshed. When done, 
securely store your credentials:
```
    [C++]
    void 
    StoreCredentials(string path, string password, const Credentials& creds)
    
        path      ::  the full path of the file to store in
        password  ::  the password used for AES256_CBC encryption
        creds     ::  the Credentials struct returned from 'RequestAccessToken'

    [C]
    inline int
    StoreCredentials( const char* path,
                      const char* password,
                      const struct Credentials* pcreds )

        ...
        pcreds :: a pointer to the Credentials struct to store
        returns -> 0 on success, error code on failure

    [Python]
    def auth.store_credentials(path, password, creds):
        ...
        creds :: the Credentials class return from 'request_access_token'
        throws CLibException on error
```           

In the future construct a new Credentials struct from the saved credentials file:
```
    [C++]
    Credentials
    LoadCredentials(string path, string password)
    
        path      ::  the full path of the file previously stored in
        password  ::  the password used above

    [C]
    inline int
    LoadCredentials( const char* path,
                     const char* password,
                     struct Credentials* pcreds )

        ...
        pcreds :: a pointer to the Credentials struct to load into
        returns -> 0 on success, error code on failure

    [Python]
    def auth.load_credentials(path, password):
        ...
        returns -> Credentials class
        throws CLibException on error
```        
    
***The format of the encrypted credentials file was changed in commit e529c2***

To avoid having to manually load and save each time your code runs use ```CredentialsManager```
to automatically load and store on construction and destruction.  
```
    [C++]
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


    [Python]
    #Context Manager
    class auth.CredentialsManager:
        def __init__(self, path, password, verbose=False):
            ...
        def __enter__(self):
            ...
        def __exit__(self, _1, _2, _3);
            ...
```   
  
You can, for instance, create a static or global ```CredentialsManager``` instance that will exist over the lifetime of the program, storing the credentials on exit. Just use the 
```.credentials``` member as an argument for the following API calls, where required. Keep in mind, with this approach the password will be stored in memory, in plain-text, for 
the life of the ```CredentialsManager``` object.
    
**C code should explicitly close the Credentials object to deallocate the underlying resources.** (C++ and Python define destructors that do this for you.)
```
inline int
CloseCredentials(struct Credentials* pcreds );
```

### Access
- - -

- ##### *HTTPS Get*

    For queries, (non-streaming) real-time data, and account information you'll make HTTPS Get requests through 'Getter' objects and convenience functions that internally use libcurl and return json objects. [Please review the full documentation](README_GET.md).

- ##### *Streaming*

    For real-time, low(er)-latency streaming data you'll establish a long-lived WebSocket connection through the StreamingSession class that will callback with json strings/objects. [Please review the full documentation](README_STREAMING.md).

- ##### *HTTPS Update/Execute* 

    For updating your account and executing trades you'll make HTTPS Put/Post/Delete requests that have yet to be implemented. *As soon as the get calls are determined stable and there's a means to test execution outside of live trading they will be added.*


#### LICENSING & WARRANTY
- - -

*TDAmeritradeAPI is released under the GNU General Public License(GPL); a copy (LICENSE.txt) should be included. If not, see http://www.gnu.org/licenses. The author reserves the right to issue current and/or future versions of TDAmeritradeAPI under other licensing agreements. Any party that wishes to use TDAmeritradeAPI, in whole or in part, in any way not explicitly stipulated by the GPL - including, but not limited to, commercial use - is thereby required to obtain a separate license from the author. The author reserves all other rights.*

*TDAmeritradeAPI includes 3rd party material operating under different licensing agreements which, although requiring adherence, do not to subsume or subordinate this agreement.*

*This software/program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. By choosing to use this software/program - under the broadest interpretation of the term 'use' - you absolve the author of ANY and ALL responsibility, for ANY and ALL damages incurred; even damages resulting from the author's gross negligence; damages including, but not limited to, those arising from the accuracy, timeliness, responsiveness, and general operation of the aformentioned software/program.*

























 
