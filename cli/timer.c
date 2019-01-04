// vim: nu et ts=8 sts=2 sw=2

typedef Sint32 phase_t;

typedef struct FrameTimer {
  Uint64 nextFrameTime;
  phase_t phase;
  Uint32 seconds;
  Uint32 framesAfterSecond;
  Uint32 endOfSecondCorrection;
  Uint32 framesPerSecond;
} FrameTimer;

#define TIMER_FRAME_INCREMENT \
  (TIMER_MILLISECOND_MULTIPLIER * 1000 / LOGICAL_FRAMES_PER_SECOND)
#define WRAPAROUND_TIME (TIMER_MILLISECOND_MULTIPLIER * (1ull << 32))
//const Uint64 WRAPAROUND_TIME = (TIMER_MILLISECOND_MULTIPLIER * (1ull << 32))
const Sint64 WRAPAROUND_TEST_DIFF = WRAPAROUND_TIME >> 2;

FrameTimer* FrameTimer_Init(FrameTimer* t, Uint32 framesPerSecond)
{
  t->nextFrameTime = GetTime();
  t->framesPerSecond = framesPerSecond;
  t->seconds = 0;
  t->framesAfterSecond = 0;
  Uint32 fullSecondIncrement = TIMER_MILLISECOND_MULTIPLIER * 1000;
  Uint32 sumOfFrameIncrements = TIMER_FRAME_INCREMENT * framesPerSecond;
  t->endOfSecondCorrection = fullSecondIncrement - sumOfFrameIncrements;
  return t;
}

static Uint64 TimeDiff(Uint64 time, Uint64 referencePoint) {
  // Check if next frame was reached. Check against WRAPAROUND_TEST_DIFF to
  // detect very big differences that result from the timer wrapping around.
  // The parentheses are important to avoid overflow.
  Sint64 diff = (Sint64)(time - referencePoint);
  if (diff > WRAPAROUND_TEST_DIFF)
    diff = referencePoint + (WRAPAROUND_TIME - time);
  else if (diff < -WRAPAROUND_TEST_DIFF)
    diff = time + (WRAPAROUND_TIME - referencePoint);
  return diff;
}

bool FrameTimer_NextFrame(FrameTimer* t)
{
  Uint64 time = GetTime();
  Sint64 diff = TimeDiff(time, t->nextFrameTime);
  bool advancedFrame = false;
  if (diff >= 0) {
    advancedFrame = true;
    t->nextFrameTime += TIMER_FRAME_INCREMENT;
    ++t->framesAfterSecond;
    if (t->framesAfterSecond == t->framesPerSecond) {
      ++t->seconds;
      t->framesAfterSecond = 0;
      t->nextFrameTime += t->endOfSecondCorrection;
    }
  }
  t->phase = (Sint32)diff;
  return advancedFrame;
}

int ScaleToPhase(phase_t phase, int n) {
  return n * (Sint32)TIMER_FRAME_INCREMENT / phase;
}



