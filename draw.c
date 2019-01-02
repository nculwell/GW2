// vim: nu et ts=8 sts=2 sw=2

void DrawTiles(Environment* env, GameState* gs, phase_t phase) {
  for (int r=0; ) {
  }
}

void DrawCharacters(Environment* env, GameState* gs, phase_t phase) {
}

void DrawMap(Environment* env, GameState* gs, phase_t phase) {
  DrawTiles(env, gs, phase);
  DrawCharacters(env, gs, phase);
}

void DrawUI(Environment* env, GameState* gs) {
  // TODO: Draw sidebar.
  bool sidebarHorizontal = (screenSize.h > screenSize.w);
  PxRect sidebarRect;
  if (sidebarHorizontal) {
    sidebarRect = (struct PxRect){ { .x = 0, .y = shortDimension },
      { .w = screenSize.w, .h = screenSize.h - shortDimension } };
  } else {
    sidebarRect = (struct PxRect){ { .x = shortDimension, .y = 0 },
      { .w = screenSize.w - shortDimension, .h = screenSize.h } };
  }
  SDL_RenderDrawRect(env->renderer, (SDL_Rect*)&sidebarRect);
}

void Draw(Environment* env, GameState* gs, phase_t phase) {
  PxSz tileSize = { w=40, h=40 }; // FIXME: Don't hardcode.
  // Calculate the bounds of the area where we're going to draw the map.
  int shortDimension = env->screenSize.w < env->screenSize.h
    ? env->screenSize.w : env->screenSize.h;
  PxRect mapDisplayRect = {
    { .x = 0, .y = 0 }, { .w = shortDimension, .h = shortDimension } };
  PxSz mapDisplaySizePx = { w=shortDimension, h=shortDimension };
  CxSz mapDisplaySizeCx = PxSz_ToCx(mapDisplaySizePx, tileSize);
  CxSz mapDisplayCenterCx =
    CxSz_DivZ(CxSz_Sub(mapDisplaySizeCx, CxSz_New(1, 1)), 2);
  CxSz mapDisplayOffset = CxSz_Sub(playerPositionCx, mapDisplayCenterCx);

  //PxSz center = PxSz_DivZ(mapDisplaySize, 2);

  if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255))
    die("SDL_SetRenderDrawColor: %s\n", SDL_Error());
  if (!SDL_RenderClear(env->renderer))
    die("SDL_RenderClear: %s\n", SDL_Error());
  DrawMap(env, gs, phase, mapDisplayRect, mapDisplayOffset);
  DrawUI(env, gs);
  if (!SDL_RenderPresent(renderer))
    die("SDL_RenderPresent: %s\n", SDL_Error());
}

