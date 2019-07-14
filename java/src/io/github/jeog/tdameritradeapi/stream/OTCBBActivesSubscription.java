package io.github.jeog.tdameritradeapi.stream;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class OTCBBActivesSubscription extends ActivesSubscriptionBase {

    public OTCBBActivesSubscription( DurationType duration, CommandType command) throws CLibException {
        super( duration, command, new CLib._OTCBBActivesSubscription_C(),
                (DurationCreatable<CLib._OTCBBActivesSubscription_C>)
                    TDAmeritradeAPI.getCLib()::OTCBBActivesSubscription_Create_ABI );
    }
    
    public OTCBBActivesSubscription( DurationType duration ) throws CLibException {
        this(duration, CommandType.SUBS);
    }
}
