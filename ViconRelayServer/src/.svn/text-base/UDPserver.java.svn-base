
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.LinkedBlockingQueue;
import java.io.IOException;

/**
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;
**/
public class UDPserver
{
	
    
    static LinkedBlockingQueue<String> msgBuf = new LinkedBlockingQueue<String>();
    static volatile boolean keepRunning = true;
    static Thread handlerThread, OnExit;
    static DatagramSocket serverSocket;
    
   public static void main(String args[]) throws Exception
      {
	     serverSocket = new DatagramSocket(CONSTANTS.port);
         System.out.println("Listening on "+CONSTANTS.port);
	     mainLoop();
         
	     
	     CONSTANTS.DebugPrint("Keeprunning = false...", 1);
	     
	     
         
         
       }//end public main

   private static void mainLoop(){
	   
	   byte[] receiveData = new byte[1024];
       byte[] sendData = new byte[1024];
       MessageHandler handler = new MessageHandler(); 
       handlerThread = new Thread(handler);
       handlerThread.start();
       exitThread exitT = new exitThread();
       OnExit = new Thread(exitT);
       Runtime.getRuntime().addShutdownHook(OnExit);
       
       
	   
	   while(UDPserver.keepRunning)
       {
          
	      
		   
	      DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
          try {
			serverSocket.receive(receivePacket);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
          String sentence = new String( receivePacket.getData());
          //System.out.println("RECEIVED+OFFERING: " + sentence);
          
          UDPserver.msgBuf.offer(sentence);
          

       }//end while true
	   
   }//end mainloop function

}//end class

class MessageHandler implements Runnable 
{
	//We are capturing this information
	float x, y, z, pitch, yaw, roll, scale, spin;
	Vector<CountryLocation> vecCountries = new Vector<CountryLocation>();
	public Vector<Thread> vecThread;
	
	

	//Message Handler Run
	public void run() {
		/** Add in Country data here **/
		
		/**y=2.4 at display wall		      
		 * |			    4
		 * |			  ^
		 * |______________x,y = 0,0 at center
		 * |			  v
		 * |      <-y->
		 * |			   -2
		 * 
		 * ^Display Wall
		 */
		//									  x      y     z   spin            wave file
		vecCountries.add(new CountryLocation(1.2f, 2.3f,  1.4f, -1, .1f , "MTU_Introduction_English_Female.wav"));
		vecCountries.add(new CountryLocation(1.2f, 2.3f, 1.4f, 1 ,.1f, "MTU_Introduction_English_Male.wav"));
		vecCountries.add(new CountryLocation(-0.2f, 2.5f, 1.6f, -1 ,.05f, "MTU_Introduction_Korean_Female.wav"));
		vecCountries.add(new CountryLocation(-0.2f, 2.5f, 1.6f, 1 ,.05f, "MTU_Introduction_Korean_Male.wav"));
		vecCountries.add(new CountryLocation(-0.5f, 2.5f, 1.6f, -1 ,.05f, "MTU_Introduction_Chinese_Female.wav"));
		vecCountries.add(new CountryLocation(-0.5f, 2.5f, 1.6f, 1 ,.05f, "MTU_Introduction_Chinese_Male.wav"));
		vecCountries.add(new CountryLocation(-2.6f, 2.5f, 2.15f, -1 ,.1f, "MTU_Introduction_British_Female.wav"));
		vecCountries.add(new CountryLocation(-2.6f, 2.5f, 2.15f, 1 ,.1f, "MTU_Introduction_British_Male.wav"));
		vecCountries.add(new CountryLocation(0.0f, 2.5f, 1.66f, -1 ,.05f, "MTU_Introduction_Japanese_Female.wav"));
		vecCountries.add(new CountryLocation(0.0f, 2.5f, 1.66f, 1 ,.05f, "MTU_Introduction_Japanese_Male.wav"));
		
		vecThread = new Vector<Thread>();
		
		
		
		//For each soundbyte, add a thread
		for (int i = 0; i < vecCountries.size(); i++){
			vecThread.add(new Thread(vecCountries.elementAt(i)));
			vecThread.elementAt(i).start();
		}

		/** End Country Data **/
		
		double messages_r = 0;
		double messages_bad = 0;
		
		System.out.println("Message Handler running...");
		
		while (UDPserver.keepRunning) {
            try {
                String msg = UDPserver.msgBuf.take();
                CONSTANTS.DebugPrint("Message Recieved: "+msg, 150);
                //msg = msg.split("\n")[0];
                String[] parts = msg.split("~");
                
                messages_r+=1;
                
	            if (parts.length >= 9){
	                x = Float.parseFloat(parts[2]);
	                y = Float.parseFloat(parts[3]);
	                z = Float.parseFloat(parts[4]);
	                roll = Float.parseFloat(parts[5]);
	                pitch = Float.parseFloat(parts[6]);
	                yaw = Float.parseFloat(parts[7]);
	                testLocation();
	                
	                CONSTANTS.DebugPrint("P: "+pitch+"Y: "+yaw+"R: "+roll, 150);
	                
	                //yaw == up
	                //roll == down
	                //create spin...
	                
	                spin = (float) ((Math.abs(yaw) - Math.abs(roll))/Math.PI);
	                
	                CONSTANTS.DebugPrint("Spin : "+spin, 50);
	                
	                
	            }
	            else{
	            	//System.out.println("Bad Message! Parts:"+parts.toString()+" Expecting 9");
	            	CONSTANTS.DebugPrint("Bad Message! Parts:"+parts.toString()+" Expecting 9", 150);
	            	messages_bad+=1;
	            	
	            	CONSTANTS.DebugPrint(msg, 150);
	            	CONSTANTS.DebugPrint("BAD RATIO: " + (messages_bad/messages_r), 150);
	            	
	            	
	            
	            }
	            
                
                
                //System.out.println("Eating :" + msg);
            } catch (InterruptedException ie) {
            	
            	
            }//end catch
        }//end while	
	}//end run
	
