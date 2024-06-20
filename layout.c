#include "layout.h"

#include <assert.h>
#include <stddef.h>


size_t hlc_layout_add(hlc_Layout* layout, hlc_Layout member) {
  assert(layout != NULL);

  size_t member_size = member.size;
  size_t member_alignment = member.alignment != 0 ? member.alignment : 1;
  size_t member_offset = (layout->size + (member_alignment - 1)) / member_alignment * member_alignment;

  layout->size = member_offset + member_size;

  if (member_alignment > layout->alignment) {
    layout->alignment = member_alignment;
  }

  return member_offset;
}
