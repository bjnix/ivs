#include "header.h"

#include <GL/glew.h>
#include <GL/glut.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "STLCommon.h"

STLCommon *common;

struct sharedStruct
{
	float x,y,z;
};
struct sharedStruct sharedData;




void display ( void ){

#ifndef STL_MASTER
	common->getSharedData(&sharedData, sizeof(struct sharedStruct));
#endif
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0,0,10,
			  0,0,0,
			  0,1,0);
	glColor3f(1,.2,.1);
	glutSolidTeapot(1);
	glPopMatrix();

	glutSwapBuffers();
	//glFlush();

	
}

void reshape (int w, int h){
	glViewport(0,0,(GLint) w, (GLint)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

#ifdef STL_MASTER
void keyboard ( unsigned char key, int x, int y){
	switch (key){
		case 27:
			common->setSharedData(&sharedData, sizeof(struct sharedStruct));
			exit(EXIT_SUCCESS);
			break;
	}
}

void processMouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		if(button == GLUT_LEFT_BUTTON)
		{
		}
		else if(button == GLUT_MIDDLE_BUTTON)
		{
		}
		else if(button == GLUT_RIGHT_BUTTON)
		{
		}
	}
	else if(state == GLUT_UP)
	{
	}
	glutPostRedisplay();
}

void mousePressMove(int x, int y){

	glutPostRedisplay();
}

void mouseMove(int x, int y){
}
#endif

void animate()
{
	glutPostRedisplay();
}






int main(int argc, char** argv){

#ifdef STL_MASTER
	common = new STLCommon(true, argc, argv);
	
	glutKeyboardFunc(keyboard);
	glutMouseFunc(processMouse);
	glutMotionFunc(mousePressMove);
	glutPassiveMotionFunc(mouseMove);

	// initialize shared data.
	sharedData.x = 0;
	common->setSharedData(&sharedData, sizeof(struct sharedStruct));
	
#else
	common = new STLCommon(false, argc, argv);
#endif

	if (glewInit() != GLEW_OK)
	{
		printf("errer glew init\n");
		return -1;
	}


	glClearColor(1.0,1.0,1.0, 1);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(animate);
	
	
	// go
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
