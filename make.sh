echo ---------------------------------------------------------------------
STD=gnu11
PACKAGES="sdl2 SDL2_image"
CFLAGS=$(pkg-config --cflags $PACKAGES)
OPTS="-g -Wall -Wextra -Werror -Wno-unused-parameter"
LIBS="-lenet -pthread"
LDFLAGS=$(pkg-config --libs $PACKAGES)
gcc --std=$STD $OPTS $CFLAGS -o bin/gw gw.c $LDFLAGS $LIBS

