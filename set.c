#include "set.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "avl.h"
#include "layout.h"
#include "math.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"


struct hlc_Set {
  hlc_AVL* root;
  size_t count;
  hlc_Layout element_layout;
  hlc_Compare_instance element_compare_instance;
  hlc_Destroy_instance element_destroy_instance;
};

const hlc_Layout hlc_set_layout = {.size = sizeof(hlc_Set), .alignment = alignof(hlc_Set)};


struct hlc_Set_iterator {
  const hlc_AVL* current;
  hlc_Layout element_layout;
};

const hlc_Layout hlc_set_iterator_layout = {.size = sizeof(hlc_Set_iterator), .alignment = alignof(hlc_Set_iterator)};


void hlc_set_create(
  hlc_Set* set,
  hlc_Layout element_layout,
  hlc_Compare_instance element_compare_instance,
  hlc_Destroy_instance element_destroy_instance
) {
  assert(set != NULL);

  set->root = NULL;
  set->count = 0;
  set->element_layout = element_layout;
  set->element_compare_instance = element_compare_instance;
  set->element_destroy_instance = element_destroy_instance;
}


size_t hlc_set_count(const hlc_Set* set) {
  assert(set != NULL);
  return set->count;
}


bool hlc_set_insert(hlc_Set* set, const void* element, hlc_Assign_instance element_assign_instance) {
  assert(set != NULL);

  if (set->root != NULL) {
    hlc_AVL* node = set->root;

    while (true) {
      void* node_element = hlc_avl_element(node, set->element_layout);
      signed char ordering = hlc_compare(element, node_element, set->element_compare_instance);

      if (ordering == 0)
        return hlc_reassign(node_element, element, element_assign_instance);

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

  hlc_AVL* node = set->root;

  while (node != NULL) {
    void* node_element = hlc_avl_element(node, set->element_layout);
    signed char ordering = hlc_compare(element, node_element, set->element_compare_instance);

    if (ordering == 0) {
      node = hlc_avl_remove(node, set->element_layout, set->element_destroy_instance);

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


void hlc_set_clear(hlc_Set* set) {
  assert(set != NULL);

  hlc_set_destroy(set);
  set->root = NULL;
  set->count = 0;
}


void hlc_set_destroy(hlc_Set* set) {
  assert(set != NULL);
  hlc_avl_delete(set->root, set->element_layout, set->element_destroy_instance);
}


void hlc_set_move_reassign(hlc_Set* target, hlc_Set* source) {
  assert(target != NULL);
  assert(source != NULL);

  hlc_avl_delete(target->root, target->element_layout, target->element_destroy_instance);
  *target = *source;

  source->root = NULL;
  source->count = 0;
}


signed char hlc_set_compare(const hlc_Set* set1, const hlc_Set* set2, hlc_Compare_instance compare_instance) {
  assert(set1 != NULL);
  assert(set2 != NULL);

  hlc_Set_iterator iterator1;
  hlc_set_iterator(set1, &iterator1);

  hlc_Set_iterator iterator2;
  hlc_set_iterator(set2, &iterator2);

  for (size_t i = 0; i < HLC_MIN(set1->count, set2->count); ++i) {
    const void* element1 = hlc_set_iterator_next(&iterator1);
    const void* element2 = hlc_set_iterator_next(&iterator2);
    signed char ordering = hlc_compare(element1, element2, compare_instance);

    if (ordering != 0)
      return ordering;
  }

  return HLC_COMPARE(set1->count, set2->count);
}


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
