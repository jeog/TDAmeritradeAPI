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

public class OptionActivesSubscription extends ActivesSubscriptionBase {

    public enum VenueType implements CLib.ConvertibleEnum {
        OPTS(0),
        CALLS(1),
        PUTS(2),
        OPTS_DESC(3),
        CALLS_DESC(4),
        PUTS_DESC(5);
            
        private int value;
        
        VenueType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static VenueType
        fromInt(int i) {
            for(VenueType d : VenueType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::VenueType_to_string_ABI);
        }    
    };
    
    public VenueType
    getVenue() throws CLibException{
        int v = CLib.Helpers.getInt( getProxy(), 
                TDAmeritradeAPI.getCLib()::OptionActivesSubscription_GetVenue_ABI);
        return VenueType.fromInt(v);
    }
    
    public void
    setVenue( VenueType venue ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), venue.toInt(), 
                TDAmeritradeAPI.getCLib()::OptionActivesSubscription_SetVenue_ABI);
    }
    
    public OptionActivesSubscription( VenueType venue, DurationType duration, CommandType command)
            throws CLibException{
        super( create(venue, duration, command) );
    }
    
    public OptionActivesSubscription( VenueType venue, DurationType duration) throws CLibException{
        super( create(venue, duration, CommandType.SUBS) );
    }
    
    @Override
    protected CLib._OptionActivesSubscription_C
    getProxy(){
        return (CLib._OptionActivesSubscription_C)super.getProxy();
    }
    
    private static CLib._OptionActivesSubscription_C
    create(VenueType venue, DurationType duration, CommandType command)
            throws CLibException{
        CLib._OptionActivesSubscription_C pSub = new CLib._OptionActivesSubscription_C();
        int err = TDAmeritradeAPI.getCLib().OptionActivesSubscription_Create_ABI(venue.toInt(), 
                duration.toInt(), command.toInt(), pSub, 0);
        if( err != 0 )
            throw new CLibException(err);
        return pSub;
    }
}
