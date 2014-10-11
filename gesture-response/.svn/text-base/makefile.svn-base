SLVEXEC=GestureResponseSlave
MSTEXEC=GestureResponseMaster

HEADERS=

SLVSOURCE=GestureResponseSlave.cpp
MSTSOURCE=GestureResponseMaster.cpp

CC=g++

FLAGS=-O2

LIBS=-L/share/apps/glew/1.9.0/lib -lGLEW -lglut -lX11 -lGL -lGLU -lstdc++ -lc -lm -pthread -lncurses

all: $(SLVEXEC) $(MSTEXEC)

$(SLVEXEC): $(SLVSOURCE) $(HEADERS)
	$(CC) -fpermissive $(SLVSOURCE) -o $(SLVEXEC) -I../boost_1_53_0/ $(LIBS) 

$(MSTEXEC): $(MSTSOURCE) $(HEADERS)
	$(CC) -fpermissive $(MSTSOURCE) -o $(MSTEXEC) -I../boost_1_53_0/ $(LIBS) -L../vicon-libs -Wl,-rpath,../vicon-libs -lViconDataStreamSDK_CPP

clean:
	rm -f $(SLVEXEC) $(MSTEXEC) *.o
