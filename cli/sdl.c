// vim: nu et ts=8 sts=2 sw=2

typedef struct Environment {
  SDL_Window* window;
  SDL_Surface* screen;
  SDL_Renderer* renderer;
  PxSz screenSize;
  int shortDimension;
  bool isOrientationHorizontal;
  bool haveJoystick;
} Environment;

#define dieSDL(FAILED_FUNCTION_NAME) \
  die("%s: %s", (FAILED_FUNCTION_NAME), SDL_GetError());

void ResizeWindow(Environment* env) {
  SDL_GetRendererOutputSize(env->renderer, &env->screenSize.w, &env->screenSize.h);
  env->isOrientationHorizontal = env->screenSize.w >= env->screenSize.h;
  env->shortDimension = env->isOrientationHorizontal ? env->screenSize.h : env->screenSize.w;
}

void InitSDL(Environment* env) {
  if (0 != SDL_Init(SDL_INIT_VIDEO))
    dieSDL("SDL_Init");
  env->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!env->window)
    dieSDL("SDL_CreateWindow");
  env->screen = SDL_GetWindowSurface(env->window);
  if (!env->screen)
    dieSDL("SDL_GetWindowSurface");
  // Add flag SDL_RENDERER_PRESENTVSYNC to enable vsync.
  env->renderer = SDL_CreateRenderer(env->window, -1, SDL_RENDERER_ACCELERATED);
  if (!env->renderer)
    dieSDL("SDL_CreateRenderer");
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    die("IMG_Init: %s", IMG_GetError());
  ResizeWindow(env);
}

SDL_Texture* loadImageAsTexture(Environment* env, const char* path) {
  SDL_Surface* surface = IMG_Load(path);
  if (!surface)
    die("IMG_Load(%s): %s", path, IMG_GetError());
  SDL_Texture* texture = SDL_CreateTextureFromSurface(env->renderer, surface);
  if (!texture)
    dieSDL("SDL_CreateTextureFromSurface");
  SDL_FreeSurface(surface);
  return texture;
}

Uint64 GetTime() {
  return (Uint64)TIMER_MILLISECOND_MULTIPLIER * SDL_GetTicks();
}

