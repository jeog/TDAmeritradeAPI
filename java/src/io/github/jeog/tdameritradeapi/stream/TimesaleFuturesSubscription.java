package io.github.jeog.tdameritradeapi.stream;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class TimesaleFuturesSubscription extends TimesaleSubscriptionBase {

    public TimesaleFuturesSubscription( Set<String> symbols, Set<FieldType> fields, 
            CommandType command) throws CLibException {
        super( symbols, fields, command, new CLib._TimesaleFuturesSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._TimesaleFuturesSubscription_C>)
                   TDAmeritradeAPI.getCLib()::TimesaleFuturesSubscription_Create_ABI );      
    }
    
    public TimesaleFuturesSubscription( Set<String> symbols, Set<FieldType> fields) 
            throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public TimesaleFuturesSubscription( Set<String> symbols) throws CLibException {
        this(symbols, FieldType.ALL, StreamingSession.CommandType.SUBS);
    }
}
