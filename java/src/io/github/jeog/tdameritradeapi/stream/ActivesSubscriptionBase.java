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

package io.github.jeog.tdameritradeapi.stream;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
import io.github.jeog.tdameritradeapi.stream.StreamingSession.CommandType;

public abstract class ActivesSubscriptionBase extends ManagedSubscriptionBase {

    public enum DurationType implements CLib.ConvertibleEnum {
        ALL_DAY(0),
        MIN_60(1),
        MIN_30(2),
        MIN_10(3),
        MIN_5(4),
        MIN_1(5);
            
        private int value;
        
        DurationType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static DurationType
        fromInt(int i) {
            for(DurationType d : DurationType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::DurationType_to_string_ABI);
        }    
    };
      
    
    public DurationType
    getDuration() throws CLibException {
        int d = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::ActivesSubscriptionBase_GetDuration_ABI );
        return DurationType.fromInt(d);
    }
    
    public void
    setDuration(DurationType duration) throws CLibException{
        CLib.Helpers.setInt( getProxy(), duration.toInt(), 
                TDAmeritradeAPI.getCLib()::ActivesSubscriptionBase_SetDuration_ABI );
    }
    
    protected static interface DurationCreatable <O extends CLib._StreamingSubscription_C>{
        int create(int duration, int command, O pSubscription, int exc);
    }
    
    protected <O extends CLib._StreamingSubscription_C>
    ActivesSubscriptionBase( O pSub ) throws CLibException {
        super( pSub);
    }
    
    protected <O extends CLib._StreamingSubscription_C> 
    ActivesSubscriptionBase( DurationType duration, CommandType command, 
            O pSub, DurationCreatable<O> createFunc ) throws CLibException{ 
        super( create(duration, command, pSub, createFunc) );
    }
    
    private static <O extends CLib._StreamingSubscription_C> O
    create( DurationType duration, CommandType command, O pSub, 
            DurationCreatable<O> createFunc ) throws CLibException{                                           
        int err = createFunc.create(duration.toInt(), command.toInt(), pSub, 0);                
        if( err != 0 )
            throw new CLibException(err);                 
        return pSub;            
    }

}
