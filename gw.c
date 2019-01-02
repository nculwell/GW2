// vim: nu et ts=8 sts=2 sw=2

#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

const unsigned int LOGICAL_FRAMES_PER_SECOND = 10;
const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const char[] WINDOW_TITLE = "GridWalk";

#include "data.c"
#include "init.c"
#include "timer.c"
#include "game.c"
#include "draw.c"

void mainLoop(Environment* env, GameState* gs) {
  MainLoopTimer timer;
  MainLoopTimer_Init(&timer, LOGICAL_FRAMES_PER_SECOND);
  for (;;) {
    while (MainLoopTimer_NextFrame(&timer)) {
      update(gs);
    }
    draw(env, gs, timer->phase);
  }
}

int main(int argc, char** argv)
{
  Environment env = {0};
  GameState gs = {0};
  Init(&env);
  load(&env, &gs);
  mainLoop(&env, &gs);
  return 0;
}

