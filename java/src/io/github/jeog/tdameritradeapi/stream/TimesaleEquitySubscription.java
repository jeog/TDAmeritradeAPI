package io.github.jeog.tdameritradeapi.stream;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class TimesaleEquitySubscription extends TimesaleSubscriptionBase {

    public TimesaleEquitySubscription( Set<String> symbols, Set<FieldType> fields, 
            CommandType command) throws CLibException {
        super( symbols, fields, command, new CLib._TimesaleEquitySubscription_C(), 
               (SymbolAndFieldCreatable<CLib._TimesaleEquitySubscription_C>)
                   TDAmeritradeAPI.getCLib()::TimesaleEquitySubscription_Create_ABI );      
    }
    
    public TimesaleEquitySubscription( Set<String> symbols, Set<FieldType> fields) 
            throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public TimesaleEquitySubscription( Set<String> symbols) throws CLibException {
        this(symbols, FieldType.ALL, StreamingSession.CommandType.SUBS);
    } 

}

