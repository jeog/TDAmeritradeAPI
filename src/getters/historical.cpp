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

#include <vector>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <cctype>
#include <string>

#include "../../include/_tdma_api.h"

using namespace std;

namespace tdma {

class HistoricalGetterBaseImpl
        : public APIGetterImpl {
    std::string _symbol;
    FrequencyType _frequency_type;
    unsigned int _frequency;
    bool _extended_hours;

    virtual void
    build() = 0;

    void
    _throw_if_invalid_frequency() const
    {
        auto valid_frequencies =
                VALID_FREQUENCIES_BY_FREQUENCY_TYPE.find(_frequency_type);
        assert( valid_frequencies != end(VALID_FREQUENCIES_BY_FREQUENCY_TYPE) );
        if( valid_frequencies->second.count(_frequency) == 0 ){
            throw ValueException("invalid frequency");
        }
    }

protected:
    HistoricalGetterBaseImpl( Credentials& creds,
                          const std::string& symbol,
                          FrequencyType frequency_type,
                          unsigned int frequency,
                          bool extended_hours )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbol(symbol),
            _frequency_type(frequency_type),
            _frequency(frequency),
            _extended_hours(extended_hours)
        {
            if( symbol.empty() )
                throw ValueException("empty symbol");
            _throw_if_invalid_frequency();
        }

    std::vector<std::pair<std::string, std::string>>
    build_query_params() const
    {
        return { {"frequencyType", to_string(_frequency_type)},
                 {"frequency", to_string(_frequency)},
                 {"needExtendedHoursData", _extended_hours ? "true" : "false"} };
    }

    void
    set_frequency(unsigned int frequency)
    {
        _frequency = frequency;
        _throw_if_invalid_frequency();
    }

    void
    set_frequency_type(FrequencyType frequency_type)
    { _frequency_type = frequency_type; }

public:
    typedef HistoricalGetterBase ProxyType;

    std::string
    get_symbol() const
    { return _symbol; }

    unsigned int
    get_frequency() const
    { return _frequency; }

    FrequencyType
    get_frequency_type() const
    { return _frequency_type; }

    bool
    is_extended_hours() const
    { return _extended_hours; }

    void
    set_symbol(const std::string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol");
        _symbol = symbol;
        build();
    }

    void
    set_extended_hours(bool extended_hours)
    {
        _extended_hours = extended_hours;
        build();
    }

    string
    get()
    {
        _throw_if_invalid_frequency();
        return APIGetterImpl::get();
    }

};


// TODO MAKE CLEAR IN DOCS THE ORDER WE NEED TO CHANGE PERIODS/FREQUENCIES
//      BECUASE OF THE INTERNAL CHECKS/CONTINGENCIES
class HistoricalPeriodGetterImpl
        : public HistoricalGetterBaseImpl {
    PeriodType _period_type;
    unsigned int _period;

    void
    _build()
    {
        vector<pair<string,string>> params( build_query_params() );
        params.emplace_back( "periodType", to_string(_period_type) );
        params.emplace_back( "period", to_string(_period) );

        string qstr = util::build_encoded_query_str(params);
        string url = URL_MARKETDATA + util::url_encode(get_symbol())
                     + "/pricehistory?" + qstr;
        APIGetterImpl::set_url(url);
    }

    /*virtual*/ void
    build()
    { _build(); }

    void
    _throw_if_invalid_frequency_type() const
    {
        auto valid_freq_types =
                VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.find(_period_type);
        assert( valid_freq_types != end(VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE) );
        if( valid_freq_types->second.count(get_frequency_type()) == 0 ){
            throw ValueException("invalid frequency type");
        }
    }

    void
    _throw_if_invalid_period() const
    {
        auto valid_periods = VALID_PERIODS_BY_PERIOD_TYPE.find(_period_type);
        assert( valid_periods != end(VALID_PERIODS_BY_PERIOD_TYPE) );
        if( valid_periods->second.count(_period) == 0 ){
            throw ValueException("invalid period (" + to_string(_period) + ")");
        }
    }

public:
    typedef HistoricalPeriodGetter ProxyType;

    HistoricalPeriodGetterImpl( Credentials& creds,
                            const std::string& symbol,
                            PeriodType period_type,
                            unsigned int period,
                            FrequencyType frequency_type,
                            unsigned int frequency,
                            bool extended_hours )
    :
        HistoricalGetterBaseImpl(creds, symbol, frequency_type, frequency,
                                 extended_hours),
        _period_type(period_type),
        _period(period)
    {
        _throw_if_invalid_frequency_type();
        _throw_if_invalid_period();
        _build();
    }

    PeriodType
    get_period_type() const
    { return _period_type; }

    unsigned int get_period() const
    { return _period; }

    void
    set_period(PeriodType period_type, unsigned int period)
    {
        _period_type = period_type;
        _period = period;
        build();
    }

    void
    set_frequency(FrequencyType frequency_type, unsigned int frequency)
    {
        // frequency_type first so we can check frequency
        HistoricalGetterBaseImpl::set_frequency_type(frequency_type);
        HistoricalGetterBaseImpl::set_frequency(frequency);
        build();
    }

    string
    get()
    {
        _throw_if_invalid_frequency_type();
        _throw_if_invalid_period();
        return HistoricalPeriodGetterImpl::get();
    }

};



class HistoricalRangeGetterImpl
        : public HistoricalGetterBaseImpl {
    unsigned long long _start_msec_since_epoch;
    unsigned long long _end_msec_since_epoch;

    void
    _build()
    {
        vector<pair<string,string>> params( build_query_params() );
        params.emplace_back( "startDate", to_string(_start_msec_since_epoch) );
        params.emplace_back( "endDate", to_string(_end_msec_since_epoch) );

        string qstr = util::build_encoded_query_str(params);
        string url = URL_MARKETDATA + util::url_encode(get_symbol())
                     + "/pricehistory?" + qstr;
        APIGetterImpl::set_url(url);
    }

    /*virtual*/ void
    build()
    { _build(); }

public:
    typedef HistoricalRangeGetter ProxyType;

    HistoricalRangeGetterImpl( Credentials& creds,
                              const std::string& symbol,
                              FrequencyType frequency_type,
                              unsigned int frequency,
                              unsigned long long start_msec_since_epoch,
                              unsigned long long end_msec_since_epoch,
                              bool extended_hours )
        :
            HistoricalGetterBaseImpl(creds, symbol, frequency_type, frequency,
                                     extended_hours),
            _start_msec_since_epoch(start_msec_since_epoch),
            _end_msec_since_epoch(end_msec_since_epoch)
        {
            _build();
        }

   unsigned long long
   get_end_msec_since_epoch() const
   { return _end_msec_since_epoch; }

   unsigned long long
   get_start_msec_since_epoch() const
   { return _start_msec_since_epoch; }

   void
   set_frequency(FrequencyType frequency_type, unsigned int frequency)
   {
       // frequency_type first so we can check frequency
       HistoricalGetterBaseImpl::set_frequency_type(frequency_type);
       HistoricalGetterBaseImpl::set_frequency(frequency);
       build();
   }

   void
   set_end_msec_since_epoch(unsigned long long end_msec_since_epoch)
   {
       _end_msec_since_epoch = end_msec_since_epoch;
       build();
   }

   void
   set_start_msec_since_epoch(unsigned long long start_msec_since_epoch)
   {
       _start_msec_since_epoch = start_msec_since_epoch;
       build();
   }
};

} /* tdma */

