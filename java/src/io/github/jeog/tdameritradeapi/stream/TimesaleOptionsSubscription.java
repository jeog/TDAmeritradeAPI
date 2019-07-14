package io.github.jeog.tdameritradeapi.stream;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class TimesaleOptionsSubscription extends TimesaleSubscriptionBase {

    public TimesaleOptionsSubscription( Set<String> symbols, Set<FieldType> fields, 
            CommandType command) throws CLibException {
        super( symbols, fields, command, new CLib._TimesaleOptionsSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._TimesaleOptionsSubscription_C>)
                   TDAmeritradeAPI.getCLib()::TimesaleOptionsSubscription_Create_ABI );      
    }
    
    public TimesaleOptionsSubscription( Set<String> symbols, Set<FieldType> fields) 
            throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public TimesaleOptionsSubscription( Set<String> symbols) throws CLibException {
        this(symbols, FieldType.ALL, StreamingSession.CommandType.SUBS);
    }
}

