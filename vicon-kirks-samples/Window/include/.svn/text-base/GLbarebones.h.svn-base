#pragma once

#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GL/gl.h>

#include "Math/Matrix4.h"

/*--- Structs----------------------------------------------------------------*/
typedef struct{
	int Width;
	int Height;
	const char* Title;
	int argv;
	char** argc;
	void (*InitCallback)();
	void (*DrawCallback)();
	void (*KeyboardCallback)(unsigned char key, int x, int y);
	void (*ExitCallback)();
} GLRenderContext;

/*--- Globals ---------------------------------------------------------------*/
// user rendering callback
void (*DrawCallback)();

/*--- Function definitions --------------------------------------------------*/
void GLRenderDraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*GLCam_View = Matrix4::CreateLook(
		Vector3(CameraPosition[0], CameraPosition[1], CameraPosition[2]),//Vector3(0, 4, -12.0f),
		Vector3(CameraLook[0], CameraLook[1], CameraLook[2]),
		Vector3(0, 1, 0)
	);*/

	if(DrawCallback) DrawCallback();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}


int GLRender_Init(GLRenderContext ctx){
	printf("glut initializing...");

	// initialize glut, create our window
	glutInit(&ctx.argv, ctx.argc);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(ctx.Width, ctx.Height);
	glutCreateWindow(ctx.Title);

	printf("initialized!\n");

	// initialize glew:
	/*int glew_err = glewInit();
	if(glew_err != GLEW_OK)
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));
*/
	DrawCallback = ctx.DrawCallback;

	// register callback functions
	glutDisplayFunc(GLRenderDraw);
	if(ctx.KeyboardCallback) glutKeyboardFunc(ctx.KeyboardCallback);
	if(ctx.ExitCallback) glutCloseFunc(ctx.ExitCallback);

	if(ctx.InitCallback) ctx.InitCallback();

	printf("Callbacks set succesfully, starting main loop.\n");

	glutMainLoop();	// hand over control to glut/opengl

	return 1;
}
