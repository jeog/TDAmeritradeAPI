### Dynamic Data Store

A module for TDAmeritradeAPI that attempts to abstract away the details of retrieving, curating, and storing historical and streaming data. 

For each symbol a time-contiguous array of OHLCV 1-min bars is loaded from disk and maintained in memory. As new streaming data is received it's automatically appended - any gaps are filled. A data accessor object is provided to allow access through relative indices or times, for single bars or sequences. When done the arrays are stored to disk so as to be availible for future sessions.

It's currently in early devlopment and will undergoe significant changes. It only works with Equties and ETFs as those are the only instruments for which TDMA provides historical data. 


#### Structure

```
--------------|-----------------------------------------------------------------
Interfaces    |            ADMIN              |      DATA ACCESSOR OBJECTS
              |  Start() Add() Remove() etc.  | .find() .between() .cbegin() etc.
              |     ||           ||           |                   /\
--------------|-----||-----------||-------------------------------||------------
              |     ||           \/                               ||
Data Layer    |     ||      Collect/Sync Data ========>  'Deque' for each symbol
              |     ||         /\        /\                       /\
--------------|-----||---------||--------||-----------------------||------------
              |     ||         \/        \/                       ||
TDMA API      |     ||     Getters  StreamingSession              ||
	      |	    ||	 	                                  ||
--------------|-----||--------------------------------------------||------------
              |     \/                                            \/
Storage Layer |              'FRONT'files, 'BACK' files, index
--------------|-----------------------------------------------------------------
```

#### Features
- Abstract away all data retrieval, curation, and storage
- Provide a simple interface for accessing OHLCV data
    - via relative indices: e.g data[0], data[100], .copy_between(100,0)
    - via minutes-since-epoch: eg. data[25896415], data[25896400], .copy_between(25896400, 25896415)
    - via const iterators: e.g .cbegin(), cend(), .find(25896415), .between(100,0)
- Fill missing bars w/ empties for contiguous data and O(C) lookups
- Avoid any local-external time sync issues by only using timestamps from server
- Store/Load data to/from user-readable text files


#### Caveats
- The most recent bar exists only if there is a trade in it (without local time sync there's no way to know the most current bar hasn't been received.)
- Upon initialization, all the active symbols will get updated using tdma::HistoricalRangeGetter. This mechanism allows for no more than 1 call every 500msec. If, for instance, you have 30 symbols/stores this could take 15+ seconds of waiting on the first 'Update'.
- Pay attention to how start/end times and indices are passed and the order data is returned. 'Start' times are passed first and are <= 'end' times, which are passed second(inclusive range). 'Start' indices are passed first and are >= 'end' indices(index 0 is most recent bar). When data is returned as a vector or pair of const iterators the OPPOSITE is true: most-recent data is first(.front() or .first), oldest is last(.back() or .second). The end iterator is 1 past the oldest.


#### Looking Forward
- Allow local-external time sync to insure a most recent bar(even if empty).
- Save to disk in realtime (not just on Finalize()); 
- Provide data accessor methods to return larger (synthetic) bars, e.g 5, 15 minute bars


#### Build

Currently no build system. Simply compile the source files in '/src' and include 'tdma_data_store.h' with your code, being sure to indicate the location of '/include', and link with the TDAmeritradeAPI library. 

For example, using a source filed called my_code.cpp (w/ a 'main' function defined):
```
user@host:~/dev/TDAmeritradeAPI/DynamicDataStore$ g++ -std=c++11 my_code.cpp src/backing_store.cpp src/data_store.cpp src/logging.cpp -Iinclude -I../include -L../Release -Wl,-rpath,../Release -lTDAmeritradeAPI -o my_code.out

```

Has only been tested on linux/gcc.




#### Admin Interface
```
#include "tdma_data_store.h"

using namespace ds;
```

```
bool
Initialize( const std::string& dir_path, Credentials& creds );
```

This will load all symbols that currently exist on disk (those previously 'added' and not 'removed').
'dir_path' is a directory (that must already exist) where the index, log, and data files are (or will be) saved.

