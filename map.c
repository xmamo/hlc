#include "map.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "avl.h"
#include "layout.h"
#include "math.h"
#include "stack.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"


struct hlc_Map {
  hlc_AVL* root;
  size_t count;
  hlc_Layout key_layout;
  hlc_Layout value_layout;
  hlc_Compare_instance key_compare_instance;
  hlc_Destroy_instance key_destroy_instance;
  hlc_Destroy_instance value_destroy_instance;

  hlc_Layout element_layout;
  size_t key_offset;
  size_t value_offset;
};


struct hlc_Map_iterator {
  hlc_AVL* current;
  hlc_Layout key_layout;
  hlc_Layout value_layout;

  hlc_Layout element_layout;
  size_t key_offset;
  size_t value_offset;
};


const hlc_Layout hlc_map_layout = {
  .size = offsetof(hlc_Map, value_offset) + sizeof(size_t),
  .alignment = alignof(hlc_Map),
};


const hlc_Layout hlc_map_iterator_layout = {
  .size = offsetof(hlc_Map_iterator, value_offset) + sizeof(size_t),
  .alignment = alignof(hlc_Map_iterator),
};


void hlc_map_create(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_instance key_compare_instance,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Destroy_instance value_destroy_instance
) {
  assert(map != NULL);

  map->root = NULL;
  map->count = 0;
  map->key_layout = key_layout;
  map->value_layout = value_layout;
  map->key_compare_instance = key_compare_instance;
  map->key_destroy_instance = key_destroy_instance;
  map->value_destroy_instance = value_destroy_instance;

  map->element_layout = (hlc_Layout){.size = 0, .alignment = 1};
  map->key_offset = hlc_layout_add(&map->element_layout, key_layout);
  map->value_offset = hlc_layout_add(&map->element_layout, value_layout);
}


size_t hlc_map_count(const hlc_Map* map) {
  assert(map != NULL);
  return map->count;
}


typedef struct hlc_Map_element_assign_context {
  hlc_Layout element_layout;
  size_t key_offset;
  size_t value_offset;
  hlc_Assign_instance key_assign_instance;
  hlc_Destroy_instance key_destroy_instance;
  hlc_Assign_instance value_assign_instance;
} hlc_Map_element_assign_context;


static bool hlc_map_element_assign(void* target, const void* _source, const hlc_Assign_trait* trait, void* _context) {
  const hlc_Map_kv_ref* source = _source;
  (void)trait;
  const hlc_Map_element_assign_context* context = _context;

  assert(source != NULL);
  assert(target != NULL);

  if (hlc_assign((char*)target + context->key_offset, source->key, context->key_assign_instance)) {
    if (hlc_assign((char*)target + context->value_offset, source->value, context->value_assign_instance)) {
      return true;
    } else {
      hlc_destroy((char*)target + context->key_offset, context->key_destroy_instance);
    }
  }

  return false;
}


static bool hlc_map_element_reassign(void* target, const void* _source, const hlc_Assign_trait* trait, void* _context) {
  const hlc_Map_kv_ref* source = _source;
  (void)trait;
  const hlc_Map_element_assign_context* context = _context;

  assert(source != NULL);
  assert(target != NULL);

  void* backup = HLC_STACK_ALLOCATE(context->element_layout.size);

  if (backup != NULL) {
    bool success = false;

    memcpy(backup, target, context->element_layout.size);

    if (hlc_reassign((char*)target + context->key_offset, source->key, context->key_assign_instance)) {
      if (hlc_reassign((char*)target + context->value_offset, source->value, context->value_assign_instance)) {
        success = true;
      } else {
        hlc_destroy((char*)target + context->key_offset, context->key_destroy_instance);
      }
    }

    if (!success) {
      memcpy(target, backup, context->element_layout.size);
    }

    HLC_STACK_FREE(backup);
    return success;
  } else {
    return false;
  }
}


