EXEC=modelload
CC=g++
FLAGS=-O2 -g -Wall -fpermissive

GLEW_INC=
GLEW_LIB=-lGLEW

# Hardcode path to GLEW since it is installed in a non-standard location.
ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIB=-Wl,-rpath,/opt/viz/lib -L/opt/viz/lib -lGLEW
endif

ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-Wl,-rpath,/usr/local/glew/1.9.0/lib -L/usr/local/glew/1.9.0/lib -lGLEW
endif

ASSIMP_INC=-I../assimp-lib/include/assimp
ASSIMP_LIB=-L../assimp-lib/lib -Wl,-Bstatic -lassimp -Wl,-Bdynamic -lz

MAGICK_INC=-I/usr/include/ImageMagick
MAGICK_LIB=-lMagick++

ALL_INC=${GLEW_INC} ${ASSIMP_INC} ${MAGICK_INC}
ALL_LIB=${GLEW_LIB} ${ASSIMP_LIB} ${MAGICK_LIB} -lglut -lX11 -lGL -lGLU -lstdc++ -lc -pthread



all: *.cpp
	${CC} ${FLAGS} ${ALL_INC} -o ${EXEC} *.cpp ${ALL_LIB}
