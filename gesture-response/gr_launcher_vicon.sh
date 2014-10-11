#!/usr/bin/env bash

rocks run host tile-0-0 command="DISPLAY=tile-0-0:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave -0.5 0 -0.5  -0.25  3 FALSE" &
rocks run host tile-0-1 command="DISPLAY=tile-0-1:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave -0.5 0 -0.25  0     3 FALSE" &
rocks run host tile-0-2 command="DISPLAY=tile-0-2:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave -0.5 0 0       0.25 3 FALSE" &
rocks run host tile-0-3 command="DISPLAY=tile-0-3:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave -0.5 0 0.25   0.5   3 FALSE" &

rocks run host tile-0-4 command="DISPLAY=tile-0-4:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave 0 0.5 -0.5    -0.25 3 FALSE" &
rocks run host tile-0-5 command="DISPLAY=tile-0-5:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave 0 0.5 -0.25   0     3 FALSE" &
rocks run host tile-0-6 command="DISPLAY=tile-0-6:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave 0 0.5 0        0.25 3 FALSE" &
rocks run host tile-0-7 command="DISPLAY=tile-0-7:0.0 /research/jwwalker/vrlab/ivs/gesture-response/GestureResponseSlave 0 0.5 0.25    0.5   3 FALSE" &

./GestureResponseMaster FALSE 127.0.0.1 9875 P1HandL P1HandR Wand