bool hlc_map_insert(
  hlc_Map* map,
  const void* key,
  const void* value,
  hlc_Assign_instance key_assign_instance,
  hlc_Assign_instance value_assign_instance
) {
  assert(map != NULL);

  hlc_Map_kv_ref kv_ref = {.key = key, .value = (void*)value};

  hlc_Assign_trait element_assign_trait = {
    .assign = hlc_map_element_assign,
    .reassign = hlc_map_element_reassign,
  };

  hlc_Map_element_assign_context element_assign_context = {
    .element_layout = map->element_layout,
    .key_offset = map->key_offset,
    .value_offset = map->value_offset,
    .key_assign_instance = key_assign_instance,
    .key_destroy_instance = map->key_destroy_instance,
    .value_assign_instance = value_assign_instance,
  };

  hlc_Assign_instance element_assign_instance = {
    .trait = &element_assign_trait,
    .context = &element_assign_context,
  };

  if (map->root != NULL) {
    hlc_AVL* node = map->root;

    while (true) {
      void* node_element = hlc_avl_element(node, map->element_layout);
      signed char ordering = hlc_compare(key, (char*)node_element + map->key_offset, map->key_compare_instance);

      if (ordering == 0)
        return hlc_reassign(node_element, &kv_ref, element_assign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, &kv_ref, map->element_layout, element_assign_instance);

        if (node != NULL) {
          if (hlc_avl_link(node, 0) == NULL) {
            map->root = node;
          }

          map->count += 1;
          assert(map->count == hlc_avl_count(map->root));
          return true;
        } else {
          return false;
        }
      }

      node = node_child;
    }
  } else {
    hlc_AVL* node = hlc_avl_new(&kv_ref, map->element_layout, element_assign_instance);

    if (node != NULL) {
      map->root = node;
      map->count += 1;
      return true;
    } else {
      return false;
    }
  }
}


typedef struct hlc_Map_element_destroy_context {
  size_t key_offset;
  size_t value_offset;
  hlc_Destroy_instance key_destroy_instance;
  hlc_Destroy_instance value_destroy_instance;
} hlc_Map_element_destroy_context;


static void hlc_map_element_destroy(void* target, const hlc_Destroy_trait* trait, void* _context) {
  (void)trait;
  const hlc_Map_element_destroy_context* context = _context;

  assert(target != NULL);
  assert(context != NULL);

  hlc_destroy((char*)target + context->key_offset, context->key_destroy_instance);
  hlc_destroy((char*)target + context->value_offset, context->value_destroy_instance);
}


bool hlc_map_remove(hlc_Map* map, const void* key) {
  assert(map != NULL);

  hlc_Destroy_trait element_destroy_trait = {
    .destroy = hlc_map_element_destroy,
  };

  hlc_Map_element_destroy_context element_destroy_context = {
    .key_offset = map->key_offset,
    .value_offset = map->value_offset,
    .key_destroy_instance = map->key_destroy_instance,
    .value_destroy_instance = map->value_destroy_instance,
  };

  hlc_Destroy_instance element_destroy_instance = {
    .trait = &element_destroy_trait,
    .context = &element_destroy_context,
  };

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, map->element_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + map->key_offset, map->key_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, map->element_layout, element_destroy_instance);

      if (node == NULL || hlc_avl_link(node, 0) == NULL) {
        map->root = node;
      }

      map->count -= 1;
      assert(map->count == hlc_avl_count(map->root));
      return true;
    }

    node = hlc_avl_link(node, ordering);
  }

  return false;
}


void* (hlc_map_lookup)(const hlc_Map* map, const void* key) {
  assert(map != NULL);

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, map->element_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + map->key_offset, map->key_compare_instance);

    if (ordering == 0) {
      return (char*)node_element + map->value_offset;
    } else {
      node = hlc_avl_link(node, ordering);
    }
  }

  return NULL;
}


bool hlc_map_contains(const hlc_Map* map, const void* key) {
  return hlc_map_lookup(map, key) != NULL;
}


