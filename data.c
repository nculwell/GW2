// vim: nu et ts=8 sts=2 sw=2

typedef struct PxXY {
  int x, y;
} PxXY;

typedef struct PxSz {
  int w, h;
} PxSz;

typedef struct CxRC {
  int r, c;
} CxRC;

typedef struct CxSz {
  int w, h;
} CxSz;

PxXY PxXY_Add(PxXY a, PxXY b) {
  PxXY s = { .x=a.x+b.x, .y=a.x+b.x };
  return s;
}

PxXY PxXY_AddSz(PxXY a, PxSz b) {
  PxXY s = { .x=a.x+b.w, .y=a.x+b.w };
  return s;
}

PxXY PxXY_Sub(PxXY a, PxXY b) {
  PxXY s = { .x=a.x-b.x, .y=a.x-b.x };
  return s;
}

PxSz PxSz_New(int w, int h) {
  PxSz s = { .w=w, .h=h };
  return s;
}

PxSz PxSz_Add(PxSz a, PxSz b) {
  PxSz s = { .w=a.w+b.w, .h=a.h+b.h };
  return s;
}

PxSz PxSz_Sub(PxSz a, PxSz b) {
  PxSz s = { .w=a.w-b.w, .h=a.h-b.h };
  return s;
}

PxSz PxSz_Scale(PxSz a, float scale) {
  float w = a.w * scale;
  float h = a.h * scale;
  PxSz s = { .w=(int)w, .h=(int)h };
  return s;
}

CxSz PxSz_ToCx(PxSz a, PxSz tileSize) {
  CxSz s = { .w=a.w/tileSize.w, .h=a.h/tileSize.h };
  return s;
}

