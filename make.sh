echo ---------------------------------------------------------------------
PACKAGES="sdl2 SDL2_image"
gcc --std=c11 -o gw gw.c $(pkg-config --cflags --libs $PACKAGES)
