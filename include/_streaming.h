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

#include <string>
#include <map>
#include <unordered_map>

#include "_tdma_api.h"

namespace tdma {

using namespace std;

StreamerServiceType
streamer_service_from_str(string service_name);


class StreamingSubscriptionImpl{
    StreamerServiceType _service;
    std::string _command;
    std::map<std::string, std::string> _parameters;

protected:
    StreamingSubscriptionImpl(
            StreamerServiceType service,
            const std::string& command,
            const std::map<std::string, std::string>& paramaters );

public:
    typedef StreamingSubscription ProxyType;

    static
    std::string
    encode_symbol(std::string symbol);

    StreamerServiceType
    get_service() const
    { return _service; }

    std::string
    get_command() const
    { return _command; }

    std::map<std::string, std::string>
    get_parameters() const
    { return _parameters; }

    virtual
    ~StreamingSubscriptionImpl(){}
};


class SubscriptionBySymbolBaseImpl
        : public StreamingSubscriptionImpl {
    std::set<std::string> _symbols;

    template<typename F>
    std::map<std::string, std::string>
    build_paramaters( const std::set<std::string>& symbols,
                        const std::set<F>& fields )
    {
        std::vector<std::string> symbols_enc;
        for(auto& s : symbols)
            symbols_enc.emplace_back(
                StreamingSubscriptionImpl::encode_symbol(s)
            );

        std::vector<std::string> fields_str(fields.size());
        std::transform( fields.begin(), fields.end(), fields_str.begin(),
                   [](F f){
                       return to_string(static_cast<unsigned int>(f));
                   } );

        return { {"fields", util::join(fields_str, ',')},
                 {"keys", util::join(symbols_enc, ',')} };
    }

public:
    typedef SubscriptionBySymbolBase ProxyType;

    std::set<std::string>
    get_symbols() const
    { return _symbols; }

protected:
    template<typename F>
    SubscriptionBySymbolBaseImpl( StreamerServiceType service,
                                      std::string command,
                                      const std::set<std::string>& symbols,
                                      const std::set<F>& fields )
        :
            StreamingSubscriptionImpl(service, command,
                                      build_paramaters(symbols, fields)),
            _symbols(symbols)
        {
        }
};


class QuotesSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = QuotesSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef QuotesSubscription ProxyType;

    QuotesSubscriptionImpl( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::QUOTE, "SUBS",
                                     symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class OptionsSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = OptionsSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef OptionsSubscription ProxyType;

    OptionsSubscriptionImpl( const std::set<std::string>& symbols,
                         const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::OPTION, "SUBS",
                                     symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class LevelOneFuturesSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneFuturesSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneFuturesSubscription ProxyType;

    LevelOneFuturesSubscriptionImpl( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::LEVELONE_FUTURES, "SUBS",
                                    symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class LevelOneForexSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneForexSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneForexSubscription ProxyType;

    LevelOneForexSubscriptionImpl( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::LEVELONE_FOREX, "SUBS",
                                     symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class LevelOneFuturesOptionsSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneFuturesOptionsSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneFuturesOptionsSubscription ProxyType;

    LevelOneFuturesOptionsSubscriptionImpl( const std::set<std::string>& symbols,
                                        const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::LEVELONE_FUTURES_OPTIONS,
                                     "SUBS", symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};

class NewsHeadlineSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = NewsHeadlineSubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef NewsHeadlineSubscription ProxyType;

    NewsHeadlineSubscriptionImpl( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::NEWS_HEADLINE,
                                     "SUBS", symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


// TODO implement ADD command
class ChartEquitySubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = ChartEquitySubscriptionField;
    static std::function<bool(int)> is_valid_field;

private:
    std::set<FieldType> _fields;

public:
    typedef ChartEquitySubscription ProxyType;

    ChartEquitySubscriptionImpl( const std::set<std::string>& symbols,
                             const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::CHART_EQUITY,
                                     "SUBS", symbols, fields),
            _fields(fields)
        {
        }

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class ChartSubscriptionBaseImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = ChartSubscriptionField;
    static std::function<bool(int)> is_valid_field;

    typedef ChartSubscriptionBase ProxyType;

    std::set<FieldType>
    get_fields() const
    { return _fields; }

private:
    std::set<FieldType> _fields;

protected:
    ChartSubscriptionBaseImpl( StreamerServiceType service,
                             const std::set<std::string>& symbols,
                             const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(service, "SUBS", symbols, fields),
            _fields(fields)
        {
        }

};


/*
 * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
 *
class ChartForexSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    ChartForexSubscriptionImpl( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : ChartSubscriptionBaseImpl( StreamerServiceType::CHART_FOREX,
                                 symbols, fields )
    {}
};
*/

class ChartFuturesSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    typedef ChartFuturesSubscription ProxyType;

    ChartFuturesSubscriptionImpl( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields )
        : ChartSubscriptionBaseImpl( StreamerServiceType::CHART_FUTURES,
                                 symbols, fields )
    {}
};


class ChartOptionsSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    typedef ChartOptionsSubscription ProxyType;

    ChartOptionsSubscriptionImpl( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields )
        : ChartSubscriptionBaseImpl( StreamerServiceType::CHART_OPTIONS,
                                 symbols, fields )
    {}
};


class TimesaleSubscriptionBaseImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = TimesaleSubscriptionField;

    typedef TimesaleSubscriptionBase ProxyType;

    static std::function<bool(int)> is_valid_field;

    std::set<FieldType>
    get_fields() const
    { return _fields; }

private:
    std::set<FieldType> _fields;

protected:
    TimesaleSubscriptionBaseImpl( StreamerServiceType service,
                               const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(service, "SUBS", symbols, fields),
            _fields(fields)
        {
        }
};


class  TimesaleEquitySubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleEquitySubscription ProxyType;

    TimesaleEquitySubscriptionImpl( const std::set<std::string>& symbols,
                                const std::set<FieldType>& fields )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_EQUITY,
                                    symbols, fields)
    {}
};

/*
 *  * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
class TimesaleForexSubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    TimesaleForexSubscriptionImpl( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_FOREX,
                                    symbols, fields)
    {}
};
*/

class TimesaleFuturesSubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleFuturesSubscription ProxyType;

    TimesaleFuturesSubscriptionImpl( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_FUTURES,
                                    symbols, fields)
    {}
};

class TimesaleOptionsSubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleOptionsSubscription ProxyType;

