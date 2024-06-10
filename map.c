#include "map.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>

#include "avl.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/delete.h"


struct hlc_Map {
  hlc_AVL* root;
  size_t count;
  hlc_Layout key_layout;
  hlc_Layout value_layout;
  hlc_Compare_trait key_compare_instance;
};


const hlc_Layout hlc_map_layout = {
  .size = offsetof(hlc_Map, key_compare_instance) + sizeof(hlc_Compare_trait),
  .alignment = alignof(hlc_Map),
};


void hlc_map_make(
  hlc_Map* map,
  hlc_Layout key_layout,
  hlc_Layout value_layout,
  hlc_Compare_trait key_compare_instance
) {
  assert(map != NULL);

  map->root = NULL;
  map->count = 0;
  map->key_layout = key_layout;
  map->value_layout = value_layout;
  map->key_compare_instance = key_compare_instance;
}


size_t hlc_map_count(const hlc_Map* map) {
  assert(map != NULL);

  return map->count;
}


void* (hlc_map_lookup)(const hlc_Map* map, const void* key) {
  assert(map != NULL);

  hlc_Layout kv_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&kv_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&kv_layout, map->value_layout);

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, kv_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + key_offset, &map->key_compare_instance);

    if (ordering == 0)
      return (char*)node_element + value_offset;

    node = hlc_avl_link(node, ordering);
  }

  return NULL;
}


typedef struct hlc_KV_ref {
  const void* key;
  const void* value;
} hlc_KV_ref;


typedef struct hlc_KV_assign_context {
  size_t key_offset;
  size_t value_offset;
  const hlc_Assign_trait* key_assign_instance;
  const hlc_Assign_trait* value_assign_instance;
} hlc_KV_assign_context;


static bool hlc_kv_assign(void* target, const void* _source, const struct hlc_Assign_trait* instance) {
  const hlc_KV_ref* source = _source;
  assert(instance != NULL);

  const hlc_KV_assign_context* context = instance->context;
  hlc_assign((char*)target + context->key_offset, source->key, context->key_assign_instance);
  hlc_assign((char*)target + context->value_offset, source->value, context->value_assign_instance);
  return true;  // TODO
}


bool hlc_map_insert(
  hlc_Map* map,
  const void* key,
  const void* value,
  const hlc_Assign_trait* key_assign_instance,
  const hlc_Assign_trait* value_assign_instance
) {
  assert(map != NULL);

  hlc_KV_ref kv_ref = {.key = key, .value = value};

  hlc_Layout kv_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&kv_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&kv_layout, map->value_layout);

  hlc_KV_assign_context kv_assign_context = {
    .key_offset = key_offset,
    .value_offset = value_offset,
    .key_assign_instance = key_assign_instance,
    .value_assign_instance = value_assign_instance,
  };

  hlc_Assign_trait kv_assign_instance = {
    .assign = hlc_kv_assign,
    .context = &kv_assign_context,
  };

  if (map->root != NULL) {
    hlc_AVL* node = map->root;

    while (true) {
      void* node_element = hlc_avl_element(node, kv_layout);
      signed char ordering = hlc_compare(key, (char*)node_element + key_offset, &map->key_compare_instance);

      if (ordering == 0)
        return hlc_assign(node_element, &kv_ref, &kv_assign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, &kv_ref, kv_layout, &kv_assign_instance);

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
    hlc_AVL* node = hlc_avl_new(&kv_ref, kv_layout, &kv_assign_instance);

    if (node != NULL) {
      map->root = node;
      map->count += 1;
      return true;
    } else {
      return false;
    }
  }
}


typedef struct hlc_KV_delete_context {
  size_t key_offset;
  const hlc_Delete_trait* key_delete_instance;
  size_t value_offset;
  const hlc_Delete_trait* value_delete_instance;
} hlc_KV_delete_context;


static void hlc_kv_delete(void* target, const struct hlc_Delete_trait* instance) {
  assert(instance != NULL);

  const hlc_KV_delete_context* context = instance->context;
  hlc_delete((char*)target + context->key_offset, context->key_delete_instance);
  hlc_delete((char*)target + context->value_offset, context->value_delete_instance);
}


bool hlc_map_remove(
  hlc_Map* map,
  const void* key,
  const hlc_Delete_trait* key_delete_instance,
  const hlc_Delete_trait* value_delete_instance1
) {
  assert(map != NULL);

  hlc_Layout kv_layout = {.size = 0, .alignment = 1};
  size_t key_offset = hlc_layout_add(&kv_layout, map->key_layout);
  size_t value_offset = hlc_layout_add(&kv_layout, map->value_layout);

  hlc_KV_delete_context kv_delete_context = {
    .key_offset = key_offset,
    .key_delete_instance = key_delete_instance,
    .value_offset = value_offset,
    .value_delete_instance = value_delete_instance1,
  };

  hlc_Delete_trait kv_delete_instance = {
    .delete = hlc_kv_delete,
    .context = &kv_delete_context,
  };

  hlc_AVL* node = map->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, kv_layout);
    signed char ordering = hlc_compare(key, (char*)node_element + key_offset, &map->key_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, kv_layout, &kv_delete_instance);

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
