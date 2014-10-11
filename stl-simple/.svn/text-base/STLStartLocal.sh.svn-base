#!/usr/bin/env bash
set -e # exit script if any command returns a non-zero exit code.

# Tell binaries where the glew library is installed:
export LD_LIBRARY_PATH="/usr/local/glew/1.9.0/lib:$LD_LIBRARY_PATH"

./STLRenderMaster 127.0.0.1 &

# We must run a relay because the master sends to a port that only the
# relay listens to---and then the relay sends out using a different
# port number that only the slave listens to.
./STLRelay 127.0.0.1 &

# We can only run one slave on a single computer because multiple
# programs cannot listen on the same port.
./STLRenderSlave -.05 0 -.05 0 256 256 &


