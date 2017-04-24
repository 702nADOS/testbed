#!/bin/sh
: ${TRACK:=a-speedway}
export TRACK

: ${SUMO_HOME:=$(pwd)/../simulators/sumo/}
export SUMO_HOME

# download submodules
git submodule update --init --depth=1

# compilation
# simulators
cd ../simulators/

cd sumo
# fix automake
aclocal
automake --add-missing
autoreconf
# ---
./configure CXXFLAGS="--std=c++11" && make -j4
cd ..

cd speed-dreams
mkdir -p build
cd build
cmake -DOPTION_SIMCOUPLER=true ../ && make -j4
cd ../../

cd ..
# simcoupler
cd SimCoupler
mkdir -p build
cd build
cmake ../ && make -j4
cd ../../

# startup
tmux new-session -d './simulators/sumo/bin/sumo-gui \
                      --remote-port=2001 \
                      -r files/TrackConverter/${TRACK}/sumoBuild/Track01.rou.xml \
                      -n files/TrackConverter/${TRACK}/sumoBuild/Track01.net.xml \
                      --start \
                      --step-length 0.001 \
                      --lateral-resolution=1'
tmux split-window -v 'sleep 1;./tools/TraCI-Proxy/TraCI-Proxy.py 127.0.0.1 2001 127.0.0.1 2002'
tmux split-window -h './SimCoupler/build/main'
tmux split-window -v './simulators/speed-dreams/build/games/speed-dreams-2'
tmux a
