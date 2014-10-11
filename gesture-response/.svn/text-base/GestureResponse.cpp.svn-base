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

// TO-DO:
// 4) Then compute some kind of easy average (proximity should be simple) and change some behavior based on that
// 5) Then do more complicated stuff

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
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
  exit(0);
}

string effName;

void display() {
  glEnable(GL_LIGHTING) ;
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0,0,0,0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, screenAspectRatio, .1, 30);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,4,1,
            0,0,1,
            0,0,1);

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

char** globalArguments;

void executeSimulation() {
    string line;
    vector<string> splitLine;
    ifstream inputFile(globalArguments[1]);
    if (inputFile.is_open()) {
      while (inputFile.good()) {
        getline(inputFile, line);
        splitLine = split(line, '~');
        if (splitLine.size() == 4) { // valid input line
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
//        					printf("current value of bufferHead is %d\n", bufferHead);
        usleep(1000); // sleep for 10 milliseconds to achieve 100 Hz
      }
    } else {
      printf("Unable to open file\n");
      exit(1);
    }
    inputFile.close();
    exit(0);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("USAGE: GestureResponse filename\n");
    return 1;
  }
  globalArguments = argv;

  if (SIMULATION) {
    if (pthread_create(&simulatorThread, NULL,
      executeSimulation, NULL) != 0)
    {
      printf("Can't start thread, terminating\n");
      return 1;
    }
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutCreateWindow("Gesture Responder");
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glutDisplayFunc(display);
  glutMainLoop();

  return 0;
}
