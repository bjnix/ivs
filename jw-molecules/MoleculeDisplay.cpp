#include "Client.h"

#include "/share/apps/glew/1.9.0/include/GL/glew.h"
#include <GL/glut.h>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace ViconDataStreamSDK::CPP;

// *****VICON*****

// Make a new client
Client MyClient;
std::string HostName = "141.219.28.107:801";
//std::string HostName = "localhost:801";

// screen width/height indicate the size of the window on our screen (not the size of the display wall). The aspect ratio must match the actual display wall.
const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

namespace
{
	std::string Adapt( const bool i_Value )
	{
		return i_Value ? "True" : "False";
	}

	std::string Adapt( const Direction::Enum i_Direction )
	{
		switch( i_Direction )
		{
			case Direction::Forward:
				return "Forward";
			case Direction::Backward:
				return "Backward";
			case Direction::Left:
				return "Left";
			case Direction::Right:
				return "Right";
			case Direction::Up:
				return "Up";
			case Direction::Down:
				return "Down";
			default:
				return "Unknown";
		}
	}

	std::string Adapt( const DeviceType::Enum i_DeviceType )
	{
		switch( i_DeviceType )
		{
			case DeviceType::ForcePlate:
				return "ForcePlate";
			case DeviceType::Unknown:
			default:
				return "Unknown";
		}
	}

	std::string Adapt( const Unit::Enum i_Unit )
	{
		switch( i_Unit )
		{
			case Unit::Meter:
				return "Meter";
			case Unit::Volt:
				return "Volt";
			case Unit::NewtonMeter:
				return "NewtonMeter";
			case Unit::Newton:
				return "Newton";
			case Unit::Kilogram:
				return "Kilogram";
			case Unit::Second:
				return "Second";
			case Unit::Ampere:
				return "Ampere";
			case Unit::Kelvin:
				return "Kelvin";
			case Unit::Mole:
				return "Mole";
			case Unit::Candela:
				return "Candela";
			case Unit::Radian:
				return "Radian";
			case Unit::Steradian:
				return "Steradian";
			case Unit::MeterSquared:
				return "MeterSquared";
			case Unit::MeterCubed:
				return "MeterCubed";
			case Unit::MeterPerSecond:
				return "MeterPerSecond";
			case Unit::MeterPerSecondSquared:
				return "MeterPerSecondSquared";
			case Unit::RadianPerSecond:
				return "RadianPerSecond";
			case Unit::RadianPerSecondSquared:
				return "RadianPerSecondSquared";
			case Unit::Hertz:
				return "Hertz";
			case Unit::Joule:
				return "Joule";
			case Unit::Watt:
				return "Watt";
			case Unit::Pascal:
				return "Pascal";
			case Unit::Lumen:
				return "Lumen";
			case Unit::Lux:
				return "Lux";
			case Unit::Coulomb:
				return "Coulomb";
			case Unit::Ohm:
				return "Ohm";
			case Unit::Farad:
				return "Farad";
			case Unit::Weber:
				return "Weber";
			case Unit::Tesla:
				return "Tesla";
			case Unit::Henry:
				return "Henry";
			case Unit::Siemens:
				return "Siemens";
			case Unit::Becquerel:
				return "Becquerel";
			case Unit::Gray:
				return "Gray";
			case Unit::Sievert:
				return "Sievert";
			case Unit::Katal:
				return "Katal";

			case Unit::Unknown:
			default:
				return "Unknown";
		}
	}
}



void viconExit()
{
    MyClient.DisableSegmentData();
//    MyClient.DisableMarkerData();
//    MyClient.DisableUnlabeledMarkerData();
//    MyClient.DisableDeviceData();

	// TODO: Disconnect seems to cause a hang. -Scott Kuhl
    // Disconnect and dispose
    int t = clock();
    std::cout << " Disconnecting..." << std::endl;
    MyClient.Disconnect();
    int dt = clock() - t;
    double secs = (double) (dt)/(double)CLOCKS_PER_SEC;
    std::cout << " Disconnect time = " << secs << " secs" << std::endl;
}

