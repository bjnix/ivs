#!/usr/bin/env bash

#DIR="/home/jwwalker/vrlab/ivs/dgr-assimp"
DIR="/research/kuhl/vrlab/ivs/dgr-assimp"

rocks run host tile-0-0 command="DISPLAY=tile-0-0:0.0 ${DIR}/DGRAssimpSlave 0 0.5 0.25    0.5" &
rocks run host tile-0-1 command="DISPLAY=tile-0-1:0.0 ${DIR}/DGRAssimpSlave 0 0.5 0        0.25" &
rocks run host tile-0-2 command="DISPLAY=tile-0-2:0.0 ${DIR}/DGRAssimpSlave 0 0.5 -0.25   0" &
rocks run host tile-0-3 command="DISPLAY=tile-0-3:0.0 ${DIR}/DGRAssimpSlave 0 0.5 -0.5    -0.25" &

rocks run host tile-0-4 command="DISPLAY=tile-0-4:0.0 ${DIR}/DGRAssimpSlave -0.5 0 0.25   0.5" &
rocks run host tile-0-5 command="DISPLAY=tile-0-5:0.0 ${DIR}/DGRAssimpSlave -0.5 0 0       0.25" &
rocks run host tile-0-6 command="DISPLAY=tile-0-6:0.0 ${DIR}/DGRAssimpSlave  -0.5 0 -0.25  0" &
rocks run host tile-0-7 command="DISPLAY=tile-0-7:0.0 ${DIR}/DGRAssimpSlave -0.5 0 -0.5  -0.25" &


# rocks run host tile-0-0 command="DISPLAY=tile-0-0:0.0 ${DIR}/DGRAssimpSlave -0.5 0 -0.5  -0.25" &
# rocks run host tile-0-1 command="DISPLAY=tile-0-1:0.0 ${DIR}/DGRAssimpSlave -0.5 0 -0.25  0" &
# rocks run host tile-0-2 command="DISPLAY=tile-0-2:0.0 ${DIR}/DGRAssimpSlave -0.5 0 0       0.25" &
# rocks run host tile-0-3 command="DISPLAY=tile-0-3:0.0 ${DIR}/DGRAssimpSlave -0.5 0 0.25   0.5" &

# rocks run host tile-0-4 command="DISPLAY=tile-0-4:0.0 ${DIR}/DGRAssimpSlave 0 0.5 -0.5    -0.25" &
# rocks run host tile-0-5 command="DISPLAY=tile-0-5:0.0 ${DIR}/DGRAssimpSlave 0 0.5 -0.25   0" &
# rocks run host tile-0-6 command="DISPLAY=tile-0-6:0.0 ${DIR}/DGRAssimpSlave 0 0.5 0        0.25" &
# rocks run host tile-0-7 command="DISPLAY=tile-0-7:0.0 ${DIR}/DGRAssimpSlave 0 0.5 0.25    0.5" &

./DGRAssimpMaster
