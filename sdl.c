// vim: nu et ts=8 sts=2 sw=2

typedef struct Environment {
  SDL_Window* window;
  SDL_Surface* screen;
  SDL_Renderer* renderer;
  PxSz screenSize;
  bool haveJoystick;
} Environment;

__attribute__((noreturn))
void die(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(1);
}

__attribute__((noreturn))
void dieSDL(const char* failedFunctionName) {
  fprintf(stderr, "%s: %s\n", SDL_GetError());
  exit(1);
}

void ResizeWindow(Environment* env) {
  SDL_GetRendererOutputSize(env->renderer, &env->screenSize.w, &env->screenSize.h);
}

void InitSDL(Environment* env) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    dieSDL("SDL_Init");
  env->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!env->window)
    dieSDL("SDL_CreateWindow");
  env->screen = SDL_GetWindowSurface(env->window);
  if (!env->screen)
    dieSDL("SDL_GetWindowSurface");
  env->renderer = SDL_GetRenderer(env->window);
  if (!env->renderer)
    dieSDL("SDL_GetRenderer");
  ResizeWindow(env);
}

// Library setup.
void Init(Environment* env) {
  InitSDL(env);
}

Uint64 GetTime() {
  return (Uint64)TIMER_MILLISECOND_MULTIPLIER * SDL_GetTicks();
}

