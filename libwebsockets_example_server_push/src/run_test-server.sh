#!/bin/sh
documentRoot=`pwd`

libdir=libwebsockets
if [ ! -d $libdir ]; then
  . ./build_test-server.sh
fi

cd $libdir/build/bin
./libwebsockets-test-server --resource_path=$documentRoot/test-server
