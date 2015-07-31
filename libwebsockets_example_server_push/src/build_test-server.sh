#!/bin/sh
# wget http://git.warmcat.com/cgi-bin/cgit/libwebsockets/snapshot/libwebsockets-1.4-chrome43-firefox-36.tar.gz
# tar xvzf libwebsockets-1.4-chrome43-firefox-36.tar.gz
#cd libwebsockets-1.4-chrome43-firefox-36

# build
git clone https://github.com/warmcat/libwebsockets.git
cd libwebsockets
documentRoot=`pwd`
mkdir build
cd build
cmake ..
make

