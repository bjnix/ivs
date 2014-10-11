#include "MyType.h"
#include "header.h"
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

#ifdef STL_MASTER
char *RELAY_IP = NULL;

struct sockaddr_in si_other;
int slen, s;
char buf [BUFLEN];
int so_broadcast = 1;
pthread_t senderThread;
#else // if SLAVE:
const float MYPI = 3.14159f;
int s, milliseconds;
struct timespec req;
pthread_t receiverThread;

struct sockaddr_in si_me, si_other;
int slen;

// command-line parameters
double ortho_left,ortho_right,ortho_bottom,ortho_top;
int screen_width,screen_height;
#endif

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


// We use atexit() to register this function as a callback for
// whenever our program exits. Therefore, anytime exit() is called, we
// send a kill signal appropriately.
void exitCallback()
{
#ifdef STL_MASTER
    tranx = trany = tranz = -1000000; // kill signal
	sprintf(buf, "%f~%f~%f~%f~%f~%f~%f~%f", tranx, trany, tranz, angle,
	    m_curquat[0], m_curquat[1], m_curquat[2], m_curquat[3]);
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other,
        slen) == -1) error ("ERROR sendto()");
#else // if SLAVE:
	close(s);
#endif
}

GLuint  createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
    GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

    glGenBuffersARB(1, &id);                        // create a vbo
    glBindBufferARB(target, id);                    // activate vbo id to use
    glBufferDataARB(target, dataSize, data, usage); // upload data to video card


    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE_ARB, &bufferSize);
    if(dataSize != bufferSize)
    {
        glDeleteBuffersARB(1, &id);
        id = 0;
        printf("[createVBO()] Data size is mismatch with input array\n");
    }

    return id;      // return VBO id
}

void init ( ){
    //	getGLExtensionFunctions().resolve(this->context());
    glShadeModel(GL_SMOOTH);
    glClearColor(1.0,1.0,1.0, 1);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#ifdef STL_MASTER
    trackball(m_curquat, 0.0, 0.0, 0.0, 0.0);
#endif
    if (glewInit() != GLEW_OK)
    {
        printf("errer glew init\n");
    }

    if (!GLEW_ARB_vertex_buffer_object)
    {
        printf( "VBO not supported\n");
    }

    if(useVBO){
        int bufferSize = 0;
        glGenBuffersARB(1, &vboId1);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId1);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*totalseg*3*2*(sides+1)*3, 0, GL_STATIC_DRAW_ARB);
        glGetBufferParameteriv(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
        //	printf("size: %d\n", bufferSize);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(float)*totalseg*3*2*(sides+1), vertexarrayVBO);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*totalseg*3*2*(sides+1), sizeof(float)*totalseg*3*2*(sides+1), colorarrayVBO);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*totalseg*3*2*(sides+1)*2, sizeof(float)*totalseg*3*2*(sides+1), normalarrayVBO);
        delete []vertexarrayVBO;
        delete []colorarrayVBO;
        delete []normalarrayVBO;

        // create VBO for index array
        // Target of this VBO is GL_ELEMENT_ARRAY_BUFFER_ARB and usage is GL_STATIC_DRAW_ARB
        vboId2 = createVBO(indexVBO, sizeof(int)*totalseg*2*(sides+1), GL_ELEMENT_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);
        delete []indexVBO;
    }
#ifndef STL_MASTER
	milliseconds = 16;
	req.tv_sec = 0;
	req.tv_nsec = milliseconds * 1000000L;
