#include <GL/glut.h>

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

// 5) Then do more complicated stuff

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*3);  
const GLdouble SCREEN_HEIGHT = 1080;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

typedef struct trackable {
  float x;
  float y;
  float z;
} trackable;

int bufferHead = -1;
const int bufferSeconds = 5;
const int dataHertz = 100;
const int bufferSize = bufferSeconds * dataHertz;
const int numTrackedObjects = 4;

const bool SIMULATION = true;
vector<string> trackNames;
map<string, vector<trackable> > trackHistory; 
vector<float> averageDistances;

pthread_t simulatorThread;

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

float absFloat(float f) {
  if (f >= 0) return f;
  else return -f;
}

float computeAverage(vector<float> values) {
  float avg = 0.0f;
  for (int i = 0; i < values.size(); i++) {
    avg += values[i];
  }
  avg /= values.size();
  return avg;
}

float compute3dDistance(trackable t1, trackable t2) {
  float xd = t1.x - t2.x;
  float yd = t1.y - t2.y;
  float zd = t1.z - t2.z;
  return sqrt(xd*xd + yd*yd + zd*zd);
}

float computeAverageDistance(vector<trackable> t) {
  vector<float> distances;
  for (int i = 0; i < t.size() - 1; i++) {
    for (int j = i + 1; j < t.size(); j++) {
      distances.push_back(compute3dDistance(t[i], t[j]));
    }
  }
  return computeAverage(distances);
}

void receiver() {
  char buf[BUFLEN];
  vector<string> splitLine;
  while (true) {
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other,
      &slen) == -1) error("ERROR recvfrom()");
    receivedPacket = true;
    framesPassed = 0;
    string itrmdt(buf);
    splitLine = split(itrmdt, '~');
    if (splitLine.size() == 4) {  // valid input line
      if (trackHistory.count(splitLine[0]) == 0) {
        trackNames.push_back(splitLine[0]);
      }
      trackable newTrackData;
      newTrackData.x = atof(splitLine[1].c_str());
      newTrackData.y = atof(splitLine[2].c_str());
      newTrackData.z = atof(splitLine[3].c_str());
      if (bufferHead >= bufferSize) bufferHead = 0;
      if (trackHistory[splitLine[0]].size() < bufferSize) {
        trackHistory[splitLine[0]].push_back(newTrackData);
      } else {
        trackHistory[splitLine[0]][bufferHead] = newTrackData;
      }
    } else {
      if (trackNames.size() == numTrackedObjects) {
        vector<trackable> points;
        for (int i = 0; i < trackNames.size(); i++) {
          points.push_back(trackHistory[trackNames[i]][bufferHead]);
        }
        if (averageDistances.size() < bufferSize) {
          averageDistances.push_back(computeAverageDistance(points));
        } else {
          averageDistances[bufferHead] = computeAverageDistance(points);
        }
      } /* */
      bufferHead++;
    }
  }
}

string effName;

void display() {

  // auto close
  framesPassed++;
  if (receivedPacket) {
    if (framesPassed > 180) exit(0);
  } else {
    if (framesPassed > 900) exit(0);
  }

  // display
//  glFrustum(ortho_left, ortho_right, ortho_bottom, ortho_top, 0.1, 5000);

  glEnable(GL_LIGHTING) ;
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0,0,0,0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(ortho_left, ortho_right, ortho_bottom, ortho_top, 0.1, 5000);
//  gluPerspective(45, screenAspectRatio, .1, 5000);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,4,1,
            0,0,1,
            0,0,1);
  glutWireCube(1.0);

//  glScalef(4.0, 4.0, 4.0);
  glTranslatef(0.0f, 3.5f, 0.0f);

//  gluPerspective(45, screenAspectRatio, .1, 30);

  int tmpBufferHead = bufferHead - 1;
  if (tmpBufferHead < 0) {
    if (trackHistory[effName].size() == bufferSize) tmpBufferHead = bufferSize - 1;
    else tmpBufferHead = 0;
  }
  float color = 1.0f;
  if (averageDistances.size() > 0) {
    color = averageDistances[tmpBufferHead] / 2.0f;
    if (color > 1.0f) color = 1.0f;
  }

  for (int i = 0; i < trackNames.size(); i++) {
    glPushMatrix();

    effName = trackNames[i];
    //bufferHead = trackHistory[effName].size() - 1;
    glTranslatef(trackHistory[effName][tmpBufferHead].x,
      trackHistory[effName][tmpBufferHead].y,
      trackHistory[effName][tmpBufferHead].z);
    glColor3f(color, color, color);
    glutSolidSphere(0.05, 16, 16);

    glPopMatrix();
  }

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv) {
  if (argc < 5) {
    printf("USAGE: GestureResponseSlave left right bottom top\n");
    return 1;
  }

  ortho_left = atof(argv[1]); //-5.0;
  ortho_right = atof(argv[2]); //0.0;
  ortho_bottom = atof(argv[3]); //-5.0;
  ortho_top = atof(argv[4]); //5.0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Gesture Responder Slave Node");
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glutDisplayFunc(display);

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

  glutMainLoop();

  return 0;
}
