/*
Copyright (C) 2019 Jonathon Ogden <jeog.dev@gmail.com>

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

package io.github.jeog.tdameritradeapi.stream;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class ChartFuturesSubscription extends ChartSubscriptionBase {

    public ChartFuturesSubscription( Set<String> symbols, Set<FieldType> fields, 
            CommandType command) throws CLibException {
        super( symbols, fields, command, new CLib._ChartFuturesSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._ChartFuturesSubscription_C>)
                   TDAmeritradeAPI.getCLib()::ChartFuturesSubscription_Create_ABI );      
    }
    
    public ChartFuturesSubscription( Set<String> symbols, Set<FieldType> fields) 
            throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public ChartFuturesSubscription( Set<String> symbols) throws CLibException {
        this(symbols, FieldType.ALL, StreamingSession.CommandType.SUBS);
    }
}
