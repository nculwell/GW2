
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define die(format...) _die(__FILE__, __LINE__, format)

__attribute__((noreturn))
static void _die(const char* file, int line, const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, " [%s:%d]\n", file, line);
  exit(1);
}

