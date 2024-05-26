#include "set.h"

#include <assert.h>
#include <stdalign.h>
#include <stdio.h>


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

      if (value < *node_value_ref) {
        hlc_AVL* node_left = hlc_avl_link(node, -1);

        if (node_left == NULL) {
          node = hlc_avl_insert(node, -1, value);

          if (node != NULL) {
            if (hlc_avl_link(node, 0) == NULL) {
              set->root = node;
            }

            return true;
          } else {
            return false;
          }
        } else {
          node = node_left;
        }
      } else if (value > *node_value_ref) {
        hlc_AVL* node_right = hlc_avl_link(node, +1);

        if (node_right == NULL) {
          node = hlc_avl_insert(node, +1, value);

          if (node != NULL) {
            if (hlc_avl_link(node, 0) == NULL) {
              set->root = node;
            }

            return true;
          } else {
            return false;
          }
        } else {
          node = node_right;
        }
      } else {
        *node_value_ref = value;
        return true;
      }
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

    if (value < node_value) {
      node = hlc_avl_link(node, -1);
    } else if (value > node_value) {
      node = hlc_avl_link(node, +1);
    } else {
      node = hlc_avl_remove(node);

      if (node == NULL || hlc_avl_link(node, 0) == NULL) {
        set->root = NULL;
      }

      return true;
    }
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
