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

public class OrdersGetter extends AccountGetterBase {

    public enum OrderStatusType implements CLib.ConvertibleEnum {
        AWAITING_PARENT_ORDER(0),
        AWAITING_CONDITION(1),
        AWAITING_MANUAL_REVIEW(2),
        ACCEPTED(3),
        AWAITING_UR_OUT(4),
        PENDING_ACTIVATION(5),
        QUEUED(6),
        WORKING(7),
        REJECTED(8),
        PENDING_CANCEL(9),
        CANCELED(10),
        PENDING_REPLACE(11),
        REPLACED(12),
        FILLED(13),
        EXPIRED(14);
            
        private int value;
        
        OrderStatusType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static OrderStatusType
        fromInt(int i) {
            for(OrderStatusType d : OrderStatusType.values()) {
                if(d.toInt() == i)
                    return d;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::OrderStatusType_to_string_ABI);
        }    
    };
    
    public OrdersGetter( Credentials creds, String accountID, int nMaxResults, String fromEnteredTime, 
            String toEnteredTime, OrderStatusType orderStatusType) throws CLibException {
        super( create(creds, accountID, nMaxResults, fromEnteredTime, toEnteredTime, orderStatusType) );
    }
    
    public int
    getNMaxResults() throws CLibException {
        return CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OrdersGetter_GetNMaxResults_ABI);
    }
    
    public void
    setNMaxResults(int nMaxResults) throws CLibException {
        CLib.Helpers.setInt( getProxy(), nMaxResults, 
                TDAmeritradeAPI.getCLib()::OrdersGetter_SetNMaxResults_ABI);
    }
    
    public String
    getFromEnteredTime() throws CLibException{
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OrdersGetter_GetFromEnteredTime_ABI);
    }
    
    public void
    setFromEnteredTime(String fromEnteredTime) throws CLibException{
        CLib.Helpers.setString( getProxy(), fromEnteredTime,
                TDAmeritradeAPI.getCLib()::OrdersGetter_SetFromEnteredTime_ABI);
    }
    
    public String
    getToEnteredTime() throws CLibException{
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::OrdersGetter_GetToEnteredTime_ABI);
    }
    
    public void
    setToEnteredTime(String toEnteredTime) throws CLibException{
        CLib.Helpers.setString( getProxy(), toEnteredTime,
                TDAmeritradeAPI.getCLib()::OrdersGetter_SetToEnteredTime_ABI);
    }
    
    public OrderStatusType
    getOrderStatusType() throws CLibException {
        int i = CLib.Helpers.getInt( getProxy(),
                TDAmeritradeAPI.getCLib()::OrdersGetter_GetOrderStatusType_ABI);
        return OrderStatusType.fromInt(i);
    }
    
    public void
    setOrderStatusType( OrderStatusType orderStatusType ) throws CLibException {
        CLib.Helpers.setInt( getProxy(), orderStatusType.toInt(),
                TDAmeritradeAPI.getCLib()::OrdersGetter_SetOrderStatusType_ABI);        
    }
    
    protected CLib._OrdersGetter_C
    getProxy() {
        return (CLib._OrdersGetter_C)super.getProxy();
    }
    
    private static CLib._OrdersGetter_C
    create( Credentials creds, String accountID, int nMaxResults, String fromEnteredTime,
            String toEnteredTime, OrderStatusType orderStatusType ) throws CLibException{
        CLib._OrdersGetter_C pGetter = new CLib._OrdersGetter_C();        
        int err = TDAmeritradeAPI.getCLib().OrdersGetter_Create_ABI( creds.getNativeCredentials(), 
                accountID, nMaxResults, fromEnteredTime, toEnteredTime, orderStatusType.toInt(), pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }
    
}
