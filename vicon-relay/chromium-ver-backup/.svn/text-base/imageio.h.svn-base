// Submit system filename: hw6/D.wwkohlst/imageio.h
#ifndef __IMAGEIO_H__
#define __IMAGEIO_H__
/* Author: Scott Kuhl
   imageio - Version 2.2
*/


/* SUMMARY: This code uses the ImageMagick or GraphicsMagick libraries to
   write (or read) images to (or from) disk in any of the image file
   formats that ImageMagick or GraphicsMagick supports.  See
   www.imagemagick.org or www.graphicsmagick.org for more information.  */

/* LICENSE: imageio.c and imageio.h are in the public domain and no warranty
   is implied or given.  Use this software at your own risk.  You can use
   this code in any way you wish and it is not necessary to give the author
   credit.

   NOTE: This software is meant to be compiled against the ImageMagick or
   GraphicsMagick libraries.  These libraries have their own licenses which
   you may also have to comply with.  */

/* COMPILATION/USAGE:
   See the Makefile information for the flags necessary for compilation.

   This software comes with several files:
   imageio.h and imageio.c: Image output/input routines.

   imageio-test.c: Compiles into a program which tests if imageio.c is
   working properly by writing and reading images back in.

   imageio-read.c: Compiles into a program that reads and prints the
   values of each pixel the image.
*/


/* ORDERING OF PIXELS: All of the functions in imageio.c and imageio.h
   assume that the image is represented by a 1D array of numbers (row major
   order, origin at the bottom left of the image).  The first "component"
   items in the array specify the bottom left pixel.  The last "component"
   items in the entire array specify the top right pixel.
*/


/* This macro returns the appropriate index into the 1D array given X
   (horizontal) and Y (vertical) coordinates.  'component' is the
   component number that you are interested in.  'width' is the width
   of the image and 'totalComponents' is the number of components per
   pixel. */
#define XY2INDEX(x,y,component, width, totalComponents) \
( (x+y*width)*totalComponents+component )

#define IMAGEIO_DEBUG 0

#include <magick/api.h>

typedef struct {

    /* image dimensions */
    unsigned long width;
    unsigned long height;

    /* file information */
    char *filename;
    unsigned long depth; /* Try to write file with this bit depth */
    int quality;  /* 0--100 (quality for lossy compression formats) */
    char *comment;  /* comment to store in image file */

    /* type of array: CharPixel, DoublePixel, FloatPixel, IntegerPixel,
       LongPixel, or ShortPixel.  Range of char/int/short are all
       [0--MaxValueOfType].  Range of float and double are [0--1]. */
    StorageType type;

    /* Example values for map:
       RGB
       RGBA
       RGBP  (rgb image + 1 padding)
       I     (I=grayscale)
       IA    (grayscale+alpha)
       CMYK
    */
    char *map;

} imageio_info;


/* Returns an array of pixels or NULL if there is an error reading the
 * file.  If iio_info->comment != NULL after imagein returns, the caller
 * should eventually free() it. */
void* imagein(imageio_info *iio_info);

/* Writes the given image stored in "array" out to the filename specified
 * in iio_info.  Returns 1 if successful.  Prints an error message and
 * returns 0 if there is a failure. */
int imageout(imageio_info *iio_info, void* array);


/* ImageMagick/Graphicsmagick puts the origin at the top left of the image.
We follow the OpenGL convention and put the origin at the bottom left
(unless you define the following variable). */
#if 0
#define ORIGIN_TOP_LEFT 1
#endif

#endif
