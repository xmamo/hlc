#ifndef HLC_LAYOUT_H
#define HLC_LAYOUT_H

#include <stdalign.h>
#include <stddef.h>

#include "api.h"

HLC_DECLARATIONS_BEGIN

typedef struct hlc_Layout {
  size_t size;
  size_t alignment;
} hlc_Layout;

/// @relates hlc_Layout
#define HLC_LAYOUT_OF(t) ((hlc_Layout){sizeof(t), alignof(t)})

/// @memberof hlc_Layout
/// @brief Adds a member to this layout.
/// @return The offset of the member which was added.
/// @pre layout != NULL
HLC_API size_t hlc_layout_add(hlc_Layout* layout, hlc_Layout member);

/// @memberof hlc_Layout
/// @brief Pads this layout by making its size a multiple of its alignment.
/// @pre layout != NULL
HLC_API void hlc_layout_pad(hlc_Layout* layout);

HLC_DECLARATIONS_END

#endif
