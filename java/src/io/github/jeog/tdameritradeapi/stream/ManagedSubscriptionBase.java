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

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.CLib._StreamingSubscription_C;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.ServiceType;

public abstract class ManagedSubscriptionBase extends StreamingSubscription {

    protected ManagedSubscriptionBase(_StreamingSubscription_C pSubscription) {
        super(pSubscription);      
    }
    
    public ServiceType
    getService() throws CLibException { 
        int s = CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::StreamingSubscription_GetService_ABI );
        return ServiceType.fromInt(s);
    }
    
    public CommandType
    getCommand() throws CLibException { 
        int s = CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::StreamingSubscription_GetCommand_ABI );
        return CommandType.fromInt(s);
    }
    
    public void
    setCommand(CommandType command) throws CLibException { 
        CLib.Helpers.setInt(getProxy(), command.toInt(), 
                TDAmeritradeAPI.getCLib()::StreamingSubscription_SetCommand_ABI );        
    }
}
