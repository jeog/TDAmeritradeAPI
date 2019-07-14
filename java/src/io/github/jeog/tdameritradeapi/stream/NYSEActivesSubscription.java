package io.github.jeog.tdameritradeapi.stream;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class NYSEActivesSubscription extends ActivesSubscriptionBase {

    public NYSEActivesSubscription( DurationType duration, CommandType command) throws CLibException {
        super( duration, command, new CLib._NYSEActivesSubscription_C(),
                (DurationCreatable<CLib._NYSEActivesSubscription_C>)
                    TDAmeritradeAPI.getCLib()::NYSEActivesSubscription_Create_ABI );
    }
    
    public NYSEActivesSubscription( DurationType duration ) throws CLibException {
        this(duration, CommandType.SUBS);
    }
}