void viconInit()
{
    // Connect to a server
    std::cout << "Connecting to " << HostName << " ..." << std::flush;
	int attemptConnectCount = 0;
	const int MAX_CONNECT_ATTEMPTS=2;
    while( !MyClient.IsConnected().Connected && attemptConnectCount < MAX_CONNECT_ATTEMPTS)
    {
		attemptConnectCount++;
		bool ok = false;
		ok =( MyClient.Connect( HostName ).Result == Result::Success );
		if(!ok)
			std::cout << "Warning - connect failed..." << std::endl;
		std::cout << ".";
		sleep(1);
    }
	if(attemptConnectCount == MAX_CONNECT_ATTEMPTS)
	{
		printf("Giving up making connection to Vicon system\n");
		return;
	}
    std::cout << std::endl;

    // Enable some different data types
    MyClient.EnableSegmentData();
    //MyClient.EnableMarkerData();
    //MyClient.EnableUnlabeledMarkerData();
    //MyClient.EnableDeviceData();

    std::cout << "Segment Data Enabled: "          << Adapt( MyClient.IsSegmentDataEnabled().Enabled )         << std::endl;
    std::cout << "Marker Data Enabled: "           << Adapt( MyClient.IsMarkerDataEnabled().Enabled )          << std::endl;
    std::cout << "Unlabeled Marker Data Enabled: " << Adapt( MyClient.IsUnlabeledMarkerDataEnabled().Enabled ) << std::endl;
    std::cout << "Device Data Enabled: "           << Adapt( MyClient.IsDeviceDataEnabled().Enabled )          << std::endl;

    // Set the streaming mode
    //MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
    // MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );
    MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ServerPush );

    // Set the global up axis
    MyClient.SetAxisMapping( Direction::Forward, 
                             Direction::Left, 
                             Direction::Up ); // Z-up
    // MyClient.SetGlobalUpAxis( Direction::Forward, 
    //                           Direction::Up, 
    //                           Direction::Right ); // Y-up

    Output_GetAxisMapping _Output_GetAxisMapping = MyClient.GetAxisMapping();
    std::cout << "Axis Mapping: X-" << Adapt( _Output_GetAxisMapping.XAxis ) 
			  << " Y-" << Adapt( _Output_GetAxisMapping.YAxis ) 
			  << " Z-" << Adapt( _Output_GetAxisMapping.ZAxis ) << std::endl;

    // Discover the version number
    Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
    std::cout << "Version: " << _Output_GetVersion.Major << "." 
			  << _Output_GetVersion.Minor << "." 
			  << _Output_GetVersion.Point << std::endl;

}



// an atexit() callback:
void exitCallback()
{
	viconExit();
	return;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27 || key == 'q')  // escape key, exit program
		exit(0);
}

// *****DISPLAY MOLECULES*****

struct Coordinate {
  float x, y, z, r, g, b;
};

struct Color {
  float r, g, b;
};

struct MoleculeModel {
  vector<Coordinate> points;
};

MoleculeModel model;
map<char, int> elements;
vector<Color> colors;

void display() {
  glEnable(GL_LIGHTING) ;
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0,0,0,0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Get a frame
  if(MyClient.GetFrame().Result != Result::Success )
    printf("WARNING: Inside display() and there is no data from Vicon...\n");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, screenAspectRatio, .1, 30);
	  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,4,1,
            0,0,1,
            0,0,1);

  // units are in millimeters, lets switch to meters
  Output_GetSegmentGlobalTranslation globalTranslate = MyClient.GetSegmentGlobalTranslation( "Wand", "Wand");
  glTranslatef(globalTranslate.Translation[ 0 ] / 1000,
    globalTranslate.Translation[ 1 ] / 1000,
    globalTranslate.Translation[ 2 ] / 1000);
  // Get the global segment rotation as a matrix

  // Vicon rotation matrix is in row-major order and OpenGL expects column-major order.
  Output_GetSegmentGlobalRotationMatrix globalRotMatrix = MyClient.GetSegmentGlobalRotationMatrix( "Wand", "Wand");

  double transposedMatrix[16] = { globalRotMatrix.Rotation[ 0 ],
    globalRotMatrix.Rotation[ 3 ],
    globalRotMatrix.Rotation[ 6 ],
    0, // end of column 1
    globalRotMatrix.Rotation[ 1 ],
    globalRotMatrix.Rotation[ 4 ],
    globalRotMatrix.Rotation[ 7 ],
    0, // end of column 2
    globalRotMatrix.Rotation[ 2 ],
    globalRotMatrix.Rotation[ 5 ],
    globalRotMatrix.Rotation[ 8 ],
    0, // end of column 3
    0, 0, 0, 1 }; // end of column 4
  glMultMatrixd(transposedMatrix);

  // Draw molecule
