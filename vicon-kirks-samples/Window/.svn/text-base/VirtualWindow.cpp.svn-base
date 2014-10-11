//#define DEBUGGING_GL
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

#define CUBE_MATRIX_ROW 4

// Make a new client
Client MyClient;

Window* win;
Viewer* v;
//-----------------------------------------------------------------------------
void Init(){
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0.1,0.1,1.0,1.0);

}
//-----------------------------------------------------------------------------
void Draw(){

	double cubeSize = 5.0;
	ViVec CubePositions[64];
	ViVec CamPosition(0.0, 0.0, 0.0);
	ViVec CamLookPos = CubePositions[0];
	ViVec CamUp(0.0, 1.0, 0.0);

	int cubes = CUBE_MATRIX_ROW; cubes *= cubes * cubes;
	int side = (int)ceil(pow(cubes, 1.0/3.0));
	int square = side * side;

	for(int i = cubes; i--; CubePositions[i] = ViVec(0.0, 0.0, 0.0));

#ifndef DEBUGGING_GL
	if(MyClient.GetFrame().Result != Result::Success )
		printf(".");

	win->EvaluateViewers();

	for(int i = win->Viewers.size(); i--;){

		Viewer* v = win->Viewers[i];
		ViVec min = win->GetMin(), max = win->GetMax(), point = v->GetLiteralLookPos();
		ViVec into = -win->GetNormal(), up = win->GetUp(), c = win->GetCenter();
		ViVec diff = max - min;

		// arrange the cubes in a 3d matrix thing
		double d = 6000.0 / (double)(side-1);
		for(int j = cubes; j--;){
			int x = j % side, y = (j % square) / side, z = j / square;

			ViVec offset = ViVec(
				-3000.0 + d * x,
				-3000.0 + d * y,
				d * z
			);
			CubePositions[j] = c + into * 500.0 + offset; // move the cube into the screen 2000mm
		}

		CamLookPos = c + into * 8000.0;  // look to the center of the cubes in the back row
		CamPosition = v->Position;
		cubeSize = 750.0;
#endif

	//--- RENDER ------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.0, screenAspectRatio, 100.0, 12750.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			CamPosition.x, CamPosition.y, CamPosition.z,
			CamLookPos.x, CamLookPos.y, CamLookPos.z,
			CamUp.x, CamUp.y, CamUp.z
		);

		// render all cubes
		for(int j = cubes; j --;){
			// cube
			glEnable(GL_LIGHTING);
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();
			glTranslatef(CubePositions[j].x, CubePositions[j].y, CubePositions[j].z);
			glutSolidCube(cubeSize);
			glPopMatrix();
		}

#ifndef DEBUGGING_GL
		break; // we only want to render for the first viewer
	}
#endif
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

#ifndef DEBUGGING_GL
		win = new Window(&MyClient, (string)"Plane");
		v = win->AddViewer("Dude");
#endif
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
