// vim: nu et ts=2 sts=2 sw=2

enum KeyDir { UP, DOWN, LEFT, RIGHT };

struct Input {
  bool move[4];
};

static enum KeyDir _directions[] = { UP, DOWN, LEFT, RIGHT };
static SDL_Scancode _directionScancodes[] = {
  SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
};
static struct Input _input = {0};

void InitJoystick(Environment* env) {
  env->haveJoystick = (SDL_NumJoysticks() > 0);
  if (env->haveJoystick) {
    SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(0);
    // TODO: Customize joystick setup.
  }
}

void HandleEvent_Keyboard(SDL_KeyboardEvent* event) {
  // TODO: Buffering of text input.
  if (event->type == SDL_KEYDOWN) {
    switch (event->keysym.sym) {
      case SDLK_q:
        exit(0); // TODO: Raise quit event.
      case SDLK_UP:
        _input.move[UP] = true;
        break;
      case SDLK_DOWN:
        _input.move[DOWN] = true;
        break;
      case SDLK_LEFT:
        _input.move[LEFT] = true;
        break;
      case SDLK_RIGHT:
        _input.move[RIGHT] = true;
        break;
    }
  }
}

void ScanKeyboard() {
  const Uint8* kbState = SDL_GetKeyboardState(NULL);
  // Arrow keys.
  for (int i=0; i < 4; ++i) {
    if (kbState[_directionScancodes[i]])
      _input.move[_directions[i]] = true;
  }
}

void InitInput(Environment* env) {
  InitJoystick(env);
}

void ScanInput(Environment* env) {
  ScanKeyboard();
  int moveX = 0, moveY = 0;
  if (_input.move[UP]) --moveY;
  if (_input.move[DOWN]) ++moveY;
  if (_input.move[LEFT]) --moveY;
  if (_input.move[RIGHT]) ++moveY;
  // TODO: Joystick.
  _input = (struct Input){0};
}

