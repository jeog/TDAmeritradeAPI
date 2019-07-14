package io.github.jeog.tdameritradeapi.get;


import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class HistoricalRangeGetter extends HistoricalGetterBase {
    
    public HistoricalRangeGetter( Credentials creds, String symbol, FrequencyType frequencyType, int frequency, 
            long startMSecSinceEpoch, long endMSecSinceEpoch, boolean extendedHours) throws  CLibException {
        super( create(creds, symbol, frequencyType, frequency, startMSecSinceEpoch, endMSecSinceEpoch, extendedHours));
    }
    
    public HistoricalRangeGetter( Credentials creds, String symbol, FrequencyType frequencyType, int frequency, 
            long startMSecSinceEpoch, long endMSecSinceEpoch) throws  CLibException {
        super( create(creds, symbol, frequencyType, frequency, startMSecSinceEpoch, endMSecSinceEpoch, true));
    }
    
    public long
    getStartMSecSinceEpoch() throws CLibException {
        return CLib.Helpers.getLong(getProxy(),
                TDAmeritradeAPI.getCLib()::HistoricalRangeGetter_GetStartMSecSinceEpoch_ABI);
    }
    
    public void
    setStartMSecSinceEpoch( long msecSinceEpoch ) throws CLibException {
        CLib.Helpers.setLong(getProxy(), msecSinceEpoch,
                TDAmeritradeAPI.getCLib()::HistoricalRangeGetter_SetStartMSecSinceEpoch_ABI);
    }
    
    public long
    getEndMSecSinceEpoch() throws CLibException {
        return CLib.Helpers.getLong(getProxy(),
                TDAmeritradeAPI.getCLib()::HistoricalRangeGetter_GetEndMSecSinceEpoch_ABI);
    }
    
    public void
    setEndMSecSinceEpoch( long msecSinceEpoch ) throws CLibException {
        CLib.Helpers.setLong(getProxy(), msecSinceEpoch,
                TDAmeritradeAPI.getCLib()::HistoricalRangeGetter_SetEndMSecSinceEpoch_ABI);
    }
    
    @Override
    protected CLib._HistoricalRangeGetter_C
    getProxy(){
        return (CLib._HistoricalRangeGetter_C)super.getProxy();
    }
    
    static private CLib._HistoricalRangeGetter_C
    create(  Credentials creds, String symbol, FrequencyType frequencyType, int frequency, 
            long startMSecSinceEpoch, long endMSecSinceEpoch, boolean extendedHours ) throws  CLibException {  
        CLib._HistoricalRangeGetter_C pGetter = new CLib._HistoricalRangeGetter_C(); 
        int err = TDAmeritradeAPI.getCLib().HistoricalRangeGetter_Create_ABI( 
                creds.getNativeCredentials(), symbol, frequencyType.toInt(), frequency, startMSecSinceEpoch,
                endMSecSinceEpoch, (extendedHours ? 1 : 0), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);       
        return pGetter;
    }
}
