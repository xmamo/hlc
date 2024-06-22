#include "set.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "avl.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"


struct hlc_Set {
  hlc_AVL* root;
  size_t count;
  hlc_Layout element_layout;
  hlc_Compare_instance element_compare_instance;
  hlc_Assign_instance element_assign_instance;
  hlc_Destroy_instance element_destroy_instance;
};


const hlc_Layout hlc_set_layout = {
  .size = offsetof(hlc_Set, element_destroy_instance) + sizeof(hlc_Destroy_instance),
  .alignment = alignof(hlc_Set),
};


void hlc_set_create(
  hlc_Set* set,
  hlc_Layout element_layout,
  hlc_Compare_instance element_compare_instance,
  hlc_Assign_instance element_assign_instance,
  hlc_Destroy_instance element_destroy_instance
) {
  assert(set != NULL);

  set->root = NULL;
  set->count = 0;
  set->element_layout = element_layout;
  set->element_compare_instance = element_compare_instance;
  set->element_assign_instance = element_assign_instance;
  set->element_destroy_instance = element_destroy_instance;
}


size_t hlc_set_count(const hlc_Set* set) {
  assert(set != NULL);
  return set->count;
}


bool hlc_set_insert(hlc_Set* set, const void* element) {
  assert(set != NULL);
  return hlc_set_insert_with(set, element, set->element_assign_instance);
}


bool hlc_set_insert_with(hlc_Set* set, const void* element, hlc_Assign_instance element_assign_instance) {
  assert(set != NULL);

  if (set->root != NULL) {
    hlc_AVL* node = set->root;

    while (true) {
      void* node_element = hlc_avl_element(node, set->element_layout);
      signed char ordering = hlc_compare(element, node_element, set->element_compare_instance);

      if (ordering == 0)
        return hlc_assign(node_element, element, element_assign_instance);

      hlc_AVL* node_child = hlc_avl_link(node, ordering);

      if (node_child == NULL) {
        node = hlc_avl_insert(node, ordering, element, set->element_layout, element_assign_instance);

        if (node != NULL){
          if (hlc_avl_link(node, 0) == NULL) {
            set->root = node;
          }

          set->count += 1;
          assert(set->count == hlc_avl_count(set->root));
          return true;
        } else {
          return false;
        }
      }

      node = node_child;
    }
  } else {
    hlc_AVL* node = hlc_avl_new(element, set->element_layout, element_assign_instance);

    if (node != NULL) {
      set->root = node;
      set->count += 1;
      return true;
    } else {
      return false;
    }
  }
}


bool hlc_set_remove(hlc_Set* set, const void* element) {
  assert(set != NULL);
  return hlc_set_remove_with(set, element, set->element_destroy_instance);
}


bool hlc_set_remove_with(hlc_Set* set, const void* element, hlc_Destroy_instance element_destroy_instance) {
  assert(set != NULL);

  hlc_AVL* node = set->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, set->element_layout);
    signed char ordering = hlc_compare(element, node_element, set->element_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, set->element_layout, element_destroy_instance);

      if (node == NULL || hlc_avl_link(node, 0) == NULL) {
        set->root = node;
      }

      set->count -= 1;
      assert(set->count == hlc_avl_count(set->root));
      return true;
    }

    node = hlc_avl_link(node, ordering);
  }

  return false;
}


bool hlc_set_contains(const hlc_Set* set, const void* key) {
  assert(set != NULL);

  hlc_AVL* node = set->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, set->element_layout);
    signed char ordering = hlc_compare(key, node_element, set->element_compare_instance);

    if (ordering == 0) {
      return true;
    } else {
      node = hlc_avl_link(node, ordering);
    }
  }

  return false;
}


void hlc_set_destroy(hlc_Set* set) {
  assert(set != NULL);
  hlc_set_destroy_with(set, set->element_destroy_instance);
}


void hlc_set_destroy_with(hlc_Set* set, hlc_Destroy_instance element_destroy_instance) {
  assert(set != NULL);

  hlc_avl_delete(set->root, set->element_layout, element_destroy_instance);
  set->root = NULL;
  set->count = 0;
}


struct hlc_Set_iterator {
  const hlc_AVL* current;
  hlc_Layout element_layout;
};


const hlc_Layout hlc_set_iterator_layout = {
  .size = offsetof(hlc_Set_iterator, element_layout) + sizeof(hlc_Layout),
  .alignment = alignof(hlc_Set_iterator),
};


void hlc_set_iterator(const hlc_Set* set, hlc_Set_iterator* iterator) {
  assert(set != NULL);
  assert(iterator != NULL);

  iterator->current = set->root != NULL ? hlc_avl_xmost(set->root, -1) : NULL;
  iterator->element_layout = set->element_layout;
}


const void* hlc_set_iterator_next(hlc_Set_iterator* iterator) {
  assert(iterator != NULL);

  if (iterator->current != NULL) {
    const void* element = hlc_avl_element(iterator->current, iterator->element_layout);
    iterator->current = hlc_avl_xcessor(iterator->current, +1);
    return element;
  } else {
    return NULL;
  }
}
