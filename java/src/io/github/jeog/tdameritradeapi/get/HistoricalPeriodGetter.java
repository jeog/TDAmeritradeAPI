package io.github.jeog.tdameritradeapi.get;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;

public class HistoricalPeriodGetter extends HistoricalGetterBase {

    public enum PeriodType implements CLib.ConvertibleEnum {
        DAY(0),
        MONTH(1),
        YEAR(2),
        YTD(3);
            
        private int value;
        
        PeriodType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static PeriodType
        fromInt(int i) {
            for(PeriodType d : PeriodType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::PeriodType_to_string_ABI);
        }    
    };
    
    public static final Map<PeriodType, Set<Integer>> VALID_PERIODS_BY_PERIOD_TYPE =
            new HashMap<PeriodType, Set<Integer>>();
    static {      
        VALID_PERIODS_BY_PERIOD_TYPE.put(PeriodType.DAY, 
                new HashSet<Integer>( Arrays.asList(1,2,3,4,5,10) ) );
        VALID_PERIODS_BY_PERIOD_TYPE.put(PeriodType.MONTH, 
                new HashSet<Integer>( Arrays.asList(1,2,3,6) ) );
        VALID_PERIODS_BY_PERIOD_TYPE.put(PeriodType.YEAR, 
                new HashSet<Integer>( Arrays.asList(1,2,3,5,10,20) ) );
        VALID_PERIODS_BY_PERIOD_TYPE.put(PeriodType.YTD, 
                new HashSet<Integer>( Arrays.asList(1) ) );
    }
    
    public static final Map<PeriodType, Set<FrequencyType>> VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE =
            new HashMap<PeriodType, Set<FrequencyType>>();
    static {      
        VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.put(PeriodType.DAY, 
                new HashSet<FrequencyType>( Arrays.asList(FrequencyType.MINUTE) ) );
        VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.put(PeriodType.MONTH, 
                new HashSet<FrequencyType>( Arrays.asList(
                        FrequencyType.DAILY, FrequencyType.WEEKLY) ) );
        VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.put(PeriodType.YEAR, 
                new HashSet<FrequencyType>( Arrays.asList(
                        FrequencyType.DAILY, FrequencyType.WEEKLY, FrequencyType.MONTHLY) ) );
        VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.put(PeriodType.YTD, 
                new HashSet<FrequencyType>( Arrays.asList(
                        FrequencyType.DAILY, FrequencyType.WEEKLY) ) );
    }
    
    public HistoricalPeriodGetter( Credentials creds, String symbol, PeriodType periodType, int period, 
            FrequencyType frequencyType, int frequency, boolean extendedHours, long msecSinceEpoch) 
                    throws  CLibException {
        super( create(creds, symbol, periodType, period, frequencyType, frequency, extendedHours, msecSinceEpoch) ) ;
    }
    
    public HistoricalPeriodGetter( Credentials creds, String symbol, PeriodType periodType, int period, 
            FrequencyType frequencyType, int frequency, boolean extendedHours) 
                    throws  CLibException {
        super( create(creds, symbol, periodType, period, frequencyType, frequency, extendedHours, 0) ) ;
    }

    public HistoricalPeriodGetter( Credentials creds, String symbol, PeriodType periodType, int period, 
            FrequencyType frequencyType, int frequency ) 
                    throws  CLibException {
        super( create(creds, symbol, periodType, period, frequencyType, frequency, true, 0) ) ;
    }
 
    public int
    getPeriod() throws CLibException {
        return CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalPeriodGetter_GetPeriod_ABI);        
    }    
   
    public PeriodType
    getPeriodType() throws CLibException {
        int f = CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::HistoricalPeriodGetter_GetPeriodType_ABI);
        return PeriodType.fromInt(f);
    }
    
    public void
    setPeriod( PeriodType periodType, int period ) throws CLibException {
        int err = TDAmeritradeAPI.getCLib().HistoricalPeriodGetter_SetPeriod_ABI(
                getProxy(), periodType.toInt(), period, 0);
        if( err != 0 )
            throw new CLibException(err);
    }
    
    public long
    getMSecSinceEpoch() throws CLibException {
        return CLib.Helpers.getLong(getProxy(),
                TDAmeritradeAPI.getCLib()::HistoricalPeriodGetter_GetMSecSinceEpoch_ABI);
    }
    
    public void
    setMSecSinceEpoch( long msecSinceEpoch ) throws CLibException {
        CLib.Helpers.setLong(getProxy(), msecSinceEpoch,
                TDAmeritradeAPI.getCLib()::HistoricalPeriodGetter_SetMSecSinceEpoch_ABI);
    }
 
    public static boolean
    isValidPeriod( PeriodType periodType, int period ) {
        Set<Integer> valid = VALID_PERIODS_BY_PERIOD_TYPE.get(periodType);
        return (valid != null) && valid.contains(period);
    }
    
    public static boolean
    isValidFrequencyType( PeriodType periodType, FrequencyType frequencyType ) {
        Set<FrequencyType> valid = VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE.get(periodType);
        return (valid != null) && valid.contains(frequencyType);
    }
    
    @Override
    protected CLib._HistoricalPeriodGetter_C
    getProxy(){
        return (CLib._HistoricalPeriodGetter_C)super.getProxy();
    }
    
    static private CLib._HistoricalPeriodGetter_C
    create( Credentials creds, String symbol, PeriodType periodType, int period, FrequencyType frequencyType,
            int frequency, boolean extendedHours, long msecSinceEpoch ) throws  CLibException {  
        CLib._HistoricalPeriodGetter_C pGetter = new CLib._HistoricalPeriodGetter_C(); 
        int err = TDAmeritradeAPI.getCLib().HistoricalPeriodGetter_Create_ABI( 
                creds.getNativeCredentials(), symbol, periodType.toInt(), period, frequencyType.toInt(), 
                frequency, (extendedHours ? 1 : 0), msecSinceEpoch, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);       
        return pGetter;
    }
    
}
