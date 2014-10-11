#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "imageio.h"

const GLuint MAX_TILES = 100;

const int SLIDESHOW_WAIT = 10; // time in seconds to wait when autoadvance is turned on.
 
// screen width/height indicate the size of the window on our screen (not the size of the display wall). The aspect ratio must match the actual display wall.
const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

int autoAdvance = 0;
int lastAdvance = 0;
float scrollAmount = 0;

GLuint numTiles=0;
GLuint texNames[MAX_TILES];
float aspectRatio;

GLuint texNamesNEXT[MAX_TILES];
GLuint numTilesNEXT=0;
float aspectRatioNEXT;

int currentTexture = 0;
int totalTextures = 0;
char **globalargv = NULL;

/* Readfile uses imageio to read in an image, and binds it to an OpenGL
 * texture name.  Requires OpenGL 2.0 or better.
 *
 * filename: name of file to load
 *
 * texName: A pointer to where the OpenGL texture name should be stored.
 * (Remember that the "texture name" is really just some unsigned int).
 *
 * returns: aspect ratio of the image in the file.
 */
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


int getNextTexture()
{
	int next = currentTexture+1;
	if(next >= totalTextures)
		return 0;
	return next;
}

int getPrevTexture()
{
	int next = currentTexture-1;
	if(next < 0)
		return totalTextures-1;
	return next;
}

void display(void);

void loadCurrentTexture()
{
	glDeleteTextures(numTiles*2, texNames);

	int oldAutoAdvance = autoAdvance;
	autoAdvance = 0;

	for(GLuint i=0; i<numTilesNEXT*2; i++)
		texNames[i] = texNamesNEXT[i];
	numTiles = numTilesNEXT;
	numTilesNEXT = 0;
	aspectRatio = aspectRatioNEXT;
	scrollAmount = 0;

	// Call display directly to ensure that we actually display image now
	// (before we start preloading next image).
	display();

	// Display() can take a while to run first time, update lastadvance time...

	printf("Pre-loading next image: %s\n", globalargv[getNextTexture()+1]);
	aspectRatioNEXT = readfile(globalargv[getNextTexture()+1], texNamesNEXT, &numTilesNEXT);

	// Call display after we preload the picture so that we
	// actually send it to the tiles (we draw the preloaded
	// texture on an infinitely small quad in the scene to do
	// this!).
	display();
	lastAdvance = time(NULL);
	autoAdvance = oldAutoAdvance;
}


