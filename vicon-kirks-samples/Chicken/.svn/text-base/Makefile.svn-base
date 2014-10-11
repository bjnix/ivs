# link with libraries in this path
SRC= ./OPaudio/*.cpp
LIBLOC=-L./bin
LIBS= -lopenal ./libs/libViconDataStreamSDK_CPP.so* ./libs/libogg.so* ./bin/libboost* ./libs/libvorbisfile.so* ./libs/libvorbis.so*
FLAGS=-Wl,-R./bin/$$ORIGIN

all:
	# g++ -c $(SRC) 
	# $ORIGIN must be used to allow for dynamic linking to SO files that are
	# located relative to the executable. 
	g++ -o ./bin/test.out $(FLAGS) $(LIBLOC) $(SRC) Chicken.cpp $(LIBS)
clean:
	rm -rf ./bin/*.o ./bin/*.out
