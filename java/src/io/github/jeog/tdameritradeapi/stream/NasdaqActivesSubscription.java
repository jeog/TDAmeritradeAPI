package io.github.jeog.tdameritradeapi.stream;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class NasdaqActivesSubscription extends ActivesSubscriptionBase {

    public NasdaqActivesSubscription( DurationType duration, CommandType command) throws CLibException {
        super( duration, command, new CLib._NasdaqActivesSubscription_C(),
                (DurationCreatable<CLib._NasdaqActivesSubscription_C>)
                    TDAmeritradeAPI.getCLib()::NasdaqActivesSubscription_Create_ABI );
    }
    
    public NasdaqActivesSubscription( DurationType duration ) throws CLibException {
        this(duration, CommandType.SUBS);
    }
}
