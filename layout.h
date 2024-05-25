#ifndef HLC_LAYOUT_H
#define HLC_LAYOUT_H

#include <stddef.h>

typedef struct hlc_Layout {
  size_t size;
  size_t alignment;
} hlc_Layout;

#endif
