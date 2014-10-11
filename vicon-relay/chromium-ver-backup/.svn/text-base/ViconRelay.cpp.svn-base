/*
program that you can call from command line

arguments:
IP, port, path to a texture to display on the wall, [set of strings for objects]

Gets tracking data bout the objects passed in and sends that data back
	position: X,Y,Z
	orientation: pitch, yaw, roll
*/


#include "Client.h"

#include "/share/apps/glew/1.9.0/include/GL/glew.h"
#include <GL/glut.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sstream>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>

#include <string>
#include <vector>
#include <time.h>
#include <sys/time.h>

#include "../boost_1_53_0/boost/format.hpp"
#include "imageio.h"

using namespace std;
using namespace ViconDataStreamSDK::CPP;
using boost::format;

#define output_stream if(!LogFile.empty()) ; else std::cout 

// ***VICON***

// Make a new client
Client MyClient;
std::string HostName = "141.219.28.107:801";
//std::string HostName = "localhost:801";

// screen width/height indicate the size of the window on our screen (not the size of the display wall). The aspect ratio must match the actual display wall.
const GLdouble SCREEN_WIDTH = (1920.0*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

// socket stuff
struct sockaddr_in si_other;
int slen, s;
int so_broadcast = 1;
pthread_t senderThread;

// timestamp stuff
time_t curtime;
char timebuff[30];
struct timeval tv;

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

// end of Vicon stuff /////////////////////////////////////////////////////////////////////////////

string ipAddress;
unsigned int port;
vector<string> objectsToTrack;
string texturePath;
string dataToSend;

const GLuint MAX_TILES = 100;
GLuint numTiles=0;
GLuint texNames[MAX_TILES];
float aspectRatio;

float readfile(char *filename, GLuint *texName, GLuint *numTiles)
{
	static int verbose=1;  // change this to 0 to print out less info

	imageio_info iioinfo;
	iioinfo.filename = filename;
	iioinfo.type = CharPixel;
	iioinfo.map = (char*) "RGBA";
	//	iioinfo.colorspace = RGBColorspace;
	char *image = (char*) imagein(&iioinfo);

	if(image == NULL)
	{
		fprintf(stderr, "\n%s: Unable to read image.\n", filename);
		return -1;
	}

	/* "image" is a 1D array of characters (unsigned bytes) with four
	 * bytes for each pixel (red, green, blue, alpha). The data in "image"
	 * is in row major order. The first 4 bytes are the color information
	 * for the lowest left pixel in the texture. */
	int width  = (int)iioinfo.width;
	int height = (int)iioinfo.height;
	float original_aspectRatio = (float)width/height;
	if(verbose)
		printf("%s: Finished reading, dimensions are %dx%d\n", filename, width, height);

	if(height > 4096*2)
	{
		printf("Source image must <= 8192 pixels tall.");
		exit(1);
	}

	int subimgH = height/2;
	int workingWidth = width;
	*numTiles = 1;
	while(workingWidth > 4096)
	{
		*numTiles = *numTiles * 2;
		workingWidth = workingWidth / 2;
	}
	int subimgW = workingWidth;

	if(*numTiles > MAX_TILES/2.0)
	{
		printf("Too many tiles");
		exit(1);
	}

	glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGB, subimgW, subimgH,
	             0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	int tmp;
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tmp);

	if(tmp == 0)
	{
		fprintf(stderr, "%s: File is too large (%d x %d). I can't load it!\n", filename, subimgW, subimgH);
		fprintf(stderr, "This ONLY works under chromium since ivs.research itself doesn't support large enough texture sizes.\n");
		free(image);
		exit(1);
	}


	glGenTextures(*numTiles*2, texName);

	for(GLuint curTile=0; curTile < *numTiles*2; curTile = curTile+2)
	{
		glPixelStorei(GL_UNPACK_ROW_LENGTH,  width);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, curTile/2.0*subimgW);
		glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);

		glBindTexture(GL_TEXTURE_2D, texName[curTile]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, subimgW, subimgH,
		             0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glPixelStorei( GL_UNPACK_SKIP_PIXELS, curTile/2.0*subimgW );
		glPixelStorei( GL_UNPACK_SKIP_ROWS, subimgH);

		glBindTexture(GL_TEXTURE_2D, texName[curTile+1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, subimgW, subimgH,
		             0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	free(image);
	return original_aspectRatio;
}

void display() {

    // Get a frame
    if(MyClient.GetFrame().Result != Result::Success )
      printf("WARNING: Inside display() and there is no data from Vicon...\n");

    Output_GetSegmentGlobalTranslation globalTranslate = MyClient.GetSegmentGlobalTranslation(objectsToTrack[0], objectsToTrack[0]);
    //Output_GetSegmentGlobalRotationEulerXYZ globalRotation = MyClient.GetSegmentGlobalRotationEulerXYZ(objectsToTrack[0], objectsToTrack[0]);

    float xMeters = (globalTranslate.Translation[0] / 1000);
    float yMeters = (globalTranslate.Translation[1] / 1000);
    float zMeters = (globalTranslate.Translation[2] / 1000);

    yMeters += 2.0f;
    zMeters -= 1.2f; // assume this is approx. where most people will hold the wand
    float scaleMap = 1.0f + (yMeters / 1.3f);
    if (scaleMap < 1.0f) scaleMap = 1.0f;
    float xScale = -xMeters / scaleMap;
    float zScale = -zMeters * (scaleMap / 3.0f);

    float quadHeightScale = 1;
    float tileWidth = 2.0/(numTiles);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1); // color of quad

    for(GLuint i=0; i<numTiles*2; i=i+2)
    {
        glBindTexture(GL_TEXTURE_2D, texNames[i]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2d(scaleMap * (i/2    *tileWidth-1 + xScale), scaleMap * (quadHeightScale*-1 + zScale));
        glTexCoord2f(1.0, 0.0); glVertex2d(scaleMap * ((i/2+1)*tileWidth-1 + xScale), scaleMap * (quadHeightScale*-1 + zScale));
        glTexCoord2f(1.0, 1.0); glVertex2d(scaleMap * ((i/2+1)*tileWidth-1 + xScale), scaleMap * (quadHeightScale*0 + zScale));
        glTexCoord2f(0.0, 1.0); glVertex2d(scaleMap * (i/2    *tileWidth-1 + xScale), scaleMap * (quadHeightScale*0 + zScale));
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texNames[i+1]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2d(scaleMap * (i/2    *tileWidth-1 + xScale), scaleMap * (quadHeightScale*0 + zScale));
        glTexCoord2f(1.0, 0.0); glVertex2d(scaleMap * ((i/2+1)*tileWidth-1 + xScale), scaleMap * (quadHeightScale*0 + zScale));
        glTexCoord2f(1.0, 1.0); glVertex2d(scaleMap * ((i/2+1)*tileWidth-1 + xScale), scaleMap * (quadHeightScale*1 + zScale));
        glTexCoord2f(0.0, 1.0); glVertex2d(scaleMap * (i/2    *tileWidth-1 + xScale), scaleMap * (quadHeightScale*1 + zScale));
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0,0,0,.3);
    glBegin(GL_QUADS);
    glVertex2d(-1,-1);
    glVertex2d(-.5,-1);
    glVertex2d(-.5,-.96);
    glVertex2d(-1,-.96);
    glEnd();

    glColor4f(1,1,1,.9);
    glRasterPos2f(-.98,-.98);
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;
    char *str = texturePath.c_str();
    for(GLuint i=0; i<strlen(str); i++)
        glutBitmapCharacter(font, str[i]);

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();

}

void sender() {

  vector<format> formatters;
  for (int i = 0; i < objectsToTrack.size(); i++) {
    formatters.push_back(format("%1%~%2%~%3%~%4%~%5%~%6%~%7%~%8%~\n"));
  }

  while (true) {

    dataToSend.clear();

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(timebuff, 30, "%m-%d-%Y %T.", localtime(&curtime));

    // Get a frame
    if(MyClient.GetFrame().Result != Result::Success )
      printf("WARNING: Inside display() and there is no data from Vicon...\n");

    for (int i = 0; i < objectsToTrack.size(); i++) {
      Output_GetSegmentGlobalTranslation globalTranslate = MyClient.GetSegmentGlobalTranslation(objectsToTrack[i], objectsToTrack[i]);
      Output_GetSegmentGlobalRotationEulerXYZ globalRotation = MyClient.GetSegmentGlobalRotationEulerXYZ(objectsToTrack[i], objectsToTrack[i]);

//      for (int m = 0; m < sizeof(buf); m++) buf[m] = '|';

//      sprintf(buf, "%s~%6.3f~%6.3f~%6.3f~%6.3f~%6.3f~%6.3f\n",
      
      formatters[i] % objectsToTrack[i];
      formatters[i] % timebuff;
      formatters[i] % (globalTranslate.Translation[0] / 1000);
      formatters[i] % (globalTranslate.Translation[1] / 1000);
      formatters[i] % (globalTranslate.Translation[2] / 1000);
      formatters[i] % globalRotation.Rotation[0];
      formatters[i] % globalRotation.Rotation[1];
      formatters[i] % globalRotation.Rotation[2];

      dataToSend.append(formatters[i].str());
    }

    if (sendto(s, dataToSend.c_str(), dataToSend.length(), 0, (struct sockaddr*)&si_other, slen) == -1) {
      perror ("ERROR sendto()");
    }
//    usleep(16000);
  }
}

int main(int argc, char* argv[]) {

  if (argc < 5) {
    printf("Usage: ViconRelay IP Port TexturePath Objects\n");
    return 1;
  }
  ipAddress = string(argv[1]);
  port = atoi(argv[2]);
  texturePath = string(argv[3]);
  for (int i = 4; i < argc; i++) {
    objectsToTrack.push_back(string(argv[i]));
  }
//  printf("IP address passed in was %s\n", ipAddress.c_str()); // OK, that's working.

  // OpenGL initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Vicon Relay");
  int glew_err = glewInit();
  if (glew_err != GLEW_OK) fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);

  atexit(exitCallback);
  viconInit(); // Vicon initialization

  //socket stuff
  slen=sizeof(si_other);
  so_broadcast = 1;

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("ERROR socket");
    exit(1);
  }

  setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(port);
  if (inet_aton(ipAddress.c_str(), &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  if (pthread_create(&senderThread, NULL, sender, NULL) != 0) {
    perror("Can't start thread, terminating\n");
    return 1;
  }

  // load image
  readfile(texturePath.c_str(), texNames, &numTiles);

  // Go
  glutMainLoop();

  return 0;

}
