#pragma once

#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GL/gl.h>

#include "Math/Matrix4.h"
#include "ShaderCompiler.h"

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

// Camera Matrices
static Matrix4 GLCam_View;
static Matrix4 GLCam_Proj;
static Matrix4 GLCam_Orientation;
static Vector4 CameraPosition;
static Vector4 CameraLook;
static Vector4 CameraDir;
static int _lastMouseX, _lastMouseY;
static float _yaw, _pitch;

// Line and quad vars
GLuint GLRender_LineShader;
GLuint GLRender_Quad;

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

void GLRenderInitQuad(){
	GLuint quadVerts;
	glGenVertexArrays(1, &quadVerts);
	glBindVertexArray(quadVerts);

	glGenBuffers(1, &GLRender_Quad);
	glBindBuffer(GL_ARRAY_BUFFER, GLRender_Quad);

	float size = 0.5f;

	GLfloat verts[] = {
		-size, -size, 0.0f,
		 size, -size, 0.0f,
		 size,  size, 0.0f, 
		-size,  size, 0.0f,
		-size, -size, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER,
		     15 * sizeof(GLfloat),
	             verts, GL_STATIC_DRAW );
}

void GLRender_BindLine(GLuint program, GLuint vertexBuffer, Matrix4 model){
			// make sure we are using the right buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); 

			// Initialize the vertex position attribute from the vertex shader
			GLuint vPosition = glGetAttribLocation( program, "aPosition" );
			glEnableVertexAttribArray(vPosition);
			glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

			GLuint uModel = glGetUniformLocation(program, "uModel");
			GLuint uView = glGetUniformLocation(program, "uView");
			GLuint uProj = glGetUniformLocation(program, "uProj");

			glUniformMatrix4fv(uModel, 1, GL_TRUE, (GLfloat*)model.ptr());
			glUniformMatrix4fv(uView, 1, GL_TRUE, (GLfloat*)GLCam_View.ptr());
			glUniformMatrix4fv(uProj, 1, GL_TRUE, (GLfloat*)GLCam_Proj.ptr());
}

void GLRender_CustomLine(GLfloat verts[6]){

	glBufferData(GL_ARRAY_BUFFER,
		     6 * sizeof(GLfloat),
	             verts, GL_STATIC_DRAW );
}

void GLRender_CustomQuad(GLfloat verts[15]){
	glBufferData(GL_ARRAY_BUFFER,
		     15 * sizeof(GLfloat),
	             verts, GL_STATIC_DRAW );
}

void GLRender_CustomQuadFilled(GLfloat verts[15]){
	float tris[18] = {
		verts[0], verts[1], verts[2],
		verts[3], verts[4], verts[5],
		verts[6], verts[7], verts[8],

		verts[6], verts[7], verts[8],
		verts[9], verts[10], verts[11],
		verts[0], verts[1], verts[2]
	};

	glBufferData(GL_ARRAY_BUFFER,
		     18 * sizeof(GLfloat),
	             tris, GL_STATIC_DRAW );
}

void GLRender_ResetQuad(){
	float size = 0.5f;

	GLfloat origVerts[] = {
		-size, -size, 0.0f,
		 size, -size, 0.0f,
		 size,  size, 0.0f, 
		-size,  size, 0.0f,
		-size, -size, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER,
		     15 * sizeof(GLfloat),
	             origVerts, GL_STATIC_DRAW );
}

void mouse(int x, int y){
	float dx = (x - _lastMouseX) / 100.0f;
	float dy = (_lastMouseY - y) / 100.0f;

	_yaw += dx;
	_pitch += dy;

	// avoid gimble lock
	if(_pitch > M_PI * 0.49f) _pitch = M_PI * 0.49f;
	if(_pitch < -M_PI * 0.49f) _pitch = -M_PI * 0.49f;

	GLCam_Orientation = Matrix4::RotateY(_yaw).Multiply(Matrix4::RotateX(_pitch));
	CameraDir = GLCam_Orientation.Multiply(Vector4(0, 0, 1, 1));
	//CameraLook = Vector4::add(CameraDir, CameraPosition);

	_lastMouseX = x; _lastMouseY = y;
}

void mouseMove(int button, int state, int x, int y){
    if(state == GLUT_DOWN){
	//cur_button = button;

		_lastMouseX = x;
		_lastMouseY = y;

    }
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
	int glew_err = glewInit();
	if(glew_err != GLEW_OK)
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));

	DrawCallback = ctx.DrawCallback;

	// register callback functions
	glutDisplayFunc(GLRenderDraw);
	glutKeyboardFunc(ctx.KeyboardCallback);
	glutCloseFunc(ctx.ExitCallback);

	GLRender_LineShader = GLRenderInitShader("./bin/Shaders/line.vert", "./bin/Shaders/line.frag");

	// set up the buffer to render simple quads
	GLRenderInitQuad();

	/*GLCam_View = Matrix4::CreateLook(
		Vector3(0, 0, -1.0f),//Vector3(0, 4, -12.0f),
		Vector3(0, 0 , 0),
		Vector3(0, 1, 0)
	);*/

	GLCam_Proj = Matrix4::CreatePerspective(
		90.0f,                         // fov
		ctx.Width / (float)ctx.Height, // aspect
		0.1f,                          // near
		1000.0f                        // far
	);
	glEnable(GL_DEPTH_TEST);

	// initialize mousemove stuff.
	glutMotionFunc(mouse);
	glutMouseFunc(mouseMove);
	CameraPosition = Vector4(6.0f, 6.0f, 6.0f, 1);
	/*glutMotionFunc(mousemotion);
	glutMouseFunc(mouse);
	setLookAt(0,0,0);    // initial point camera is looking at
	setPosition(5,5,5); // initial position of camera
	setUpAxis(0,1,0);    // camera up vector
	setTranslationFactor(.05);  // translation speed adjustment
	setRotationFactor(0.5);     // rotation speed adjustment
	*/

	if(ctx.InitCallback) ctx.InitCallback();

	glutMainLoop();	// hand over control to glut/opengl

	return 1;
}
