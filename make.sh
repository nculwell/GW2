echo ---------------------------------------------------------------------
PACKAGES="sdl2 SDL2_image"
gcc --std=c11 -o gw `pkg-config --cflags --libs $PACKAGES` gw.c
