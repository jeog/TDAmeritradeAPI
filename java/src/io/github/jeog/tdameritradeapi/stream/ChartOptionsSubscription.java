package io.github.jeog.tdameritradeapi.stream;

import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public class ChartOptionsSubscription extends ChartSubscriptionBase {

    public ChartOptionsSubscription( Set<String> symbols, Set<FieldType> fields, 
            CommandType command) throws CLibException {
        super( symbols, fields, command, new CLib._ChartOptionsSubscription_C(), 
               (SymbolAndFieldCreatable<CLib._ChartOptionsSubscription_C>)
                   TDAmeritradeAPI.getCLib()::ChartOptionsSubscription_Create_ABI );      
    }
    
    public ChartOptionsSubscription( Set<String> symbols, Set<FieldType> fields) 
            throws CLibException {
        this(symbols, fields, StreamingSession.CommandType.SUBS);
    }
    
    public ChartOptionsSubscription( Set<String> symbols) throws CLibException {
        this(symbols, FieldType.ALL, StreamingSession.CommandType.SUBS);
    }
}
