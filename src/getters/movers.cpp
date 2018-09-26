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

class MoversGetterImpl
        : public APIGetterImpl{
    MoversIndex _index;
    MoversDirectionType _direction_type;
    MoversChangeType _change_type;

    void
    _build()
    {
        vector<pair<string,string>> params = {{"change", to_string(_change_type)}};

        if( _direction_type != MoversDirectionType::up_and_down ){
            params.emplace_back("direction", to_string(_direction_type));
        }

        string qstr = util::build_encoded_query_str(params);
        string url = URL_MARKETDATA + util::url_encode(to_string(_index))
                     + "/movers?" + qstr;
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    typedef MoversGetter ProxyType;
    static const int TYPE_ID_LOW = TYPE_ID_GETTER_MOVERS;
    static const int TYPE_ID_HIGH = TYPE_ID_GETTER_MOVERS;

    MoversGetterImpl( Credentials& creds,
                        MoversIndex index,
                        MoversDirectionType direction_type,
                        MoversChangeType change_type )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _index(index),
            _direction_type(direction_type),
            _change_type(change_type)
        {
            _build();
        }

    MoversIndex
    get_index() const
    { return _index; }

    MoversDirectionType
    get_direction_type() const
    { return _direction_type; }

    MoversChangeType
    get_change_type() const
    { return _change_type; }

    void
    set_index(MoversIndex index)
    {
        _index = index;
        build();
    }

    void
    set_direction_type(MoversDirectionType direction_type)
    {
        _direction_type = direction_type;
        build();
    }

    void
    set_change_type(MoversChangeType change_type)
    {
        _change_type = change_type;
        build();
    }
};

} /* tdma */

using namespace tdma;

int
MoversGetter_Create_ABI( struct Credentials *pcreds,
                            int index,
                            int direction_type,
                            int change_type,
                            MoversGetter_C *pgetter,
                            int allow_exceptions )
{
    using ImplTy = MoversGetterImpl;

    int err = getter_is_creatable<ImplTy>(pcreds, pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_ENUM_KILL_PROXY( MoversIndex, index, allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( MoversDirectionType, direction_type,
                               allow_exceptions, pgetter );
    CHECK_ENUM_KILL_PROXY( MoversChangeType, change_type, allow_exceptions,
                               pgetter );

    static auto meth = +[](Credentials *c, int m, int d, int ct){
        return new ImplTy( *c, static_cast<MoversIndex>(m),
                           static_cast<MoversDirectionType>(d),
                           static_cast<MoversChangeType>(ct) );
    };

    ImplTy *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds,
                                     index, direction_type, change_type);
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
MoversGetter_Destroy_ABI( MoversGetter_C *pgetter, int allow_exceptions)
{
    return destroy_proxy<MoversGetterImpl>(pgetter, allow_exceptions);
}

int
MoversGetter_GetIndex_ABI( MoversGetter_C *pgetter,
                              int *index,
                              int allow_exceptions)
{
    return ImplAccessor<int>::template get<MoversGetterImpl, MoversIndex>(
        pgetter, &MoversGetterImpl::get_index, index, "index", allow_exceptions
        );
}

int
MoversGetter_SetIndex_ABI( MoversGetter_C *pgetter,
                              int index,
                              int allow_exceptions )
{
    CHECK_ENUM(MoversIndex, index, allow_exceptions);

    return ImplAccessor<int>::template set<MoversGetterImpl, MoversIndex>(
        pgetter, &MoversGetterImpl::set_index, index, allow_exceptions
        );
}

int
MoversGetter_GetDirectionType_ABI( MoversGetter_C *pgetter,
                                       int *direction_type,
                                       int allow_exceptions)
{
    return ImplAccessor<int>::template
        get<MoversGetterImpl, MoversDirectionType>(
            pgetter, &MoversGetterImpl::get_direction_type, direction_type,
            "direction_type", allow_exceptions
        );
}

int
MoversGetter_SetDirectionType_ABI( MoversGetter_C *pgetter,
                                       int direction_type,
                                       int allow_exceptions )
{
    CHECK_ENUM( MoversDirectionType, direction_type, allow_exceptions );

    return ImplAccessor<int>::template
        set<MoversGetterImpl, MoversDirectionType>(
            pgetter, &MoversGetterImpl::set_direction_type, direction_type,
            allow_exceptions
        );
}

int
MoversGetter_GetChangeType_ABI( MoversGetter_C *pgetter,
                                   int *change_type,
                                   int allow_exceptions)
{
    return ImplAccessor<int>::template
        get<MoversGetterImpl, MoversChangeType>(
            pgetter, &MoversGetterImpl::get_change_type, change_type,
            "change_type", allow_exceptions
        );
}

int
MoversGetter_SetChangeType_ABI( MoversGetter_C *pgetter,
                                   int change_type,
                                   int allow_exceptions )
{
    CHECK_ENUM( MoversChangeType, change_type, allow_exceptions );

    return ImplAccessor<int>::template
        set<MoversGetterImpl, MoversChangeType>(
            pgetter, &MoversGetterImpl::set_change_type, change_type,
            allow_exceptions
        );
}




