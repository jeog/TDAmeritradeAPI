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

public class TransactionHistoryGetter extends AccountGetterBase {

    public enum TransactionType implements CLib.ConvertibleEnum {
        ALL(0),
        TRADE(1),
        BUY_ONLY(2),
        SELL_ONLY(3),
        CASH_IN_OR_CASH_OUT(4),
        CHECKING(5),
        DIVIDEND(6),
        INTEREST(7),
        OTHER(8),
        ADVISOR_FEES(9);
            
        private int value;
        
        TransactionType(int value){ this.value = value; } 
        
        @Override
        public int toInt() { return value; }
        
        public static TransactionType
        fromInt(int i) {
            for(TransactionType o : TransactionType.values()) {
                if(o.toInt() == i)
                    return o;
            }
            return null;
        }   
        
        @Override
        public String
        toString() {
            return CLib.Helpers.convertibleEnumToString( this, 
                    TDAmeritradeAPI.getCLib()::TransactionType_to_string_ABI);
        }    
    };
    
    public TransactionHistoryGetter( Credentials creds, String accountID, TransactionType transactionType,
            String symbol, String startDate, String endDate ) throws CLibException {
        super( create(creds, accountID, transactionType, symbol, startDate, endDate) );
    }
    
    public TransactionType
    getTransactionType() throws CLibException {
        int i = CLib.Helpers.getInt(getProxy(), 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_GetTransactionType_ABI);
        return TransactionType.fromInt(i);
    }
    
    public void
    setTransactionType( TransactionType transactionType ) throws CLibException {
        CLib.Helpers.setInt(getProxy(), transactionType.toInt(),
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_SetTransactionType_ABI);
    }
    
    public String
    getSymbol() throws  CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_GetSymbol_ABI);
    }
    
    public void
    setSymbol(String symbol) throws  CLibException {
        CLib.Helpers.setString( getProxy(), symbol, 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_SetSymbol_ABI);
    }
        
    public String
    getStartDate() throws  CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_GetStartDate_ABI);
    }
    
    public void
    setStartDate(String startDate) throws  CLibException {
        CLib.Helpers.setString( getProxy(), startDate, 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_SetStartDate_ABI);
    }
    
    public String
    getEndDate() throws  CLibException {
        return CLib.Helpers.getString( getProxy(), 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_GetEndDate_ABI);
    }
    
    public void
    setEndDate(String endDate) throws  CLibException {
        CLib.Helpers.setString( getProxy(), endDate, 
                TDAmeritradeAPI.getCLib()::TransactionHistoryGetter_SetEndDate_ABI);
    }
    
    
    protected CLib._TransactionHistoryGetter_C
    getProxy() {
        return (CLib._TransactionHistoryGetter_C)super.getProxy();
    }
    
    private static CLib._TransactionHistoryGetter_C
    create( Credentials creds, String accountID, TransactionType transactionType,
            String symbol, String startDate, String endDate ) throws CLibException{
        CLib._TransactionHistoryGetter_C pGetter = new CLib._TransactionHistoryGetter_C();        
        int err = TDAmeritradeAPI.getCLib().TransactionHistoryGetter_Create_ABI( creds.getNativeCredentials(), 
                accountID, transactionType.toInt(), symbol, startDate, endDate, pGetter, 0);
        if( err != 0 )
            throw new CLibException(err);        
        return pGetter;
    }

}