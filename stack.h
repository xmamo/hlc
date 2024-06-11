#ifndef HLC_STACK_H
#define HLC_STACK_H

#include <stdlib.h>

#ifdef __has_include
  #if __has_include(<malloc.h>)
    #include <malloc.h>
  #endif
#endif

#if defined(_ALLOCA_S_THRESHOLD)
  #define HLC_STACK_ALLOCATE(size) _malloca((size))
  #define HLC_STACK_FREE(memory) _freea((memory))
#elif defined(alloca)
  #define HLC_STACK_ALLOCATE(size) alloca((size))
  #define HLC_STACK_FREE(memory) (void)(memory)
#else
  #define HLC_STACK_ALLOCATE(size) malloc((size))
  #define HLC_STACK_FREE(memory) free((memory))
#endif

#endif
