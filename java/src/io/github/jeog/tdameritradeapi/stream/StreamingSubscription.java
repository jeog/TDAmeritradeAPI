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

public class StreamingSubscription {
    
    private CLib._StreamingSubscription_C pSubscription;
    
    protected StreamingSubscription( CLib._StreamingSubscription_C pSubscription ){
        this.pSubscription = pSubscription;
    }
    
    protected CLib._StreamingSubscription_C
    getProxy(){
        return pSubscription; 
    }
    
    @Override
    public boolean
    equals( Object other ) { // NO THROW
        if( other == this )
            return true;
        
        if( !(other instanceof StreamingSubscription) )
            return false;
        
        int b[] = {0};
        int err = TDAmeritradeAPI.getCLib().StreamingSubscription_IsSame_ABI(getProxy(),
                ((StreamingSubscription)other).getProxy(), b, 0);          
        return (err == 0) && (b[0] == 1);
    }
    
    @Override
    public int
    hashCode() {       
        return pSubscription.getClass().hashCode();
    }

}
