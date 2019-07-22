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