#endif
}
void accFrustum(GLdouble myleft, GLdouble myright, GLdouble mybottom, GLdouble mytop, GLdouble mynear, GLdouble myfar, GLdouble mypixdx, GLdouble mypixdy, GLdouble myeyedx, GLdouble myeyedy, GLdouble myfocus){
    GLdouble xwsize, ywsize;
    GLdouble dx, dy;
    GLint viewport[4];

    glGetIntegerv (GL_VIEWPORT, viewport);

    xwsize = myright - myleft;
    ywsize = mytop - mybottom;
    dx = -((mypixdx*xwsize)/(GLdouble)viewport[2] + (myeyedx*mynear)/myfocus);
    dy = -(mypixdy*ywsize/(GLdouble) viewport[3] + myeyedy*mynear/myfocus);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (myleft + dx, myright + dx, mybottom + dy, mytop + dy, mynear, myfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (-myeyedx, -myeyedy, 0.0);
    glFrustum(myleft, myright, mybottom, mytop, mynear, myfar);
}

void accPerspective(GLdouble myfovy, GLdouble myaspect,
                    GLdouble mynear, GLdouble myfar, GLdouble mypixdx, GLdouble mypixdy,
                    GLdouble myeyedx, GLdouble myeyedy, GLdouble myfocus)
{
    GLdouble fov2,myleft,myright,mybottom,mytop;
    fov2 = ((myfovy*PI_) / 180.0) / 2.0;

    mytop = mynear/(cos(fov2)/sin(fov2));
    mybottom = -mytop;
    myright = mytop * myaspect;
    myleft = -myright;
#ifdef STL_MASTER
    accFrustum (myleft, myright, mybottom, mytop, mynear, myfar, mypixdx, mypixdy, myeyedx, myeyedy, myfocus);
#else
	accFrustum (ortho_left, ortho_right, ortho_bottom, ortho_top, mynear, myfar, mypixdx, mypixdy, myeyedx, myeyedy, myfocus);
#endif
}
void setLight(){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, conatt);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linatt);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quaatt);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shine);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glEnable(GL_COLOR_MATERIAL);
}

void myDisplay(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    setLight();
    float m[4][4];
    gluLookAt(myxv, myyv, eyeradius + myzv, 0, 0, 0, 0, 1, 0);

    build_rotmatrix(m, m_curquat);
    glMultMatrixf(&(m[0][0]));

    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH);

    glRotatef( angle, 0.0, 1.0, 0.0);

    glTranslatef(tranx, trany, tranz);

    glPushMatrix();
    glPushAttrib(GL_CURRENT_BIT);
    glTranslatef(-(vfwidth-1)/2.0, -(vfheight-1)/2.0, -(vfdepth-1)/2.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);

    if(displayBounding){
        glLineWidth(1.5);
        glColor3f(0.0,0.0,0.0);

        glBegin(GL_LINES);
        glVertex3f(0.0, vfheight-1, vfdepth-1);
        glVertex3f(vfwidth-1, vfheight-1, vfdepth-1);
        glVertex3f(vfwidth-1, vfheight-1, vfdepth-1);
        glVertex3f(vfwidth-1, 0.0, vfdepth-1);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(vfwidth-1, 0.0, vfdepth-1);
        glVertex3f(0.0, 0.0, vfdepth-1);
        glColor3d(0.0,1.0,0.0);
        glVertex3f(0.0, 0.0, vfdepth-1);
        glVertex3f(0.0, vfheight-1, vfdepth-1);

        glColor3d(0.0,0.0,0.0);
        glVertex3f(0.0,vfheight-1, 0.0);
        glVertex3f(vfwidth-1, vfheight-1, 0.0);
        glVertex3f(vfwidth-1, vfheight-1, 0.0);
        glVertex3f(vfwidth-1, 0.0, 0.0);
        glVertex3f(vfwidth-1, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, vfheight-1, 0.0);

        glVertex3f(0.0, vfheight-1, vfdepth-1);
        glVertex3f(0.0, vfheight-1, 0.0);
        glColor3d(0.0,0.0,1.0);
        glVertex3f(0.0, 0.0, vfdepth-1);
        glVertex3f(0.0, 0.0, 0.0);
        glColor3d(0.0,0.0,0.0);
        glVertex3f(vfwidth-1, 0.0, vfdepth-1);
        glVertex3f(vfwidth-1, 0.0, 0.0);
        glVertex3f(vfwidth-1, vfheight-1, vfdepth-1);
        glVertex3f(vfwidth-1, vfheight-1, 0.0);
        glEnd();
    }

    if(displayEntropy){
        glEnable(GL_LIGHTING);
        if(displaytube){
            if(useVBO){
                glEnableClientState(GL_NORMAL_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);
                // // bind VBOs with IDs and set the buffer offsets of the bound VBOs
                //// When buffer object is bound with its ID, all pointers in gl*Pointer()
                //// are treated as offset instead of real pointer.
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId1);

                // before draw, specify vertex and index arrays with their offsets
                glVertexPointer(3, GL_FLOAT, 0, 0);
                glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(float)*totalseg*3*2*(sides+1)));
                glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(float)*totalseg*3*2*(sides+1)*2));

                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboId2);
                glIndexPointer(GL_UNSIGNED_INT, 0, 0);

                //use beststreamlineset
                list<streamLine>::iterator s;
                int ind;
                for(s = streamlineset.begin(); s != streamlineset.end(); s++){
                    //find index arry offset first for each stl
                    ind = 0;
                    for(int j = 0; j < (*s).id; j++){
                        ind += segarray[j]*2*(sides+1);
                    }

                    //draw cylinder for this streamline
                    for(int j = 0; j <(*s).totalsegments; j++){
                        glDrawElements(GL_QUAD_STRIP, (sides+1)*2, GL_UNSIGNED_INT, (GLuint*)0 + ind + j*2*(sides+1));
                    }
                }

                glDisableClientState(GL_NORMAL_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_VERTEX_ARRAY);

                glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

            }
            else{
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, 0, normalarrayVBO);
                glVertexPointer(3, GL_FLOAT, 0, vertexarrayVBO);
                glColorPointer(3, GL_FLOAT, 0, colorarrayVBO);
                for(int i = 0; i < totalseg; i++)
                    glDrawElements(GL_QUAD_STRIP, (sides+1)*2, GL_UNSIGNED_INT, indexVBO + i*(2*(sides+1)));
            }

        }
        else{
            list<streamLine>::iterator s;
            for(s = streamlineset.begin(); s != streamlineset.end(); s++){
                glBegin(GL_LINE_STRIP);
                glColor4fv(red);
                list<vector3d2>::iterator p;
                for(p = (*s).pointslist.begin(); p!= (*s).pointslist.end(); p++){
                    glVertex3f((*p).vect.x,(*p).vect.y,(*p).vect.z);
                }
                glEnd();
            }
        }
    }
    glPopAttrib();
    glPopMatrix();
}

