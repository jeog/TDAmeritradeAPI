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

#include "../../include/_tdma_api.h"

namespace tdma {

using namespace std;


StreamingSubscription::StreamingSubscription(
        StreamerService::type service,
        const string& command,
        const map<string, string>& paramaters
        )
    :
        _service(service),
        _command(command),
        _parameters(paramaters)
    {
    }


string
StreamingSubscription::encode_symbol(string symbol)
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


QuotesSubscription::QuotesSubscription( const set<string>& symbols,
                                        const set<FieldType>& fields )
   :
      SubscriptionBySymbolBase(StreamerService::type::QUOTE, "SUBS",
                               symbols, fields),
      _fields(fields)
    {
    }


TimesalesSubscriptionBase::TimesalesSubscriptionBase(
        StreamerService::type service,
        const set<string>& symbols,
        const set<FieldType>& fields
        )
    :
        SubscriptionBySymbolBase(service, "SUBS", symbols, fields),
        _fields(fields)
    {
    }


ActivesSubscriptionBase::ActivesSubscriptionBase( StreamerService::type service,
                                                  string venue,
                                                  DurationType duration )
    :
        StreamingSubscription( service, "SUBS",
            { {"keys", venue + "-" + to_string(duration)}, {"fields", "0,1"} } ),
        _venue(venue),
        _duration(duration)
    {
    }


OptionActivesSubscription::OptionActivesSubscription( VenueType venue,
                                                      DurationType duration )
    :
        ActivesSubscriptionBase(StreamerService::type::ACTIVES_OPTIONS,
                                to_string(venue), duration),
        _venue(venue)
    {
    }


OptionsSubscription::OptionsSubscription( const set<string>& symbols,
                                          const set<FieldType>& fields )
   :
      SubscriptionBySymbolBase(StreamerService::type::OPTION, "SUBS",
                               symbols, fields),
      _fields(fields)
    {
    }


LevelOneFuturesSubscription::LevelOneFuturesSubscription( 
        const set<string>& symbols,
        const set<FieldType>& fields 
        )
   :
      SubscriptionBySymbolBase(StreamerService::type::LEVELONE_FUTURES, "SUBS",
                               symbols, fields),
      _fields(fields)
    {
    }


LevelOneForexSubscription::LevelOneForexSubscription( 
        const set<string>& symbols,
        const set<FieldType>& fields 
        )
   :
      SubscriptionBySymbolBase(StreamerService::type::LEVELONE_FOREX, "SUBS",
                               symbols, fields),
      _fields(fields)
    {
    }


LevelOneFuturesOptionsSubscription::LevelOneFuturesOptionsSubscription(
        const set<string>& symbols,
        const set<FieldType>& fields
        )
   :
      SubscriptionBySymbolBase(StreamerService::type::LEVELONE_FUTURES_OPTIONS,
                               "SUBS", symbols, fields),
      _fields(fields)
    {
    }


NewsHeadlineSubscription::NewsHeadlineSubscription(
        const set<string>& symbols,
        const set<FieldType>& fields
        )
   :
      SubscriptionBySymbolBase(StreamerService::type::NEWS_HEADLINE,
                               "SUBS", symbols, fields),
      _fields(fields)
    {
    }


ChartEquitySubscription::ChartEquitySubscription(
        const set<string>& symbols,
        const set<FieldType>& fields
        )
   :
      SubscriptionBySymbolBase(StreamerService::type::CHART_EQUITY,
                               "SUBS", symbols, fields),
      _fields(fields)
    {
    }


ChartSubscriptionBase::ChartSubscriptionBase(
        StreamerService::type service,
        const set<string>& symbols,
        const set<FieldType>& fields
        )
    :
        SubscriptionBySymbolBase(service, "SUBS", symbols, fields),
        _fields(fields)
    {
    }

} /* tdma */
