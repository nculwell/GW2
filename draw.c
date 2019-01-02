// vim: nu et ts=8 sts=2 sw=2

void draw(Environment* env, GameState* gs, phase_t phase) {
  PxSz tileSize = { w=40, h=40 };
  int shortDimension = env->screenSize.w < env->screenSize.h
    ? env->screenSize.w : env->screenSize.h;
  PxSz mapDisplaySizePx = { w=shortDimension, h=shortDimension };
  CxSz mapDisplaySizeCx = PxSz_ToCx(mapDisplaySizePx, tileSize);
  PxSz center = PxSz_Scale(mapDisplaySize, .5);

}

