#ifndef HLC_SET_H
#define HLC_SET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "api.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"

HLC_DECLARATIONS_BEGIN

typedef struct hlc_Set hlc_Set;

/// @memberof hlc_Set
extern HLC_API const hlc_Layout hlc_set_layout;

/// @relates hlc_Set
typedef struct hlc_Set_iterator hlc_Set_iterator;

/// @memberof hlc_Set_iterator
extern HLC_API const hlc_Layout hlc_set_iterator_layout;

/// @memberof hlc_Set
/// @brief Creates an empty set.
/// @pre set != NULL
HLC_API void hlc_set_create(
  hlc_Set* set,
  hlc_Layout element_layout,
  hlc_Compare_instance element_compare_instance,
  hlc_Destroy_instance element_destroy_instance
);

/// @memberof hlc_Set
/// @brief Returns the number of elements in this set.
/// @pre set != NULL
HLC_API size_t hlc_set_count(const hlc_Set* set);

/// @memberof hlc_Set
/// @brief Inserts an element into this set.
/// @return true on success, false on insufficient memory.
/// @pre set != NULL
HLC_API bool hlc_set_insert(hlc_Set* set, const void* element, hlc_Assign_instance element_assign_instance);

/// @memberof hlc_Set
/// @brief Removes an element from this set.
/// @return true on success, false if the element was not an element of this set.
/// @pre set != NULL
HLC_API bool hlc_set_remove(hlc_Set* set, const void* element);

/// @memberof hlc_Set
/// @brief Checks if this set contains the given key.
/// @pre set != NULL
HLC_API bool hlc_set_contains(const hlc_Set* set, const void* key);

/// @memberof hlc_Set
/// @brief Clears this set.
/// @pre set != NULL
HLC_API void hlc_set_clear(hlc_Set* set);

/// @memberof hlc_Set
/// @brief Destroys this set.
/// @pre set != NULL
HLC_API void hlc_set_destroy(hlc_Set* set);

/// @memberof hlc_Set
/// @pre target != NULL && source != NULL
HLC_API void hlc_set_move_reassign(hlc_Set* target, hlc_Set* source);

/// @brief hlc_Set
/// @brief Compares two sets.
/// @pre set1 != NULL && set2 != NULL
HLC_API signed char hlc_set_compare(const hlc_Set* set1, const hlc_Set* set2, hlc_Compare_instance compare_instance);

/// @memberof hlc_Set
/// @relates hlc_Set_iterator
/// @brief Creates an iterator for this set.
/// @pre set != NULL && iterator != NULL
HLC_API void hlc_set_iterator(const hlc_Set* set, hlc_Set_iterator* iterator);

/// @memberof hlc_Set_iterator
/// @brief Returns the current element and advances the iterator.
/// @return The current element, or NULL if the last element of the set was reached.
/// @pre iterator != NULL
HLC_API const void* hlc_set_iterator_next(hlc_Set_iterator* iterator);

HLC_DECLARATIONS_END

#endif
