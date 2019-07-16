
import java.util

import org.json.JSONObject

import io.github.jeog.tdameritradeapi.TDAmeritradeAPI
import io.github.jeog.tdameritradeapi.Error
import io.github.jeog.tdameritradeapi.Auth
import io.github.jeog.tdameritradeapi.stream.StreamingSession
import io.github.jeog.tdameritradeapi.stream.TimesaleEquitySubscription
import io.github.jeog.tdameritradeapi.get.QuotesGetter;

object Test {
    val STREAMING_WAIT : Int = 10000

    class MyCallback extends StreamingSession.Callback{
        override def call(cbType: Int, ssType: Int, timestamp: Long, data: String): Unit = {
            val cb : String = StreamingSession.CallbackType.fromInt(cbType).toString()
            val ss : String = StreamingSession.ServiceType.fromInt(ssType).toString()
            println("CALLBACK: [callback: " + cb + ", service: " + ss  + ", timestamp:" + timestamp.toString + "]")
            println("  DATA: " + data)
        }
    }

    def main(args: Array[String]) : Unit = {

        val nArgs = args.length;
        if( nArgs == 2 || nArgs > 4 ){
            println("Usage: ");
            println("  Test ")
            println("  Test <library path>")
            println("  Test <credentials path> <credentials password> <account id> ")
            println("  Test <credentials path> <credentials password> <account id> <library path>")     
            return;
        }

        if( !TDAmeritradeAPI.libraryIsLoaded() ){
            if( nArgs != 1 && nArgs != 4 ){
                System.err.println(" Library not loaded automatically; pass library path as 1st or 4th arg to load manually")
                return
            }else{
                if( !TDAmeritradeAPI.init( args(nArgs-1) ) ){
                    System.err.println(" Failed to load TDAmeritradeAPI from: " + args(nArgs-1));
                    return;
                }
                System.out.println(" Successfully loaded TDAmeritradeAPI (manually) from : " + args(nArgs-1))
            }
        }           

        try {
            if( Error.lastErrorCode() != 0 ){
                System.err.println("lastErrorCode != 0; exiting...")
                return
            }
            System.out.println("Successfully called into library!");

            if( nArgs < 3 ){
                System.out.println("no credential/account args; exiting...")
                return
            }  

            val cm: Auth.CredentialsManager = new Auth.CredentialsManager(args(0), args(1));
            testQuotesGetter(cm.getCredentials);
            testStreaming(cm.getCredentials);
        }catch{
            case e : Throwable => System.err.println("Error: " + e.toString())
        }
    }

    private[this]
    def testQuotesGetter(creds: Auth.Credentials): Unit ={
        val qGetter : QuotesGetter = new QuotesGetter(
            creds, new util.HashSet(util.Arrays.asList("SPY","QQQ","IWM"))
        )

        var j : JSONObject = qGetter.get.asInstanceOf[JSONObject];
        println( "QUOTES GETTER DATA: " + qGetter.getSymbols.toString )
        println( j.toString(4) )

        qGetter.removeSymbol("QQQ" )
        qGetter.removeSymbol( "IWM" )
        qGetter.addSymbol( "GLD" )

        j = qGetter.get.asInstanceOf[JSONObject];
        println( "QUOTES GETTER DATA: " + qGetter.getSymbols.toString )
        println( j.toString(4) )
    }

    private[this]
    def testStreaming(creds: Auth.Credentials): Unit ={
        val tse : TimesaleEquitySubscription =
            new TimesaleEquitySubscription( new util.HashSet(util.Arrays.asList("SPY","QQQ")) );

        val session: StreamingSession = new StreamingSession(creds, new MyCallback);
        try {
            val result : Boolean = session.start(tse);
            if( !result ) {
                println(" session.start failed!");
                return;
            }
            Thread.sleep(10000);
        }finally{
            session.stop();
        }
    }
}
