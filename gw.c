// vim: nu et ts=8 sts=2 sw=2

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

const unsigned int LOGICAL_FRAMES_PER_SECOND = 10;
const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const char[] WINDOW_TITLE = "GridWalk";

// Used to multiply times in order to increase the precision of tracking the
// frame duration.
const Uint32 TIMER_MILLISECOND_MULTIPLIER = 8;

#include "data.c"
#include "sdl.c"
#include "timer.c"
#include "input.c"
#include "sdlevent.c"
#include "game.c"
#include "draw.c"

void Init(Environment* env) {
  InitSDL(env);
}

void MainLoop(Environment* env, GameState* gs) {
  FrameTimer timer;
  FrameTimer_Init(&timer, LOGICAL_FRAMES_PER_SECOND);
  for (;;) {
    while (FrameTimer_NextFrame(&timer)) {
      ScanInput(env);
      Update(gs);
    }
    Draw(env, gs, timer->phase);
  }
}

int main(int argc, char** argv)
{
  Environment env = {0};
  GameState gs = {0};
  Init(&env);
  load(&env, &gs);
  MainLoop(&env, &gs);
  return 0;
}

