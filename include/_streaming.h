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
#include "tdma_api_streaming.h"

namespace tdma {

using namespace std;

StreamerServiceType
streamer_service_from_str(string service_name);


StreamerInfo
get_streamer_info(Credentials& creds);

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

// THROWS
StreamingSubscriptionImpl
C_sub_ptr_to_impl(StreamingSubscription_C *psub);


} /* tdma */