	//Test the location that we are at to see if something needs to be done
	private void testLocation(){
		/* Test Code */
		String messageOut = new String();
		messageOut += "\n\t\tx:" +x +"\ty:"+ y+ "\tz:" + z;
		/* Test Code */
		
		//Get all elements in the country locations
		Object[] loc = vecCountries.toArray();
		
		//TODO: Write in the code that will actually test the location and set the volume
		//test each element, adjust vol.
		for (int i  = 0; i < vecCountries.size(); i++){
			float volset;
			CountryLocation locC = (CountryLocation)loc[i];
			//Use pathagran to figure out distance...
			float locx, locy, locz, locspin, locfall;
			locx = locC.x;
			locy = locC.y;
			locz = locC.z;
			locspin = locC.spin;
			locfall = locC.falloff;
			
			//use just x and z...
			float distance = (float) Math.sqrt(Math.pow(x - locx,2) + Math.pow(z - locz,2));
			float volp1 = (float) (-3.0*(distance/locfall)); 
			//CONSTANTS.scale
			float volp2;
			
			
		
			
			//Calculate precentages of both...
			if (y <= 0){
				volp2 = (float)1.00;
			}
			else {
				volp2 = (float) (1 - y/2.4);
			}
			if (volp2 < 0) {volp2 = 0;}
			
			volset = (float) ((1-volp2)*volp1 - volp2*30.0);
			
			volset += (Math.abs((spin-locspin)/2)*-80.0f);
			
			messageOut+=("\nDistance: " + distance + "\tvolp1: " + volp1+ "\tvolp2: " + volp2 + "\tvolset: "+ volset);
			
			//volset = (float) (Math.abs((locC.x - x))*-10*(2.4-y));
			if( volset < -80){ //Force a proper volume
				volset = -79;
			}
			if (volset > 0){  //Force a proper volume
				volset = 0;
			}
			locC.vol = volset;
			
		}//end test each element
		
		/* Test Code */
		CONSTANTS.DebugPrint(messageOut, 100);
		/* Test Code */
		
	}//end testLocation()
}//end MessageHandler class
	


class exitThread implements Runnable {
	
	public void run() {
		System.out.println("Exiting");
		UDPserver.serverSocket.close();
		UDPserver.keepRunning = false;
	}//end run exitThread
}//end class exitThread

