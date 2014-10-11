#include "Client.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <stdlib.h>
#include <time.h>

#include "OPaudio/OPaudio.h"
#include "OPaudio/OPsoundEmitter.h"


using namespace ViconDataStreamSDK::CPP;
using namespace std;


int main( int argc, char* argv[] ){
	if(argc < 2){
		printf("Please provide the server's host name.\n");
		exit(-1);
	}
	else{
		string hostName = argv[1];

		// Make a new client
		Client MyClient;

// --- Connect to the system --------------------------------------------------
		for(int i=0; i != 3; ++i){ // repeat to check disconnecting doesn't wreck next connect
	
			// Connect to a server
			cout << "Connecting to " << hostName << "...\n" << flush;
			while( !MyClient.IsConnected().Connected ){
				// Direct connection
				bool ok = ( MyClient.Connect( hostName ).Result == Result::Success );
				if(!ok) cout << "Warning - connect failed..." << endl;

				// wait for a second
				sleep(1);
			}
		}
// --- Enable request only marker data ----------------------------------------
		MyClient.EnableMarkerData();

// --- Set streaming mode, and axis orientation -------------------------------
		MyClient.SetStreamMode( StreamMode::ServerPush );
		
		MyClient.SetAxisMapping(
			Direction::Left, 
			Direction::Up, 
			Direction::Forward
		); // Y-up

// --- Recieve data here ------------------------------------------------------
		unsigned int oldMarkerCount = 0;
		int stateChanges = 0;			
		while(1){
			while( MyClient.GetFrame().Result != Result::Success ){
				// Sleep a little so that we don't lumber the CPU with a busy poll
				sleep(1);

				cout << ".";
			}
			int subjects = MyClient.GetSubjectCount().SubjectCount;
			if(subjects){
				string SubjectName = MyClient.GetSubjectName(0).SubjectName;		
				int MarkerCount = MyClient.GetMarkerCount( SubjectName ).MarkerCount;
				int visible = 0;
	
				for(int i = MarkerCount; i--;){
					string markerName = MyClient.GetMarkerName( SubjectName, i ).MarkerName;
          				Output_GetMarkerGlobalTranslation posData = 
            				MyClient.GetMarkerGlobalTranslation( SubjectName, markerName );

					if(!posData.Occluded){
						++visible;
					}
				}				

				if(visible != oldMarkerCount)
					cout << stateChanges++ << " " << SubjectName << " - Markers: " << visible << endl;

				oldMarkerCount = visible;
			}
		}
	}
}
