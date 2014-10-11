# link with libraries in this path.
LIBLOC=-L./bin
LIBS= ./bin/libViconDataStreamSDK_CPP.so*   
FLAGS=-Wl,-R./bin/$$ORIGIN

all:
	# g++ -c $(SRC) 
	# $ORIGIN must be used to allow for dynamic linking to SO files that are
	# located relative to the executable. 
	g++ -o ./bin/test.out $(FLAGS) $(LIBLOC) Markers.cpp $(LIBS)
clean:
	rm -rf ./bin/*.o ./bin/*.out
