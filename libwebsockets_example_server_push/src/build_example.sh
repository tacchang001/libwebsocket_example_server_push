#!/bin/sh
libdir=libwebsockets

gcc main.c -o example -I$libdir/lib -I$libdir/build -L$libdir/build/lib -lwebsockets