void display ( void ){
    if(smooth){
        GLint viewport[4];
        int jitter;
        glGetIntegerv (GL_VIEWPORT, viewport);
        glClear(GL_ACCUM_BUFFER_BIT);
        for (jitter = 0; jitter < ACSIZE; jitter++) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            accPerspective (50.0,
                            (GLdouble) viewport[2]/(GLdouble) viewport[3],
                            0.1, 5000, j8[jitter].x, j8[jitter].y,
                            0.0, 0.0, 1.0);
            myDisplay();
            glAccum(GL_ACCUM, 1.0/ACSIZE);
        }
        glAccum (GL_RETURN, 1.0);
    }
    else
        myDisplay();

    glutSwapBuffers();
    //glFlush();

}

void reshape (int w, int h){
    Wd = w;
    Ht = h;
    glViewport(0,0,(GLint) w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100000);
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

#ifdef STL_MASTER
void keyboard ( unsigned char key, int x, int y){
    switch (key){
		case 27:
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
            last_x = (float)x;
            last_y = (float)y;
            inRotation = true;
        }
        else if(button == GLUT_MIDDLE_BUTTON)
        {
            //middle button code
        }
        else if(button == GLUT_RIGHT_BUTTON)
        {
            //right button code
        }
    }
    else if(state == GLUT_UP)
    {
        inRotation = false;
    }
    glutPostRedisplay();
}