using namespace tdma;

int
HistoricalGetterBase_GetSymbol_ABI( Getter_C *pgetter,
                                        char **buf,
                                        size_t *n,
                                        int allow_exceptions )
{
    return GetterImplAccessor<char**>::template get<HistoricalGetterBaseImpl>(
        pgetter, &HistoricalGetterBaseImpl::get_symbol, buf, n, allow_exceptions
        );
}

int
HistoricalGetterBase_SetSymbol_ABI( Getter_C *pgetter,
                                        const char *symbol,
                                        int allow_exceptions )
{
    return GetterImplAccessor<char**>::template set<HistoricalGetterBaseImpl>(
        pgetter, &HistoricalGetterBaseImpl::set_symbol, symbol, allow_exceptions
        );
}

int
HistoricalGetterBase_GetFrequency_ABI( Getter_C *pgetter,
                                           unsigned int *frequency,
                                           int allow_exceptions )
{
    return GetterImplAccessor<unsigned int>::template
        get<HistoricalGetterBaseImpl>(
            pgetter, &HistoricalGetterBaseImpl::get_frequency, frequency,
            "frequency", allow_exceptions
        );
}

int
HistoricalGetterBase_GetFrequencyType_ABI( Getter_C *pgetter,
                                                int *frequency_type,
                                                int allow_exceptions )
{
    return GetterImplAccessor<int>::template
        get<HistoricalGetterBaseImpl, FrequencyType>(
            pgetter, &HistoricalGetterBaseImpl::get_frequency_type,
            frequency_type, "frequency_type", allow_exceptions
        );
}