void hlc_map_clear(hlc_Map* map) {
  assert(map != NULL);

  hlc_map_destroy(map);
  map->root = NULL;
  map->count = 0;
}


void hlc_map_destroy(hlc_Map* map) {
  assert(map != NULL);

  hlc_Destroy_trait element_destroy_trait = {
    .destroy = hlc_map_element_destroy,
  };

  hlc_Map_element_destroy_context element_destroy_context = {
    .key_offset = map->key_offset,
    .value_offset = map->value_offset,
    .key_destroy_instance = map->key_destroy_instance,
    .value_destroy_instance = map->value_destroy_instance,
  };

  hlc_Destroy_instance element_destroy_instance = {
    .trait = &element_destroy_trait,
    .context = &element_destroy_context,
  };

  hlc_avl_delete(map->root, map->element_layout, element_destroy_instance);
}


void hlc_map_move_reassign(hlc_Map* map, hlc_Map* other) {
  assert(map != NULL);
  assert(other != NULL);

  hlc_Destroy_trait element_destroy_trait = {
    .destroy = hlc_map_element_destroy,
  };

  hlc_Map_element_destroy_context element_destroy_context = {
    .key_offset = map->key_offset,
    .value_offset = map->value_offset,
    .key_destroy_instance = map->key_destroy_instance,
    .value_destroy_instance = map->value_destroy_instance,
  };

  hlc_Destroy_instance element_destroy_instance = {
    .trait = &element_destroy_trait,
    .context = &element_destroy_context,
  };

  hlc_avl_delete(map->root, map->element_layout, element_destroy_instance);
  *map = *other;
  other->root = NULL;
  other->count = 0;
}


signed char hlc_map_compare(
  const hlc_Map* map1,
  const hlc_Map* map2,
  hlc_Compare_instance key_compare_instance,
  hlc_Compare_instance value_compare_instance
) {
  assert(map1 != NULL);
  assert(map2 != NULL);

  hlc_Map_iterator iterator1;
  hlc_map_iterator(map1, &iterator1);

  hlc_Map_iterator iterator2;
  hlc_map_iterator(map2, &iterator2);

  for (size_t i = 0; i < HLC_MIN(map1->count, map2->count); ++i) {
    hlc_Map_kv_ref kv_ref_1 = hlc_map_iterator_next(&iterator1);
    hlc_Map_kv_ref kv_ref_2 = hlc_map_iterator_next(&iterator2);

    signed char ordering = hlc_compare(kv_ref_1.key, kv_ref_2.key, key_compare_instance);

    if (ordering != 0)
      return ordering;

    ordering = hlc_compare(kv_ref_1.value, kv_ref_2.value, value_compare_instance);

    if (ordering != 0)
      return ordering;
  }

  return HLC_COMPARE(map1->count, map2->count);
}


void hlc_map_iterator(const hlc_Map* map, hlc_Map_iterator* iterator) {
  assert(map != NULL);
  assert(iterator != NULL);

  iterator->current = map->root != NULL ? hlc_avl_xmost(map->root, -1) : NULL;
  iterator->key_layout = map->key_layout;
  iterator->value_layout = map->value_layout;

  iterator->element_layout = map->element_layout;
  iterator->key_offset = map->key_offset;
  iterator->value_offset = map->value_offset;
}


hlc_Map_kv_ref hlc_map_iterator_next(hlc_Map_iterator* iterator) {
  assert(iterator != NULL);

  if (iterator->current != NULL) {
    const void* element = hlc_avl_element(iterator->current, iterator->element_layout);
    iterator->current = hlc_avl_xcessor(iterator->current, +1);

    return (hlc_Map_kv_ref){
      .key = (const char*)element + iterator->key_offset,
      .value = (char*)element + iterator->value_offset,
    };
  } else {
    return (hlc_Map_kv_ref){.key = NULL, .value = NULL};
  }
}
