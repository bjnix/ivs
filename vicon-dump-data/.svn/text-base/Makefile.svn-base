BIN=ViconDataStreamSDK_CPPTest

VICON_LIBS=-L../vicon-libs -Wl,-rpath,../vicon-libs -lViconDataStreamSDK_CPP

${BIN}: Client.h ViconDataStreamSDK_CPPTest.cpp Makefile
	g++ -o ${BIN} ViconDataStreamSDK_CPPTest.cpp ${VICON_LIBS}

clean:
	rm ${BIN} 
