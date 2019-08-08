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

import org.json.JSONArray;

import io.github.jeog.tdameritradeapi.CLib;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
import io.github.jeog.tdameritradeapi.Auth.Credentials;
import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;


public class MoversGetter extends APIGetter {

    public enum MoversIndexType implements CLib.ConvertibleEnum {
        COMPX(0),
        DJI(1),
        SPX(2);        
            
        private int value;
        
        MoversIndexType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static MoversIndexType
        fromInt(int i) {
            for(MoversIndexType d : MoversIndexType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::MoversIndex_to_string_ABI);
        }    
    };
    
    public enum MoversDirectionType implements CLib.ConvertibleEnum {
        UP(0),
        DOWN(1),
        UP_AND_DOWN(2);        
            
        private int value;
        
        MoversDirectionType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static MoversDirectionType
        fromInt(int i) {
            for(MoversDirectionType d : MoversDirectionType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::MoversDirectionType_to_string_ABI);
        }    
    };
    
    public enum MoversChangeType implements CLib.ConvertibleEnum {
        VALUE(0),
        PERCENT(1);        
            
        private int value;
        
        MoversChangeType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static MoversChangeType
        fromInt(int i) {
            for(MoversChangeType d : MoversChangeType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::MoversChangeType_to_string_ABI);
        }    
    };
    
    public MoversGetter( Credentials creds, MoversIndexType index, 
            MoversDirectionType directionType, MoversChangeType changeType) throws CLibException {
        super( create(creds, index, directionType, changeType) );
    }
    
    @Override
    public JSONArray
    get() throws  CLibException {        
        return new JSONArray( getRaw() );
    }    
     
    public MoversIndexType
    getIndex() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::MoversGetter_GetIndex_ABI);
        return MoversIndexType.fromInt(i);
    }
    
    public void
    setIndex( MoversIndexType index ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), index.toInt(),
                TDAmeritradeAPI.getCLib()::MoversGetter_SetIndex_ABI);        
    }
    
    public MoversDirectionType
    getDirectionType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::MoversGetter_GetDirectionType_ABI);
        return MoversDirectionType.fromInt(i);
    }
    
    public void
    setDirectionType( MoversDirectionType directionType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), directionType.toInt(),
                TDAmeritradeAPI.getCLib()::MoversGetter_SetDirectionType_ABI);        
    }
    
    public MoversChangeType
    getChangeType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::MoversGetter_GetChangeType_ABI);
        return MoversChangeType.fromInt(i);
    }
    
    public void
    setChangeType( MoversChangeType ChangeType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), ChangeType.toInt(),
                TDAmeritradeAPI.getCLib()::MoversGetter_SetChangeType_ABI);        
    }
    
    protected CLib._MoversGetter_C
    getProxy() {
        return (CLib._MoversGetter_C)super.getProxy();
    }
    
    private static CLib._MoversGetter_C
    create( Credentials creds, MoversIndexType index, 
            MoversDirectionType directionType, MoversChangeType changeType) throws CLibException{
        CLib._MoversGetter_C pGetter = new CLib._MoversGetter_C();        
        int err = TDAmeritradeAPI.getCLib().MoversGetter_Create_ABI( creds.getNativeCredentials(), 
                index.toInt(), directionType.toInt(), changeType.toInt(), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
}
