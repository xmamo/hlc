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

/// @memberof hlc_Map
extern HLC_API const hlc_Layout hlc_map_layout;

/// @memberof hlc_Map
/// @brief Makes an empty map.
/// @pre map != NULL
HLC_API void hlc_map_make(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_trait key_compare_instance
);

/// @memberof hlc_Map
/// @brief Returns the number of elements in this map.
/// @pre map != NULL
HLC_API size_t hlc_map_count(const hlc_Map* map);

/// @memberof hlc_Map
/// @brief Inserts a key/value pair into this map.
/// @return true on success, false on insufficient memory.
/// @pre map != NULL && element != NULL && key_assign_instance != NULL && value_assign_instance != NULL
HLC_API bool hlc_map_insert(
  hlc_Map* map,
  const void* key,
  const void* value,
  const hlc_Assign_trait* key_assign_instance,
  const hlc_Assign_trait* value_assign_instance
);

/// @memberof hlc_Map
/// @brief Removes a key from this map.
/// @return true on success, false if the element was not an element of this map.
/// @pre map != NULL && key_delete_instance != NULL && value_delete_instance != NULL
HLC_API bool hlc_map_remove(
  hlc_Map* map,
  const void* key,
  const hlc_Delete_trait* key_delete_instance,
  const hlc_Delete_trait* value_delete_instance
);

/// @memberof hlc_Map
/// @brief Returns the value corresponding to the given key, if any.
/// @return The value on success, or NULL if the key wasn't in this map.
HLC_API void* hlc_map_lookup(const hlc_Map* map, const void* key);

/// @memberof hlc_Map
/// @brief Returns the value corresponding to the given key, if any.
/// @return The value on success, or NULL if the key wasn't in this map.
#define hlc_map_lookup(map, key) _Generic(               \
  true ? (map) : (void*)(map),                           \
  void*: hlc_map_lookup((map), (key)),                   \
  const void*: (const void*)hlc_map_lookup((map), (key)) \
)

/// @memberof hlc_Map
/// @brief Checks if this map contains the given key.
/// @pre set != NULL
HLC_API bool hlc_map_contains(const hlc_Map* map, const void* key);

#endif