    TimesaleOptionsSubscriptionImpl( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_OPTIONS,
                                    symbols, fields)
    {}
};


class ActivesSubscriptionBaseImpl
        : public StreamingSubscriptionImpl {
    std::string _venue;
    DurationType _duration;

protected:
    ActivesSubscriptionBaseImpl(StreamerServiceType service,
                            std::string venue,
                            DurationType duration)
        :
            StreamingSubscriptionImpl( service, "SUBS",
                { {"keys", venue + "-" + to_string(duration)}, {"fields", "0,1"} } ),
            _venue(venue),
            _duration(duration)
        {
        }

public:
    typedef ActivesSubscriptionBase ProxyType;

    static std::function<bool(int)> is_valid_duration;

    DurationType
    get_duration() const
    { return _duration; }
};



class NasdaqActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NasdaqActivesSubscription ProxyType;

    NasdaqActivesSubscriptionImpl(DurationType duration)
        : ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_NASDAQ,
                                  "NASDAQ", duration)
    {}
};

class NYSEActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NYSEActivesSubscription ProxyType;

    NYSEActivesSubscriptionImpl(DurationType duration)
        : ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_NYSE,
                                  "NYSE", duration)
    {}
};

class OTCBBActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef OTCBBActivesSubscription ProxyType;

    OTCBBActivesSubscriptionImpl(DurationType duration)
        : ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_OTCBB,
                                  "OTCBB", duration)
    {}
};

class OptionActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
    VenueType _venue;

public:
    typedef OptionActivesSubscription ProxyType;
    static std::function<bool(int)> is_valid_venue;

    OptionActivesSubscriptionImpl(VenueType venue, DurationType duration)
        :
            ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_OPTIONS,
                                    to_string(venue), duration),
            _venue(venue)
        {
        }

    VenueType
    get_venue() const
    { return _venue; }
};

// THROWS
StreamingSubscriptionImpl
C_sub_ptr_to_impl(StreamingSubscription_C *psub);


template<typename ToTy, typename FromTy>
set<ToTy> // not safe
buffers_to_set(FromTy *buffers, size_t n)
{
    set<ToTy> s;
    while( n-- )
        s.emplace(static_cast<ToTy>(buffers[n]));
    return s;
}

template<>
inline set<string> // not safe
buffers_to_set(const char **buffers, size_t n)
{
    set<string> s;
    while( n-- ){
        const char* c = buffers[n];
        assert(c);
        s.emplace(c);
    }
    return s;
}


} /* tdma */


