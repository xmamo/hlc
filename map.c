#include "map.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "avl.h"
#include "layout.h"
#include "stack.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"
#include "traits/reassign.h"


struct hlc_Map {
  hlc_AVL* root;
  size_t count;
  hlc_Layout key_layout;
  hlc_Layout value_layout;
  hlc_Compare_instance key_compare_instance;
  hlc_Assign_instance key_assign_instance;
  hlc_Reassign_instance key_reassign_instance;
  hlc_Destroy_instance key_destroy_instance;
  hlc_Assign_instance value_assign_instance;
  hlc_Reassign_instance value_reassign_instance;
  hlc_Destroy_instance value_destroy_instance;
};


const hlc_Layout hlc_map_layout = {
  .size = offsetof(hlc_Map, value_destroy_instance) + sizeof(hlc_Destroy_instance),
  .alignment = alignof(hlc_Map),
};


void hlc_map_create(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_instance key_compare_instance,
  hlc_Assign_instance key_assign_instance,
  hlc_Reassign_instance key_reassign_instance,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Assign_instance value_assign_instance,
  hlc_Reassign_instance value_reassign_instance,
  hlc_Destroy_instance value_destroy_instance
) {
  assert(map != NULL);

  map->root = NULL;
  map->count = 0;
  map->key_layout = key_layout;
  map->value_layout = value_layout;
  map->key_compare_instance = key_compare_instance;
  map->key_assign_instance = key_assign_instance;
  map->key_reassign_instance = key_reassign_instance;
  map->key_destroy_instance = key_destroy_instance;
  map->value_assign_instance = value_assign_instance;
  map->value_reassign_instance = value_reassign_instance;
  map->value_destroy_instance = value_destroy_instance;
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


typedef struct hlc_Map_element_reassign_context {
  hlc_Layout element_layout;
  size_t key_offset;
  size_t value_offset;
  hlc_Reassign_instance key_reassign_instance;
  hlc_Destroy_instance key_destroy_instance;
  hlc_Reassign_instance value_reassign_instance;
} hlc_Map_element_reassign_context;


static bool hlc_map_element_reassign(void* target, const void* _source, const hlc_Reassign_trait* trait, void* _context) {
  const hlc_Map_kv_ref* source = _source;
  (void)trait;
  const hlc_Map_element_reassign_context* context = _context;

  assert(source != NULL);
  assert(target != NULL);

  void* backup = HLC_STACK_ALLOCATE(context->element_layout.size);

  if (backup != NULL) {
    memcpy(backup, target, context->element_layout.size);

    bool success = false;

    if (hlc_reassign((char*)target + context->key_offset, source->key, context->key_reassign_instance)) {
      if (hlc_reassign((char*)target + context->value_offset, source->value, context->value_reassign_instance)) {
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


bool hlc_map_insert(hlc_Map* map, const void* key, const void* value) {
  assert(map != NULL);

  return hlc_map_insert_with(
    map,
    key,
    value,
    map->key_assign_instance,
    map->key_reassign_instance,
    map->key_destroy_instance,
    map->value_assign_instance,
    map->value_reassign_instance
  );
}


bool hlc_map_insert_with(
  hlc_Map* map,
  const void* key,
  const void* value,
  hlc_Assign_instance key_assign_instance,
  hlc_Reassign_instance key_reassign_instance,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Assign_instance value_assign_instance,
  hlc_Reassign_instance value_reassign_instance
) {
  assert(map != NULL);

  hlc_Map_kv_ref kv_ref = {.key = key, .value = (void*)value};

  hlc_Layout element_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&element_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&element_layout, map->value_layout);

  hlc_Assign_trait element_assign_trait = {
    .assign = hlc_map_element_assign,
  };

  hlc_Map_element_assign_context element_assign_context = {
    .element_layout = element_layout,
    .key_offset = key_offset,
    .value_offset = value_offset,
    .key_assign_instance = key_assign_instance,
    .key_destroy_instance = key_destroy_instance,
    .value_assign_instance = value_assign_instance,
  };

  hlc_Assign_instance element_assign_instance = {
    .trait = &element_assign_trait,
    .context = &element_assign_context,
  };

  hlc_Reassign_trait element_reassign_trait = {
    .reassign = hlc_map_element_reassign,
  };

  hlc_Map_element_reassign_context element_reassign_context = {
    .element_layout = element_layout,
    .key_offset = key_offset,
    .value_offset = value_offset,
    .key_reassign_instance = key_reassign_instance,
    .key_destroy_instance = key_destroy_instance,
    .value_reassign_instance = value_reassign_instance,
  };

  hlc_Reassign_instance element_reassign_instance = {
    .trait = &element_reassign_trait,
    .context = &element_reassign_context,
  };

  if (map->root != NULL) {
    hlc_AVL* node = map->root;

    while (true) {
      void* node_element = hlc_avl_element(node, element_layout);
      signed char ordering = hlc_compare(key, (char*)node_element + key_offset, map->key_compare_instance);

      if (ordering == 0)
        return hlc_reassign(node_element, &kv_ref, element_reassign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, &kv_ref, element_layout, element_assign_instance);

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
    hlc_AVL* node = hlc_avl_new(&kv_ref, element_layout, element_assign_instance);

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
  hlc_Destroy_instance key_destroy_instance;
  size_t value_offset;
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
  return hlc_map_remove_with(map, key, map->key_destroy_instance, map->value_destroy_instance);
}


bool hlc_map_remove_with(
  hlc_Map* map,
  const void* key,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Destroy_instance value_destroy_instance
) {
  assert(map != NULL);

  hlc_Layout element_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&element_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&element_layout, map->value_layout);

  hlc_Destroy_trait element_destroy_trait = {
    .destroy = hlc_map_element_destroy,
  };

  hlc_Map_element_destroy_context element_destroy_context = {
    .key_offset = key_offset,
    .key_destroy_instance = key_destroy_instance,
    .value_offset = value_offset,
    .value_destroy_instance = value_destroy_instance,
  };

  hlc_Destroy_instance element_destroy_instance = {
    .trait = &element_destroy_trait,
    .context = &element_destroy_context,
  };

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, element_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + key_offset, map->key_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, element_layout, element_destroy_instance);

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

  hlc_Layout element_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&element_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&element_layout, map->value_layout);

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, element_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + key_offset, map->key_compare_instance);

    if (ordering == 0) {
      return (char*)node_element + value_offset;
    } else {
      node = hlc_avl_link(node, ordering);
    }
  }

  return NULL;
}


bool hlc_map_contains(const hlc_Map* map, const void* key) {
  return hlc_map_lookup(map, key) != NULL;
}


void hlc_map_destroy(hlc_Map* map) {
  assert(map != NULL);
  hlc_map_destroy_with(map, map->key_destroy_instance, map->value_destroy_instance);
}


void hlc_map_destroy_with(
  hlc_Map* map,
  hlc_Destroy_instance key_destroy_instance,
  hlc_Destroy_instance value_destroy_instance
) {
  assert(map != NULL);

  hlc_Layout element_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&element_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&element_layout, map->value_layout);

  hlc_Destroy_trait element_destroy_trait = {
    .destroy = hlc_map_element_destroy,
  };

  hlc_Map_element_destroy_context element_destroy_context = {
    .key_offset = key_offset,
    .key_destroy_instance = key_destroy_instance,
    .value_offset = value_offset,
    .value_destroy_instance = value_destroy_instance,
  };

  hlc_Destroy_instance element_destroy_instance = {
    .trait = &element_destroy_trait,
    .context = &element_destroy_context,
  };

  hlc_avl_delete(map->root, element_layout, element_destroy_instance);
  map->root = NULL;
  map->count = 0;
}


