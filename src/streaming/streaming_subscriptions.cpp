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

#include "../../include/_streaming.h"

using std::string;
using std::vector;
using std::set;
using std::map;
using std::function;
using std::tie;

namespace tdma {

StreamingSubscriptionImpl::StreamingSubscriptionImpl(
        StreamerServiceType service,
        CommandType command)
    :
        _service(service),
        _command(command)
    {
    }

string
StreamingSubscriptionImpl::encode_symbol(string symbol)
{
    static std::unordered_map<char, string> R{
        {'.',"/"}, {'-',"p"}, {'+', "/WS/"}
    };
    static auto REND = R.end();

    size_t s = symbol.size();
    if(s < 2)
        return symbol;

    for(auto& s : symbol)
        s = toupper(s);

    char c2 = symbol[s-2];
    auto r = R.find(c2);
    if( r != REND ){
        return string(symbol.begin(), symbol.end()-2) + r->second + symbol[s-1];
    }else if( symbol[s-1] == '+' ){
        symbol.pop_back();
        symbol += "/WS";
    }

    return symbol;
}

class SubscriptionBySymbolBaseImpl
        : public StreamingSubscriptionImpl {
    set<string> _symbols;

    void
    check_symbols( const set<string>& symbols ) const
    {
        CommandType cmd = get_command();
        if( cmd == CommandType::UNSUBS || cmd == CommandType::VIEW )
            return;

        if( symbols.empty() ){
            std::string msg = " CommandType::" + to_string( cmd )
                + " requires at least one symbol";
            TDMA_API_THROW(ValueException, msg.c_str());
        }
    }

protected:
    template<typename F>
    void
    check_fields( const set<F>& fields) const
    {
        CommandType cmd = get_command();
        if( cmd == CommandType::UNSUBS )
            return;

        if( fields.empty() ){
            std::string msg = " CommandType::" + to_string( cmd )
                + " requires at least one field";
            TDMA_API_THROW(ValueException, msg.c_str());
        }
    }

    template<typename F>
    std::string
    build_fields_value( const set<F>& fields) const
    {
        vector<string> fields_str(fields.size());
        transform( fields.begin(), fields.end(), fields_str.begin(),
                   [](F f){
                       return std::to_string(static_cast<unsigned int>(f));
                   } );
        return util::join(fields_str, ',');
    }

    std::string
    build_symbols_value( const set<string>& symbols) const
    {
        vector<string> symbols_enc;
        for(auto& s : symbols)
            symbols_enc.emplace_back(
                StreamingSubscriptionImpl::encode_symbol(s)
            );
        return util::join(symbols_enc, ',');
    }

    template<typename F>
    map<string, string>
    build_parameters( const set<F>& fields ) const
    {
        return { {"fields", build_fields_value(fields)},
                 {"keys", build_symbols_value(_symbols)} };
    }

    virtual std::map<std::string, std::string>
    build_parameters() const
    { return {}; }


public:
    typedef SubscriptionBySymbolBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;

    set<string>
    get_symbols() const
    { return _symbols; }

    void
    set_symbols( const set<string>& symbols )
    {
        check_symbols(symbols);
        _symbols = util::toupper(symbols);
        set_parameters( build_parameters() );
    }

    bool
    operator==( const SubscriptionBySymbolBaseImpl& sub ) const
    {
        return StreamingSubscriptionImpl::operator==(sub)
            && sub._symbols == _symbols;
    }


protected:
    SubscriptionBySymbolBaseImpl( StreamerServiceType service,
                                  CommandType command,
                                  const set<string>& symbols )
        :
            StreamingSubscriptionImpl(service, command)
        {
            set_symbols( symbols );
        }
};


class QuotesSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = QuotesSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef QuotesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_QUOTES;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    QuotesSubscriptionImpl( const set<string>& symbols,
                            const set<FieldType>& fields,
                            CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::QUOTE, command,
                                         symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const QuotesSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }

};


class OptionsSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = OptionsSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef OptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_OPTIONS;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    OptionsSubscriptionImpl( const set<string>& symbols,
                             const set<FieldType>& fields,
                             CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::OPTION, command,
                                         symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const OptionsSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }
};


class LevelOneFuturesSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneFuturesSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef LevelOneFuturesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FUTURES;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FUTURES;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    LevelOneFuturesSubscriptionImpl( const set<string>& symbols,
                                     const set<FieldType>& fields,
                                     CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::LEVELONE_FUTURES,
                                         command, symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const LevelOneFuturesSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }
};


class LevelOneForexSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneForexSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef LevelOneForexSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FOREX;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    LevelOneForexSubscriptionImpl( const set<string>& symbols,
                                   const set<FieldType>& fields,
                                   CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::LEVELONE_FOREX,
                                         command, symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const LevelOneForexSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }
};


class LevelOneFuturesOptionsSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = LevelOneFuturesOptionsSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef LevelOneFuturesOptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    LevelOneFuturesOptionsSubscriptionImpl( const set<string>& symbols,
                                            const set<FieldType>& fields,
                                            CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(
                StreamerServiceType::LEVELONE_FUTURES_OPTIONS, command,
                symbols )
        {
            set_fields(fields);
        }

    bool
    operator==( const LevelOneFuturesOptionsSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }
};


class NewsHeadlineSubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = NewsHeadlineSubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef NewsHeadlineSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_NEWS_HEADLINE;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_NEWS_HEADLINE;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    NewsHeadlineSubscriptionImpl( const set<string>& symbols,
                                  const set<FieldType>& fields,
                                  CommandType command = CommandType::SUBS)
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::NEWS_HEADLINE,
                                         command, symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const NewsHeadlineSubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }

};


// TODO implement ADD command
class ChartEquitySubscriptionImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = ChartEquitySubscriptionField;

private:
    set<FieldType> _fields;

