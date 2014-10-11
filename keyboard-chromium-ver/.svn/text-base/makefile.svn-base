BIN=keyboard
SRC=keyboard.cpp

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

BOOST_INC=-I../boost_1_53_0/

VICON_LIBS=-L../vicon-libs -Wl,-rpath,../vicon-libs -lViconDataStreamSDK_CPP
BASIC_LIBS=-lGLEW -lglut -lX11 -lGL -lGLU -lstdc++ -lc -lm -pthread -lncurses
MAGICK_LIBS=`Magick-config --ldflags --libs --cflags`

ALL_LIBS=${GLEW_LIBS} ${MAGICK_LIBS} ${VICON_LIBS} ${BASIC_LIBS}
ALL_INC=${GLEW_INC} ${BOOST_INC}

${BIN}: Client.h ${SRC} makefile
	g++ -fpermissive -g -Wall -o ${BIN} ${ALL_INC} ${SRC} ${ALL_LIBS}

clean:
	rm ${BIN}