void display(void)
{
	int needsScroll = 0;
	float ratioOfRatio = screenAspectRatio/aspectRatio;
	float quadHeightScale = 1;
	float tileWidth = 2.0/(numTiles);
	if(ratioOfRatio < 1)
	{
		quadHeightScale = ratioOfRatio;
	}
	else
	{
		tileWidth = tileWidth/ratioOfRatio;
	}

	if(quadHeightScale < .8)
	{
		quadHeightScale = quadHeightScale * 1/ratioOfRatio;
		tileWidth = tileWidth * 1/ratioOfRatio;
	}

	if(tileWidth*numTiles-scrollAmount > 2)
	{
		needsScroll = 1;
		scrollAmount = scrollAmount+.0004;
	}

	/* If we are auto advancing (and scrolling is done), display will be called frequently. We need
	 * to figure out if we actually need to load another image. */
	if(autoAdvance == 1 && needsScroll == 0)
	{
		// Not enough time elapsed, sleep, and display() again later:
		if(time(NULL)-lastAdvance < SLIDESHOW_WAIT)
		{
		  //			usleep(1000);
			glutPostRedisplay();
			return;
		}
		else // time to show new image:
		{
			// load next texture
			currentTexture = getNextTexture();
			// temporarily turn off auto-advance to ensure that image gets
			// drawn when loadCurrentTexture() calls display().
			autoAdvance = 0;
			loadCurrentTexture();
			autoAdvance = 1;
			// make sure we continue to call display()
			glutPostRedisplay();
			return;
		}
	}


	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1); // color of quad

	for(GLuint i=0; i<numTiles*2; i=i+2)
	{
		glBindTexture(GL_TEXTURE_2D, texNames[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2d(i/2    *tileWidth-1-scrollAmount, quadHeightScale*-1);
		glTexCoord2f(1.0, 0.0); glVertex2d((i/2+1)*tileWidth-1-scrollAmount, quadHeightScale*-1);
		glTexCoord2f(1.0, 1.0); glVertex2d((i/2+1)*tileWidth-1-scrollAmount, quadHeightScale*0);
		glTexCoord2f(0.0, 1.0); glVertex2d(i/2    *tileWidth-1-scrollAmount, quadHeightScale*0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texNames[i+1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2d(i/2    *tileWidth-1-scrollAmount, quadHeightScale*0);
		glTexCoord2f(1.0, 0.0); glVertex2d((i/2+1)*tileWidth-1-scrollAmount, quadHeightScale*0);
		glTexCoord2f(1.0, 1.0); glVertex2d((i/2+1)*tileWidth-1-scrollAmount, quadHeightScale*1);
		glTexCoord2f(0.0, 1.0); glVertex2d(i/2    *tileWidth-1-scrollAmount, quadHeightScale*1);
		glEnd();
	}
#if 1

	for(GLuint i=0; i<numTilesNEXT*2; i=i+2)
	{
	  //	  	  printf("here!\n");
		glBindTexture(GL_TEXTURE_2D, texNamesNEXT[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2d(0,0);
		glTexCoord2f(1.0, 0.0); glVertex2d(0,0);
		glTexCoord2f(1.0, 1.0); glVertex2d(0,0);
		glTexCoord2f(0.0, 1.0); glVertex2d(0,0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texNamesNEXT[i+1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2d(0,0);
		glTexCoord2f(1.0, 0.0); glVertex2d(0,0);
		glTexCoord2f(1.0, 1.0); glVertex2d(0,0);
		glTexCoord2f(0.0, 1.0); glVertex2d(0,0);
		glEnd();
	}
#endif


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
	char *str = globalargv[currentTexture+1];
	for(GLuint i=0; i<strlen(str); i++)
		glutBitmapCharacter(font, str[i]);


	glFlush();
	glutSwapBuffers();

	// if autoadvancing, call display again.
	// if image is wide enough to require scrolling, and isn't done scrolling, call display again.
	if(autoAdvance || needsScroll == 1)
	{
	  //		usleep(1000);
		glutPostRedisplay();
	}
}




void keyboard(unsigned char key, int x, int y)
{
  if (key == 'n' || key == ' ')
	{
	  printf("Advancing to next image...please wait...\n");

		// If we press spacebar, advance to next image even if scrolling
		// didn't finish---so we will artificially add a lot of scroll so
		// display() thinks scrolling is done.
		scrollAmount = 1000;

		// if auto-advance, just force next picture by adjusting time the
		// picture was first displayed!
		if(autoAdvance)
			lastAdvance = 0;
		else
		{
			currentTexture = getNextTexture();
			loadCurrentTexture();
		}
	}

	if (key == 'b' || key == 'p' || key == 73) // back or previous - 73=pageup
	{
	  printf("Advancing to previous image...please wait...\n");
		// If we press spacebar, advance to next image even if scrolling
		// didn't finish---so we will artificially add a lot of scroll so
		// display() thinks scrolling is done.
		scrollAmount = 1000;

		// if auto-advance, just force next picture by adjusting time the
		// picture was first displayed!
		if(autoAdvance)
		      lastAdvance = 0;
		else
		{
		  // load the previous texture into NEXT
                        currentTexture = getPrevTexture();
			glDeleteTextures(numTilesNEXT*2, texNamesNEXT);
		        aspectRatioNEXT = readfile(globalargv[currentTexture+1], texNamesNEXT, &numTilesNEXT);
			loadCurrentTexture();
		}
	}


	if (key == 27 || key == 'q')  // escape key, exit program
		exit(0);
	if(key == 's')
	{
		if(autoAdvance == 1)
		{
			printf("stopping auto-advance.\n");
			autoAdvance = 0;
		}
		else
		{
			printf("starting auto-advance.\n");
			autoAdvance = 1;
			glutPostRedisplay();
		}
	}

}

void special_keyboard(unsigned char key, int x, int y)
{
  if(key == GLUT_KEY_PAGE_DOWN)
    keyboard('n', 0,0);
  if(key == GLUT_KEY_PAGE_UP)
    keyboard('p', 0,0);
}

void init(void)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// prepopulate the NEXT variables since we loadCurrentTexture() assumes
	// we've preloaded the upcoming texture.
	aspectRatioNEXT = readfile(globalargv[1], texNamesNEXT, &numTilesNEXT);
	loadCurrentTexture();
}


////////////////////////////////////////////////////////
// main function
////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	totalTextures = argc-1;
	if(totalTextures == 0)
	{
		printf("ERROR: Provide textures to use.\n");
		exit(1);
	}
	globalargv = argv;

	glutInit(&argc, argv); //initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  //set display mode
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); //set window size
	glutInitWindowPosition(0, 0); //set window position on screen
	glutCreateWindow("High-Resolution Slideshow"); //open the screen window

	int glew_err = glewInit();
	if(glew_err != GLEW_OK)
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard); // pageup,pagedown, etc
	

	init();

	glutMainLoop();
}