public:
    typedef ChartEquitySubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_EQUITY;
    static function<bool(int)> is_valid_field;

    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    ChartEquitySubscriptionImpl( const set<string>& symbols,
                                 const set<FieldType>& fields,
                                 CommandType command = CommandType::SUBS )
        :
            SubscriptionBySymbolBaseImpl(StreamerServiceType::CHART_EQUITY,
                                         command, symbols)
        {
            set_fields(fields);
        }

    bool
    operator==( const ChartEquitySubscriptionImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }
};


class ChartSubscriptionBaseImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = ChartSubscriptionField;
    typedef ChartSubscriptionBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_OPTIONS;
    static function<bool(int)> is_valid_field;

private:
    set<FieldType> _fields;

public:
    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    bool
    operator==( const ChartSubscriptionBaseImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }

protected:
    ChartSubscriptionBaseImpl( StreamerServiceType service,
                               CommandType command,
                               const set<string>& symbols,
                               const set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(service, command, symbols)
        {
            set_fields(fields);
        }

};


/*
 * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
 *
class ChartForexSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_FOREX;

    ChartForexSubscriptionImpl( const set<string>& symbols,
                               const set<FieldType>& fields )
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

    ChartFuturesSubscriptionImpl( const set<string>& symbols,
                                  const set<FieldType>& fields,
                                  CommandType command = CommandType::SUBS )
        : ChartSubscriptionBaseImpl( StreamerServiceType::CHART_FUTURES,
                                     command, symbols, fields  )
    {}
};


class ChartOptionsSubscriptionImpl
        : public ChartSubscriptionBaseImpl {
public:
    typedef ChartOptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_CHART_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_CHART_OPTIONS;

    ChartOptionsSubscriptionImpl( const set<string>& symbols,
                                  const set<FieldType>& fields,
                                  CommandType command = CommandType::SUBS )
        : ChartSubscriptionBaseImpl( StreamerServiceType::CHART_OPTIONS,
                                     command, symbols, fields )
    {}
};


class TimesaleSubscriptionBaseImpl
        : public SubscriptionBySymbolBaseImpl {
public:
    using FieldType = TimesaleSubscriptionField;
    typedef TimesaleSubscriptionBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;
    static function<bool(int)> is_valid_field;

private:
    set<FieldType> _fields;

public:
    set<FieldType>
    get_fields() const
    { return _fields; }

    void
    set_fields(const set<FieldType>& fields)
    {
        check_fields(fields);
        _fields = fields;
        set_parameters( build_parameters() );
    }

    std::map<std::string, std::string>
    get_parameters() const
    {
        return get_parameters();
    }

    std::map<std::string, std::string>
    build_parameters() const
    {
        return SubscriptionBySymbolBaseImpl::build_parameters(_fields);
    }

    bool
    operator==( const TimesaleSubscriptionBaseImpl& sub ) const
    {
        return SubscriptionBySymbolBaseImpl::operator==(sub)
            && sub._fields == _fields;
    }

protected:
    TimesaleSubscriptionBaseImpl( StreamerServiceType service,
                                  CommandType command,
                                  const set<string>& symbols,
                                  const set<FieldType>& fields )
        :
            SubscriptionBySymbolBaseImpl(service, command, symbols)
        {
            set_fields(fields);
        }
};


class  TimesaleEquitySubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleEquitySubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_EQUITY;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_EQUITY;

    TimesaleEquitySubscriptionImpl( const set<string>& symbols,
                                    const set<FieldType>& fields,
                                    CommandType command = CommandType::SUBS )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_EQUITY,
                                        command, symbols, fields)
    {}
};

/*
 *  * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
class TimesaleForexSubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_FOREX;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_FOREX;

    TimesaleForexSubscriptionImpl( const set<string>& symbols,
                               const set<FieldType>& fields )
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

    TimesaleFuturesSubscriptionImpl( const set<string>& symbols,
                                     const set<FieldType>& fields,
                                     CommandType command = CommandType::SUBS )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_FUTURES,
                                        command, symbols, fields )
    {}
};

class TimesaleOptionsSubscriptionImpl
        : public TimesaleSubscriptionBaseImpl {
public:
    typedef TimesaleOptionsSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_TIMESALE_OPTIONS;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_TIMESALE_OPTIONS;

    TimesaleOptionsSubscriptionImpl( const set<string>& symbols,
                                     const set<FieldType>& fields,
                                     CommandType command = CommandType::SUBS )
        : TimesaleSubscriptionBaseImpl( StreamerServiceType::TIMESALE_OPTIONS,
                                        command, symbols, fields)
    {}
};


class ActivesSubscriptionBaseImpl
        : public StreamingSubscriptionImpl {
    string _venue;
    DurationType _duration;

protected:
    std::map<std::string, std::string>
    build_parameters() const
    {
        return { {"keys", _venue + "-" + to_string(_duration)},
                 {"fields", "0,1"} };
    }

    void
    set_venue_string( const std::string& v )
    { _venue = v; }

    ActivesSubscriptionBaseImpl( StreamerServiceType service,
                                 CommandType command,
                                 string venue,
                                 DurationType duration )
        :
            StreamingSubscriptionImpl( service, command ),
            _venue(venue),
            _duration(duration)
        {
            set_parameters( build_parameters() );
        }

public:
    typedef ActivesSubscriptionBase ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NASDAQ;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OPTION;
    static function<bool(int)> is_valid_duration;

    DurationType
    get_duration() const
    { return _duration; }

    void
    set_duration( DurationType duration )
    {
        _duration = duration;
        set_parameters( build_parameters() );
    }

    bool
    operator==( const ActivesSubscriptionBaseImpl& sub ) const
    {
        return StreamingSubscriptionImpl ::operator==(sub)
            && sub._venue == _venue && sub._duration == _duration;
    }

};



class NasdaqActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NasdaqActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NASDAQ;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_NASDAQ;

    NasdaqActivesSubscriptionImpl( DurationType duration,
                                   CommandType command = CommandType::SUBS )
        : ActivesSubscriptionBaseImpl( StreamerServiceType::ACTIVES_NASDAQ,
                                       command, "NASDAQ", duration )
    {}
};

class NYSEActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef NYSEActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_NYSE;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_NYSE;

    NYSEActivesSubscriptionImpl( DurationType duration,
                                 CommandType command = CommandType::SUBS )
        : ActivesSubscriptionBaseImpl( StreamerServiceType::ACTIVES_NYSE,
                                       command, "NYSE", duration )
    {}
};

class OTCBBActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
public:
    typedef OTCBBActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_OTCBB;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OTCBB;

    OTCBBActivesSubscriptionImpl( DurationType duration,
                                  CommandType command = CommandType::SUBS )
        : ActivesSubscriptionBaseImpl( StreamerServiceType::ACTIVES_OTCBB,
                                       command, "OTCBB", duration )
    {}
};

class OptionActivesSubscriptionImpl
        : public ActivesSubscriptionBaseImpl {
    VenueType _venue;

public:
    typedef OptionActivesSubscription ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_SUB_ACTIVES_OPTION;
    static const int TYPE_ID_HIGH = TYPE_ID_SUB_ACTIVES_OPTION;
    static function<bool(int)> is_valid_venue;

    OptionActivesSubscriptionImpl( VenueType venue,
                                   DurationType duration,
                                   CommandType command = CommandType::SUBS )
        :
            ActivesSubscriptionBaseImpl( StreamerServiceType::ACTIVES_OPTIONS,
                                         command, to_string(venue), duration),
            _venue(venue)
        {
        }

    VenueType
    get_venue() const
    { return _venue; }

    void
    set_venue( VenueType venue )
    {
        _venue = venue;
        set_venue_string( to_string(venue) );
        set_parameters( build_parameters() );
    }

    bool
    operator==( const OptionActivesSubscriptionImpl& sub ) const
    {
        return ActivesSubscriptionBaseImpl::operator==(sub)
            && sub._venue == _venue;
    }

};


function<bool(int)> QuotesSubscriptionImpl::is_valid_field =
    QuotesSubscriptionField_is_valid;

function<bool(int)> OptionsSubscriptionImpl::is_valid_field =
    OptionsSubscriptionField_is_valid;

function<bool(int)> LevelOneFuturesSubscriptionImpl::is_valid_field =
    LevelOneFuturesSubscriptionField_is_valid;

function<bool(int)> LevelOneForexSubscriptionImpl::is_valid_field =
    LevelOneForexSubscriptionField_is_valid;

function<bool(int)> LevelOneFuturesOptionsSubscriptionImpl::is_valid_field =
    LevelOneFuturesOptionsSubscriptionField_is_valid;

function<bool(int)> NewsHeadlineSubscriptionImpl::is_valid_field =
    NewsHeadlineSubscriptionField_is_valid;

function<bool(int)> ChartEquitySubscriptionImpl::is_valid_field =
    ChartEquitySubscriptionField_is_valid;

function<bool(int)> ChartSubscriptionBaseImpl::is_valid_field =
    ChartSubscriptionField_is_valid;

function<bool(int)> TimesaleSubscriptionBaseImpl::is_valid_field =
    TimesaleSubscriptionField_is_valid;

function<bool(int)> ActivesSubscriptionBaseImpl::is_valid_duration =
    DurationType_is_valid;

function<bool(int)> OptionActivesSubscriptionImpl::is_valid_venue =
    VenueType_is_valid;

StreamingSubscriptionImpl*
C_sub_ptr_to_impl_ptr(StreamingSubscription_C *psub)
{
    base_proxy_is_callable(psub, 1);
    switch( psub->type_id ){
    case TYPE_ID_SUB_QUOTES:
        return reinterpret_cast<QuotesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_OPTIONS:
        return reinterpret_cast<OptionsSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES:
        return reinterpret_cast<LevelOneFuturesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_LEVEL_ONE_FOREX:
        return reinterpret_cast<LevelOneForexSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS:
        return reinterpret_cast<LevelOneFuturesOptionsSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_NEWS_HEADLINE:
        return reinterpret_cast<NewsHeadlineSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_CHART_EQUITY:
        return reinterpret_cast<ChartEquitySubscriptionImpl*>(psub->obj);
    //case TYPE_ID_SUB_CHART_FOREX: NOT WORKING
    case TYPE_ID_SUB_CHART_FUTURES:
        return reinterpret_cast<ChartFuturesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_CHART_OPTIONS:
        return reinterpret_cast<ChartOptionsSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_TIMESALE_EQUITY:
        return reinterpret_cast<TimesaleEquitySubscriptionImpl*>(psub->obj);
    //case TYPE_ID_SUB_TIMESALE_FOREX: NOT WORKING
    case TYPE_ID_SUB_TIMESALE_FUTURES:
        return reinterpret_cast<TimesaleFuturesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_TIMESALE_OPTIONS:
        return reinterpret_cast<TimesaleOptionsSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_ACTIVES_NASDAQ:
        return reinterpret_cast<NasdaqActivesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_ACTIVES_NYSE:
        return reinterpret_cast<NYSEActivesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_ACTIVES_OTCBB:
        return reinterpret_cast<OTCBBActivesSubscriptionImpl*>(psub->obj);
    case TYPE_ID_SUB_ACTIVES_OPTION:
        return reinterpret_cast<OptionActivesSubscriptionImpl*>(psub->obj);
    default:
        TDMA_API_THROW(TypeException,"invalid C subscription type_id");
    }
}

// NOTE we're upcasting by-value,
StreamingSubscriptionImpl
C_sub_ptr_to_impl(StreamingSubscription_C *psub)
{  return *C_sub_ptr_to_impl_ptr(psub); }


} /* tdma */


