#!/bin/sh

# Tested with MinGW64, should work on Linux as well.
# Install packages for SDL2, SDL2_image and enet.

STD=gnu11
PACKAGES="sdl2 SDL2_image"
CFLAGS=$(pkg-config --cflags $PACKAGES)
OPTS="-g -Wall -Wextra -Werror -Wno-unused-parameter"
LIBS="-lenet -pthread -lWs2_32"
LDFLAGS=$(pkg-config --libs $PACKAGES)
echo ---------------------------------------------------------------------
gcc --std=$STD $OPTS $CFLAGS -o bin/gw gw.c $LDFLAGS $LIBS
echo ---------------------------------------------------------------------
gcc --std=$STD $OPTS $CFLAGS -DNET_TEST -o bin/net net.c $LDFLAGS $LIBS