void mousePressMove(int x, int y){
    if(inRotation){
        if(last_x != (float)x || last_y != (float)y)
        {
			float lastquat[4];
            trackball(lastquat,
                      (2.0*last_x - Wd) / Wd,
                      (Ht - 2.0*last_y) / Ht,
                      (2.0*x - Wd) / Wd,
                      (Ht - 2.0*y) / Ht);

            add_quats(lastquat, m_curquat, m_curquat);
            last_x = (float)x;
            last_y = (float)y;
        }
    }
    glutPostRedisplay();
}

void mouseMove(int x, int y){

}
#endif

void animate(){
#if STL_MASTER
    angle+=0.5;
    if(angle>=360.0)
        angle=0.0;
#endif
    glutPostRedisplay();
}

void readHeader(){
    ifstream inf;
    inf.open(headerPath.c_str());
    if(inf.is_open()){
        printf("Open header successfully!\n");
        inf>>vfwidth>>vfheight>>vfdepth;

        printf("width:%d, height:%d, depth: %d\n", vfwidth, vfheight, vfdepth);
    }
    else
        printf("Open header failed!\n");
    inf.close();
}

void readPar(){
    int tempi;
    float tempf;
    ifstream inp;
    inp.open(parPath.c_str(),ios::binary);
    if(inp.is_open()){
        printf("Open par successfully!\n");
        inp.read(reinterpret_cast<char *>(&tempf), sizeof(float));
        inp.read(reinterpret_cast<char *>(&tempf), sizeof(float));
        inp.read(reinterpret_cast<char *>(&tempf), sizeof(float));
        inp.read(reinterpret_cast<char *>(&radius), sizeof(float));
        inp.read(reinterpret_cast<char *>(&tempi), sizeof(int));
        inp.read(reinterpret_cast<char *>(&tempi), sizeof(int));
        inp.read(reinterpret_cast<char *>(&eyeradius), sizeof(float));
        inp.close();

        printf("radius: %f, eyeradius: %f\n", radius, eyeradius);
        maxdimension = sqrt((float)((vfdepth/2)*(vfdepth/2)+(vfwidth/2)*(vfwidth/2)+(vfheight/2)*(vfheight/2)));
    }
    else
        printf("Open par failed!\n");
    inp.close();
}

void initData(){
    for(int i =0; i< 4; i++){
        light0_color[i] =  1.0;
    }

    light0_position[0] = 10000.0;
    light0_position[1] = 10000.0;
    light0_position[2] = 10000.0;
    light0_position[3] = 1.0;

    light0_direction[0] = -1.0;
    light0_direction[1] = -1.0;
    light0_direction[2] = -1.0;

    exponent = 0.0;
    conatt = 1.0;
    linatt = 0.0;
    quaatt = 0.0;
    mat_shine = 4;
    cutoff = 360;

    ambient_r = 0.5;
    ambient_g = 0.5;
    ambient_b = 0.5;
    diffuse_r = 0.5;
    diffuse_g = 0.5;
    diffuse_b = 0.5;
    specular_r = 0.6;
    specular_g = 0.6;
    specular_b = 0.6;

    mat_ambient[0] = 0.5;
    mat_ambient[1] = 0.5;
    mat_ambient[2] = 0.5;
    mat_ambient[3] = 1.0;

    mat_diffuse[0] = 0.5;
    mat_diffuse[1] = 0.5;
    mat_diffuse[2] = 0.5;
    mat_diffuse[3] = 1.0;

    mat_specular[0] = 0.6;
    mat_specular[1] = 0.6;
    mat_specular[2] = 0.6;
    mat_specular[3] = 1.0;

    light0_ambient[0] = ambient_r;
    light0_ambient[1] = ambient_g;
    light0_ambient[2] = ambient_b;
    light0_ambient[3] = 1.0;

    light0_diffuse[0] = diffuse_r;
    light0_diffuse[1] = diffuse_g;
    light0_diffuse[2] = diffuse_b;
    light0_diffuse[3] = 1.0;

    light0_specular[0] = specular_r;
    light0_specular[1] = specular_g;
    light0_specular[2] = specular_b;
    light0_specular[3] = 1.0;

    tranx = 0.0;
    trany = 0.0;
    tranz = 0.0;

    rotationX = 0;
    rotationY = 0;

    displayBounding = true;
    displayEntropy = true;
    displaytube = true;
    inRotation = false;
    smooth = true;
    useVBO = true;

    myxv = 0;
    myyv = 0;
    myzv = 0;

    segnum = 0;

    angle = 0.0;
}

