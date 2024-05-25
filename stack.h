#ifndef HLC_STACK_H
#define HLC_STACK_H

#include <stdlib.h>

#ifdef HLC_HAVE_ALLOCA
#define HLC_STACK_ALLOCATE(size) alloca((size))
#define HLC_STACK_FREE(p) (void)(p)
#else
#define HLC_STACK_ALLOCATE(size) malloc((size))
#define HLC_STACK_FREE(p) free((p))
#endif

#endif
