
import java.io.*;
import javax.sound.sampled.*;
import javax.sound.sampled.Control.Type;
/**
 * Use SourceDataLine to read line-by-line from the external sound file.
 * For computer game, use Clip to pre-load short-duration sound files.
 */
public class CountryLocation implements Runnable {
	
	public volatile float vol = 0; 	//Range from -80 to 0 (db)
									// -30 is about whisper range
	public volatile boolean stillrunning = true;
	
	public float x, y, z, spin, falloff;			//location
	private String fileName;
	

	
	public CountryLocation(float X, float Y, float Z, float SPIN, float FALLOFF, String FileName){
		
		x = X;
		y = Y;
		z = Z;
		spin = SPIN;
		fileName = FileName;
		falloff = FALLOFF;
		

	}//end constructor
	

	@Override
	public void run() {
      SourceDataLine soundLine = null;
      int BUFFER_SIZE = 64*1024;  // 64 KB
   
      // Set up an audio input stream piped from the sound file.
      try {
         File soundFile = new File(fileName);
         AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(soundFile);
         AudioFormat audioFormat = audioInputStream.getFormat();
         DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);
         soundLine = (SourceDataLine) AudioSystem.getLine(info);
         soundLine.open(audioFormat);
         soundLine.start();
         int nBytesRead = 0;
         byte[] sampledData = new byte[BUFFER_SIZE];
         FloatControl volume= (FloatControl) soundLine.getControl(FloatControl.Type.MASTER_GAIN);
         FloatControl pan = (FloatControl) soundLine.getControl(FloatControl.Type.PAN);
         
         //Force a good value for pan value
         float pval = (x/CONSTANTS.panScale);
         if (pval > 1)
        	 {pval = 1;}
         else if (pval < -1)
        	 {pval = -1;}
         
         pan.setValue(pval);
         //System.out.println(audioFormat.toString());
         //System.out.println(volume.toString());
         
         
         
         while(stillrunning){
	         while (nBytesRead != -1 && stillrunning) {
	            nBytesRead = audioInputStream.read(sampledData, 0, sampledData.length);
	            if (nBytesRead >= 0) {
	               //vol = (float) (vol - 1);
	               if(vol > -80 & vol < 0){
	            	   volume.setValue(vol); //Set the volume
	               }
	               else if(vol < -80){vol = -80;}
	               else{vol = (float) 0.0;}
	               
	            	//System.out.println(volume.toString());
	               soundLine.write(sampledData, 0, nBytesRead); //Write the data to the stream
	            }//end if read bytes
	         }//End while more data
	         audioInputStream = AudioSystem.getAudioInputStream(soundFile); //Start the stream over
	         nBytesRead = 0;
         }//end for play while running
         
      } catch (UnsupportedAudioFileException ex) {
         ex.printStackTrace();
      } catch (IOException ex) {
         ex.printStackTrace();
      } catch (LineUnavailableException ex) {
         ex.printStackTrace();
      } finally {
         soundLine.drain();
         soundLine.close();
      }//end finally
   }//end run
}//end class