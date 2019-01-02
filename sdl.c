// vim: nu et ts=8 sts=2 sw=2

typedef struct Environment {
  SDL_Window* window;
  SDL_Surface* screen;
  SDL_Renderer* renderer;
  PxSz screenSize;
} Environment;

void InitSDL(Environment* env) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    die("SDL_Init: %s\n", SDL_GetError());
  env->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!env->window)
    die("SDL_CreateWindow: %s\n", SDL_GetError());
  env->screen = SDL_GetWindowSurface(env->window);
  if (!env->screen)
    die("SDL_GetWindowSurface: %s\n", SDL_GetError());
  env->renderer = SDL_GetRenderer(env->window);
  if (!env->renderer)
    die("SDL_GetRenderer: %s\n", SDL_GetError());
  ResizeWindow(env);
}

// Library setup.
void Init(Environment* env) {
  InitSDL(env);
}

void ResizeWindow(Environment* env) {
  SDL_GetRendererOutputSize(env->renderer, &env->screenSize.w, &env->screenSize.h);
}

Uint64 GetTime() {
  return (Uint64)TIMER_MILLISECOND_MULTIPLIER * SDL_GetTicks();
}

