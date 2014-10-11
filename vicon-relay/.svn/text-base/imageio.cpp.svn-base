// Submit system filename: hw6/D.wwkohlst/imageio.c
/* Author: Scott Kuhl
   See imageio.h for documentation and license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "imageio.h"


int imageio_type_to_bytes(StorageType t)
{
    switch(t)
    {
	    case CharPixel:    return sizeof(char);
	    case DoublePixel:  return sizeof(double);
	    case FloatPixel:   return sizeof(float);
	    case IntegerPixel: return sizeof(int);
	    case LongPixel:    return sizeof(long);
	    case ShortPixel:   return sizeof(short);
	    default:
		    fprintf(stderr, "imageio_type_to_bytes: Wrong type\n");
		    return 0;
    }
}

/* Imagemagick defaults with the first pixel at the upper left corner.
 * OpenGL and other packages put the first pixel at the bottom left corner.
 * This code flips the image depending on how imageio was compiled. */
Image* flip(Image *image)
{
#ifdef ORIGIN_TOP_LEFT
    return image;
#else
    /* Flip image so origin is at bottom left. */
    ExceptionInfo exception;
    GetExceptionInfo(&exception);
    image = FlipImage(image, &exception);
    MagickError(exception.severity, exception.reason, exception.description);
    return image;
#endif
}

/* In ImageMagick (and this program):
   'A' or alpha means 0=transparent, 1=opaque
   'O' or opacity means 0=opaque, 1=transparent

   The mappings from GraphicsMagick to ImageMagick:

   IM  -> GM
   ---------------
   O   -> A
   A   -> O

   T is in GraphicsMagick, but not ImageMagick.  We will ignore it.

   If using graphicsmagick, convertmap() will convert the map as described
   above.  Caller must free the returned string. */
char* convertmap(const char* input)
{
    char* ret = (char*) malloc(sizeof(char)*strlen(input)+1);
    strcpy(ret, input);

    /* If using ImageMagick */
    if(strcmp(MagickPackageName, "ImageMagick") == 0)
	return ret;

    /* If using GraphicsMagick */
    if(strcmp(MagickPackageName, "GraphicsMagick") == 0)
    {
	for(unsigned int i=0; i<strlen(ret); i++)
	{
	    if(ret[i] == 'A')
		ret[i] = 'O';
	    else if(ret[i] == 'O')
		ret[i] = 'A';
	}
	return ret;
    }
    else
    {
	    fprintf(stderr, "Unknown magick package: %s\n", MagickPackageName);
	    exit(1);
    }
}

/* This method checks if there is an alpha component in the current map.
 * This should return MagickTrue/MagickFalse, but it is a compile #define
 * in graphicsmagick and a specific type in imagemagick---and returning
 * ints will not produce any compiler warnings...
 */
int hasAlpha(const char *m)
{
	for(int i=0; i<(int) strlen(m); i++)
		if(m[i]=='A' || m[i] == 'O')
			return 1;
	return 0;
}

int imageout(imageio_info *iio_info, void* array)
{
    Image *image = NULL;
    ImageInfo image_info;
    ExceptionInfo exception;
//    int bytes_per_pixel   = strlen(iio_info->map) * imageio_type_to_bytes(iio_info->type);

    if(IMAGEIO_DEBUG) {
	    printf("imageout %s: Trying to write file with %lu bit/channel.\n", iio_info->filename, iio_info->depth);
	    printf("imageout %s: You provided %d bit/channel %s.\n",
	           iio_info->filename, 8*imageio_type_to_bytes(iio_info->type), iio_info->map);
	    printf("imageout %s: Dimensions: %lu x %lu\n", iio_info->filename, iio_info->width, iio_info->height);
    }

    /* Prepare to write the image. */
    InitializeMagick(NULL);
    GetExceptionInfo(&exception);
    char *map = convertmap(iio_info->map);
    image = ConstituteImage(iio_info->width, iio_info->height, map, iio_info->type,
			    array, &exception);
    image->matte = hasAlpha(map) == 1 ? MagickTrue : MagickFalse ;
    free(map);
    MagickError(exception.severity, exception.reason, exception.description);
    image = flip(image);
    SyncImagePixels(image);

    /* Set image comment */
    if(iio_info->comment != NULL)
        SetImageAttribute(image, "comment", iio_info->comment);
    /* Set other image information. */
    GetImageInfo( &image_info );
    image_info.quality = iio_info->quality;
    image_info.depth   = iio_info->depth;
    strncpy(image_info.filename, iio_info->filename, MaxTextExtent-1);
    strncpy(image->filename, iio_info->filename, MaxTextExtent-1);

    /* Write the image out to a file */
    if( WriteImage( &image_info, image ) == MagickFalse )
    {
	fprintf(stderr, "ERROR writing %s: %s\n", iio_info->filename, image->exception.reason);
	return 0;
    }

    DestroyImage(image);

    if(IMAGEIO_DEBUG)
	printf("imageout %s: DONE\n", iio_info->filename);

    return 1;
}



void* imagein(imageio_info *iio_info)
{
    Image *image = NULL;
    ImageInfo image_info;
    ExceptionInfo exception;
    int bytes_per_pixel   = strlen(iio_info->map) * imageio_type_to_bytes(iio_info->type);
    void *array = NULL;;

    if(IMAGEIO_DEBUG)
    {
	    printf("imagein  %s: Requested %d bit/channel %s\n", iio_info->filename, 8*imageio_type_to_bytes(iio_info->type), iio_info->map);
	    printf("imagein  %s: Reading file...\n", iio_info->filename);
    }

    /* read in the image */
    InitializeMagick(NULL);
    GetImageInfo( &image_info );
    strncpy(image_info.filename, iio_info->filename, MaxTextExtent-1);
    GetExceptionInfo(&exception);
    image = ReadImage(&image_info, &exception);
    if(image == NULL)
    {
	    fprintf(stderr, "ERROR reading %s: %s\n", iio_info->filename, exception.reason);
	return NULL;
    }
    image = flip(image);

    if(IMAGEIO_DEBUG)
    {
	    printf("imagein  %s: Finished reading from disk.\n", iio_info->filename);
	//printf("imagein  %s: File is %u bit/channel\n", iio_info->filename, image->depth);
	    printf("imagein  %s: Dimensions: %lu x %lu\n", iio_info->filename, image->columns, image->rows);
    }

    /* allocate array to copy data into */
    array = malloc(bytes_per_pixel*image->columns*image->rows);
    if(array == NULL)
    {
	printf("imagein  %s: Failed to allocate enough memory.\n", iio_info->filename);
	return NULL;
    }

    /* Copy the data into the array */
    char *map = convertmap(iio_info->map);
    GetExceptionInfo(&exception);
    DispatchImage(image, 0, 0, image->columns, image->rows, map,
                  iio_info->type, array, &exception);
    free(map);
    MagickError(exception.severity, exception.reason, exception.description);
    SyncImagePixels(image);

    /* Copy information from the file into our iio_info struct */
    iio_info->width = image->columns;
    iio_info->height = image->rows;
    iio_info->quality = -1;  /* N/A */
    iio_info->depth = image->depth;
    if(GetImageAttribute(image, "comment") != NULL)
    {
	    char* imagecomment = GetImageAttribute(image, "comment")->value;
	    iio_info->comment = (char*) malloc(sizeof(char)*strlen(imagecomment));
	    strcpy(iio_info->comment, imagecomment);
    }
    else
	    iio_info->comment = NULL;

    DestroyImage(image);

    if(IMAGEIO_DEBUG)
	printf("imagein  %s: DONE.\n", iio_info->filename);

    return array;
}
