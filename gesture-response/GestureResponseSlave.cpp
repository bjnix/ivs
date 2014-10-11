// Interactive Sonification Visualization Component
// Author: James Walker jwwalker a+ mtu d0+ edu

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
#include "../boost_1_53_0/boost/lexical_cast.hpp"

#define SEND_IP "255.255.255.255"  // broadcast address
#define BUFLEN 512
#define NPACK 10
#define PORT 25884

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*3);  
const GLdouble SCREEN_HEIGHT = 1080;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

ofstream debugFile;
bool simulation;
bool logger;

typedef struct trackable {
  float x;
  float y;
  float z;
} trackable;

typedef struct particle {
  float x;
  float y;
  float z;
  float x_vel;
  float y_vel;
  float z_vel;
  float colorR;
  float colorG;
  float colorB;
  float colorA;
} particle;

int bufferHead = -1;
const int bufferSeconds = 5;
const int dataHertz = 100;
const int bufferSize = bufferSeconds * dataHertz;
int numTrackedObjects;

const bool SIMULATION = true;
vector<string> trackNames;
map<string, vector<trackable> > trackHistory; 
vector<float> averageDistances;

map<string, vector<trackable> > afterImages;
vector<particle> particles;

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

const int numAfterImages = 24;

void addAfterImage(string key, trackable addMe) {
  if (afterImages[key].size() < numAfterImages) {
    afterImages[key].push_back(addMe);
  } else {
    afterImages[key].erase(afterImages[key].end()-1);
    afterImages[key].insert(afterImages[key].begin(), addMe);
  } /* */
}

trackable calculateVelocity(string key) {
  trackable retData;
  retData.x = 0;
  retData.y = 0;
  retData.z = 0;
  float xVel = 0;
  float yVel = 0;
  float zVel = 0;
  if (bufferHead >= 6) {
    for (int t = bufferHead; t > bufferHead - 5; t--) {
      xVel += trackHistory[key][t].x - trackHistory[key][t-1].x;
      yVel += trackHistory[key][t].y - trackHistory[key][t-1].y;
      zVel += trackHistory[key][t].z - trackHistory[key][t-1].z;
    }
    retData.x = xVel / 5.0f;
    retData.y = yVel / 5.0f;
    retData.z = zVel / 5.0f;
  }
  return retData;
}

float calculateAverageVelocity() {
  trackable retData;
  retData.x = 0.0f;
  retData.y = 0.0f;
  retData.z = 0.0f;
  for (int i = 0; i < trackNames.size(); i++) {
    trackable runningAvg = calculateVelocity(trackNames[i]);
    retData.x += absFloat(runningAvg.x);
    retData.y += absFloat(runningAvg.y);
    retData.z += absFloat(runningAvg.z);
  }
  retData.x /= (float)numTrackedObjects;
  retData.y /= (float)numTrackedObjects;
  retData.z /= (float)numTrackedObjects;
  return ((retData.x + retData.y + retData.z) / 3.0f);
}

int getTmpBufferHead(string effName) {
  int tmpBufferHead = bufferHead - 1;
  if (tmpBufferHead < 0) {
    if (trackHistory[effName].size() == bufferSize) tmpBufferHead = bufferSize - 1;
    else tmpBufferHead = 0;
  }
  return tmpBufferHead;
}

trackable getColors(string effName) {
  trackable color;
  color.x = color.y = color.z = 1.0f;
  int tmpBufferHead = getTmpBufferHead(effName);
  if (averageDistances.size() > 0) {
    color.x = averageDistances[tmpBufferHead] / 2.0f;
    color.z = 1.0f - averageDistances[tmpBufferHead] / 2.0f;
    if (color.x > color.z) color.y = color.x - color.z;
    else color.y = color.z - color.x;
    if (color.x > 1.0f) color.x = 1.0f;
    if (color.z < 0.0f) color.z = 0.0f;
    if (color.y > 1.0f) color.y = 1.0f;
  }
  return color;
}

float cubeRotationA = 0.0f;
float cubeRotationB = 0.0f;
float cubeRotationC = 0.0f;
float cubeColorR = 0.3f;
float cubeColorG = 0.6f;
float cubeColorB = 0.9f;

int executionCtr = 0;
int totalCtr = 0;

void averageDistanceHelper() {
            executionCtr++;
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
            }
            bufferHead++;
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
      if (executionCtr % 3 == 0) addAfterImage(splitLine[0], newTrackData);
      // add particles
      if (executionCtr % 50 == 0) {
        trackable velocityData = calculateVelocity(splitLine[0]);
        trackable color = getColors(splitLine[0]);
        if (velocityData.x != 0 && velocityData.y != 0 && velocityData.z != 0) {
          particle newParticle;
          newParticle.x = newTrackData.x;
          newParticle.y = newTrackData.y;
          newParticle.z = newTrackData.z;
          newParticle.x_vel = velocityData.x;
          newParticle.y_vel = velocityData.y;
          newParticle.z_vel = velocityData.z;
          newParticle.colorR = color.x;
          newParticle.colorG = color.y;
          newParticle.colorB = color.z;
          newParticle.colorA = 1.0f;
          particles.push_back(newParticle);
        }
      }
      if (!simulation) {  // counting for live tracking
        totalCtr++;
        if (trackNames.size() > 0) {
          if (totalCtr % trackNames.size() == 0) {
            averageDistanceHelper();
          }
        }
      }
    } else if (simulation) { // counting for data dump reading
      averageDistanceHelper();
      // compute average proximity
// DEBUG CODE
//if (logger) {
//  debugFile << "trackNames.size() is " << boost::lexical_cast<string>(trackNames.size()) << "\n";
//  debugFile << "numTrackedObjects is " << boost::lexical_cast<string>(numTrackedObjects) << "\n";
//}
// END DEBUG CODE
    }
  }
}

