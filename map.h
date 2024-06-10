#ifndef HLC_MAP_H
#define HLC_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "api.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/delete.h"

typedef struct hlc_Map hlc_Map;
extern HLC_API const hlc_Layout hlc_map_layout;

HLC_API void hlc_map_make(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_trait key_compare_instance
);

HLC_API size_t hlc_map_count(const hlc_Map* map);

HLC_API void* hlc_map_lookup(const hlc_Map* map, const void* key);

#define hlc_map_lookup(map, key) _Generic(               \
  true ? (map) : (void*)(map),                           \
  void*: hlc_map_lookup((map), (key)),                   \
  const void*: (const void*)hlc_map_lookup((map), (key)) \
)

HLC_API bool hlc_map_insert(
  hlc_Map* map,
  const void* key,
  const void* value,
  const hlc_Assign_trait* key_assign_instance,
  const hlc_Assign_trait* value_assign_instance
);

HLC_API bool hlc_map_remove(
  hlc_Map* map,
  const void* key,
  const hlc_Delete_trait* key_delete_instance,
  const hlc_Delete_trait* value_delete_instance
);

#endif