int
HistoricalGetterBase_IsExtendedHours_ABI( Getter_C *pgetter,
                                               int *is_extended_hours,
                                               int allow_exceptions )
{
    return GetterImplAccessor<int>::template
        get<HistoricalGetterBaseImpl, bool>(
            pgetter, &HistoricalGetterBaseImpl::is_extended_hours,
            is_extended_hours, "is_extended_hours", allow_exceptions
        );
}

int
HistoricalGetterBase_SetExtendedHours_ABI( Getter_C *pgetter,
                                               int is_extended_hours,
                                               int allow_exceptions )
{
    return GetterImplAccessor<int>::template
        set<HistoricalGetterBaseImpl, bool>(
            pgetter, &HistoricalGetterBaseImpl::set_extended_hours,
            is_extended_hours, allow_exceptions
        );
}

/* HistoricalPeriodGetter */
int
HistoricalPeriodGetter_Create_ABI( struct Credentials *pcreds,
                                       const char* symbol,
                                       int period_type,
                                       unsigned int period,
                                       int frequency_type,
                                       unsigned int frequency,
                                       int extended_hours,
                                       HistoricalPeriodGetter_C *pgetter,
                                       int allow_exceptions )
{
    int err = getter_is_creatable<HistoricalPeriodGetterImpl>(
        pcreds, pgetter, allow_exceptions
        );
    if( err )
        return err;

    err = check_abi_enum(FrequencyType_is_valid, frequency_type, pgetter,
                         allow_exceptions);
    if( err )
        return err;

    err = check_abi_enum(PeriodType_is_valid, period_type,  pgetter,
                         allow_exceptions);
    if( err )
        return err;

    if( !symbol ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return tdma::handle_error<tdma::ValueException>(
            "null symbol", allow_exceptions
            );
    }

    static auto meth = +[]( Credentials *c, const char* s, int pt,
                            unsigned int p, int ft, unsigned int f, int eh ){
        return new HistoricalPeriodGetterImpl( *c, s, static_cast<PeriodType>(pt),
                                               p, static_cast<FrequencyType>(ft),
                                               f, static_cast<bool>(eh) );
    };

    HistoricalPeriodGetterImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds, symbol,
                                     period_type, period, frequency_type,
                                     frequency, extended_hours );
    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( HistoricalPeriodGetter::TYPE_ID_LOW ==
            HistoricalPeriodGetter::TYPE_ID_HIGH );
    pgetter->type_id = HistoricalPeriodGetter::TYPE_ID_LOW;
    return 0;
}

int
HistoricalPeriodGetter_Destroy_ABI( HistoricalPeriodGetter_C *pgetter,
                                        int allow_exceptions)
{ return destroy_getter<HistoricalPeriodGetterImpl>(pgetter, allow_exceptions); }

int
HistoricalPeriodGetter_GetPeriodType_ABI( HistoricalPeriodGetter_C *pgetter,
                                               int *period_type,
                                               int allow_exceptions )
{
    return GetterImplAccessor<int>::template
        get<HistoricalPeriodGetterImpl, PeriodType>(
            pgetter, &HistoricalPeriodGetterImpl::get_period_type,
            period_type, "period_type", allow_exceptions
        );
}


int
HistoricalPeriodGetter_GetPeriod_ABI( HistoricalPeriodGetter_C *pgetter,
                                           unsigned int *period,
                                           int allow_exceptions )
{
    return GetterImplAccessor<unsigned int>::template
        get<HistoricalPeriodGetterImpl>(
            pgetter, &HistoricalPeriodGetterImpl::get_period,
            period, "period", allow_exceptions
        );
}

int
HistoricalPeriodGetter_SetPeriod_ABI( HistoricalPeriodGetter_C *pgetter,
                                           int period_type,
                                           unsigned int period,
                                           int allow_exceptions )
{
    int err = check_abi_enum(PeriodType_is_valid, period_type, pgetter,
                             allow_exceptions);
    if( err )
        return err;

    return GetterImplAccessor<int>::template
        set<HistoricalPeriodGetterImpl, PeriodType>(
            pgetter, &HistoricalPeriodGetterImpl::set_period, period_type,
            period, allow_exceptions
            );
}

