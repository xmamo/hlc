#ifndef HLC_STACK_H
#define HLC_STACK_H

#ifdef __has_include
  #if __has_include(<crtdbg.h>) && __has_include(<malloc.h>)
    #include <crtdbg.h>
    #include <malloc.h>

    #define HLC_STACK_ALLOCATE(size) _malloca((size))
    #define HLC_STACK_FREE(memory) _freea((memory))
  #endif
#endif

#if !defined(HLC_STACK_ALLOCATE) || !defined(HLC_STACK_FREE)
  #undef HLC_STACK_ALLOCATE
  #undef HLC_STACK_FREE

  #include <stdlib.h>

  #ifdef alloca
    #define HLC_STACK_ALLOCATE(size) alloca((size))
    #define HLC_STACK_FREE(memory) (void)(memory)
  #else
    #define HLC_STACK_ALLOCATE(size) malloc((size))
    #define HLC_STACK_FREE(memory) free((memory))
  #endif
#endif

#endif
