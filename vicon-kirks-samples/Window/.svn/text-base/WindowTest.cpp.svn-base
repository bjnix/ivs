#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include "include/Client.h"
#include "include/Window.h"
#include "include/GLRenderer.h"
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
ViVec CamPos;

int keys[256];

//#define DEBUGGING_GL

void KeyDown(unsigned char key, int x, int y){
  keys[key] = true;
}
void KeyUp(unsigned char key, int x, int y){
  keys[key] = false;
}

void Keyboard(){

  float delta = 0.1f;

  if(keys['w']){ CamPos += ViVec(0, 0, delta); }
  if(keys['s']){ CamPos -= ViVec(0, 0, delta); }
  if(keys['a']){ CamPos -= ViVec(delta, 0, 0); }
  if(keys['d']){ CamPos += ViVec(delta, 0, 0); }
  if(keys['e']){ CamPos += ViVec(0, delta, 0); }
  if(keys['q']){ CamPos -= ViVec(0, delta, 0); }
}

void Draw(){
	float scl = 0.001f;
	Keyboard();

#ifndef DEBUGGING_GL
	while( MyClient.GetFrame().Result != Result::Success ){
		// Sleep a little so that we don't lumber the CPU with a busy poll
		sleep(1);
	}

	//win->RecalculateWindow("Plane");

	// update all the viewers
	win->EvaluateViewers();

	ViVec max = win->GetMax() / 1000;	
	ViVec min = win->GetMin() / 1000;
	
	ViVec* cnr = win->GetCorners();

	GLfloat line[4][6] = {
		{(GLfloat)cnr[0].x * scl, (GLfloat)cnr[0].y * scl, (GLfloat)cnr[0].z * scl,
		(GLfloat)cnr[1].x * scl, (GLfloat)cnr[1].y * scl, (GLfloat)cnr[1].z * scl},

		{(GLfloat)cnr[1].x * scl, (GLfloat)cnr[1].y * scl, (GLfloat)cnr[1].z * scl,
		(GLfloat)cnr[2].x * scl, (GLfloat)cnr[2].y * scl, (GLfloat)cnr[2].z * scl},

		{(GLfloat)cnr[2].x * scl, (GLfloat)cnr[2].y * scl, (GLfloat)cnr[2].z * scl,
		(GLfloat)cnr[3].x * scl, (GLfloat)cnr[3].y * scl, (GLfloat)cnr[3].z * scl},

		{(GLfloat)cnr[3].x * scl, (GLfloat)cnr[3].y * scl, (GLfloat)cnr[3].z * scl,
		(GLfloat)cnr[0].x * scl, (GLfloat)cnr[0].y * scl, (GLfloat)cnr[0].z * scl}
	};

#else
	ViVec min = ViVec(1);	
	ViVec max = ViVec(-1);

	GLfloat line[4][6] = {
		{(GLfloat)min.x, (GLfloat)min.y, (GLfloat)min.z,
		(GLfloat)max.x, (GLfloat)max.y, (GLfloat)max.z},

		{(GLfloat)min.x, (GLfloat)min.y, (GLfloat)min.z,
		(GLfloat)max.x, (GLfloat)max.y, (GLfloat)max.z},

		{(GLfloat)min.x, (GLfloat)min.y, (GLfloat)min.z,
		(GLfloat)max.x, (GLfloat)max.y, (GLfloat)max.z},

		{(GLfloat)min.x, (GLfloat)min.y, (GLfloat)min.z,
		(GLfloat)max.x, (GLfloat)max.y, (GLfloat)max.z}
	};
#endif

	// DRAW THE PLANE
	ViVec c = win->GetCenter() * scl;
	ViVec n = win->GetNormal();

	GLfloat screenNormal[6] = {
			(GLfloat)c.x, (GLfloat)c.y, (GLfloat)c.z,
			(GLfloat)c.x + n.x, (GLfloat)c.y + n.y, (GLfloat)c.z + n.z
	};

	CameraLook = Vector4(c.x, c.y, c.z, 1);

	GLCam_View = Matrix4::CreateLook(
		Vector3(CamPos[0], CamPos[1], CamPos[2]),//Vector3(0, 4, -12.0f),
		Vector3(CameraLook[0], CameraLook[1], CameraLook[2]),
		Vector3(0, 1, 0)
	);

	Matrix4 ident;
	GLRender_BindLine(GLRender_LineShader, GLRender_Quad, ident);

	//std::cout << Max << " " << Min << "\n";
	for(int i = 4; i--;){
		GLRender_CustomLine(line[i]);
		glDrawArrays(GL_LINES, 0, 2);
	}

	GLRender_CustomLine(screenNormal);
	glDrawArrays(GL_LINES, 0, 2);

	// DRAW THE VIEWERS
	for(int i = win->Viewers.size(); i--;){
		Viewer* v = win->Viewers[i];

		ViVec p = v->Position * scl, d = p + v->LookDirection;
		ViVec li = v->GetLookPos() * scl;

		GLfloat line[3][6] = {
			// up
			{(GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z,
			(GLfloat)0 + p.x, (GLfloat)0.5 + p.y, (GLfloat)0 + p.z},

			// forward
			{(GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z,
			(GLfloat)d.x, (GLfloat)d.y, (GLfloat)d.z},

			// intersect
			{(GLfloat)p.x, (GLfloat)p.y, (GLfloat)p.z,
			(GLfloat)li.x, (GLfloat)li.y, (GLfloat)li.z},
		};

		for(int j = 3; j--;){
			GLRender_CustomLine(line[j]);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
}

void Init(){
	glClearColor(0.1f, 0.2f, 0.7f, 1.0f);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
}

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
		cout << "Connected!\n";
		
// --- Enable request only marker data ----------------------------------------
		MyClient.EnableMarkerData();

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
		/*while(1){
			while( MyClient.GetFrame().Result != Result::Success ){
				// Sleep a little so that we don't lumber the CPU with a busy poll
				sleep(1);
			}

			//win->RecalculateWindow("Plane");

			// update all the viewers
			win->EvaluateViewers();

			if(v->LookingAtScreen){
				std::cout << "Looking at screen!\n";
			}
		}*/
	}

	GLRenderContext creation = {
		800,                // window width
		600,                // window height
		"Visualizer", // window title
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