using namespace tdma;

namespace {

template<typename ImplTy>
int
subscription_is_creatable( typename ImplTy::ProxyType::CType *sub,
                           int allow_exceptions )
{
    static_assert( ImplTy::TYPE_ID_LOW > 0 && ImplTy::TYPE_ID_HIGH > 0,
                   "invalid subscription type" );

    CHECK_PTR(sub, "subscription", allow_exceptions);
    return 0;
}

template<typename ImplTy>
int
create_symbol_field_subscription( const char **symbols,
                                  size_t nsymbols,
                                  int *fields,
                                  size_t nfields,
                                  int command,
                                  typename ImplTy::ProxyType::CType *psub,
                                  int allow_exceptions )
{
    int err = subscription_is_creatable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    assert( (symbols != nullptr) || (nsymbols == 0) );
    assert( (fields != nullptr) || (nfields == 0) );

    CHECK_ENUM_KILL_PROXY(CommandType, command, allow_exceptions, psub);


    if( nsymbols > SUBSCRIPTION_MAX_SYMBOLS ){
        return HANDLE_ERROR_EX( ValueException,
                                "nsymbols > SUBSCRIPTION_MAX_SYMBOLS",
                                allow_exceptions, psub );
    }

    if( nfields > SUBSCRIPTION_MAX_FIELDS ){
        return HANDLE_ERROR_EX( ValueException,
                                "nfields > SUBSCRIPTION_MAX_FIELDS",
                                allow_exceptions, psub );
    }
    
    auto s_symbols = util::buffers_to_set<string>(symbols, nsymbols);

    //TODO combine with buffers_to_set
    for(size_t i = 0; i < nfields; ++i){
        if( !ImplTy::is_valid_field(fields[i]) ){
            return HANDLE_ERROR_EX( ValueException, "invalid FieldType value",
                                    allow_exceptions, psub );
        }
    }

    auto s_fields = util::buffers_to_set<typename ImplTy::FieldType>(
        fields, nfields
        );

    static auto meth = +[]( const set<string>& symbols,
                            const set<typename ImplTy::FieldType>& fields,
                            int cmd){
        return new ImplTy( symbols, fields, static_cast<CommandType>(cmd) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, s_symbols,
                                     s_fields, command);
    if( err ){
        kill_proxy(psub);
        return err;
    }

    psub->obj = reinterpret_cast<void*>(obj);
    assert(ImplTy::TYPE_ID_LOW == ImplTy::TYPE_ID_HIGH);
    psub->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}


template<typename ImplTy>
int
create_duration_subscription( int duration,
                              int command,
                              typename ImplTy::ProxyType::CType *psub,
                              int allow_exceptions )
{
    int err = subscription_is_creatable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY(CommandType, command, allow_exceptions, psub);

    if( !ImplTy::is_valid_duration(duration) ){
        return HANDLE_ERROR_EX( ValueException, "invalid DurationType value",
                                allow_exceptions, psub );
    }

    static auto meth = +[]( int d, int cmd ){
        return new ImplTy( static_cast<tdma::DurationType>(d),
                           static_cast<CommandType>(cmd) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, duration, command);
    if( err ){
        kill_proxy(psub);
        return err;
    }

    psub->obj = reinterpret_cast<void*>(obj);
    assert(ImplTy::TYPE_ID_LOW == ImplTy::TYPE_ID_HIGH);
    psub->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}


template<typename ImplTy>
int
get_fields( typename ImplTy::ProxyType::CType *psub,
            int** fields,
            size_t *n,
            int allow_exceptions )
{
    int err = proxy_is_callable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[]( void *obj ){
        return reinterpret_cast<ImplTy*>(obj)->get_fields();
    };

    set<typename ImplTy::FieldType> f;
    tie(f, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    if( err )
        return err;

    *n = f.size();
    if( *n == 0 ){
        *fields = nullptr;
        return 0;
    }

    err = tdma::alloc_to_buffer(fields, *n, allow_exceptions);
    if( err )
        return err;

    int i = 0;
    for(auto& ff : f)
        (*fields)[i++] = static_cast<int>(ff);

    return 0;
}

template<typename ImplTy>
int
set_fields( typename ImplTy::ProxyType::CType *psub,
            int* fields,
            size_t n,
            int allow_exceptions )
{
    int err = proxy_is_callable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    assert( (fields != nullptr) || (n == 0) );

    if( n > SUBSCRIPTION_MAX_FIELDS ){
        return HANDLE_ERROR_EX( ValueException,
                                "n > SUBSCRIPTION_MAX_FIELDS",
                                allow_exceptions, psub );
    }

    for(size_t i = 0; i < n; ++i){
        if( !ImplTy::is_valid_field(fields[i]) ){
            return HANDLE_ERROR_EX( ValueException, "invalid FieldType value",
                                    allow_exceptions, psub );
        }
    }

    auto f = util::buffers_to_set<typename ImplTy::FieldType>(fields, n);

    static auto meth = +[]( void *obj, const set<typename ImplTy::FieldType>& f ){
        reinterpret_cast<ImplTy*>(obj)->set_fields(f);
    };

   return CallImplFromABI( allow_exceptions, meth, psub->obj, f);
}


template<typename ImplTy>
std::pair<void*, int>
copy_construct_impl( ImplTy *from,
                    int allow_exceptions,
                 typename std::enable_if<
                    std::is_base_of<SubscriptionBySymbolBaseImpl,
                                    ImplTy>::value>::type *_ = nullptr)
{
    static auto meth = +[]( const set<string>& symbols,
                            const set<typename ImplTy::FieldType>& fields,
                            CommandType cmd){
        return reinterpret_cast<void*>( new ImplTy(symbols, fields, cmd) );
    };

    return CallImplFromABI( allow_exceptions, meth, from->get_symbols(),
                            from->get_fields(), from->get_command() );
}

template<typename ImplTy> //excludes OptionActivesSubscriptionImpl
std::pair<void*, int>
copy_construct_impl( ImplTy *from,
                     int allow_exceptions,
                 typename std::enable_if<
                    std::is_base_of<ActivesSubscriptionBaseImpl, ImplTy>::value
                    && !std::is_same<ImplTy, OptionActivesSubscriptionImpl>::value
                        >::type *_ = nullptr)
{
    static auto meth = +[]( DurationType d, CommandType cmd){
        return reinterpret_cast<void*>( new ImplTy( d, cmd ) );
    };

    return CallImplFromABI( allow_exceptions, meth, from->get_duration(),
                            from->get_command() );
}

template<typename ImplTy>
std::pair<void*, int>
copy_construct_impl( OptionActivesSubscriptionImpl *from, int allow_exceptions )
{
    static auto meth = +[]( VenueType v, DurationType d, CommandType cmd ){
        return reinterpret_cast<void*>(
            new OptionActivesSubscriptionImpl( v, d, cmd)
        );
    };

    return CallImplFromABI( allow_exceptions, meth, from->get_venue(),
                            from->get_duration(), from->get_command() );
}


template<typename ImplTy>
int
copy_construct( typename ImplTy::ProxyType::CType *from,
                typename ImplTy::ProxyType::CType *to,
                int allow_exceptions )
{
    CHECK_PTR(to, "to subscription", allow_exceptions);

    // bad 'from' fails silently, so allow client to check for non null ->obj
    kill_proxy(to);

    if( proxy_is_callable<ImplTy>(from, 0) == 0 ){
        ImplTy* f = reinterpret_cast<ImplTy*>(from->obj);

        int err;
        std::tie(to->obj, err) = copy_construct_impl<ImplTy>(f, allow_exceptions);
        if( err ){
            kill_proxy(to);
            return err;
        }

        to->type_id = ImplTy::TYPE_ID_LOW;
    }

    return 0;
}


#define CAST_TO_COPY_CONSTRUCT(name,from,to, exc) \
copy_construct<name##Impl>( reinterpret_cast<name##_C*>(from), \
                            reinterpret_cast<name##_C*>(to), exc )

int
copy_construct_generic( StreamingSubscription_C *from,
                        StreamingSubscription_C *to,
                        int allow_exceptions )
{
    switch( from->type_id ){
    case TYPE_ID_SUB_QUOTES:
        return CAST_TO_COPY_CONSTRUCT(QuotesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_OPTIONS:
        return CAST_TO_COPY_CONSTRUCT(OptionsSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES:
        return CAST_TO_COPY_CONSTRUCT(LevelOneFuturesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FOREX:
        return CAST_TO_COPY_CONSTRUCT(LevelOneForexSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS:
        return CAST_TO_COPY_CONSTRUCT(LevelOneFuturesOptionsSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_NEWS_HEADLINE:
        return CAST_TO_COPY_CONSTRUCT(NewsHeadlineSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_CHART_EQUITY:
        return CAST_TO_COPY_CONSTRUCT(ChartEquitySubscription, from, to, allow_exceptions);
    //case TYPE_ID_SUB_CHART_FOREX: NOT WORKING
    case TYPE_ID_SUB_CHART_FUTURES:
        return CAST_TO_COPY_CONSTRUCT(ChartFuturesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_CHART_OPTIONS:
        return CAST_TO_COPY_CONSTRUCT(ChartOptionsSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_TIMESALE_EQUITY:
        return CAST_TO_COPY_CONSTRUCT(TimesaleEquitySubscription, from, to, allow_exceptions);
    //case TYPE_ID_SUB_TIMESALE_FOREX: NOT WORKING
    case TYPE_ID_SUB_TIMESALE_FUTURES:
        return CAST_TO_COPY_CONSTRUCT(TimesaleFuturesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_TIMESALE_OPTIONS:
        return CAST_TO_COPY_CONSTRUCT(TimesaleOptionsSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_NASDAQ:
        return CAST_TO_COPY_CONSTRUCT(NasdaqActivesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_NYSE:
        return CAST_TO_COPY_CONSTRUCT(NYSEActivesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_OTCBB:
        return CAST_TO_COPY_CONSTRUCT(OTCBBActivesSubscription, from, to, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_OPTION:
        return CAST_TO_COPY_CONSTRUCT(OptionActivesSubscription, from, to, allow_exceptions);
    default:
        return HANDLE_ERROR_EX( TypeException, "invalid C subscription type_id",
                                allow_exceptions, to);
    }
}


template<typename ImplTy>
int
is_same_impl( StreamingSubscription_C *l,
              StreamingSubscription_C *r,
              int *is_same,
              int allow_exceptions )
{
    assert(l);
    assert(r);
    assert(l->type_id == r->type_id);

    static auto meth = +[](void *l, void *r){
        return static_cast<int>(
            *reinterpret_cast<ImplTy*>(l) == *reinterpret_cast<ImplTy*>(r)
            );
    };

    int err;
    tie(*is_same, err) = CallImplFromABI( allow_exceptions, meth, l->obj,
                                               r->obj);
    return err;
}

int
is_same_impl_generic( StreamingSubscription_C *l,
                      StreamingSubscription_C *r,
                      int *is_same,
                      int allow_exceptions )
{
    assert(l);
    assert(r);
    assert(l->type_id == r->type_id);
    // allows us to kill both on failure

    switch( l->type_id ){
    case TYPE_ID_SUB_QUOTES:
        return is_same_impl<QuotesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_OPTIONS:
        return is_same_impl<OptionsSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES:
        return is_same_impl<LevelOneFuturesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FOREX:
        return is_same_impl<LevelOneForexSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS:
        return is_same_impl<LevelOneFuturesOptionsSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_NEWS_HEADLINE:
        return is_same_impl<NewsHeadlineSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_CHART_EQUITY:
        return is_same_impl<ChartEquitySubscriptionImpl>(l, r, is_same, allow_exceptions);
    //case TYPE_ID_SUB_CHART_FOREX: NOT WORKING
    case TYPE_ID_SUB_CHART_FUTURES:
        return is_same_impl<ChartFuturesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_CHART_OPTIONS:
        return is_same_impl<ChartOptionsSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_TIMESALE_EQUITY:
        return is_same_impl<TimesaleEquitySubscriptionImpl>(l, r, is_same, allow_exceptions);
    //case TYPE_ID_SUB_TIMESALE_FOREX: NOT WORKING
    case TYPE_ID_SUB_TIMESALE_FUTURES:
        return is_same_impl<TimesaleFuturesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_TIMESALE_OPTIONS:
        return is_same_impl<TimesaleOptionsSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_NASDAQ:
        return is_same_impl<NasdaqActivesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_NYSE:
        return is_same_impl<NYSEActivesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_OTCBB:
        return is_same_impl<OTCBBActivesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    case TYPE_ID_SUB_ACTIVES_OPTION:
        return is_same_impl<OptionActivesSubscriptionImpl>(l, r, is_same, allow_exceptions);
    default:
        kill_proxy(l);
        kill_proxy(r);
        return HANDLE_ERROR( TypeException, "invalid C subscription type_id",
                             allow_exceptions);
    }
}

} /* namespace */


/* Destroy for each subscription type */
#define DEFINE_CSUB_DESTROY_FUNC(name) \
int \
name##_Destroy_ABI(name##_C *psub, int allow_exceptions) \
{ return destroy_proxy<name##Impl>(psub, allow_exceptions); }

DEFINE_CSUB_DESTROY_FUNC(QuotesSubscription);
DEFINE_CSUB_DESTROY_FUNC(OptionsSubscription);
DEFINE_CSUB_DESTROY_FUNC(LevelOneFuturesSubscription);
DEFINE_CSUB_DESTROY_FUNC(LevelOneForexSubscription);
DEFINE_CSUB_DESTROY_FUNC(LevelOneFuturesOptionsSubscription);
DEFINE_CSUB_DESTROY_FUNC(NewsHeadlineSubscription);
DEFINE_CSUB_DESTROY_FUNC(ChartEquitySubscription);
DEFINE_CSUB_DESTROY_FUNC(ChartFuturesSubscription);
DEFINE_CSUB_DESTROY_FUNC(ChartOptionsSubscription);
DEFINE_CSUB_DESTROY_FUNC(TimesaleFuturesSubscription);
DEFINE_CSUB_DESTROY_FUNC(TimesaleEquitySubscription);
DEFINE_CSUB_DESTROY_FUNC(TimesaleOptionsSubscription);
DEFINE_CSUB_DESTROY_FUNC(NasdaqActivesSubscription);
DEFINE_CSUB_DESTROY_FUNC(NYSEActivesSubscription);
DEFINE_CSUB_DESTROY_FUNC(OTCBBActivesSubscription);
DEFINE_CSUB_DESTROY_FUNC(OptionActivesSubscription);
#undef DEFINE_CSUB_DESTROY_FUNC

/* Generic Destroy */
int
StreamingSubscription_Destroy_ABI( StreamingSubscription_C *psub,
                                   int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    static auto meth = +[](StreamingSubscription_C * obj){
        // this can throw, so needs to pass through CallImplFromABI
        delete C_sub_ptr_to_impl_ptr(obj);
    };

    err = CallImplFromABI(allow_exceptions, meth, psub);
    kill_proxy(psub);
    return err;
}

#define DEFINE_CSUB_COPY_FUNC(name) \
int \
name##_Copy_ABI(name##_C *from, name##_C *to, int allow_exceptions) \
{ return copy_construct<name##Impl>(from, to, allow_exceptions); }

DEFINE_CSUB_COPY_FUNC(QuotesSubscription);
DEFINE_CSUB_COPY_FUNC(OptionsSubscription);
DEFINE_CSUB_COPY_FUNC(LevelOneFuturesSubscription);
DEFINE_CSUB_COPY_FUNC(LevelOneForexSubscription);
DEFINE_CSUB_COPY_FUNC(LevelOneFuturesOptionsSubscription);
DEFINE_CSUB_COPY_FUNC(NewsHeadlineSubscription);
DEFINE_CSUB_COPY_FUNC(ChartEquitySubscription);
DEFINE_CSUB_COPY_FUNC(ChartFuturesSubscription);
DEFINE_CSUB_COPY_FUNC(ChartOptionsSubscription);
DEFINE_CSUB_COPY_FUNC(TimesaleFuturesSubscription);
DEFINE_CSUB_COPY_FUNC(TimesaleEquitySubscription);
DEFINE_CSUB_COPY_FUNC(TimesaleOptionsSubscription);
DEFINE_CSUB_COPY_FUNC(NasdaqActivesSubscription);
DEFINE_CSUB_COPY_FUNC(NYSEActivesSubscription);
DEFINE_CSUB_COPY_FUNC(OTCBBActivesSubscription);
DEFINE_CSUB_COPY_FUNC(OptionActivesSubscription);
#undef DEFINE_CSUB_COPY_FUNC


int
StreamingSubscription_Copy_ABI( StreamingSubscription_C *from,
                                StreamingSubscription_C * to,
                                int allow_exceptions )
{
    return copy_construct_generic(from, to, allow_exceptions);
}


int
StreamingSubscription_IsSame_ABI( StreamingSubscription_C *l,
                                  StreamingSubscription_C *r,
                                  int *is_same,
                                  int allow_exceptions )
{
    int err = base_proxy_is_callable(l, allow_exceptions);
    if( err )
        return err;

    err = base_proxy_is_callable(r, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(is_same, "is_same", allow_exceptions);
    if( l->type_id != r->type_id ){
        *is_same = 0;
        return 0;
    }

    return is_same_impl_generic(l, r, is_same, allow_exceptions);
}


int
StreamingSubscription_GetService_ABI( StreamingSubscription_C *psub,
                                      int *service,
                                      int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(service, "service", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<StreamingSubscriptionImpl*>(obj)->get_service()
            );
    };

    tie(*service, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}

int
StreamingSubscription_GetCommand_ABI( StreamingSubscription_C *psub,
                                      int *command,
                                      int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(command, "command", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<StreamingSubscriptionImpl*>(obj)->get_command()
            );
    };

    tie(*command, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}

int
SubscriptionBySymbolBase_GetSymbols_ABI( StreamingSubscription_C *psub,
                                         char ***buffers,
                                         size_t *n,
                                         int allow_exceptions )
{
    int err = proxy_is_callable<SubscriptionBySymbolBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(buffers, "buffers", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return reinterpret_cast<SubscriptionBySymbolBaseImpl*>(obj)
            ->get_symbols();
    };

    set<string> strs;
    tie(strs,err) = CallImplFromABI(allow_exceptions, meth, psub->obj);
    if( err )
       return err;

    return to_new_char_buffers(strs, buffers, n, allow_exceptions);
}


#define DEFINE_CSUB_GET_FIELDS_FUNC(name) \
int \
name##_GetFields_ABI(name##_C *psub, int **fields, size_t *n, int allow_exceptions) \
{ return get_fields<name##Impl>(psub, fields, n, allow_exceptions); }

#define DEFINE_CSUB_GET_FIELDS_BASE_FUNC(name) \
int \
name##_GetFields_ABI(StreamingSubscription_C *psub, int **fields, size_t *n, int \
                      allow_exceptions) \
{ return get_fields<name##Impl>(psub, fields, n, \
                                allow_exceptions); }

DEFINE_CSUB_GET_FIELDS_FUNC(QuotesSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(OptionsSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(LevelOneFuturesSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(LevelOneForexSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(NewsHeadlineSubscription)
DEFINE_CSUB_GET_FIELDS_FUNC(ChartEquitySubscription)

DEFINE_CSUB_GET_FIELDS_BASE_FUNC(ChartSubscriptionBase)
DEFINE_CSUB_GET_FIELDS_BASE_FUNC(TimesaleSubscriptionBase)

int
ActivesSubscriptionBase_GetDuration_ABI( StreamingSubscription_C *psub,
                                         int *duration,
                                         int allow_exceptions )
{
    int err = proxy_is_callable<ActivesSubscriptionBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(duration, "duration", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<ActivesSubscriptionBaseImpl*>(obj)
                ->get_duration()
            );
    };

    tie(*duration, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}

int
OptionActivesSubscription_GetVenue_ABI( OptionActivesSubscription_C *psub,
                                        int *venue,
                                        int allow_exceptions )
{
    int err = proxy_is_callable<OptionActivesSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(venue, "venue", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<OptionActivesSubscriptionImpl*>(obj)
                ->get_venue()
            );
    };

    tie(*venue, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}


int
StreamingSubscription_SetCommand_ABI( StreamingSubscription_C *psub,
                                      int command,
                                      int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM(CommandType, command, allow_exceptions);

    static auto meth = +[]( void *obj, int c){
        reinterpret_cast<StreamingSubscriptionImpl*>(obj)
            ->set_command( static_cast<CommandType>(c) );
    };

    return CallImplFromABI( allow_exceptions, meth, psub->obj, command);
}

// TODO check symbols < MAX
int
SubscriptionBySymbolBase_SetSymbols_ABI( StreamingSubscription_C *psub,
                                         const char **buffers,
                                         size_t n,
                                         int allow_exceptions )
{
    int err = proxy_is_callable<SubscriptionBySymbolBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    assert( (buffers != nullptr) || (n == 0) );

    auto symbols = util::buffers_to_set<string>(buffers, n);

    static auto meth = +[]( void *obj, set<string>& s ){
        reinterpret_cast<SubscriptionBySymbolBaseImpl*>(obj)->set_symbols(s);
    };

    return CallImplFromABI(allow_exceptions, meth, psub->obj, symbols);
}


#define DEFINE_CSUB_SET_FIELDS_FUNC(name) \
int \
name##_SetFields_ABI(name##_C *psub, int *fields, size_t n, \
                     int allow_exceptions) \
{ return set_fields<name##Impl>(psub, fields, n, allow_exceptions); }

#define DEFINE_CSUB_SET_FIELDS_BASE_FUNC(name) \
int \
name##_SetFields_ABI(StreamingSubscription_C *psub, int *fields, size_t n, int \
                      allow_exceptions) \
{ return set_fields<name##Impl>(psub, fields, n, \
                                allow_exceptions); }

DEFINE_CSUB_SET_FIELDS_FUNC(QuotesSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(OptionsSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(LevelOneFuturesSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(LevelOneForexSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(LevelOneFuturesOptionsSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(NewsHeadlineSubscription)
DEFINE_CSUB_SET_FIELDS_FUNC(ChartEquitySubscription)

DEFINE_CSUB_SET_FIELDS_BASE_FUNC(ChartSubscriptionBase)
DEFINE_CSUB_SET_FIELDS_BASE_FUNC(TimesaleSubscriptionBase)

int
ActivesSubscriptionBase_SetDuration_ABI( StreamingSubscription_C *psub,
                                         int duration,
                                         int allow_exceptions )
{
    int err = proxy_is_callable<ActivesSubscriptionBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM(DurationType, duration, allow_exceptions);

    static auto meth = +[]( void *obj, int d ){
        reinterpret_cast<ActivesSubscriptionBaseImpl*>(obj)
            ->set_duration( static_cast<DurationType>(d) );
    };

    return CallImplFromABI( allow_exceptions, meth, psub->obj, duration);
}

int
OptionActivesSubscription_SetVenue_ABI( OptionActivesSubscription_C *psub,
                                        int venue,
                                        int allow_exceptions )
{
    int err = proxy_is_callable<OptionActivesSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM(VenueType, venue, allow_exceptions);

    static auto meth = +[]( void *obj, int v ){
        reinterpret_cast<OptionActivesSubscriptionImpl*>(obj)
            ->set_venue( static_cast<VenueType>(v) );
    };

    return CallImplFromABI( allow_exceptions, meth, psub->obj, venue);
}

int
QuotesSubscription_Create_ABI( const char **symbols,
                               size_t nsymbols,
                               int *fields,
                               size_t nfields,
                               int command,
                               QuotesSubscription_C *psub,
                               int allow_exceptions )
{
    return create_symbol_field_subscription<QuotesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
OptionsSubscription_Create_ABI( const char **symbols,
                                size_t nsymbols,
                                int *fields,
                                size_t nfields,
                                int command,
                                OptionsSubscription_C *psub,
                                int allow_exceptions )
{
    return create_symbol_field_subscription<OptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}

int
LevelOneFuturesSubscription_Create_ABI(
    const char **symbols,
    size_t nsymbols,
    int *fields,
    size_t nfields,
    int command,
    LevelOneFuturesSubscription_C *psub,
    int allow_exceptions
    )
{
    return create_symbol_field_subscription<LevelOneFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}



int
LevelOneForexSubscription_Create_ABI( const char **symbols,
                                      size_t nsymbols,
                                      int *fields,
                                      size_t nfields,
                                      int command,
                                      LevelOneForexSubscription_C *psub,
                                      int allow_exceptions )
{
    return create_symbol_field_subscription<LevelOneForexSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
LevelOneFuturesOptionsSubscription_Create_ABI(
    const char **symbols,
    size_t nsymbols,
    int *fields,
    size_t nfields,
    int command,
    LevelOneFuturesOptionsSubscription_C *psub,
    int allow_exceptions )
{
    return create_symbol_field_subscription
        <LevelOneFuturesOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
NewsHeadlineSubscription_Create_ABI( const char **symbols,
                                     size_t nsymbols,
                                     int *fields,
                                     size_t nfields,
                                     int command,
                                     NewsHeadlineSubscription_C *psub,
                                     int allow_exceptions )
{
    return create_symbol_field_subscription<NewsHeadlineSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}

int
ChartEquitySubscription_Create_ABI( const char **symbols,
                                    size_t nsymbols,
                                    int *fields,
                                    size_t nfields,
                                    int command,
                                    ChartEquitySubscription_C *psub,
                                    int allow_exceptions )
{
    return create_symbol_field_subscription<ChartEquitySubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}



int
ChartFuturesSubscription_Create_ABI( const char **symbols,
                                     size_t nsymbols,
                                     int *fields,
                                     size_t nfields,
                                     int command,
                                     ChartFuturesSubscription_C *psub,
                                     int allow_exceptions )
{
    return create_symbol_field_subscription<ChartFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
ChartOptionsSubscription_Create_ABI( const char **symbols,
                                     size_t nsymbols,
                                     int *fields,
                                     size_t nfields,
                                     int command,
                                     ChartOptionsSubscription_C *psub,
                                     int allow_exceptions )
{
    return create_symbol_field_subscription<ChartOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}

int
TimesaleEquitySubscription_Create_ABI( const char **symbols,
                                       size_t nsymbols,
                                       int *fields,
                                       size_t nfields,
                                       int command,
                                       TimesaleEquitySubscription_C *psub,
                                       int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleEquitySubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
TimesaleFuturesSubscription_Create_ABI( const char **symbols,
                                        size_t nsymbols,
                                        int *fields,
                                        size_t nfields,
                                        int command,
                                        TimesaleFuturesSubscription_C *psub,
                                        int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
TimesaleOptionsSubscription_Create_ABI( const char **symbols,
                                        size_t nsymbols,
                                        int *fields,
                                        size_t nfields,
                                        int command,
                                        TimesaleOptionsSubscription_C *psub,
                                        int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, command, psub, allow_exceptions
    );
}


int
NasdaqActivesSubscription_Create_ABI( int duration_type,
                                      int command,
                                      NasdaqActivesSubscription_C *psub,
                                      int allow_exceptions )

{
    return create_duration_subscription<NasdaqActivesSubscriptionImpl>(
        duration_type, command, psub, allow_exceptions
    );
}


int
NYSEActivesSubscription_Create_ABI( int duration_type,
                                    int command,
                                    NYSEActivesSubscription_C *psub,
                                    int allow_exceptions )
{
    return create_duration_subscription<NYSEActivesSubscriptionImpl>(
        duration_type, command, psub, allow_exceptions
    );
}


int
OTCBBActivesSubscription_Create_ABI( int duration_type,
                                     int command,
                                     OTCBBActivesSubscription_C *psub,
                                     int allow_exceptions )
{
    return create_duration_subscription<OTCBBActivesSubscriptionImpl>(
        duration_type, command, psub, allow_exceptions
    );
}

int
OptionActivesSubscription_Create_ABI( int venue,
                                      int duration_type,
                                      int command,
                                      OptionActivesSubscription_C *psub,
                                      int allow_exceptions )
{
    int err = subscription_is_creatable<OptionActivesSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY(CommandType, command, allow_exceptions, psub);

    if( !OptionActivesSubscriptionImpl::is_valid_venue(venue) ){
        return HANDLE_ERROR_EX( ValueException, "invalid VenueType value",
                                allow_exceptions, psub );
    }

    if( !OptionActivesSubscriptionImpl::is_valid_duration(duration_type) ){
        return HANDLE_ERROR_EX( ValueException, "invalid DurationType value",
                                allow_exceptions, psub );
    }

    static auto meth = +[]( int v, int d, int cmd ){
        return new OptionActivesSubscriptionImpl(static_cast<VenueType>(v),
                                                 static_cast<DurationType>(d),
                                                 static_cast<CommandType>(cmd));
    };

    OptionActivesSubscriptionImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, venue,
                                     duration_type, command );
    if( err ){
        kill_proxy(psub);
        return err;
    }

    psub->obj = reinterpret_cast<void*>(obj);
    assert(OptionActivesSubscriptionImpl::TYPE_ID_LOW
        == OptionActivesSubscriptionImpl::TYPE_ID_HIGH);
    psub->type_id = OptionActivesSubscriptionImpl::TYPE_ID_LOW;
    return 0;
}





