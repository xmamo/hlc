#ifndef HLC_MATH_H
#define HLC_MATH_H

#include <stdlib.h>

#if defined(max)
  #define HLC_MAX(x, y) max((x), (y))
#elif defined(__max)
  #define HLC_MAX(x, y) __max((x), (y))
#else
  #define HLC_MAX(x, y) ((y) > (x) ? (y) : (x))
#endif

#if defined(min)
  #define HLC_MIN(x, y) min((x), (y))
#elif defined(__min)
  #define HLC_MIN(x, y) __min((x), (y))
#else
  #define HLC_MIN(x, y) ((y) < (x) ? (y) : (x))
#endif

#define HLC_ABS(x) ((x) >= 0 ? (x) : -(x))

#define HLC_SIGN(x) ((x) < 0 ? -1 : (x) > 0 ? +1 : 0)

#define HLC_CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

#define HLC_COMPARE(x, y) ((x) < (y) ? -1 : (x) > (y) ? +1 : 0)

#endif
