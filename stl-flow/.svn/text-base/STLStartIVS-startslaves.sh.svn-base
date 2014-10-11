#!/usr/bin/env bash
#
# This script is called by STLSTartIVS.sh and is run directly on the
# IVS computer. You should not run this script directly because it
# does not start a master copy of the program on your local computer.

set -e # exit script if any command returns a non-zero exit code.

# The hostname that we expect to be running on:
EXPECT_HOSTNAME="ivs.research.mtu.edu"

if [[ ${HOSTNAME} != ${EXPECT_HOSTNAME} ]]; then
    echo "This script ($0) only works with $EXPECT_HOSTNAME. You are INCORRECTLY running it on ${HOSTNAME}."
    exit
fi


echo "Starting slaves..."
ROCKS_BIN=/opt/rocks/bin/rocks
STL_BIN=./STLRenderSlave
DIR=`pwd`

${ROCKS_BIN} run host tile-0-0 command="cd $DIR && DISPLAY=tile-0-0:0.0 ${STL_BIN} -0.05 0 -0.05  -0.025 5760 1080" &
${ROCKS_BIN} run host tile-0-1 command="cd $DIR && DISPLAY=tile-0-1:0.0 ${STL_BIN} -0.05 0 -0.025  0     5760 1080" &
${ROCKS_BIN} run host tile-0-2 command="cd $DIR && DISPLAY=tile-0-2:0.0 ${STL_BIN} -0.05 0 0       0.025 5760 1080" &
${ROCKS_BIN} run host tile-0-3 command="cd $DIR && DISPLAY=tile-0-3:0.0 ${STL_BIN} -0.05 0 0.025   0.05  5760 1080" &

${ROCKS_BIN} run host tile-0-4 command="cd $DIR && DISPLAY=tile-0-4:0.0 ${STL_BIN} 0 0.05 -0.05  -0.025 5760 1080" &
${ROCKS_BIN} run host tile-0-5 command="cd $DIR && DISPLAY=tile-0-5:0.0 ${STL_BIN} 0 0.05 -0.025  0     5760 1080" &
${ROCKS_BIN} run host tile-0-6 command="cd $DIR && DISPLAY=tile-0-6:0.0 ${STL_BIN} 0 0.05  0      0.025 5760 1080" &
${ROCKS_BIN} run host tile-0-7 command="cd $DIR && DISPLAY=tile-0-7:0.0 ${STL_BIN} 0 0.05  0.025  0.05  5760 1080" &

echo "Starting relay..."
./STLRelay 10.2.255.255 &

