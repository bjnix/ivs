#ifndef HEADER_H
#define HEADER_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <math.h>
#include <list>
#include "jitter.h"
#include "trackball.c"
using namespace std;

// Master will sent UDP packets to a relay which listens on this port
#define RELAY_LISTEN_PORT 25885
#define SLAVE_LISTEN_PORT 25884
#define BUFLEN 512

static string headerPath = "./data/3D/5cp/5cp.hdr";
static string dataPath = "./data/3D/5cp/5cp.vec";
static string STLPath = "./data/viewindependent/5cp/viewindependent/AllStreamLines.dat";
static string parPath = "./data/3D/5cp/5cp.par";

list<streamLine> streamlineset;
int totallines;
int vfwidth;
int vfheight;
int vfdepth;
int maxdimension;

float tranx;
float trany;
float tranz;


bool displayBounding;
bool displayEntropy;
bool displaytube;

bool inRotation;

float angle;

int segmentnum;

float myxv;
float myyv;
float myzv;
float last_x;
float last_y;
float rotationX;
float rotationY;

float modelviewMatrix[16];
float projectionMatrix[16];
float *vertexarrayVBO;
float *colorarrayVBO;
float *normalarrayVBO;
float *totalarrayVBO;
int *indexVBO;
int *indices;
int sides;
int segnum;
int totalsegments;

bool smooth;
float eyeradius;
int totalseg;
float radius;
GLuint vboId1, vboId2;
bool useVBO;
int *segarray;

float Wd;
float Ht;
int m_mousex;
int m_mousey;
float m_curquat[4];

float light0_color[4];
float light0_position[4];
float light0_direction[3];
float light0_diffuse[4];
float light0_ambient[4];
float light0_specular[4];

float exponent;
float conatt;
float linatt;
float quaatt ;
float mat_shine;
float cutoff;

float ambient_r;
float ambient_g;
float ambient_b;
float diffuse_r;
float diffuse_g;
float diffuse_b;
float specular_r;
float specular_g;
float specular_b;

float mat_ambient[4];
float mat_ambient2[4];
float mat_diffuse[4];
float mat_specular[4];

#endif // HEADER_H
