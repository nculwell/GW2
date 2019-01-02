echo ---------------------------------------------------------------------
gcc --std=c11 -o gw -Dmain=SDL_main -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -mwindows gw.c
