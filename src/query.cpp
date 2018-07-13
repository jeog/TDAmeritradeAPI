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

#include "../include/_tdma_api.h"

namespace tdma {

using namespace std;

const string URL_INSTRUMENTS = URL_BASE + "instruments";


void
query_api_on_error_callback(long code, const string& data)
{
    /*
     *  codes 500, 503, 401, 403, 404 handled by base callback
     */
    switch(code){
    case 400:
        throw InvalidRequest("validation problem", code);
    case 406:
        throw InvalidRequest("invalid regex or excessive requests", code);
    };
}

/*

InstrumentInfoGetter::InstrumentInfoGetter( Credentials& creds,
                                            InstrumentSearchType search_type ,
                                            const string& query_string )
    :
        APIGetter(creds, query_api_on_error_callback),
        _query_string(query_string),
        _search_type(search_type)
    {
        _build();
    }


void
InstrumentInfoGetter::_build()
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

    APIGetter::set_url(url);
}


virtual void
InstrumentInfoGetter::build()
{ _build(); }


void
InstrumentInfoGetter::set_query( InstrumentSearchType search_type,
                                 const string& query_string )
{
    _search_type = search_type;
    _query_string = query_string;
    build();
}
*/


} /* tdma */