int
HistoricalPeriodGetter_SetFrequency_ABI( HistoricalPeriodGetter_C *pgetter,
                                             int frequency_type,
                                             unsigned int frequency,
                                             int allow_exceptions )
{
    int err = check_abi_enum(FrequencyType_is_valid, frequency_type, pgetter,
                             allow_exceptions);
    if( err )
        return err;

    return GetterImplAccessor<int>::template
        set<HistoricalPeriodGetterImpl, FrequencyType>(
            pgetter, &HistoricalPeriodGetterImpl::set_frequency, frequency_type,
            frequency, allow_exceptions
            );
}

/* HistoricalRangeGetter */
int
HistoricalRangeGetter_Create_ABI( struct Credentials *pcreds,
                                       const char* symbol,
                                       int frequency_type,
                                       unsigned int frequency,
                                       unsigned long long start_msec_since_epoch,
                                       unsigned long long end_msec_since_epoch,
                                       int extended_hours,
                                       HistoricalRangeGetter_C *pgetter,
                                       int allow_exceptions )
{
    int err = getter_is_creatable<HistoricalRangeGetterImpl>(
        pcreds, pgetter, allow_exceptions
        );
    if( err )
        return err;

    err = check_abi_enum(FrequencyType_is_valid, frequency_type, pgetter,
                         allow_exceptions);
    if( err )
        return err;

    if( !symbol ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return tdma::handle_error<tdma::ValueException>(
            "null symbol", allow_exceptions
            );
    }

    static auto meth = +[]( Credentials *c, const char* s, int ft,
                            unsigned int f, unsigned long long sm,
                            unsigned long long em, int eh ){
        return new HistoricalRangeGetterImpl( *c, s,
                                              static_cast<FrequencyType>(ft),
                                              f, sm, em, static_cast<bool>(eh) );
    };

    HistoricalRangeGetterImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds, symbol,
                                     frequency_type, frequency,
                                     start_msec_since_epoch, end_msec_since_epoch,
                                     extended_hours );

    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( HistoricalRangeGetter::TYPE_ID_LOW ==
            HistoricalRangeGetter::TYPE_ID_HIGH );
    pgetter->type_id = HistoricalRangeGetter::TYPE_ID_LOW;
    return 0;
}

int
HistoricalRangeGetter_Destroy_ABI( HistoricalRangeGetter_C *pgetter,
                                        int allow_exceptions)
{ return destroy_getter<HistoricalRangeGetterImpl>(pgetter, allow_exceptions); }

int
HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *end_msec,
    int allow_exceptions )
{
    return GetterImplAccessor<unsigned long long>::template
        get<HistoricalRangeGetterImpl>(
            pgetter, &HistoricalRangeGetterImpl::get_end_msec_since_epoch,
            end_msec, "end_msec", allow_exceptions
            );
}

int
HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long end_msec,
    int allow_exceptions )
{
    return GetterImplAccessor<unsigned long long>::template
        set<HistoricalRangeGetterImpl>(
            pgetter, &HistoricalRangeGetterImpl::set_end_msec_since_epoch,
            end_msec, allow_exceptions
            );
}


int
HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long *start_msec,
    int allow_exceptions )
{
    return GetterImplAccessor<unsigned long long>::template
        get<HistoricalRangeGetterImpl>(
            pgetter, &HistoricalRangeGetterImpl::get_start_msec_since_epoch,
            start_msec, "start_msec", allow_exceptions
            );
}

int
HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI(
    HistoricalRangeGetter_C *pgetter,
    unsigned long long start_msec,
    int allow_exceptions )
{
    return GetterImplAccessor<unsigned long long>::template
        set<HistoricalRangeGetterImpl>(
            pgetter, &HistoricalRangeGetterImpl::set_start_msec_since_epoch,
            start_msec, allow_exceptions
            );
}


int
HistoricalRangeGetter_SetFrequency_ABI( HistoricalRangeGetter_C *pgetter,
                                             int frequency_type,
                                             unsigned int frequency,
                                             int allow_exceptions )
{
    int err = check_abi_enum(FrequencyType_is_valid, frequency_type, pgetter,
                             allow_exceptions);
    if( err )
        return err;

    return GetterImplAccessor<int>::template
        set<HistoricalRangeGetterImpl, FrequencyType>(
            pgetter, &HistoricalRangeGetterImpl::set_frequency, frequency_type,
            frequency, allow_exceptions
            );
}





