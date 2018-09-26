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

#include "../../include/_tdma_api.h"

using namespace std;

namespace tdma {


class QuoteGetterImpl
        : public APIGetterImpl {
    string _symbol;

    void
    _build()
    {
        string url = URL_MARKETDATA + util::url_encode(_symbol) + "/quotes";
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    typedef QuoteGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_QUOTE;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_QUOTE;

    QuoteGetterImpl( Credentials& creds, const string& symbol )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbol( util::toupper(symbol) )
        {
            if( symbol.empty() )
                throw ValueException("empty symbol string");

            _build();
        }

    string
    get_symbol() const
    { return _symbol; }

    void
    set_symbol(const string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol");

        _symbol = util::toupper(symbol);
        build();
    }
};


class QuotesGetterImpl
        : public APIGetterImpl {
    set<string> _symbols;

    void
    _build()
    {
        string qstr = util::build_encoded_query_str(
            {{"symbol", util::join(_symbols,',')}}
        );
        string url = URL_MARKETDATA + "/quotes?" + qstr;
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

    void
    _throw_if_bad_input(const string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol");
    }

    void
    _throw_if_bad_input(const set<string>& symbols)
    {
        if( symbols.empty() )
            throw ValueException("no symbols");
        for(auto& s : symbols)
            _throw_if_bad_input(s);
    }

public:
    typedef QuotesGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_QUOTES;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_QUOTES;

    QuotesGetterImpl( Credentials& creds, const set<string>& symbols)
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbols( util::toupper(symbols) )
        {
            _throw_if_bad_input(symbols);
            _build();
        }

    string
    get()
    { return _symbols.empty() ? string() : APIGetterImpl::get(); }

    set<string>
    get_symbols() const
    { return _symbols; }

    void
    set_symbols(const set<string>& symbols)
    {
        _throw_if_bad_input(symbols);
        _symbols = util::toupper(symbols);
        build();
    }

    void
    add_symbol(const string& symbol)
    {
        _throw_if_bad_input(symbol);
        _symbols.insert( util::toupper(symbol) );
        build();
    }

    void
    remove_symbol(const string& symbol)
    {
        _throw_if_bad_input(symbol);
        _symbols.erase( util::toupper(symbol) );
        build();
    }

    void
    add_symbols(const set<string>& symbols)
    {
        _throw_if_bad_input(symbols);
        for(auto& s : symbols)
            _symbols.insert( util::toupper(s) );
        build();
    }

    void
    remove_symbols(const set<string>& symbols)
    {
        _throw_if_bad_input(symbols);
        for(auto& s : symbols)
            _symbols.erase( util::toupper(s) );
        build();
    }
};

} /* tdma */

using namespace tdma;


int
QuoteGetter_Create_ABI( Credentials *pcreds,
                           const char* symbol,
                           QuoteGetter_C *pgetter,
                           int allow_exceptions )
{
    using ImplTy = QuoteGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, pgetter);

    static auto meth = +[](Credentials *c, const char* s){
        return new ImplTy(*c, s);
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, pcreds, symbol);
    if( err ){
        kill_proxy(pgetter);
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( ImplTy::TYPE_ID_LOW == ImplTy::TYPE_ID_HIGH );
    pgetter->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}


int
QuoteGetter_Destroy_ABI(QuoteGetter_C *pgetter, int allow_exceptions)
{
    return destroy_proxy<QuoteGetterImpl>(pgetter, allow_exceptions);
}


int
QuoteGetter_GetSymbol_ABI( QuoteGetter_C *pgetter,
                              char **buf,
                              size_t *n,
                              int allow_exceptions)
{
    return ImplAccessor<char**>::template get<QuoteGetterImpl>(
        pgetter, &QuoteGetterImpl::get_symbol, buf, n, allow_exceptions
        );
}


int
QuoteGetter_SetSymbol_ABI( QuoteGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions )
{
    return ImplAccessor<char**>::template set<QuoteGetterImpl>(
        pgetter, &QuoteGetterImpl::set_symbol, symbol, allow_exceptions
        );
}


int
QuotesGetter_Create_ABI( Credentials *pcreds,
                            const char** symbols,
                            size_t nsymbols,
                            QuotesGetter_C *pgetter,
                            int allow_exceptions )
{
    using ImplTy = QuotesGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR_KILL_PROXY(symbols, "symbols", allow_exceptions, pgetter);

    static auto meth = +[](Credentials *c, const char** s, size_t n){
        set<string> strs;
        while( n-- )
            strs.insert(s[n]);
        return new ImplTy(*c, strs);
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, pcreds, symbols,
                                    nsymbols);
    if( err ){
        kill_proxy(pgetter);
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( ImplTy::TYPE_ID_LOW == ImplTy::TYPE_ID_HIGH );
    pgetter->type_id = ImplTy::TYPE_ID_LOW;
    return 0;
}


int
QuotesGetter_Destroy_ABI(QuotesGetter_C *pgetter, int allow_exceptions)
{
    return destroy_proxy<QuotesGetterImpl>(pgetter, allow_exceptions);
}


int
QuotesGetter_GetSymbols_ABI( QuotesGetter_C *pgetter,
                                char ***buf,
                                size_t *n,
                                int allow_exceptions)
{
    return ImplAccessor<char***>::template get<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::get_symbols, buf, n, allow_exceptions
        );
}


int
QuotesGetter_SetSymbols_ABI( QuotesGetter_C *pgetter,
                                const char **symbols,
                                size_t nsymbols,
                                int allow_exceptions )
{
    return ImplAccessor<char***>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::set_symbols, symbols, nsymbols,
        allow_exceptions
        );
}


int
QuotesGetter_AddSymbol_ABI( QuotesGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions )
{
    return ImplAccessor<char**>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::add_symbol, symbol, allow_exceptions
        );
}

int
QuotesGetter_RemoveSymbol_ABI( QuotesGetter_C *pgetter,
                                   const char *symbol,
                                   int allow_exceptions )
{
    return ImplAccessor<char**>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::remove_symbol, symbol, allow_exceptions
        );
}

int
QuotesGetter_AddSymbols_ABI( QuotesGetter_C *pgetter,
                                const char **symbols,
                                size_t nsymbols,
                                int allow_exceptions )
{
    return ImplAccessor<char***>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::add_symbols, symbols, nsymbols,
        allow_exceptions
        );
}

int
QuotesGetter_RemoveSymbols_ABI( QuotesGetter_C *pgetter,
                                   const char **symbols,
                                   size_t nsymbols,
                                  int allow_exceptions )
{
    return ImplAccessor<char***>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::remove_symbols, symbols, nsymbols,
        allow_exceptions
        );
}

