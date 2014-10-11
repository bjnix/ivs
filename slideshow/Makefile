EXEC=slideshow
HEADERS=
SOURCE=slideshow.cpp imageio.cpp

CC=g++

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

BASIC_LIBS=-lGL -lstdc++ -lc -lm -lglut -lGLU
MAGICK_LIBS=`Magick-config --ldflags --libs --cflags`

ALL_LIBS=${GLEW_LIBS} ${MAGICK_LIBS} ${BASIC_LIBS}
ALL_INC=${GLEW_INC}

FLAGS=-g -fpermissive -Wall

$(EXEC): $(SOURCE) $(HEADERS)
	$(CC) -o $(EXEC) $(FLAGS) $(ALL_INC) $(SOURCE) $(ALL_LIBS)
	chmod --quiet a+rx $(EXEC) chromium
	chmod --quiet a+r *


clean:
	rm -f $(EXEC) *.o
