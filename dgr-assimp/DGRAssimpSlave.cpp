/*

  Copyright 2011 Etay Meiri

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.	If not, see <http://www.gnu.org/licenses/>.

  Tutorial 22 - Loading models using the Open Assert Import Library
*/

#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "util.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "mesh.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SEND_IP "255.255.255.255"  // broadcast address
#define BUFLEN 512
#define NPACK 10
#define PORT 25884

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*3);  
const GLdouble SCREEN_HEIGHT = 1080;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

float rotation = 0.0f;

static float FieldDepth = 10.0f;

// DGR vars
int s, milliseconds;
struct timespec req;
pthread_t receiverThread;
struct sockaddr_in si_me, si_other;
int slen;

double ortho_left;
double ortho_right;
double ortho_bottom;
double ortho_top;

bool receivedPacket = false;
int framesPassed = 0;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  return split(s, delim, elems);
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void closeProgram() {
  close(s);
  exit(0);
}

void receiver() {
  char buf[BUFLEN];
  vector<string> splits;
  while (true) {
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) == -1) error("ERROR recvfrom()");
    receivedPacket = true;
    framesPassed = 0;
    rotation = (float)atof(buf);
  }
}

class ModelLoad : public ICallbacks
{
  private:

	LightingTechnique* m_pEffect;
	Camera* m_pGameCamera;
	float m_scale;
	DirectionalLight m_directionalLight;
	Mesh* m_pMesh;
	char* m_modelFile;

  public:
	ModelLoad(char* modelFile)
	{
		m_modelFile = modelFile;
		m_pGameCamera = NULL;
		m_pEffect = NULL;
		m_scale = 0.0f;
		m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
		m_directionalLight.AmbientIntensity = 1.0f;
		m_directionalLight.DiffuseIntensity = 0.01f;
		m_directionalLight.Direction = Vector3f(1.0f, -1.0, 0.0);
	}

	~ModelLoad()
	{
		delete m_pEffect;
		delete m_pGameCamera;
		delete m_pMesh;
	}

	bool Init()
	{
		Vector3f Pos(3.0f, 7.0f, -10.0f);
		Vector3f Target(0.0f, -0.2f, 1.0f);
		Vector3f Up(0.0, 1.0f, 0.0f);

		m_pGameCamera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT, Pos, Target, Up);
	  
		m_pEffect = new LightingTechnique();

		if (!m_pEffect->Init()) {
			printf("Error initializing the lighting technique\n");
			return false;
		}

		m_pEffect->Enable();

		m_pEffect->SetTextureUnit(0);

		m_pMesh = new Mesh();

