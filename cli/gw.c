// vim: nu et ts=8 sts=2 sw=2

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "SDL.h"
#include "SDL_image.h"

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const char WINDOW_TITLE[] = "GridWalk";

#define EVENT_QUEUE_SIZE 1024
#define LOGICAL_FRAMES_PER_SECOND 10

// Used to multiply times in order to increase the precision of tracking the
// frame duration.
#define TIMER_MILLISECOND_MULTIPLIER 8
//const Uint32 TIMER_MILLISECOND_MULTIPLIER = 8;

#include "die.c"
#include "data.c"
//#include "event.c"
#include "sdl.c"
#include "timer.c"
#include "input.c"
#include "sdlevent.c"
#include "game.c"
#include "net.c"
#include "draw.c"

void Init(Environment* env) {
  InitNet();
  InitSDL(env);
}

void MainLoop(Environment* env, GameState* gs) {
  FrameTimer timer;
  FrameTimer_Init(&timer, LOGICAL_FRAMES_PER_SECOND);
  for (;;) {
    while (FrameTimer_NextFrame(&timer)) {
      PollNet();
      PollEvents(env);
      ScanInput(env);
      Update(gs);
      fflush(stdout);
    }
    Draw(env, gs, timer.phase);
    SDL_Delay(0);
  }
}

int main(int argc, char** argv)
{
  Environment env = {0};
  GameState gs = {0};
  Init(&env);
  Load(&env, &gs);
  MainLoop(&env, &gs);
  return 0;
}

