MASEXEC=DGRMaster
SLVEXEC=DGRSlave
RLYEXEC=DGRRelay

HEADERS=

GLEW_INC=
GLEW_LIB=-lGLEW

ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIB=-L/opt/viz/lib -lGLEW
endif

# We statically link GLEW on CCSR with our binaries so when we
# transfer the binaries to the IVS tiles, glew works properly. CCSR
# and IVS have different glew versions installed in different places
# with different library filenames.
ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-L/usr/local/glew/1.9.0/lib -Wl,-Bstatic -lGLEW -Wl,-Bdynamic
endif

ALL_INC=${GLEW_INC}
ALL_LIB=${GLEW_LIB} -lglut -lX11 -lGL -lGLU -lstdc++ -lc -pthread 

MUTSOURCE=DGRMutant.cpp
RLYSOURCE=DGRRelay.cpp

CC=g++

FLAGS=-fpermissive -O2 -g -Wall


all: $(SLVEXEC) $(RLYEXEC) $(MASEXEC)

$(MASEXEC): $(MUTSOURCE) $(HEADERS)
	@echo "=== COMPILING MASTER ==="
	$(CC) -DDGR_MASTER=1 $(FLAGS) $(MUTSOURCE) -o $(MASEXEC) $(ALL_INC) $(ALL_LIB)
	@echo "=== COMPILING SLAVE ==="
	$(CC) $(FLAGS) $(MUTSOURCE) -o $(SLVEXEC) $(ALL_INC) $(ALL_LIB)

$(SLVEXEC): $(MASEXEC)

$(RLYEXEC): $(RLYSOURCE)
	@echo "=== COMPILING RELAY ==="
	$(CC) $(FLAGS) $(RLYSOURCE) -o $(RLYEXEC) $(ALL_INC) $(ALL_LIB)

clean:
	rm -f $(SLVEXEC) $(RLYEXEC) $(MASEXEC) *.o