void loadVIData(){
    streamlineset.clear();
    int offset = 0;		//used to find the offset of the first point of each stl in the total pointlist
    totalseg = 0;
    ifstream infvi;
    infvi.open(STLPath.c_str(),ios::binary);
    if(infvi.is_open()){
        printf("Open VIdata successfully!\n");
        int *inttp1 = new int;
        infvi.read(reinterpret_cast<char *>(inttp1), sizeof(int));	//read the totallines
        totallines = *inttp1;
        printf("totallines:%d\n",totallines);
        infvi.read(reinterpret_cast<char *>(inttp1), sizeof(int));		//read sides
        sides = *inttp1;
        infvi.read(reinterpret_cast<char *>(inttp1), sizeof(int));		//read # of totalpoints
        infvi.read(reinterpret_cast<char *>(inttp1), sizeof(int));		//read myscl

        segarray = new int[totallines];
        for(int i = 0; i < totallines; i++){
            streamLine vidata;
            int *intpt = new int;
            float *floatpt = new float;
            vector3d2 *vector3d2pt = new vector3d2;
            vector3d *vector3dpt = new vector3d;
            //vidata.Energyv = new float[eyenum];
            infvi.read(reinterpret_cast<char *>(intpt), sizeof(int));	//read id
            //	printf("id: %d\n", (*intpt));
            vidata.id = *intpt;
            infvi.read(reinterpret_cast<char *>(floatpt), sizeof(float));	//read entropy
            vidata.entropy = *floatpt;
            infvi.read(reinterpret_cast<char *>(intpt), sizeof(int));	//read totalsegments
            vidata.totalsegments = *intpt;
            totalseg += vidata.totalsegments;
            segarray[i] = vidata.totalsegments;
            infvi.read(reinterpret_cast<char *>(floatpt), sizeof(int));		//read maxmag;
            vidata.maxmag = *floatpt;
            //infvi.read(reinterpret_cast<char *>(floatpt), sizeof(float));	//read score
            //vidata.score = *floatpt;
            //vidata.score = 0.0;	//init score
            int sizevi;
            infvi.read(reinterpret_cast<char *>(intpt), sizeof(int));	//read points size
            sizevi = *intpt;
            for(int j = 0; j < sizevi; j++){		//read all points into pointslist
                vector3d2 vectvi;
                infvi.read(reinterpret_cast<char *>(vector3d2pt), sizeof(vector3d2));
                vectvi = *vector3d2pt;
                vidata.pointslist.push_back(vectvi);
            }
            vidata.offset = offset;
            offset += sizevi;

            for(int j = 0; j < sizevi; j++){		//read all dirs but don't store them
                infvi.read(reinterpret_cast<char *>(vector3dpt), sizeof(vector3d));
                vidata.dirslist.push_back(*vector3dpt);
            }

            float *vertexarray = new float[3*sides*sizevi];			//read vertexarary
            infvi.read(reinterpret_cast<char *>(vertexarray), sizeof(float)*3*sides*sizevi);

            //reorder vertexarray for VBO rendering
            vidata.vertexarray = new float[2*(sides+1)*3*vidata.totalsegments];
            int count = 0;
            for(int j = 0; j < vidata.totalsegments; j++){
                for(int s = 3*j*sides; s < 3*j*sides + 3*sides; s+=3){
                    vidata.vertexarray[count] = vertexarray[s];
                    vidata.vertexarray[count+1] = vertexarray[s+1];
                    vidata.vertexarray[count+2] = vertexarray[s+2];

                    vidata.vertexarray[count+3] = vertexarray[s+sides*3];
                    vidata.vertexarray[count+4] = vertexarray[s+sides*3+1];
                    vidata.vertexarray[count+5] = vertexarray[s+sides*3+2];

                    count +=6;
                }

                //last two points of the quad strip
                vidata.vertexarray[count] = vertexarray[3*j*sides];
                vidata.vertexarray[count+1] = vertexarray[3*j*sides+1];
                vidata.vertexarray[count+2] = vertexarray[3*j*sides+2];

                vidata.vertexarray[count+3] = vertexarray[3*j*sides+sides*3];
                vidata.vertexarray[count+4] = vertexarray[3*j*sides+sides*3+1];
                vidata.vertexarray[count+5] = vertexarray[3*j*sides+sides*3+2];

                count +=6;
            }
            delete []vertexarray;

            float *colorarray = new float[3*sides*sizevi];			//read colorarray
            infvi.read(reinterpret_cast<char *>(colorarray), sizeof(float)*3*sides*sizevi);

            //reorder colorarray for VBO rendering
            vidata.colorarray = new float[2*(sides+1)*3*vidata.totalsegments];
            count = 0;
            for(int j = 0; j < vidata.totalsegments; j++){
                for(int s = 3*j*sides; s < 3*j*sides + 3*sides; s+=3){
                    vidata.colorarray[count] = colorarray[s];
                    vidata.colorarray[count+1] = colorarray[s+1];
                    vidata.colorarray[count+2] = colorarray[s+2];

                    vidata.colorarray[count+3] = colorarray[s+sides*3];
                    vidata.colorarray[count+4] = colorarray[s+sides*3+1];
                    vidata.colorarray[count+5] = colorarray[s+sides*3+2];

                    count +=6;
                }

                //last two points of the quad strip
                vidata.colorarray[count] = colorarray[3*j*sides];
                vidata.colorarray[count+1] = colorarray[3*j*sides+1];
                vidata.colorarray[count+2] = colorarray[3*j*sides+2];

                vidata.colorarray[count+3] = colorarray[3*j*sides+sides*3];
                vidata.colorarray[count+4] = colorarray[3*j*sides+sides*3+1];
                vidata.colorarray[count+5] = colorarray[3*j*sides+sides*3+2];

                count +=6;
            }
            delete []colorarray;

            float *normalarray = new float[3*sides*sizevi];			//read normalarray
            infvi.read(reinterpret_cast<char *>(normalarray), sizeof(float)*3*sides*sizevi);

            //reorder normalarray for VBO rendering
            vidata.normalarray = new float[2*(sides+1)*3*vidata.totalsegments];
            count = 0;
            list<vector3d2>::iterator p, next;

            for(p = vidata.pointslist.begin(); p != vidata.pointslist.end(); p++){
                next = p;
                next++;

                if(next != vidata.pointslist.end()){
                    for(int j = 0; j < (sides+1); j++){
                        vidata.normalarray[count] = ( vidata.vertexarray[count] - (*p).vect.x)/radius;
                        vidata.normalarray[count+1] = (vidata.vertexarray[count+1] - (*p).vect.y)/radius;
                        vidata.normalarray[count+2] = (vidata.vertexarray[count+2] - (*p).vect.z)/radius;

                        vidata.normalarray[count+3] = (vidata.vertexarray[count+3] - (*next).vect.x)/radius;
                        vidata.normalarray[count+4] = (vidata.vertexarray[count+4] - (*next).vect.y)/radius;
                        vidata.normalarray[count+5] = (vidata.vertexarray[count+5] - (*next).vect.z)/radius;
                        count+=6;
                    }
                }

            }
            delete []normalarray;

            int *indices = new int[2*(sides+1)*(sizevi-1)];					//read indicesarray
            infvi.read(reinterpret_cast<char *>(indices), sizeof(int)*2*(sides+1)*(sizevi-1));

            vidata.indices = new int[2*(sides+1)*vidata.totalsegments];
            for(int j = 0; j < 2*(sides+1)*vidata.totalsegments; j++){
                vidata.indices[j] = j;
            }
            delete []indices;
            streamlineset.push_back(vidata);
        }

        infvi.close();

        printf("totalseg: %d\n", totalseg);

        vertexarrayVBO = new float[totalseg*3*2*(sides+1)];
        colorarrayVBO = new float[totalseg*3*2*(sides+1)];
        normalarrayVBO = new float[totalseg*3*2*(sides+1)];
        indexVBO = new int[totalseg*2*(sides+1)];

        int count = 0;
        list<streamLine>::iterator j;
        for(j = streamlineset.begin(); j != streamlineset.end(); j++){
            for(int a = 0; a <(*j).totalsegments*3*2*(sides+1);a++){
                vertexarrayVBO[count] = (*j).vertexarray[a];
                colorarrayVBO[count] = (*j).colorarray[a];
                normalarrayVBO[count] = (*j).normalarray[a];
                if(count%3 == 0)
                    indexVBO[count/3] = count/3;
                count++;
            }
            delete [](*j).vertexarray;
            delete [](*j).colorarray;
            delete [](*j).normalarray;
            delete [](*j).indices;
        }
        printf("finish loading VIdata\n");
    }
    else
        printf("Open VIdata failed!\n");
}
#if STL_MASTER
void sender() {
	while (true) {
		sprintf(buf, "%f~%f~%f~%f~%f~%f~%f~%f", tranx, trany, tranz, angle,
			m_curquat[0], m_curquat[1], m_curquat[2], m_curquat[3]);
		if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other,
			slen) == -1) error ("ERROR sendto()");
