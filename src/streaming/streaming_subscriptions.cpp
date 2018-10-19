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

namespace tdma {

StreamingSubscriptionImpl::StreamingSubscriptionImpl(
        StreamerServiceType service,
        const std::string& command,
        const std::map<std::string, std::string>& paramaters )
    :
        _service(service),
        _command(command),
        _parameters(paramaters)
    {
    }

std::string
StreamingSubscriptionImpl::encode_symbol(std::string symbol)
{
    static unordered_map<char, string> R{{'.',"/"}, {'-',"p"}, {'+', "/WS/"}};
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
    std::set<std::string> _symbols;

    template<typename F>
    std::map<std::string, std::string>
    build_paramaters( const std::set<std::string>& symbols,
                        const std::set<F>& fields )
    {
        if( symbols.empty() )
            TDMA_API_THROW(ValueException,"no symbols");

        if( fields.empty() )
            TDMA_API_THROW(ValueException,"no fields");

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


std::function<bool(int)> QuotesSubscriptionImpl::is_valid_field =
    QuotesSubscriptionField_is_valid;

std::function<bool(int)> OptionsSubscriptionImpl::is_valid_field =
    OptionsSubscriptionField_is_valid;

std::function<bool(int)> LevelOneFuturesSubscriptionImpl::is_valid_field =
    LevelOneFuturesSubscriptionField_is_valid;

std::function<bool(int)> LevelOneForexSubscriptionImpl::is_valid_field =
    LevelOneForexSubscriptionField_is_valid;

std::function<bool(int)> LevelOneFuturesOptionsSubscriptionImpl::is_valid_field =
    LevelOneFuturesOptionsSubscriptionField_is_valid;

std::function<bool(int)> NewsHeadlineSubscriptionImpl::is_valid_field =
    NewsHeadlineSubscriptionField_is_valid;

std::function<bool(int)> ChartEquitySubscriptionImpl::is_valid_field =
    ChartEquitySubscriptionField_is_valid;

std::function<bool(int)> ChartSubscriptionBaseImpl::is_valid_field =
    ChartSubscriptionField_is_valid;

std::function<bool(int)> TimesaleSubscriptionBaseImpl::is_valid_field =
    TimesaleSubscriptionField_is_valid;

std::function<bool(int)> ActivesSubscriptionBaseImpl::is_valid_duration =
    DurationType_is_valid;

std::function<bool(int)> OptionActivesSubscriptionImpl::is_valid_venue =
    VenueType_is_valid;


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


template<typename ImplTy>
int
create_symbol_field_subscription( const char **symbols,
                                     size_t nsymbols,
                                     int *fields,
                                     size_t nfields,
                                     typename ImplTy::ProxyType::CType *psub,
                                     int allow_exceptions )
{
    int err = subscription_is_creatable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR_KILL_PROXY(symbols, "symbols", allow_exceptions, psub);
    CHECK_PTR_KILL_PROXY(fields, "fields", allow_exceptions, psub);

    if( nsymbols > SUBSCRIPTION_MAX_SYMBOLS ){
        return HANDLE_ERROR_EX(ValueException,
            "nsymbols > SUBSCRIPTION_MAX_SYMBOLS", allow_exceptions, psub
            );
    }

    if( nfields > SUBSCRIPTION_MAX_FIELDS ){
        return HANDLE_ERROR_EX(ValueException,
            "nfields > SUBSCRIPTION_MAX_FIELDS", allow_exceptions, psub
            );
    }
    
    auto s_symbols = util::buffers_to_set<string>(symbols, nsymbols);

    //TODO combine with buffers_to_set
    for(size_t i = 0; i < nfields; ++i){
        if( !ImplTy::is_valid_field(fields[i]) ){
            return HANDLE_ERROR_EX(ValueException,
                "invalid FieldType value", allow_exceptions, psub
                );
        }
    }

    auto s_fields = util::buffers_to_set<typename ImplTy::FieldType>(
        fields, nfields
        );

    static auto meth = +[]( const set<string>& symbols,
                            const set<typename ImplTy::FieldType>& fields ){
        return new ImplTy( symbols, fields );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, s_symbols, s_fields);
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
                                 typename ImplTy::ProxyType::CType *psub,
                                 int allow_exceptions )
{
    int err = subscription_is_creatable<ImplTy>(psub, allow_exceptions);
    if( err )
        return err;

    if( !ImplTy::is_valid_duration(duration) ){
        return HANDLE_ERROR_EX(ValueException,
            "invalid DurationType value", allow_exceptions, psub
            );
    }

    static auto meth = +[]( int d ){
        return new ImplTy( static_cast<DurationType>(d) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, duration);
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
get_fields(typename ImplTy::ProxyType::CType *psub, int** fields, size_t *n,
            int allow_exceptions)
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
    *fields = reinterpret_cast<int*>(malloc(*n * sizeof(int)));
    if( !*fields ){
        return HANDLE_ERROR(MemoryError,
            "failed to allocate buffer memory", allow_exceptions
            );
    }
    int i = 0;
    for(auto& ff : f)
        (*fields)[i++] = static_cast<int>(ff);

    return 0;
}


} /* namespace */



using namespace tdma;

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
StreamingSubscription_Destroy_ABI(StreamingSubscription_C *psub,
                                      int allow_exceptions)
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
                                          char **buf,
                                          size_t *n,
                                          int allow_exceptions )
{
    int err = proxy_is_callable<StreamingSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(buf, "buf", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return reinterpret_cast<StreamingSubscriptionImpl*>(obj)->get_command();
    };

    string r;
    tie(r, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    if( err )
        return err;

    return to_new_char_buffer(r, buf, n, allow_exceptions);
}

int
SubscriptionBySymbolBase_GetSymbols_ABI( StreamingSubscription_C *psub,
                                              char ***buffers,
                                              size_t *n,
                                              int allow_exceptions)
{
    int err = proxy_is_callable<SubscriptionBySymbolBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(buffers, "buffers", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return reinterpret_cast<SubscriptionBySymbolBaseImpl*>(obj)->get_symbols();
    };

    std::set<std::string> strs;
    std::tie(strs,err) = CallImplFromABI(allow_exceptions, meth, psub->obj);
    if( err )
       return err;

    *n = strs.size();
    *buffers = reinterpret_cast<char**>(malloc((*n) * sizeof(char*)));
    if( !*buffers ){
       return HANDLE_ERROR(MemoryError,
           "failed to allocate buffer memory", allow_exceptions
           );
    }

    int cnt = 0;
    for(auto& s : strs){
       size_t s_sz = s.size();
       (*buffers)[cnt] = reinterpret_cast<char*>(malloc(s_sz+1));
       if( !(*buffers)[cnt] ){
           return HANDLE_ERROR(MemoryError,
               "failed to allocate buffer memory", allow_exceptions
               );
       }
       (*buffers)[cnt][s_sz] = 0;
       strncpy((*buffers)[cnt], s.c_str(), s_sz);
       ++cnt;
    }

    return 0;
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
ActivesSubscriptionBase_GetDuration_ABI(StreamingSubscription_C *psub,
                                             int *duration,
                                             int allow_exceptions)
{
    int err = proxy_is_callable<ActivesSubscriptionBaseImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(duration, "duration", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<ActivesSubscriptionBaseImpl*>(obj)->get_duration()
            );
    };

    tie(*duration, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}

int
OptionActivesSubscription_GetVenue_ABI(OptionActivesSubscription_C *psub,
                                            int *venue,
                                            int allow_exceptions)
{
    int err = proxy_is_callable<OptionActivesSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    CHECK_PTR(venue, "venue", allow_exceptions);

    static auto meth = +[]( void *obj ){
        return static_cast<int>(
            reinterpret_cast<OptionActivesSubscriptionImpl*>(obj)->get_venue()
            );
    };

    tie(*venue, err) = CallImplFromABI( allow_exceptions, meth, psub->obj);
    return err;
}

int
QuotesSubscription_Create_ABI( const char **symbols,
                                   size_t nsymbols,
                                   int *fields,
                                   size_t nfields,
                                   QuotesSubscription_C *psub,
                                   int allow_exceptions )
{
    return create_symbol_field_subscription<QuotesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields,  psub, allow_exceptions
    );
}


int
OptionsSubscription_Create_ABI( const char **symbols,
                                    size_t nsymbols,
                                    int *fields,
                                    size_t nfields,
                                    OptionsSubscription_C *psub,
                                    int allow_exceptions )
{
    return create_symbol_field_subscription<OptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}

int
LevelOneFuturesSubscription_Create_ABI(
    const char **symbols,
    size_t nsymbols,
    int *fields,
    size_t nfields,
    LevelOneFuturesSubscription_C *psub,
    int allow_exceptions
    )
{
    return create_symbol_field_subscription<LevelOneFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}



int
LevelOneForexSubscription_Create_ABI( const char **symbols,
                                           size_t nsymbols,
                                           int *fields,
                                           size_t nfields,
                                           LevelOneForexSubscription_C *psub,
                                           int allow_exceptions )
{
    return create_symbol_field_subscription<LevelOneForexSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
LevelOneFuturesOptionsSubscription_Create_ABI(
    const char **symbols,
    size_t nsymbols,
    int *fields,
    size_t nfields,
    LevelOneFuturesOptionsSubscription_C *psub,
    int allow_exceptions )
{
    return create_symbol_field_subscription
        <LevelOneFuturesOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
NewsHeadlineSubscription_Create_ABI( const char **symbols,
                                         size_t nsymbols,
                                         int *fields,
                                         size_t nfields,
                                         NewsHeadlineSubscription_C *psub,
                                         int allow_exceptions )
{
    return create_symbol_field_subscription<NewsHeadlineSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}

int
ChartEquitySubscription_Create_ABI( const char **symbols,
                                        size_t nsymbols,
                                        int *fields,
                                        size_t nfields,
                                        ChartEquitySubscription_C *psub,
                                        int allow_exceptions )
{
    return create_symbol_field_subscription<ChartEquitySubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}



int
ChartFuturesSubscription_Create_ABI( const char **symbols,
                                         size_t nsymbols,
                                         int *fields,
                                         size_t nfields,
                                         ChartFuturesSubscription_C *psub,
                                         int allow_exceptions )
{
    return create_symbol_field_subscription<ChartFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
ChartOptionsSubscription_Create_ABI( const char **symbols,
                                         size_t nsymbols,
                                         int *fields,
                                         size_t nfields,
                                         ChartOptionsSubscription_C *psub,
                                         int allow_exceptions )
{
    return create_symbol_field_subscription<ChartOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}

int
TimesaleEquitySubscription_Create_ABI( const char **symbols,
                                            size_t nsymbols,
                                            int *fields,
                                            size_t nfields,
                                            TimesaleEquitySubscription_C *psub,
                                            int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleEquitySubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
TimesaleFuturesSubscription_Create_ABI( const char **symbols,
                                             size_t nsymbols,
                                             int *fields,
                                             size_t nfields,
                                             TimesaleFuturesSubscription_C *psub,
                                             int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleFuturesSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
TimesaleOptionsSubscription_Create_ABI( const char **symbols,
                                             size_t nsymbols,
                                             int *fields,
                                             size_t nfields,
                                             TimesaleOptionsSubscription_C *psub,
                                             int allow_exceptions )
{
    return create_symbol_field_subscription<TimesaleOptionsSubscriptionImpl>(
        symbols, nsymbols, fields, nfields, psub, allow_exceptions
    );
}


int
NasdaqActivesSubscription_Create_ABI( int duration_type,
                                           NasdaqActivesSubscription_C *psub,
                                           int allow_exceptions )

{
    return create_duration_subscription<NasdaqActivesSubscriptionImpl>(
        duration_type, psub, allow_exceptions
    );
}


int
NYSEActivesSubscription_Create_ABI( int duration_type,
                                         NYSEActivesSubscription_C *psub,
                                         int allow_exceptions )
{
    return create_duration_subscription<NYSEActivesSubscriptionImpl>(
        duration_type, psub, allow_exceptions
    );
}


int
OTCBBActivesSubscription_Create_ABI( int duration_type,
                                          OTCBBActivesSubscription_C *psub,
                                          int allow_exceptions )
{
    return create_duration_subscription<OTCBBActivesSubscriptionImpl>(
        duration_type, psub, allow_exceptions
    );
}

int
OptionActivesSubscription_Create_ABI( int venue,
                                           int duration_type,
                                           OptionActivesSubscription_C *psub,
                                           int allow_exceptions )
{
    int err = subscription_is_creatable<OptionActivesSubscriptionImpl>(
        psub, allow_exceptions
        );
    if( err )
        return err;

    if( !OptionActivesSubscriptionImpl::is_valid_venue(venue) ){
        return HANDLE_ERROR_EX(ValueException,
            "invalid VenueType value", allow_exceptions, psub
            );
    }

    if( !OptionActivesSubscriptionImpl::is_valid_duration(duration_type) ){
        return HANDLE_ERROR_EX(ValueException,
            "invalid DurationType value", allow_exceptions, psub
            );
    }

    static auto meth = +[]( int v, int d ){
        return new OptionActivesSubscriptionImpl( static_cast<VenueType>(v),
                                                  static_cast<DurationType>(d) );
    };

    OptionActivesSubscriptionImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, venue, duration_type);
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