***Inititalize must be called and succeed before anything else can happen.***


```
bool
Add( const std::string& symbol );
```

Add a symbol to the store, create the underlying file(s) if it doesn't exist.

```
bool
Remove( const std::string& symbol, bool delete_file=false);
```

Remove a symbol from the store if it exists. Set 'delete_file' to true if you also want to remove/delete the underlying file(s).

```
bool
Start( std::chrono::milliseconds listening_timeout 
          = std::chrono::milliseconds(60000) );
```

Begin the streaming connection. If successful and at least one symbol exists ```IsRunning() == true```.

It doesn't need to be running to use ```Add()``` or ```Remove()``` but it does if you want to use the index-based ```DataAccessor``` methods (at all) or the time-based methods outside of the available range.

```
void
Update();
```
**Important:** Internally streaming data is stored in queues/caches and not released to the main deques until:
1. one of the data revieval methods of ```DataAccessor``` (e.g .between(), .operator[]) is used AND returns successfully - **EXCEPT FOR** .cbegin() and .cend(); or 
2. ```Update()``` is called

This allows for consistency between the underlying data collection methods and the methods to query the current start/end times/indices. (see example below)

```
void
Stop();
```

Stop the streaming connection.

```
void
Finalize();
```

Store data to files and return to state before ```Initialize(...)``` was called.

```
bool
IsInitialized();

bool
IsRunning();

std::set<std::string>
GetSymbols();

bool
Contains( const std::string& symbol );
```


#### Data Access Interface
```
#include "tdma_data_store.h"

using namespace ds;
```

```
struct OHLCVData{
    unsigned long long min_since_epoch;
    double open;
    double high;
    double low;
    double close;
    long long volume;
    // ...
};
```
How bar data is stored.

```
class DataAccessor{
public:
    typedef std::deque<OHLCVData>::const_iterator const_iterator;

    DataAccessor( const std::string& symbol );
    // ...
};
```

Class used for querying and retrieving data from the store.

```DataAccessor``` should be created using a symbol that has already been added. (```Contains(symbol) == true```) If not it will throw a std::logic_error.

It will also throw a std::logic_error if:
1. not initialized(```IsInitialized() == false```)
2. streamer isn't running(```IsRunning() == false```) and an access-by-index method is called
3. streamer isn't running and an access-by-time method is called for a time outside the currently available range.

It will throw std::invalid_argument if passed invalid times or indices.

##### Query
```
    std::chrono::minutes
    start_minute() const;
```
```
    std::chrono::minutes
    end_minute() const;
```
```
    std::pair<std::chrono::minutes, std::chrono::minutes>
    start_end_minutes() const;
```
```
    int
    start_index() const;
```
```
    int
    minute_to_index( std::chrono::minutes min_since_epoch ) const;
```
```
    std::chrono::minutes
    index_to_minute( unsigned int indx ) const;
```

- negative return values indicate unavailable time/index 

##### Copy
```
    OHLCVData
    operator[](unsigned int indx) const;
```
```
    OHLCVData
    operator[](std::chrono::minutes min_since_epoch) const;
```
```
    std::vector<OHLCVData>
    copy_between(std::chrono::minutes start_min_since_epoch,
                 std::chrono::minutes end_min_since_epoch) const;
```
```
    std::vector<OHLCVData>
    copy_between(unsigned int start_indx, unsigned int end_indx=0) const;
```
```
    // return between 'start_min_since_epoch' and most recent bar
    std::vector<OHLCVData> 
    copy_between(std::chrono::minutes start_min_since_epoch) const;
```
```
    // copy ALL
    std::vector<OHLCVData> 
    copy_between() const;
```

- empty vectors or ```OHLCVData::null``` objects indicate unavailable time/index
- **all methods call Update() before returning**

