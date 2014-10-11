BIN=vicon-pong

GLEW_INC=
GLEW_LIB=-lGLEW

ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIB=-Wl,-rpath,/opt/viz/lib -L/opt/viz/lib -lGLEW
endif

ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-Wl,-rpath,/usr/local/glew/1.9.0/lib -L/usr/local/glew/1.9.0/lib -lGLEW
endif

VICON_LIB=-L../vicon-libs -Wl,-rpath,../vicon-libs -lViconDataStreamSDK_CPP
BASIC_LIB=-lGL -lstdc++ -lc -lm -lglut -lGLU

ALL_LIB=${GLEW_LIB} ${VICON_LIB} ${BASIC_LIB}
ALL_INC=${GLEW_INC}

FLAGS=-g -Wall
CC=g++

${BIN}: Client.h vicon-pong.cpp Makefile
	$(CC) $(FLAGS) -o ${BIN} ${ALL_INC} vicon-pong.cpp ${ALL_LIB}

clean:
	rm ${BIN} 
