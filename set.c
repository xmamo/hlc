#include "set.h"

#include <assert.h>
#include <stdalign.h>
#include <stdio.h>

#include "avl.h"
#include "layout.h"
#include "math.h"


struct hlc_Set {
  hlc_AVL* root;
  hlc_Layout value_layout;
  hlc_Compare_trait value_compare_instance;
};


const hlc_Layout hlc_set_layout = HLC_LAYOUT_OF(hlc_Set);


void hlc_set_make(hlc_Set* set, hlc_Layout value_layout, hlc_Compare_trait value_compare_instance) {
  assert(set != NULL);

  set->root = NULL;
  set->value_layout = value_layout;
  set->value_compare_instance = value_compare_instance;
}


size_t hlc_set_count(const hlc_Set* set) {
  assert(set != NULL);

  return hlc_avl_count(set->root);
}


bool hlc_set_insert(hlc_Set* set, const void* value, const hlc_Assign_trait* value_assign_instance) {
  assert(set != NULL);

  if (set->root != NULL) {
    hlc_AVL* node = set->root;

    while (true) {
      void* node_value = hlc_avl_value(node, set->value_layout);
      int ordering = hlc_compare(value, node_value, &set->value_compare_instance);

      if (ordering == 0)
        return hlc_assign(node_value, value, value_assign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, value, set->value_layout, value_assign_instance);
        bool success = node != NULL;

        if (success && hlc_avl_link(node, 0) == NULL) {
          set->root = node;
        }

        return success;
      }

      node = node_child;
    }
  } else {
    set->root = hlc_avl_new(value, set->value_layout, &hlc_int_assign_instance);
    return set->root != NULL;
  }
}


bool hlc_set_remove(hlc_Set* set, const void* value, const hlc_Delete_trait* value_delete_instance) {
  assert(set != NULL);

  hlc_AVL* node = set->root;

  while (node != NULL) {
    void* node_value = hlc_avl_value(node, set->value_layout);
    int ordering = hlc_compare(value, node_value, &set->value_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, set->value_layout, value_delete_instance);

      if (node == NULL || hlc_avl_link(node, 0) == NULL) {
        set->root = node;
      }

      return true;
    }

    node = hlc_avl_link(node, ordering);
  }

  return false;
}


void hlc_set_dot(const hlc_Set* set, FILE* stream) {
  hlc_avl_dot(set->root, stream);
}