//		if (tranx < -999999) exit(0);
		usleep(32000);
	}
}
#else // if SLAVE:
void receiver() {
	char buf[BUFLEN];
	vector<string> splits;
	while (true) {
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other,
			&slen) == -1) error("ERROR recvfrom()");
		string itrmdt(buf);
		splits = split(itrmdt, '~');
		tranx = (float)atof(splits[0].c_str());
		trany = (float)atof(splits[1].c_str());
		tranz = (float)atof(splits[2].c_str());
		angle = (float)atof(splits[3].c_str());
		m_curquat[0] = (float)atof(splits[4].c_str());
		m_curquat[1] = (float)atof(splits[5].c_str());
		m_curquat[2] = (float)atof(splits[6].c_str());
		m_curquat[3] = (float)atof(splits[7].c_str());
		if (tranx < -999999 && trany < -999999 && tranz < -999999) exit(EXIT_FAILURE);
//		nanosleep(&req, (struct timespec *)NULL);
	}
}
#endif

int main(int argc, char** argv){
#ifdef STL_MASTER
	if (argc != 2) {
		printf("USAGE: %s relay-ip-address\n", argv[0]);
		return 1;
	}
	RELAY_IP=argv[1];
#else // if SLAVE:
	ortho_left = atof(argv[1]); //-5.0;
	ortho_right = atof(argv[2]); //0.0;
	ortho_bottom = atof(argv[3]); //-5.0;
	ortho_top = atof(argv[4]); //5.0;
	screen_width = atoi(argv[5]); //5.0;
	screen_height = atoi(argv[6]); //5.0;

#endif

	atexit(exitCallback);
	
    // STL init
    readHeader();
    readPar();
    initData();
    loadVIData();

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
#ifdef STL_MASTER
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(processMouse);
    glutMotionFunc(mousePressMove);
    glutPassiveMotionFunc(mouseMove);
#else
	glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("STL Slave Node");
#endif
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(animate);

#ifdef STL_MASTER
    // socket to send data to relay
    slen=sizeof(si_other);
    so_broadcast = 1;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");

    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(RELAY_LISTEN_PORT);
    if (inet_aton(RELAY_IP, &si_other.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    if (pthread_create(&senderThread, NULL, sender, NULL) != 0) {
        perror("Can't start thread, terminating\n");
        return 1;
    }
#else
	// Socket to read data from relay
	slen=sizeof(si_other);
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(SLAVE_LISTEN_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) error("ERROR bind");

	// listen for updates
	if (pthread_create(&receiverThread, NULL, receiver, NULL) != 0) {
		perror("Can't start thread, terminating");
		return 1;
	}
#endif

    // go
    glutMainLoop();
	exit(EXIT_SUCCESS);
}
