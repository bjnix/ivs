# link with libraries in this path
TESTSRC= ./include/Math/*.cpp WindowTest.cpp
POINTERSRC= ./include/Math/*.cpp Pointer.cpp
VIRTWINSRC= ./include/Math/*.cpp VirtualWindow.cpp

VICON_LIB=-L../../vicon-libs -Wl,-rpath,../../vicon-libs -L../../vicon-libs/32bit -Wl,-rpath,../../vicon-libs/32bit -lViconDataStreamSDK_CPP
BASIC_LIB= -lglut -lX11 -lGL -lGLU -lm -lstdc++
GLEW_LIB=-lGLEW
WINDOW_LIB=-L./lib/ -Wl,-rpath,./lib/ -lWindow

ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIB=-Wl,-rpath,/opt/viz/lib -L/opt/viz/lib -lGLEW
endif

ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-Wl,-rpath,/usr/local/glew/1.9.0/lib -L/usr/local/glew/1.9.0/lib -lGLEW
endif

WINDOWSRC= ./src
WINDOWLIB= libWindow.so

window:
	g++ -Wall -fPIC -c $(WINDOWSRC)/*.cpp
	g++ -shared -Wl,-soname,$(WINDOWLIB) -o ./lib/$(WINDOWLIB) ./*.o $(VICON_LIB) $(BASIC_LIB)
test:
	g++ -o ./bin/Window.out -I./include/ $(GLEW_INC) $(TESTSRC) $(VICON_LIB) $(BASIC_LIB) $(GLEW_LIB) $(WINDOW_LIB)
pointer:
	g++ -o ./Pointer.out -I./include/ $(GLEW_INC) $(POINTERSRC) $(VICON_LIB) $(BASIC_LIB) $(GLEW_LIB) $(WINDOW_LIB)
vwindow:
	g++ -o ./Vwindow.out -I./include/ $(GLEW_INC) $(VIRTWINSRC) $(VICON_LIB) $(BASIC_LIB) $(GLEW_LIB) $(WINDOW_LIB)
all:
	make window
	make test
	make pointer
clean:
	rm -rf ./*.o ./bin/*.out ./*.out ./lib/*.so
