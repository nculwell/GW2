echo ---------------------------------------------------------------------
PACKAGES="sdl2 SDL2_image"
OPTS="-g -Wall -Wextra -Werror -Wno-unused-parameter"
gcc --std=c11 $OPTS -o bin/gw gw.c $(pkg-config --cflags --libs $PACKAGES)