		if(m_modelFile == NULL)
			return m_pMesh->LoadMesh("/research/kuhl/vrlab/ivs//dgr-assimp/models/phoenix_ugv.md2");
		else
			return m_pMesh->LoadMesh(m_modelFile);

	}

	void Run()
	{
		GLUTBackendRun(this);
	}

	virtual void RenderSceneCB()
	{

  // auto close
  framesPassed++;
  if (receivedPacket) {
    if (framesPassed > 180) exit(0);
  } else {
    if (framesPassed > 900) exit(0);
  }

		m_pGameCamera->OnRender();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		PointLight pl[2];
		pl[0].DiffuseIntensity = 0.25f;
		pl[0].Color = Vector3f(1.0f, 0.5f, 0.0f);
		pl[0].Position = Vector3f(3.0f, 1.0f, FieldDepth * (cosf(rotation) + 1.0f) / 2.0f);
		pl[0].Attenuation.Linear = 0.1f;
		pl[1].DiffuseIntensity = 0.25f;
		pl[1].Color = Vector3f(0.0f, 0.5f, 1.0f);
		pl[1].Position = Vector3f(7.0f, 1.0f, FieldDepth * (sinf(rotation) + 1.0f) / 2.0f);
		pl[1].Attenuation.Linear = 0.1f;
		m_pEffect->SetPointLights(2, pl);

		SpotLight sl[2];
		sl[0].DiffuseIntensity = 0.9f;
		sl[0].Color = Vector3f(0.0f, 1.0f, 1.0f);
		sl[0].Position = m_pGameCamera->GetPos();
		sl[0].Direction = m_pGameCamera->GetTarget();
		sl[0].Attenuation.Linear = 0.1f;
		sl[0].Cutoff = 10.0f;

		sl[1].DiffuseIntensity = 0.75f;
		sl[1].Color = Vector3f(0.0f, 0.5f, 1.0f);
		sl[1].Position = Vector3f(7.0f, 1.0f, FieldDepth * (sinf(rotation) + 1.0f) / 2.0f);
		sl[1].Attenuation.Linear = 0.1f;
		m_pEffect->SetSpotLights(1, sl);

		Pipeline p;
		p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, rotation, 0.0f);
		p.WorldPos(0.0f, 0.0f, 10.0f);
		p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
		// p.SetPerspectiveProj(60.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f);

		GLdouble fW, fH;
		fH = tan(60.0f / 360.0 * M_PI) * 1.0;
		fW = fH * (SCREEN_WIDTH/(float)SCREEN_HEIGHT);
		p.SetFrustumProj(-fW, fW, -fH, fH, 1.0, 100);

		// p.SetFrustumProj(ortho_left, ortho_right, ortho_bottom, ortho_top, 0.8f, 500.0f);
		//		p.SetFrustumProj(-.1, .1, -.1, .1, 0.1f, 500.0f);
		m_pEffect->SetWVP(p.GetWVPTrans());
		m_pEffect->SetWorldMatrix(p.GetWorldTrans());
		m_pEffect->SetDirectionalLight(m_directionalLight);
		m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos()); /* */
		m_pEffect->SetMatSpecularIntensity(0.0f);
		m_pEffect->SetMatSpecularPower(0); /* */
		m_pMesh->Render();

		glutSwapBuffers();
		glutPostRedisplay(); // request redisplay
	}

	virtual void IdleCB()
	{

	}

	virtual void SpecialKeyboardCB(int Key, int x, int y)
	{
		m_pGameCamera->OnKeyboard(Key);
	}


	virtual void KeyboardCB(unsigned char Key, int x, int y)
	{
		switch (Key) {
			case 'q':
			case 27:
				printf("exiting...\n");
				exit(0);
				break;

			case 'a':
				m_directionalLight.AmbientIntensity += 0.05f;
				break;

			case 's':
				m_directionalLight.AmbientIntensity -= 0.05f;
				break;

			case 'z':
				m_directionalLight.DiffuseIntensity += 0.05f;
				break;

			case 'x':
				m_directionalLight.DiffuseIntensity -= 0.05f;
				break;
		}
	}
  

	virtual void PassiveMouseCB(int x, int y)
	{
		m_pGameCamera->OnMouse(x, y);
	}

};


int main(int argc, char** argv)
{
  if (argc < 5) {
    printf("USAGE: DGRAssimpSlave left right bottom top\n");
    return 1;
  }

  ortho_left = atof(argv[1]); //-5.0;
  ortho_right = atof(argv[2]); //0.0;
  ortho_bottom = atof(argv[3]); //-5.0;
  ortho_top = atof(argv[4]); //5.0;

	GLUTBackendInit(argc, argv);

	if (!GLUTBackendCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 32, false, argv[0])) {
		return 1;
	}

	ModelLoad* pApp;
	pApp = new ModelLoad(NULL);

	if (!pApp->Init()) {
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

  // socket stuff
  slen=sizeof(si_other);
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) error("ERROR bind");

  // listen for updates
  if (pthread_create(&receiverThread, NULL, receiver, NULL) != 0) {
    perror("Can't start thread, terminating");
    return 1;
  }

	pApp->Run();

	delete pApp;
 
	return 0;
}
