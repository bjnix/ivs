BIN=MoleculeDisplay

# CCSR: glew is installed in /usr/local/glew/VERSION
# IVS: glew is installed in /opt/viz/
GLEW_INC=
GLEW_LIBS=-lGLEW

ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIBS=-Wl,-rpath,/opt/viz/lib -L/opt/viz/lib -lGLEW
endif

ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIBS=-Wl,-rpath,/usr/local/glew/1.9.0/lib -L/usr/local/glew/1.9.0/lib -lGLEW
endif

VICON_LIBS=-L../vicon-libs -Wl,-rpath,../vicon-libs -lViconDataStreamSDK_CPP
BASIC_LIBS=-lGLEW -lglut -lX11 -lGL -lGLU -lstdc++ -lc -lm -pthread -lncurses

ALL_LIBS=${GLEW_LIBS} ${VICON_LIBS} ${BASIC_LIBS}
ALL_INC=${GLEW_INC}

${BIN}: Client.h MoleculeDisplay.cpp makefile
	g++ -fpermissive -g -Wall -o ${BIN} ${ALL_INC} MoleculeDisplay.cpp ${ALL_LIBS}

clean:
	rm ${BIN}
