#include "set.h"

#include <assert.h>
#include <stdalign.h>
#include <stdio.h>

#include "math.h"


struct hlc_Set {
  hlc_AVL* root;
};


const hlc_Layout hlc_set_layout = {
  .size = sizeof(hlc_Set),
  .alignment = alignof(hlc_Set),
};


void hlc_set_make(hlc_Set* set) {
  assert(set != NULL);
  set->root = NULL;
}


size_t hlc_set_count(const hlc_Set* set) {
  assert(set != NULL);
  return hlc_avl_count(set->root);
}


bool hlc_set_insert(hlc_Set* set, double value) {
  assert(set != NULL);

  if (set->root != NULL) {
    hlc_AVL* node = set->root;

    while (true) {
      double* node_value_ref = hlc_avl_value_ref(node);
      int ordering = HLC_COMPARE(value, *node_value_ref);

      if (ordering == 0) {
        *node_value_ref = value;
        return true;
      }

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, value);
        bool success = node != NULL;

        if (success && hlc_avl_link(node, 0) == NULL) {
          set->root = node;
        }

        return success;
      }

      node = node_child;
    }
  } else {
    set->root = hlc_avl_new(value);
    return set->root != NULL;
  }
}


bool hlc_set_remove(hlc_Set* set, double value) {
  assert(set != NULL);

  hlc_AVL* node = set->root;

  while (node != NULL) {
    double node_value = *hlc_avl_value_ref(node);
    int ordering = HLC_COMPARE(value, node_value);

    if (ordering == 0) {
      node = hlc_avl_remove(node);

      if (node == NULL || hlc_avl_link(node, 0) == NULL) {
        set->root = node;
      }

      return true;
    }

    node = hlc_avl_link(node, ordering);
  }

  return false;
}


void hlc_set_print(const hlc_Set* set) {
  const hlc_AVL* node = set->root;

  if (node != NULL) {
    node = hlc_avl_xmost(node, -1);
    printf("%g", *hlc_avl_value_ref(node));

    while ((node = hlc_avl_xcessor(node, +1)) != NULL) {
      printf(", %g", *hlc_avl_value_ref(node));
    }
  }

  putchar('\n');
}


void hlc_set_dot(const hlc_Set* set, FILE* stream) {
  hlc_avl_dot(set->root, stream);
}
