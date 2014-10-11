#!/usr/bin/env bash

ssh -t ivs.research.mtu.edu <<EOF 
killall STLRelay
rocks run host tile-0-0 tile-0-1 tile-0-2 tile-0-3 tile-0-4 tile-0-5 tile-0-6 tile-0-7 command="killall STLRenderSlave"
EOF

