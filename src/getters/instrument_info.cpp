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
#include <iostream>

#include "../../include/_tdma_api.h"

namespace tdma {

using namespace std;

class InstrumentInfoGetterImpl
        : public APIGetterImpl {
    std::string _query_string;
    InstrumentSearchType _search_type;

    void
    _build()
    {
        string url;

        if( _search_type == InstrumentSearchType::cusip ){
             url = URL_INSTRUMENTS + "/" + util::url_encode(_query_string);
        }else{
            vector<pair<string,string>> params{
                {"symbol", _query_string},
                {"projection", to_string(_search_type)}
            };

            string qstr = util::build_encoded_query_str(params);
            url = URL_INSTRUMENTS + "?" + qstr;
        }

        APIGetterImpl::set_url(url);
    }

    /*virtual*/ void
    build()
    { _build(); }

public:
    typedef InstrumentInfoGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_INSTRUMENT_INFO;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_INSTRUMENT_INFO;

    InstrumentInfoGetterImpl( Credentials& creds,
                                 InstrumentSearchType search_type,
                                 const std::string& query_string )
        :
            APIGetterImpl(creds, query_api_on_error_callback),
            _query_string(query_string),
            _search_type(search_type)
        {
            if( query_string.empty() )
                throw ValueException("empty query_string");
            _build();
        }

    std::string
    get_query_string() const
    { return _query_string; }

    InstrumentSearchType
    get_search_type() const
    { return _search_type; }

    void
    set_query(InstrumentSearchType search_type, const std::string& query_string)
    {
        if( query_string.empty() )
            throw ValueException("empty query_string");
        _search_type = search_type;
        _query_string = query_string;
        build();
    }
};

} /* tdma */

using namespace tdma;

int
InstrumentInfoGetter_Create_ABI( struct Credentials *pcreds,
                                     int search_type,
                                     const char* query_string,
                                     InstrumentInfoGetter_C *pgetter,
                                     int allow_exceptions )
{
    using ImplTy = InstrumentInfoGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY( InstrumentSearchType, search_type,
                               allow_exceptions, pgetter );
    CHECK_PTR_KILL_PROXY( query_string, "query_string",
                               allow_exceptions, pgetter );

    static auto meth = +[]( Credentials *c, int s, const char* q ){
        return new ImplTy(*c, static_cast<InstrumentSearchType>(s), q);
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds,
                                     search_type, query_string );
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
InstrumentInfoGetter_Destroy_ABI( InstrumentInfoGetter_C *pgetter,
                                      int allow_exceptions )
{ return destroy_proxy<InstrumentInfoGetterImpl>(pgetter, allow_exceptions); }

int
InstrumentInfoGetter_GetSearchType_ABI( InstrumentInfoGetter_C *pgetter,
                                              int *search_type,
                                              int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<InstrumentInfoGetterImpl, InstrumentSearchType>(
            pgetter, &InstrumentInfoGetterImpl::get_search_type,
            search_type, "search_type", allow_exceptions
        );
}

int
InstrumentInfoGetter_GetQueryString_ABI( InstrumentInfoGetter_C *pgetter,
                                              char **buf,
                                              size_t *n,
                                              int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<InstrumentInfoGetterImpl>(
            pgetter, &InstrumentInfoGetterImpl::get_query_string,
            buf, n, allow_exceptions
        );
}

int
InstrumentInfoGetter_SetQuery_ABI( InstrumentInfoGetter_C *pgetter,
                                       int search_type,
                                       const char* query_string,
                                       int allow_exceptions )
{
    CHECK_ENUM(InstrumentSearchType, search_type,allow_exceptions);

    return ImplAccessor<int>::template
        set<InstrumentInfoGetterImpl, InstrumentSearchType>(
            pgetter, &InstrumentInfoGetterImpl::set_query, search_type,
            query_string, allow_exceptions
            );
}

