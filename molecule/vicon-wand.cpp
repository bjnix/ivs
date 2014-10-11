#include "Client.h"

#include <GL/glew.h>
#include <GL/glut.h>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>

#ifdef WIN32
#include <conio.h>   // For _kbhit()
#include <cstdio>   // For getchar()
#include <windows.h> // For Sleep()
#endif // WIN32

#include <time.h>

using namespace ViconDataStreamSDK::CPP;

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

void display()
{
	glEnable(GL_LIGHTING) ;
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);

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

	// Draw a 1x1 meter square at origin.
	glPointSize(10);
	glBegin(GL_POINTS);
	for(float i=-.5; i<.5; i=i+.1)
		for(float j=-.5; j<.5; j=j+.1)
			glVertex3f(i,j,0);
	glEnd();
		  
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
//	glMultMatrixd(transposedMatrix);

	// cube is drawn centered at the origin, move it up so that it
	// is sitting on the xy plane.
	glTranslatef(0, 0, .5);

	glScalef(.1,.1,.1);
	// C
	float C[][3] = {{-0.2612557475,   0.2614923077,   -0.7541451761},
					{-0.9808750615,   1.0747676366,   -1.5626033561},
					{-2.3443261579,   1.2678912300,   -1.2519095458},
					{-2.9764703958,   0.5650648910,   -0.2817677551},
					{-2.2329547365,   -0.2830495164,   0.5161574581},
					{-0.8620443248,   -0.4922277356,   0.2739953239},
					{-0.1309382452,   -1.4315012344,   1.1944612694},
					{1.9966889284,   0.8448088059,   -0.3071940054},
					{3.3944477812,   0.5042309144,   -0.7737859964}};
	for(int i=0; i<9; i++)
	{
		glColor3f(1,0,0);
		glPushMatrix();
		glTranslatef(C[i][0], C[i][1], C[i][2]);
		glutSolidSphere(.1, 10, 10);
		glPopMatrix();
	}

	float O[][3] = {{1.1316961316,   -1.6172673440,   0.9554737394},
					{-0.7303575118,   -1.9985896615,   2.0910255592},
					{1.0976599323,   0.1121205154,   -1.0823456013},
					{1.6414391457,   1.6127208417,   0.5042130211}};
	for(int i=0; i<4; i++)
	{
		glColor3f(0,1,0);
		glPushMatrix();
		glTranslatef(O[i][0], O[i][1], O[i][2]);
		glutSolidSphere(.1, 10, 10);
		glPopMatrix();
	}
	float H[][3] = {{4.0498147597,   0.9366806027,   -0.1829185069},
					{3.5241412542,   -0.4668764733,   -0.7418551455},
					{3.5239979113,   0.8215223959,   -1.6903452424},
					{-0.5836551618,   1.5000593726,   -2.3144270121},
					{1.4456523214,   -2.1555320906,   1.5186314339},
					{-2.6540118673,   -0.7328218223,   1.2392700151},
					{-3.9157499137,   0.6528945482,   -0.1559325773},
					{-2.8346420606,  1.9162579174,  -1.7383657843}};
	for(int i=0; i<4; i++)
	{
		glColor3f(0,0,1);
		glPushMatrix();
		glTranslatef(H[i][0], H[i][1], H[i][2]);
		glutSolidSphere(.1, 10, 10);
		glPopMatrix();
	}


					
//	glutSolidCube(1); // 1x1x1 cube.



	glFlush();
	glutSwapBuffers();
	glutPostRedisplay(); // call display() repeatedly
}


int main( int argc, char* argv[] )
{
	glutInit(&argc, argv); //initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  //set display mode
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); //set window size
	glutInitWindowPosition(0, 0); //set window position on screen
	glutCreateWindow(argv[0]); //open the screen window

	int glew_err = glewInit();
	if(glew_err != GLEW_OK)
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	atexit(exitCallback);
	viconInit();

	glutMainLoop();

}
