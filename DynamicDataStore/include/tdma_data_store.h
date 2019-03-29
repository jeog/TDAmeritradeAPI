/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/


#ifndef TDMA_DATA_STORE_H_
#define TDMA_DATA_STORE_H_

#include <string>

#include "tdma_common.h"

namespace ds {

const std::chrono::minutes ERROR_MINUTES(-1);

struct OHLCVData {
    unsigned long long min_since_epoch;
    double open;
    double high;
    double low;
    double close;
    long long volume;

    OHLCVData( unsigned long long min, double o, double h, double l,
               double c, long long v )
        :
            min_since_epoch( min ),
            open( o ),
            high( h ),
            low( l ),
            close( c ),
            volume( v )
        {}

    OHLCVData( unsigned long long min )
        :
            OHLCVData(min,0,0,0,0,0)
        {}

    OHLCVData() : OHLCVData(0) {}

    inline bool
    is_empty_bar() const
    { return !open && !high && !low && !close && !volume; }

    bool
    operator==(const OHLCVData& d) const
    { return min_since_epoch == d.min_since_epoch && open == d.open
        && high == d.high && low == d.low && close == d.close
        && volume == d.volume; }

    bool
    operator!=(const OHLCVData& d) const
    { return !(*this == d); }

    static bool
    IsNewer( const OHLCVData& a, const OHLCVData& b )
    { return a.min_since_epoch > b.min_since_epoch; }

    static bool
    IsOlder( const OHLCVData& a, const OHLCVData& b )
    { return a.min_since_epoch < b.min_since_epoch; }

    static const OHLCVData null;
};


bool
Initialize( const std::string& dir_path, Credentials& creds );

bool
Start( std::chrono::milliseconds listening_timeout
        = std::chrono::milliseconds(60000) );

bool
Add( const std::string& symbol );

bool
Remove( const std::string& symbol, bool delete_file = false );

void
Stop();

void
Finalize();

bool
IsInitialized();

bool
IsRunning();

std::set<std::string>
GetSymbols();

bool
Contains( const std::string& symbol );

void
Update();



class DataAccessor {
public:
    typedef std::deque<OHLCVData>::const_iterator const_iterator;

    DataAccessor( const std::string& symbol );

    void
    set_symbol( const std::string& symbol );

    std::string
    get_symbol() const;

    // QUERY
    std::chrono::minutes
    start_minute() const;

    std::chrono::minutes
    end_minute() const;

    std::pair<std::chrono::minutes, std::chrono::minutes>
    start_end_minutes() const;

    int
    start_index() const;

    int
    minutes_to_index( std::chrono::minutes min_since_epoch ) const;

    std::chrono::minutes
    index_to_minutes( unsigned int indx ) const;

    // COPY
    OHLCVData
    copy_at(std::chrono::minutes min_since_epoch) const;

    OHLCVData
    copy_at(unsigned int indx) const;

    OHLCVData
    operator[](unsigned int indx) const;

    OHLCVData
    operator[](std::chrono::minutes min_since_epoch) const;

    std::vector<OHLCVData>
    copy_between(std::chrono::minutes start_min_since_epoch,
                 std::chrono::minutes end_min_since_epoch) const;

    std::vector<OHLCVData>
    copy_between(unsigned int start_indx, unsigned int end_indx=0) const;

    std::vector<OHLCVData>
    copy_from(std::chrono::minutes start_min_since_epoch) const;

    std::vector<OHLCVData>
    copy_all() const;

    // ITERS
    const_iterator // newest
    cbegin() const; // NO UPDATE CALLED

    const_iterator // oldest + 1
    cend() const; // NO UPDATE CALLED

    std::pair< DataAccessor::const_iterator, // newest
               DataAccessor::const_iterator> // oldest + 1
    at(std::chrono::minutes min_since_epoch) const;

    std::pair< DataAccessor::const_iterator, // newest
               DataAccessor::const_iterator> // oldest + 1
    at(unsigned int indx) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between(std::chrono::minutes start_min_since_epoch,
            std::chrono::minutes end_min_since_epoch) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    between(unsigned int start_indx, unsigned int end_indx=0) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    from(std::chrono::minutes start_min_since_epoch) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    all() const;

    template< typename ContainerTy = std::vector<OHLCVData> >
    static ContainerTy
    ToSequence( const std::pair<const_iterator, const_iterator>& p )
    { return ContainerTy(p.first, p.second); }

    static OHLCVData
    ToObject( const std::pair<const_iterator, const_iterator>& p )
    { return (p.first == p.second) ? OHLCVData::null : *p.first; }

    static int
    ToMinuteOfHour( std::chrono::minutes min_since_epoch );

    static int
    ToHourOfDay( std::chrono::minutes min_since_epoch );

    static int
    ToDayOfMonth( std::chrono::minutes min_since_epoch );

    static int
    ToMonthOfYear( std::chrono::minutes min_since_epoch );

    static int
    ToYear( std::chrono::minutes min_since_epoch );

    static std::tuple<int, int, int, int, int>
    ToYYMMDDhhmm( std::chrono::minutes min_since_epoch );

    static std::string
    ToFormatString( std::chrono::minutes min_since_epoch,
                    const std::string& format_str = "%m/%d/%Y %H:%M");

    static std::chrono::minutes
    BuildMinutesSinceEpoch( unsigned int year,
                            unsigned int month,
                            unsigned int day,
                            unsigned int hour,
                            unsigned int minute );

private:
    std::string _symbol;

    void
    _set_symbol( const std::string& symbol );

    std::chrono::minutes
    _start_minute() const;

    std::chrono::minutes
    _end_minute() const;

    const_iterator // newest
    _cbegin() const;

    const_iterator // oldest + 1
    _cend() const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    _between(std::chrono::minutes start_min_since_epoch,
             std::chrono::minutes end_min_since_epoch) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    _between(unsigned int start_indx, unsigned int end_indx=0) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    _from(std::chrono::minutes start_min_since_epoch) const;

    typename std::pair< const_iterator, // newest
                        const_iterator > // oldest + 1
    _all() const;

};


std::ostream&
operator<<(std::ostream& out, const OHLCVData& data);


}; /* namespace ds */


#endif /* TDMA_DATA_STORE_H_ */
