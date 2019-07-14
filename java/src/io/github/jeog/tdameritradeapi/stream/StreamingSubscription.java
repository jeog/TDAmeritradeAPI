package io.github.jeog.tdameritradeapi.stream;

import io.github.jeog.tdameritradeapi.CLib;

public class StreamingSubscription {
    
    private CLib._StreamingSubscription_C pSubscription;
    
    protected StreamingSubscription( CLib._StreamingSubscription_C pSubscription ){
        this.pSubscription = pSubscription;
    }
    
    protected CLib._StreamingSubscription_C
    getProxy(){
        return pSubscription; 
    }
    
    
}