##### Const Iterators  
```
    const_iterator // newest
    cbegin() const; // NO UPDATE CALLED
```
```
    const_iterator // oldest + 1
    cend() const; // NO UPDATE CALLED
```
```
    std::pair< DataAccessor::const_iterator, // newest
               DataAccessor::const_iterator> // oldest + 1
    find(std::chrono::minutes min_since_epoch) const;
```
```
    std::pair< DataAccessor::const_iterator, // newest
               DataAccessor::const_iterator> // oldest + 1
    find(unsigned int indx) const;
```
```
    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between(std::chrono::minutes start_min_since_epoch,
            std::chrono::minutes end_min_since_epoch) const;
```
```
    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between(unsigned int start_indx, unsigned int end_indx=0) const;
```
```
    // return {.cbegin(), '1 past start_min_since_epoch'}
    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between(std::chrono::minutes start_min_since_epoch) const; 
```
```
    // return {.cbegin(), .cend()}
    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between() const; 
```
- pair.first == pair.second if unavailable time/index
- cbegin() == cend() if no data available
- **all methods EXCEPT cbegin/cend call Update() before returning**
- the order of the iterators is OPPOSITE that of the args; (unless they are ==, see above) the first iterator is the most recent(end arg), while the second is the oldest + 1 (start arg -1) 
- as mentioned, the second iterator references one position older than 'start'

#### Example 
```
#include "tdma_data_store.h"

using namespace ds;
using namespace std::chrono;


if( !Initialize(directory_path, credentials) ){
    // error
}

if( !Start() ){
    // error
}

if( !Add("SPY") ){
    // error
}

DataAccessor spy("SPY");

Update(); // UPDATE FIRST

minutes e = spy.end_minute(); // THEN GET NEWEST MINUTES-SINCE-EPOCH
minutes s = spy.start_minute();

OHLCVData d_new = spy[e]; // get newest bar
/* 
 * 'Update() called internally by operator[] after retrieval so 'e' may no 
 *  longer represent the minutes of the most recent bar
 */

unsigned int si = spy.start_index(); // GET OLDEST INDEX
OHLCVData d_old = spy[si]; // get oldest bar, Update() CALLED 
/*
 * since Update() was called 'si' may no longer be the oldest index'
 */

// COPY all
std::vector<OHLCVdata> d_all = spy.copy_between(); // Update() CALLED

e = spy.end_minute(); // get a new end

// COPY from 2 minutes before end to end (inclusive)
// note - 'end' is most recent data
std::vector<OHLCVData> d_from = spy.copy_between( e - minutes(2) ); // Update() CALLED
/*
 *  d_from.front() <- most recent data 
 *  d_from.back() <- oldest data ( e - minutes(2) )
 */

// get iter of most recent data point, Update() CALLED
std::pair< DataAccessor::const_iterator, 
           DataAccessor::const_iterator> iters = spy.find(0); 
if( iters.first == iters.second ){
   // ERROR
}

/* IMPORANT - iterators are returned in the opposite order as arguments are passed:
 *            e.g .between(start, end) takes the start/oldest first and the 
 *            end/newest last. The iterator pair that is returned has 
 *            end/newest/front first (.first) and (start/oldest/back + 1)
 *            last (.second) 
 * 
 *            NOTICE - the second iterator is ONE PAST the last element
 *
 *            NOTICE - if the iters are == there is no valid data
 */

if( spy.cend() - spy.cbegin() >= 10 ){ // Update() NOT CALLED on cbegin/cend
      minutes last_min = spy.start_minute(); 

      // iters for the range between [last_min+9, last_min] INCLUSIVE
      auto iters = spy.between( last_min, last_min +9); // Update() CALLED      

      // iterates newest(last_min+9) to oldest(last_min)
      for( auto a = iters.first; a < iters.second; ++a ){
          std::cout<< DataAccessor:ToFormatString( a->min_since_epoch ) << " "
                   << *a << std::endl;
      }

      // copy iters into vector
      std::vector<OHLCVData> v = DataAccessor::ToSequence( iters );
}

// keep 'SPY' around for future sessions so don't call 'Remove()'
Stop();
Finalize();

```
