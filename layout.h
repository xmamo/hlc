#ifndef HLC_LAYOUT_H
#define HLC_LAYOUT_H

#include <stdalign.h>
#include <stddef.h>

#include "api.h"

typedef struct hlc_Layout {
  size_t size;
  size_t alignment;
} hlc_Layout;

#define HLC_LAYOUT_OF(t) ((hlc_Layout){ \
  .size = sizeof(t),                    \
  .alignment = alignof(t),              \
})

/// @memberof hlc_Layout
/// @brief Makes an empty layout.
/// @pre layout != NULL
HLC_API void hlc_layout_make(hlc_Layout* layout);

/// @memberof hlc_Layout
/// @brief Adds a member to this layout.
/// @return The offset of the member which was added.
/// @pre layout != NULL
HLC_API size_t hlc_layout_add(hlc_Layout* layout, hlc_Layout member);

#endif
