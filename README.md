
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
- [New Features](#new-features)
- [Status](#status)
- [Structure](#structure)
- [Getting Started](#getting-started)
    - [Build Dependencies](#build-dependencies)
    - [Build Library](#build-library)
    	- [Precompiled](#precompiled)
    - [Install](#install)
    - [Use - C/C++](#use---cc)
    - [Use - Python3](#use---python3)
- [Conventions](#conventions)
- [Errors & Exceptions](#errors--exceptions)
- [Authentication](#authentication)
    - [Access Code](#access-code)
        - [Simple Approach for Personal Use](#simple-approach-for-personal-use)
        - [Other Approaches](#other-approaches)
    - [Credentials](#credentials)
- [Access](#access)
    - [Get](#get)
    - [Streaming](#streaming)
    - [Execute](#execute)
- [Utilities](#utilities)
    - [DynamicDataStore](#dynamicdatastore)
    - [OptionSymbols](#optionsymbols)
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

### New Features
- - -

- [DynamicDataStore](DynamicDataStore) - module that abstracts away data retrieval, providing a simple bar-based interface
- Mac build (see below)
- Raw Subscriptions for complete control over accessing the Streaming interface
- ADD, VIEW, UNSUBS commands for Streaming interface
- Setters for Subscription objects, copy semantics for C++ versions

### Status
- - -
| | Get Interface  |  Streaming Interface  |  Execute Interface 
-------------------|---------------|---------------------|--------------------
**C**              | *Working*     | *Working*   | *OrderTicket, Builders, Send/Cancel*
**C++**            | *Working*     | *Working*   | *OrderTicket, Builders, Send/Cancel*
**Python**         | *Working*     | *Working*   | *OrderTicket, Builders, Send/Cancel*


*Note: 'Working' does not necessarily mean 'Stable'*  
*Note: Execute Interface has undergone very little testing*  

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

There are certain binary compatibility issues when exporting C++ code accross compilations(from name mangling, differing runtimes, changes to STL implementations etc.). Although we attempt to limit these issues with an ABI layer and 'proxy' objects, it's recommended to compile client code and the library using the same compiler/settings and link to the same libraries.

### Getting Started
- - -

1. build the necessary dependencies
2. build the TDAmeritradeAPI shared library(or use a precompiled version).
3. install the dependencies and TDAmeritradeAPI library in a place the linker can find
4. 
    - (**C/C++**) include the necessary headers in your code and link to the library
    - (**Python**) install the tdma_api package

It's recommend you build from source. If you're not comfortable with this or just want to use the Python interface it may be easier to use a [precompiled linux/windows library](#precompiled). 

#### Build Dependencies 

##### Unix/Linux

If using a package manager, like apt, install the libcurl and libssl dev packages (this should take care of the other dependencies): 

    user@host:~$ sudo apt-get install libcurl4-openssl-dev libssl-dev

Alternatively, you can download them manually via github:

    user@host:~$ git clone https://github.com/openssl/openssl.git
    user@host:~$ git clone https://github.com/curl/curl.git 
    user@host:~$ git clone https://github.com/madler/zlib.git

...or the individual project sites:
- [openssl](https://www.openssl.org/source)
- [libcurl](https://curl.haxx.se/download.html)
- [zlib](https://zlib.net) 

Then follow the build/install instructions in each project's README/INSTALL files.

*[libuv](https://github/com/libuv/libuv) is not necessary if epoll is available.*

##### Mac

You'll need the same libraries as the Unix/Linux build AND libuv.

##### Windows

Compiled dependencies - 32 and 64 bit release builds of openssl, curl, zlib, and uv - are included in the *vsbuild/deps/libs* directory. If you'd still like to download and build them yourselves visit the links from the section above.


#### Build Library

The library is implemented in C++ and needs to be built accordingly. It exports a C/ABI layer which is wrapped by header defined C calls and C++ calls/objects, allowing for access from pure C, C++ and Python via ctypes.py.
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

- By default, exceptions are caught inside the library, exported as error codes, and (for C++) reconstituted/rethrown on the client side of the ABI. 

- ```#define ALLOW_EXCEPTIONS_ACROSS_ABI``` to allow exceptions to be thrown directly from the library. ***Be sure there's binary compatibility between your code and the library.*** (*On Windows you'll need to compile your code with the /EHs switch to allow exceptions from 'extern C' functions.*)

- ```#define DEBUG_VERBOSE_1_``` to send verbose logging/debug info to stdout. (Debug builds do this automatically.)

- If you have a build issue file an issue or send an email.

##### Unix/Linux

The Eclipse/CDT generated makefiles are in the  'Debug' and 'Release' subdirectories. *(The makefiles may need some tweaks for non-linux systems.)*

    user@host:~/dev/TDAmeritradeAPI/Release$ make


##### Mac

Same as for Unix/Linux except you need to manually add '-luv' to objects.mk. It should read:
```
USER_OBJS := 
LIBS := -lssl -lcrypto -lz -lcurl -lpthread -lutil -ldl -luv
```

##### WIndows

The build solution provided was created in ***VisualStudio2017*** using toolset ***v141***. (*If it doesn't work in your version of VS you'll need to tweak the project settings and/or source.*)

1. Open vsbuild/vsbuild.sln in VisualStudio
2. Select an x64 or Win32 Release build configuration. 
    - ***(optional)*** *If you want a debug configuration you'll have to edit the 'Additional Library Directories' in Properties->Linker->General for the TDAmeritradeAPI project so it looks for Release builds of the dependencies. In the path strings change '$(Configuration)' to 'Release\\'.*
3. Build->Build Solution. 
 
If sucessful, the library and dependencies will be in *vsbuild\\x64* or *vsbuild\\Win32*. 

Files you should have:
- TDAmeritradeAPI.dll
- libcurl.dll
- libssl-[version-arch].dll
- libcrypto-[version-arch].dll
- libuv.dll
- zlibwapi.dll


#### Precompiled

Some precompiled versions of the library are provided for convenience. You'll still need to [build](#build-dependencies) and/or [install](#install) the dependencies. *For practical reasons these binaries will not be re-compiled with each commit so you may be using a stale library.*

##### Linux

bin/[distro]-[kernel version]--[architecture]--[toolchain]

- ~~bin/debian-3.16--i386--gcc/~~
- bin/debian-3.20--x86_64--gcc/  

The Linux/ELF binaries in the directories above may or may not work on your particular system/distro BUT [building the library from source](#unix-like-1) is pretty straight forward. 
 
##### Windows

- bin/windows-msvc/Win32
- bin/windows-msvc/x64

The Windows/PE binaries are built using VisualStudio2017 and statically linked against the runtime libraries to limit dependency issues.

#### Install

Once built, make sure the TDAmeritradeAPI library and dependencies are in a location the linker can find, or you'll need to tell the linker where to look.

##### Unix/Linux

If dependencies were built/installed using a package manager they should be in the correct location already.

To make *libTDAmeritradeAPI.so* available to your program:
- move it to the a directory in the search path e.g ```/usr/local/lib```
- *-or-* link your code with the appropriate flags ```'-L/path/to/lib -Wl,-rpath,/path/to/lib'```
- *-or-* add its path to the LD_LIBRARY_PATH environment variable
- *-or-* move the file to the location of the binary that will link to it  

##### Windows

Since all the dependencies are included(or built manually) you'll need to manage them AND *TDAmeritradeAPI.dll*.

- move them to a systems folder
- *-or-* link your code with the appropriate flags (use link settings in VisualStudio)   
- *-or-* add their folder(s) to your PATH variable.
- *-or-* move the files to the location of the binary that will link to it  

#### Use - C/C++

1. include headers:
    - "tdma_api_get.h" for the 'HTTPS Get' interface 
    - "tdma_api_streaming.h" for the 'Streaming' interface  
    - "tdma_api_execute.h" for the 'Execute' interface
    - *(headers/source use relative include links, don't change the directory structure)*
2. add Library/API calls to your code
3. compile 
4. link your code with the TDAmeritradeAPI library *(be sure the linker can find it)*
5. run 

#### Use - Python3

1. Be sure to have built/installed the dependencies and shared library(above)
2. Be sure the library build(32 vs 64 bit) matches the python build
3. ```user@host:~/dev/TDAmeritradeAPI/python$ python setup.py install```
    - if your ```python``` links to ```python2``` run ```python3 setup.py install``` instead
4. Import the package or module(s):
    ``` 
    import tdma_api # -or-
    from tdma_api import auth     # authorization methods and objects
    from tdma_api import get      # 'getter' objects and utilities
    from tdma_api import stream   # 'streaming' class and subscriptions
    from tdma_api import execute  # order objects, builders and execution calls
    ```
    - the python package will try to load the library automatically
    - if it can't, it will output an error message on package import 
        - the most common issue is the library not being installed in the default library search path
        - move the lib(see [Install](#install)) or load it manually:   
            ```>>> tdma_api.clib.init("path/to/lib.so")```
    - if you get an error message concerning the dependencies you'll need to
      move them to a location the dynamic linker can find. (see [Install](#install))

### Conventions
- - -

- All front-end C++ library code is in namespace ```tdma```. 

- **ONLY** Symbol strings are converted to upper-case by the library, e.g 'sPy' -> 'SPY'.

- This library generally ignores character encoding issues:
    - uses 1 byte wide characters: char, std::string etc.
    - assumes utf-8 encoded string data is returned from server
    - C -> Python is handled by ctypes.py

- Currently, decimal values are represented with ```double``` type and passed to the API as a string with four decimal places of fixed precision. This **shouldn't** be an issue for the current nature of access, but keep in mind:
    - Numbers requiring more precision than 4 decimal points will be passed to the API *incorrectly*
    - Very large floating point numbers can lose accuracy
    - Rounding behavior is the default used by stringstream with std::fixed and std::setprecision(4)
    
- Enums are defined for both C and C++ code using MACROS. Python mimics these enums by defining constant values. For example:
    ```
    DECL_C_CPP_TDMA_ENUM(AdminCommandType, 0, 2,
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, LOGIN),
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, LOGOUT),
        BUILD_C_CPP_TDMA_ENUM_NAME(AdminCommandType, QOS)
        );
    ```
    ... indicates values from index 0(LOGIN) to 2(QOS) are valid to pass to the API. The macros expand to:
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
        - ```FreeOrderLegBuffer(OrderLeg_C *legs)```
        - ```FreeOrderTicketBuffer(OrderTicket_C *orders)```
    - All buffers are accompanied w/ a size_t* arg to be filled w/ the size of the populated buffer. Buffers of type char* return the size of the string + 1 for the null term.
    - Arrays passed by caller have to pass a size_t value of the number of elements

- Only ABI functions(those ending in '_ABI') are directly exported from the library. The C interface calls and C++ calls/classes wrap these as header-defined statics/inlines.

### Errors & Exceptions
- - -
All exceptional/error states from the C++ interface will cause exceptions to be thrown. 

- If ```ALLOW_EXCEPTIONS_ACROSS_ABI``` is not defined (default behavior) exceptions will be caught at the library boundary, converted to error codes, and reconstituted/rethrown from the client side. 
    - Exceptions defined by the libaray will contain the line number, source file name, and corresponding error codes. Upon being reconstituted/rethrown this information will be appended to the string returned by the exception's ```what()``` method.
    - Exceptions inheriting from std::exception and NOT defined by the library will be reconstituted/rethrown as
```tdma::StdException``` objects without additional line number and filename info.
    - Anything else thrown be will be reconstituted/rethrown as a ```tdma::UnknownException``` object.

- Library Exceptions:
    - ```APIExcetion``` : base class and generic exceptions
        - ```LocalCredentialException``` : an issue creating/loading/storing/using credentials
        - ```ValueException``` : bad/invalid argument to a function or constructor (checked locally)
        - ```TypeException``` : type inconsistency of a proxy object
        - ```MemoryError``` : error allocating memory within the ABI layer
        - ```ConnectException``` : general error connecting/communicating with the server  
            - ```AuthenticationException``` : error authenticating with the server
            - ```InvalidRequest``` : user made an invalid/malformed request to the server
            - ```ServerError``` : server has returned some type of error status
        - ```StreamingException``` : general error connecting/communicating via StreamingSession   
        - ```ExectuteException``` : general error building/managing/executing orders
        - ```StdException``` : non-library exception, derived from std::exception, thrown from the library
        - ```UnknownException``` : indicates something unexpected was thrown from the library         

- 3rd Party Exceptions:
    - ```json::exception``` : base class for exceptions from the json library (review the documentation
for the derived classes). If ```ALLOW_EXCEPTIONS_ACROSS_ABI``` is not defined (default behavior) this will be rethrown as ```StdException```.

- The C interface has corresponding error codes:
    ```
    #define TDMA_API_ERROR 1
    #define TDMA_API_CRED_ERROR 2
    #define TDMA_API_VALUE_ERROR 3
    #define TDMA_API_TYPE_ERROR 4
    #define TDMA_API_MEMORY_ERROR 5

    #define TDMA_API_CONNECT_ERROR 101
    #define TDMA_API_AUTH_ERROR 102
    #define TDMA_API_REQUEST_ERROR 103
    #define TDMA_API_SERVER_ERROR 104

    #define TDMA_API_STREAM_ERROR 201

    #define TDMA_API_EXECUTE_ERROR 301

    #define TDMA_API_STD_EXCEPTION 501

    #define TDMA_API_UNKNOWN_EXCEPTION 1001
    ```
- The Python interface throws ```tdma_api.clib.LibraryNotLoaded``` and ```tdma_api.clib.CLibException``` w/ the error code, name, and message returned from the library.         

- ```LastErrorMsg```, ```LastErrorCode```, ```LastErrorLineNumber```, and ```LastErrorFilename``` can be used to get the last error message, code, line number and filename, respectively. This error state is only set by errors/exceptions from WITHIN the library, not errors/exceptions from code defined in the headers.

### Authentication
- - -
Authentication is done through OAuth2 using your account login information. 

1. Get an access code. (***once***)
2. Use the access code to get a `Credentials` object containing access and refresh tokens. (***once***)
3. Pass the `Credentials` object to the various library calls to get data, access your account etc. - the library will refresh the access token for you when it expires.
4. Use the library to encrypt and store your `Credentials` object when done 
5. Use the library to load and decrypt your `Credentials` object for future use

#### Access Code

##### Simple Approach For Personal Use

1. [Set up a developer account.](https://developer.tdameritrade.com/user/register)
    - Set the 'redirect_uri' field to the localhost: `https://127.0.0.1`
    - Set a 'client_id' e.g `MY_ID@AMER.OAUTHAP`
2. Open the ```tools/get-access-code.html``` file with your web browser and follow instructions.
    - Temporarily disable any browser add-ons that block pop-ups/redirects/JavaScript
    - If this doesn't work [follow these instructions](https://developer.tdameritrade.com/content/simple-auth-local-apps).
3. **Securely** record the access code it gives you. You'll only need to pass this to the API **once**. Once you use this code to create and successfully store a Credentials object(see below) you can dispose of it.

##### Other Approaches
  
For more robust authentication e.g writing an app for 3rd party users. You'll need to [set up a developer account.](https://developer.tdameritrade.com/user/register) (The 'redirect_uri' field to use depends on what you want to do.)
  
- [Set up your own server](https://developer.tdameritrade.com/content/web-server-authentication-python-3)
- Use a 3rd party solution e.g [auth0](https://auth0.com)

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
  
You can, for instance, create a static or global ```CredentialsManager``` instance that will exist over the lifetime of the program, storing the credentials on exit. Just use the ```.credentials``` member as an argument for the API calls, where required. Keep in mind, with this approach the password will be stored in memory, in plain-text, for the life of the ```CredentialsManager``` object.
    
**C code should explicitly close the Credentials object to deallocate the underlying resources.** (C++ and Python define destructors that do this for you.)
```
inline int
CloseCredentials(struct Credentials* pcreds );
```

### Access
- - -

- ##### *Get*

    For queries, (non-streaming) real-time data, and account information you'll make HTTPS Get requests through 'Getter' objects that internally use libcurl and return json objects or strings. [Please review the full documentation](README_GET.md).

- ##### *Streaming*

    For real-time, low(er)-latency streaming data you'll establish a long-lived WebSocket connection through the StreamingSession class that will callback with json objects or strings. [Please review the full documentation](README_STREAMING.md).

- ##### *Execute* 

    For executing trades you'll make HTTPS Put/Post/Delete requests using the JSON from OrderTicket/Leg objects.  Building OrderTickets/Legs can be done manually or through static Builders that help with popular order types. ***This has undergone very limited live testing - we are waiting on a mechanism from Ameritrade to test execution outside of live trading***. [Please review the preliminary documentation](README_EXECUTE.md).


### Utilities
- - -

#### DynamicDataStore

[A module for TDAmeritradeAPI](DynamicDataStore) that attempts to abstract away the details of retrieving historical and streaming data, providing an indexable interface while caching historical data to disk.

#### Option Symbols

To construct a standard option symbol that can be used with the certain getter and execution objects:

```
[C++]
inline std::string
BuildOptionSymbol( const std::string& underlying,
                     unsigned int month,
                     unsigned int day,
                     unsigned int year,
                     bool is_call,
                     double strike )

[C]
static inline int
BuildOptionSymbol( const char* underlying,
                     unsigned int month,
                     unsigned int day,
                     unsigned int year,
                     int is_call,
                     double strike,
                     char **buf,
                     size_t *n )

[Python]
def common.build_option_symbol(underlying, month, day, year, is_call, strike):
    returns -> str
```

This is not guaranteed to work on all underlying types but generally:

```BuildOptionSymbol("SPY", 1, 17, 2020, true, 300.00) --> "SPY_011720C300"```

For Example:
```
string spy_c300 = BuildOptionSymbol("SPY", 1, 17, 2020, true, 300.00);
string spy_p250 = BuildOptionSymbol("SPY", 1, 17, 2020, false, 250.00);

QuotesGetter qg(creds, {spy_c300, spy_p250});
qg.get();
```
**If using C don't forget to call ```FreeBuffer``` on the populated 'buf' when done.**

To check if a standard option symbol string is formatted properly:
```
[C++]
inline void
CheckOptionSymbol( const std::string& symbol )

[C]
static inline int
CheckOptionSymbol( const char* symbol )

[Python]
def common.check_option_symbol( symbol ):
```
Invalid symbols will throw ```ValueException```(C++,Python) or return ```TDMA_API_VALUE_ERROR```(C). C code can use ```LastErrorMsg``` to get a description of the issue.


#### LICENSING & WARRANTY
- - -

*TDAmeritradeAPI is released under the GNU General Public License(GPL); a copy (LICENSE.txt) should be included. If not, see http://www.gnu.org/licenses. The author reserves the right to issue current and/or future versions of TDAmeritradeAPI under other licensing agreements. Any party that wishes to use TDAmeritradeAPI, in whole or in part, in any way not explicitly stipulated by the GPL - including, but not limited to, commercial use - is thereby required to obtain a separate license from the author. The author reserves all other rights.*

*TDAmeritradeAPI includes 3rd party material operating under different licensing agreements which, although requiring adherence, do not to subsume or subordinate this agreement.*

*This software/program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. By choosing to use this software/program - under the broadest interpretation of the term 'use' - you absolve the author of ANY and ALL responsibility, for ANY and ALL damages incurred; even damages resulting from the author's gross negligence; damages including, but not limited to, those arising from the accuracy, timeliness, responsiveness, and general operation of the aformentioned software/program.*

























 
