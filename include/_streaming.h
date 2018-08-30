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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OPTION;

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
        if( symbols.empty() )
            throw ValueException("no symbols");

        if( fields.empty() )
            throw ValueException("no fields");

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;

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
            _symbols( util::toupper(symbols) )
        {
        }
};


class QuotesSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = QuotesSubscriptionField;

private:
    std::set<FieldType> _fields;

public:
    typedef QuotesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_QUOTES;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef OptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_OPTIONS;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneFuturesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FUTURES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FUTURES;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneForexSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FOREX;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef LevelOneFuturesOptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef NewsHeadlineSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_NEWS_HEADLINE;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_NEWS_HEADLINE;
    static std::function<bool(int)> is_valid_field;

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

private:
    std::set<FieldType> _fields;

public:
    typedef ChartEquitySubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_EQUITY;
    static std::function<bool(int)> is_valid_field;

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

public:
    typedef ChartSubscriptionBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_OPTIONS;
    static std::function<bool(int)> is_valid_field;

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


/*
 * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
 *
class ChartForexSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_FOREX;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_FUTURES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_FUTURES;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_OPTIONS;

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

public:
    typedef TimesaleSubscriptionBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;
    static std::function<bool(int)> is_valid_field;

    std::set<FieldType>
    get_fields() const
    { return _fields; }

};


class  TimesaleEquitySubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleEquitySubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_EQUITY;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_FOREX;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_FUTURES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_FUTURES;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NASDAQ;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OPTION;
    static std::function<bool(int)> is_valid_duration;

    DurationType
    get_duration() const
    { return _duration; }
};



class NasdaqActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NasdaqActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NASDAQ;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_NASDAQ;

    NasdaqActivesSubscriptionImpl(DurationType duration)
        : ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_NASDAQ,
                                  "NASDAQ", duration)
    {}
};

class NYSEActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NYSEActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NYSE;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_NYSE;

    NYSEActivesSubscriptionImpl(DurationType duration)
        : ActivesSubscriptionBaseImpl(StreamerServiceType::ACTIVES_NYSE,
                                  "NYSE", duration)
    {}
};

class OTCBBActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef OTCBBActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_OTCBB;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OTCBB;

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
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_OPTION;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OPTION;
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