struct hlc_Map_iterator {
  hlc_AVL* current;
  hlc_Layout key_layout;
  hlc_Layout value_layout;
};


const hlc_Layout hlc_map_iterator_layout = {
  .size = offsetof(hlc_Map_iterator, value_layout) + sizeof(hlc_Layout),
  .alignment = alignof(hlc_Map_iterator),
};


void hlc_map_iterator(const hlc_Map* map, hlc_Map_iterator* iterator) {
  assert(map != NULL);
  assert(iterator != NULL);

  iterator->current = map->root != NULL ? hlc_avl_xmost(map->root, -1) : NULL;
  iterator->key_layout = map->key_layout;
  iterator->value_layout = map->value_layout;
}


hlc_Map_kv_ref hlc_map_iterator_next(hlc_Map_iterator* iterator) {
  assert(iterator != NULL);

  if (iterator->current != NULL) {
    hlc_Layout element_layout = {.size = 0, .alignment = 1};
    size_t key_offset = hlc_layout_add(&element_layout, iterator->key_layout);
    size_t value_offset = hlc_layout_add(&element_layout, iterator->value_layout);

    const void* element = hlc_avl_element(iterator->current, element_layout);
    iterator->current = hlc_avl_xcessor(iterator->current, +1);
    return (hlc_Map_kv_ref){.key = (const char*)element + key_offset, .value = (char*)element + value_offset};
  } else {
    return (hlc_Map_kv_ref){.key = NULL, .value = NULL};
  }
}
