echo ---------------------------------------------------------------------
PACKAGES="sdl2 SDL2_image"
CFLAGS=$(pkg-config --cflags $PACKAGES)
OPTS="-g -Wall -Wextra -Werror -Wno-unused-parameter"
LIBS=-lenet
LDFLAGS=$(pkg-config --libs $PACKAGES)
gcc --std=c11 $OPTS $CFLAGS -o bin/gw gw.c $LDFLAGS $LIBS

