// vim: nu et ts=8 sts=2 sw=2

void DrawTiles(Environment* env, GameState* gs, phase_t phase, PxRect rect,
    CxRC offset) {
  for (int r=rect.off.y; r < rect.off.y + rect.sz.h; ++r) {
    for (int c=rect.off.x; c < rect.off.x + rect.sz.w; ++c) {
    }
  }
}

void DrawCharacters(Environment* env, GameState* gs, phase_t phase) {
}

void DrawMap(Environment* env, GameState* gs, phase_t phase, PxRect rect, CxRC offset) {
  // TODO: Make centering/offset pixel-precise for smooth scrolling.
  DrawTiles(env, gs, phase, rect, offset);
  DrawCharacters(env, gs, phase);
}

void DrawUI(Environment* env, GameState* gs) {
  // TODO: Draw sidebar.
  PxRect sidebarRect;
  int shortDim = env->shortDimension;
  if (env->isOrientationHorizontal) {
    sidebarRect = (struct PxRect){ { .x = shortDim, .y = 0 },
      { .w = env->screenSize.w - shortDim, .h = env->screenSize.h } };
  } else {
    sidebarRect = (struct PxRect){ { .x = 0, .y = shortDim },
      { .w = env->screenSize.w, .h = env->screenSize.h - shortDim } };
  }
  if (0 != SDL_RenderDrawRect(env->renderer, (SDL_Rect*)&sidebarRect))
    dieSDL("SDL_RenderDrawRect");
}

void Draw(Environment* env, GameState* gs, phase_t phase) {
  PxSz tileSize = { 40, 40 }; // FIXME: Don't hardcode.
  // Calculate the bounds of the area where we're going to draw the map.
  int shortDim = env->shortDimension;
  PxRect mapDisplayRect = {
    { .x = 0, .y = 0 }, { .w = shortDim, .h = shortDim } };
  PxSz mapDisplaySizePx = { .w = shortDim, .h = shortDim };
  CxSz mapDisplaySizeCx = PxSz_ToCx(mapDisplaySizePx, tileSize);
  CxSz mapDisplayCenterCx =
    CxSz_DivZ(CxSz_Sub(mapDisplaySizeCx, CxSz_New(1, 1)), 2);
  CxRC mapDisplayOffset = CxRC_SubSz(gs->playerPositionRC, mapDisplayCenterCx);

  //PxSz center = PxSz_DivZ(mapDisplaySize, 2);

  if (0 != SDL_SetRenderDrawColor(env->renderer, 0, 0, 0, 255))
    dieSDL("SDL_SetRenderDrawColor");
  if (0 != SDL_RenderClear(env->renderer))
    dieSDL("SDL_RenderClear");
  if (0 != SDL_SetRenderDrawColor(env->renderer, 255, 255, 255, 255))
    dieSDL("SDL_SetRenderDrawColor");
  DrawMap(env, gs, phase, mapDisplayRect, mapDisplayOffset);
  DrawUI(env, gs);
  SDL_RenderPresent(env->renderer);
}

