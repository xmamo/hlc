#ifndef HLC_MAP_H
#define HLC_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "api.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"

HLC_DECLARATIONS_BEGIN

typedef struct hlc_Map hlc_Map;

/// @memberof hlc_Map
extern HLC_API const hlc_Layout hlc_map_layout;

/// @relates hlc_Map
typedef struct hlc_Map_iterator hlc_Map_iterator;

/// @memberof hlc_Map_iterator
extern HLC_API const hlc_Layout hlc_map_iterator_layout;

/// @relates hlc_Map_iterator
typedef struct hlc_Map_kv_ref {
  const void* key;
  void* value;
} hlc_Map_kv_ref;

/// @memberof hlc_Map
/// @brief Creates an empty map.
/// @pre map != NULL
HLC_API void hlc_map_create(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_instance key_compare_instance,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Destroy_instance value_destroy_instance
);

/// @memberof hlc_Map
/// @brief Returns the number of elements in this map.
/// @pre map != NULL
HLC_API size_t hlc_map_count(const hlc_Map* map);

/// @memberof hlc_Map
/// @brief Inserts a key/value pair into this map.
/// @return true on success, false on insufficient memory.
/// @pre map != NULL
HLC_API bool hlc_map_insert(
  hlc_Map* map,
  const void* key,
  const void* value,
  hlc_Assign_instance key_assign_instance,
  hlc_Assign_instance value_assign_instance
);

/// @memberof hlc_Map
/// @brief Removes a key from this map.
/// @return true on success, false if the element was not an element of this map.
/// @pre map != NULL
HLC_API bool hlc_map_remove(hlc_Map* map, const void* key);

/// @memberof hlc_Map
/// @brief Returns the value corresponding to the given key, if any.
/// @return The value on success, or NULL if the key wasn't in this map.
/// @pre map != NULL
HLC_API void* hlc_map_lookup(const hlc_Map* map, const void* key);

/// @memberof hlc_Map
/// @brief Returns the value corresponding to the given key, if any.
/// @return The value on success, or NULL if the key wasn't in this map.
/// @pre map != NULL
#define hlc_map_lookup(map, key) _Generic(               \
  true ? (map) : (void*)(map),                           \
  void*: hlc_map_lookup((map), (key)),                   \
  const void*: (const void*)hlc_map_lookup((map), (key)) \
)

/// @memberof hlc_Map
/// @brief Checks if this map contains the given key.
/// @pre map != NULL
HLC_API bool hlc_map_contains(const hlc_Map* map, const void* key);

/// @memberof hlc_Map
/// @brief Clears this map.
/// @pre map != NULL
HLC_API void hlc_map_clear(hlc_Map* map);

/// @memberof hlc_Map
/// @brief Destroys this map.
/// @pre map != NULL
HLC_API void hlc_map_destroy(hlc_Map* map);

/// @memberof hlc_Map
/// @pre target != NULL && source != NULL
HLC_API void hlc_map_move_reassign(hlc_Map* target, hlc_Map* source);

/// @brief hlc_Map
/// @brief Compares two maps.
/// @pre map1 != NULL && map2 != NULL
HLC_API signed char hlc_map_compare(
  const hlc_Map* map1,
  const hlc_Map* map2,
  hlc_Compare_instance key_compare_instance,
  hlc_Compare_instance value_compare_instance
);

/// @memberof hlc_Map
/// @relates hlc_Map_iterator
/// @brief Creates an iterator for this map.
/// @pre map != NULL && iterator != NULL
HLC_API void hlc_map_iterator(const hlc_Map* map, hlc_Map_iterator* iterator);

/// @memberof hlc_Map_iterator
/// @brief Returns the current key/value pair and advances the iterator.
/// @return Pointers to the current key and value, or a pair of NULLs if the last key/value pair of the map was reached.
/// @pre iterator != NULL
HLC_API hlc_Map_kv_ref hlc_map_iterator_next(hlc_Map_iterator* iterator);

HLC_DECLARATIONS_END

#endif