/*  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  gluPerspective(60, 1.77, 0, 10);
  glTranslatef(0.0f, 0.0f, -6.0f); /* */
  for (int i = 0; i < model.points.size(); i++) {
    glPushMatrix();
    glColor3f(model.points[i].r, model.points[i].g, model.points[i].b);
    glTranslatef(model.points[i].x, model.points[i].y, model.points[i].z);
    glutSolidSphere(0.05, 16, 16);
    glPopMatrix();
  }

  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("USAGE: MoleculeDisplay filename_of_molecule.xyz\n");
    return 1;
  }

  // initialize colors
  Color red;
  red.r = 1.0f; red.g = 0.0f; red.b = 0.0f;
  colors.push_back(red);
  Color green;
  green.r = 0.0f; green.g = 1.0f; green.b = 0.0f;
  colors.push_back(green);
  Color blue;
  blue.r = 0.0f; blue.g = 0.0f; blue.b = 1.0f;
  colors.push_back(blue);
  Color yellow;
  yellow.r = 1.0f; yellow.g = 1.0f; yellow.b = 0.0f;
  colors.push_back(yellow);
  Color violet;
  violet.r = 1.0f; violet.g = 0.0f; violet.b = 1.0f;
  colors.push_back(violet);
  Color aqua;
  aqua.r = 0.0f; aqua.g = 1.0f; aqua.b = 1.0f;
  colors.push_back(aqua);
  Color white;
  white.r = 1.0f; white.g = 1.0f; white.b = 1.0f;
  colors.push_back(white);

  // load model data
  int numElements = 0;

  ifstream moleculeFile;
  moleculeFile.open(argv[1], ios::in);
  if (moleculeFile.is_open()) {
    string line;
    while (moleculeFile.good()) {
      getline(moleculeFile, line);
      istringstream iss(line);
      vector<string> tokens;
      copy(istream_iterator<string>(iss), istream_iterator<string>(),
        back_inserter<vector<string> >(tokens));
      if (tokens.size() == 0) break; // done reading file
      else if (tokens.size() != 4) {
        printf("Formatting error in file %s\nLine:%s\n", argv[1], line.c_str());
        moleculeFile.close();
        return 1;
      }
      map<char, int>::iterator it = elements.find(tokens[0].c_str()[0]);
      if (it == elements.end()) { // not already in map, so insert it
        elements[tokens[0].c_str()[0]] = numElements;
        numElements++;
        if (numElements >= colors.size()) numElements = 0;
      }
      struct Coordinate c;
      c.x = atof(tokens[1].c_str()) / 2.0f;
      c.y = atof(tokens[2].c_str()) / 2.0f;
      c.z = atof(tokens[3].c_str()) / 2.0f;
      c.r = colors[elements[tokens[0].c_str()[0]]].r;
      c.g = colors[elements[tokens[0].c_str()[0]]].g;
      c.b = colors[elements[tokens[0].c_str()[0]]].b;
      model.points.push_back(c);
    }
  } else {
    printf("Unable to open file %s\n", argv[1]);
    return 1;
  }
  moleculeFile.close();

  // OpenGL initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Display Molecules");
  int glew_err = glewInit();
  if (glew_err != GLEW_OK) fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);

  atexit(exitCallback);
  viconInit();

  // Go
  glutMainLoop();

  return 0;
}
