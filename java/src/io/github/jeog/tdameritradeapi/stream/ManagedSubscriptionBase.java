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
