#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include "include/Client.h"
#include "include/Window.h"
#include "include/GLbarebones.h"
#include "include/Math/Vector4.h"

using namespace ViconDataStreamSDK::CPP;
using namespace std;

const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

// Make a new client
Client MyClient;

Window* win;
Viewer* v;
//-----------------------------------------------------------------------------
void Init(){
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);


	glClearColor(0.9,0.9,1.0,1.0);

}
//-----------------------------------------------------------------------------
void Draw(){
	if(MyClient.GetFrame().Result != Result::Success )
		printf(".");

	win->EvaluateViewers();

	for(int i = win->Viewers.size(); i--;){
		Viewer* v = win->Viewers[i];
		ViVec min = win->GetMin(), max = win->GetMax(), point = v->GetLookPos();

		ViVec diff = max - min;
		ViVec scaled = (point - min) / diff;
		ViVec screenPos = ViVec(-1, -1, 0) * scaled + ViVec(1, 1, 0) * (ViVec(1, 1, 0) - scaled);

	//--- RENDER ------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1,1,-1,1,-1,1);
		  
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// ball
		glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
		glTranslatef((GLfloat)screenPos.x, (GLfloat)screenPos.y, 0);
		// make ball round even though screen is stretched horizontally
		glScalef(1,screenAspectRatio,1);
		glutSolidSphere(0.01, 100, 100);
		glPopMatrix();
	}
}
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ){
	if(argc < 2){
		printf("Please provide the server's host name.\n");
		//exit(-1);
	}
	else{
		string hostName = argv[1];

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
		MyClient.EnableSegmentData();

// --- Set streaming mode, and axis orientation -------------------------------
		MyClient.SetStreamMode( StreamMode::ServerPush );
		
		MyClient.SetAxisMapping(
			Direction::Left, 
			Direction::Up, 
			Direction::Forward
		); // Y-up

		win = new Window(&MyClient, (string)"Plane");
		v = win->AddViewer("Dude");
// --- Recieve data here ------------------------------------------------------
		unsigned int oldMarkerCount = 0;
		int stateChanges = 0;			

		GLRenderContext creation = {
			SCREEN_WIDTH,                // window width
			SCREEN_HEIGHT,               // window height
			"Visualizer",                // window title
			argc,               // cmd line arg count
			argv,               // cmd line args
			// callback functions
			Init,
			Draw,
			NULL,
			NULL
		};

		printf("created!\n");
		GLRender_Init(creation);
	}
}
