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
#include "traits/delete.h"


struct hlc_Map {
  hlc_AVL* root;
  size_t count;
  hlc_Layout key_layout;
  hlc_Layout value_layout;
  hlc_Compare_instance key_compare_instance;
  hlc_Assign_instance key_assign_instance;
  hlc_Delete_instance key_delete_instance;
  hlc_Assign_instance value_assign_instance;
  hlc_Delete_instance value_delete_instance;
};


const hlc_Layout hlc_map_layout = {
  .size = offsetof(hlc_Map, value_delete_instance) + sizeof(hlc_Delete_instance),
  .alignment = alignof(hlc_Map),
};


void hlc_map_make(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_instance key_compare_instance,
  hlc_Assign_instance key_assign_instance,
  hlc_Delete_instance key_delete_instance,
  hlc_Assign_instance value_assign_instance,
  hlc_Delete_instance value_delete_instance
) {
  assert(map != NULL);

  map->root = NULL;
  map->count = 0;
  map->key_layout = key_layout;
  map->value_layout = value_layout;
  map->key_compare_instance = key_compare_instance;
  map->key_assign_instance = key_assign_instance;
  map->key_delete_instance = key_delete_instance;
  map->value_assign_instance = value_assign_instance;
  map->value_delete_instance = value_delete_instance;
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
  hlc_Assign_instance value_assign_instance;
} hlc_Map_element_assign_context;


static bool hlc_map_element_assign(void* target, const void* _source, const hlc_Assign_trait* trait, void* _context) {
  const void* const* source = _source;
  (void)trait;
  const hlc_Map_element_assign_context* context = _context;

  assert(source != NULL);
  assert(target != NULL);

  void* backup = HLC_STACK_ALLOCATE(context->element_layout.size);

  if (backup != NULL) {
    memcpy(backup, target, context->element_layout.size);

    bool ok1 = hlc_assign((char*)target + context->key_offset, source[0], context->key_assign_instance);
    bool ok2 = hlc_assign((char*)target + context->value_offset, source[1], context->value_assign_instance);
    bool success = ok1 && ok2;

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
  return hlc_map_insert_with(map, key, value, map->key_assign_instance, map->value_assign_instance);
}


bool hlc_map_insert_with(
  hlc_Map* map,
  const void* key,
  const void* value,
  hlc_Assign_instance key_assign_instance,
  hlc_Assign_instance value_assign_instance
) {
  assert(map != NULL);

  const void* kv[] = {key, value};

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
    .value_assign_instance = value_assign_instance,
  };

  hlc_Assign_instance element_assign_instance = {
    .trait = &element_assign_trait,
    .context = &element_assign_context,
  };

  if (map->root != NULL) {
    hlc_AVL* node = map->root;

    while (true) {
      void* node_element = hlc_avl_element(node, element_layout);
      signed char ordering = hlc_compare(key, (char*)node_element + key_offset, map->key_compare_instance);

      if (ordering == 0)
        return hlc_assign(node_element, kv, element_assign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, kv, element_layout, element_assign_instance);

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
    hlc_AVL* node = hlc_avl_new(kv, element_layout, element_assign_instance);

    if (node != NULL) {
      map->root = node;
      map->count += 1;
      return true;
    } else {
      return false;
    }
  }
}


typedef struct hlc_Map_element_delete_context {
  size_t key_offset;
  hlc_Delete_instance key_delete_instance;
  size_t value_offset;
  hlc_Delete_instance value_delete_instance;
} hlc_Map_element_delete_context;


static void hlc_map_element_delete(void* target, const hlc_Delete_trait* trait, void* _context) {
  (void)trait;
  const hlc_Map_element_delete_context* context = _context;

  assert(target != NULL);
  assert(context != NULL);

  hlc_delete((char*)target + context->key_offset, context->key_delete_instance);
  hlc_delete((char*)target + context->value_offset, context->value_delete_instance);
}


bool hlc_map_remove(hlc_Map* map, const void* key) {
  assert(map != NULL);
  return hlc_map_remove_with(map, key, map->key_delete_instance, map->value_delete_instance);
}


bool hlc_map_remove_with(
  hlc_Map* map,
  const void* key,
  hlc_Delete_instance key_delete_instance,
  hlc_Delete_instance value_delete_instance
) {
  assert(map != NULL);

  hlc_Layout element_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&element_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&element_layout, map->value_layout);

  hlc_Delete_trait element_delete_trait = {
    .delete = hlc_map_element_delete,
  };

  hlc_Map_element_delete_context element_delete_context = {
    .key_offset = key_offset,
    .key_delete_instance = key_delete_instance,
    .value_offset = value_offset,
    .value_delete_instance = value_delete_instance,
  };

  hlc_Delete_instance element_delete_instance = {
    .trait = &element_delete_trait,
    .context = &element_delete_context,
  };

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, element_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + key_offset, map->key_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, element_layout, element_delete_instance);

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


void hlc_map_dot(const hlc_Map* map, FILE* stream) {
  assert(map != NULL);
  hlc_avl_dot(map->root, stream);
}