string effName;

void display() {

  // auto close
  framesPassed++;
  if (receivedPacket) {
    if (framesPassed > 180) {
      if (logger && debugFile.is_open()) debugFile.close(); // DEBUGGING CODE
      exit(0);
    }
  } else {
    if (framesPassed > 900) {
      if (logger && debugFile.is_open()) debugFile.close(); // DEBUGGING CODE
      exit(0);
    }
  }

  // display

  glEnable(GL_LIGHTING) ;
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  // flashing background
  trackable bgColor;
  bgColor.x = bgColor.y = bgColor.z = 0;
  if (trackNames.size() >= 1) {
    bgColor = getColors(trackNames[0]);
    bgColor.x -= 0.5f;
    bgColor.x *= 2;
    bgColor.x += 0.5f;
    if (bgColor.x < 0) bgColor.x = 0;
    else if (bgColor.x > 1) bgColor.x = 1;
  }
  glClearColor(bgColor.x, bgColor.x, bgColor.x, 0);

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

  glTranslatef(0.0f, 3.5f, 0.0f);

  // draw huge wireframe cubes that spin in response to user movement
  glBlendFunc(GL_ONE, GL_ZERO);
  glLineWidth(5.0f);
  float avgVel = calculateAverageVelocity();
  cubeRotationA += (avgVel * 3);
  cubeRotationB += (avgVel * 5);
  cubeRotationC += (avgVel * 7);
  cubeColorR += 0.01f;
  cubeColorG += 0.03f;
  cubeColorB += 0.05f;
  if (cubeColorR > 1.0f) cubeColorR = 0.0f;
  if (cubeColorG > 1.0f) cubeColorG = 0.0f;
  if (cubeColorB > 1.0f) cubeColorB = 0.0f;
  glColor4f(cubeColorR, cubeColorG, cubeColorB, 1.0f);
  glPushMatrix();
  glRotatef(cubeRotationA, 0.0f, 0.0f, 1.0f);
  glutWireSphere(6.0, 8, 8);
  glPopMatrix();
  glColor4f(cubeColorB, cubeColorR, cubeColorG, 1.0f);
  glPushMatrix();
  glRotatef(cubeRotationB, 0.0f, 1.0f, 0.0f);
  glutWireSphere(6.0, 8, 8);
  glPopMatrix();
  glColor4f(cubeColorG, cubeColorB, cubeColorR, 1.0f);
  glPushMatrix();
  glRotatef(cubeRotationC, 1.0f, 0.0f, 0.0f);
  glutWireSphere(6.0, 8, 8);
  glPopMatrix();
  glLineWidth(1.0f);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);

  for (int i = 0; i < trackNames.size(); i++) {
    glPushMatrix();

    // basic display
    effName = trackNames[i];

    int tmpBufferHead = getTmpBufferHead(effName);
    trackable color = getColors(effName);

    if (trackHistory[effName][tmpBufferHead].z != 0) {
      glTranslatef(trackHistory[effName][tmpBufferHead].x,
        trackHistory[effName][tmpBufferHead].y,
        trackHistory[effName][tmpBufferHead].z);
      glColor3f(color.x, color.y, color.z);
      glutSolidSphere(0.1, 12, 12);
      glPopMatrix();
    }
    // display afterimages (trail)
    float runningSize = 0.1f;
    float runningAlpha = 1.0f;
    if (afterImages[effName].size() == numAfterImages) {
      for (int a = 0; a < numAfterImages; a++) {
        if (afterImages[effName][a].z != 0) {
          glPushMatrix();
          glTranslatef(afterImages[effName][a].x,
            afterImages[effName][a].y,
            afterImages[effName][a].z);
          glColor4f(color.x, color.y, color.z, runningAlpha);
          glutSolidSphere(runningSize, 8, 8);
          glPopMatrix();
          runningSize -= 0.005f;
          runningAlpha -= 0.05f;
        }
      }
    }
  } // end loop thru trackNames

  // display particles
  for (int i = particles.size() - 1; i >= 0; i--) {
    particles[i].colorA -= 0.0025f;
    if (particles[i].colorA <= 0) particles.erase(particles.begin() + i);
    else {
      particles[i].x += particles[i].x_vel;
      particles[i].y += particles[i].y_vel;
      particles[i].z += particles[i].z_vel;
      glPushMatrix();
      glTranslatef(particles[i].x, particles[i].y, particles[i].z);
      glColor4f(particles[i].colorR, particles[i].colorG,
        particles[i].colorB, particles[i].colorA);
      glutSolidCube(0.07);
      glPopMatrix();
    }
  }

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv) {
  if (argc < 7) {
    printf("USAGE: GestureResponseSlave left right bottom top num_tracked_objects simulation\n");
    return 1;
  }
  if (argc > 7) logger = true;
  else logger = false;

  if (logger) { // DEBUGGING CODE
    debugFile.open("/home/jwwalker/vrlab/ivs/gesture-response/debuglog.txt");
  }

  ortho_left = atof(argv[1]); //-5.0;
  ortho_right = atof(argv[2]); //0.0;
  ortho_bottom = atof(argv[3]); //-5.0;
  ortho_top = atof(argv[4]); //5.0;
  numTrackedObjects = atoi(argv[5]);
  simulation = (strcmp(argv[6], "FALSE") != 0);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Gesture Responder Slave Node");
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
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
