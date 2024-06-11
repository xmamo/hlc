#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "map.h"
#include "set.h"
#include "stack.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/delete.h"


static int random_in(int min, int max) {
  assert(max >= min);
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}


static void shuffle(int* xs, size_t count) {
  assert(xs != NULL);

  for (size_t i = 0; i + 1 < count; ++i) {
    int j = random_in((int)i, (int)(count - 1));
    int t = xs[i];
    xs[i] = xs[j];
    xs[j] = t;
  }
}


#undef NDEBUG
#include <assert.h>

#define ITERATIONS (10)
#define COUNT (10000)


int main(void) {
  puts("Testing hlc_Set:");

  for (size_t i = 1; i <= ITERATIONS; ++i) {
    printf("\tIteration %zu\n", i);

    int* elements = malloc(sizeof(int) * COUNT);
    assert(elements != NULL);

    for (size_t j = 0; j < COUNT; ++j) {
      elements[j] = (int)(j + 1);
    }

    hlc_Set* set = HLC_STACK_ALLOCATE(hlc_set_layout.size);
    assert(set != NULL);

    hlc_set_make(
      set,
      HLC_LAYOUT_OF(int),
      hlc_int_compare_instance,
      hlc_int_assign_instance,
      hlc_no_delete_instance
    );

    shuffle(elements, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_set_insert(set, &elements[j]);
      bool contains = hlc_set_contains(set, &elements[j]);
      assert(ok && contains);
    }

    shuffle(elements, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_set_remove(set, &elements[j]);
      bool contains = hlc_set_contains(set, &elements[j]);
      assert(ok && !contains);
    }

    HLC_STACK_FREE(set);
    free(elements);
  }

  puts("\nTesting hlc_Map:");

  for (size_t i = 1; i <= ITERATIONS; ++i) {
    printf("\tIteration %zu\n", i);

    int* keys = malloc(sizeof(int) * COUNT);
    assert(keys != NULL);

    for (size_t j = 0; j < COUNT; ++j) {
      keys[j] = (int)(j + 1);
    }

    hlc_Map* map = HLC_STACK_ALLOCATE(hlc_map_layout.size);
    assert(map != NULL);

    hlc_map_make(
      map,
      HLC_LAYOUT_OF(int),
      HLC_LAYOUT_OF(double),
      hlc_int_compare_instance,
      hlc_int_assign_instance,
      hlc_no_delete_instance,
      hlc_double_assign_instance,
      hlc_no_delete_instance
    );

    shuffle(keys, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      double value = -keys[j];
      bool ok = hlc_map_insert(map, &keys[j], &value);
      bool contains = hlc_map_contains(map, &keys[j]);
      const double* lookup = hlc_map_lookup(map, &keys[j]);
      assert(ok && contains && lookup != NULL && *lookup == value);
    }

    shuffle(keys, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_map_remove(map, &keys[j]);
      bool contains = hlc_map_contains(map, &keys[j]);
      const double* lookup = hlc_map_lookup(map, &keys[j]);
      assert(ok && !contains && lookup == NULL);
    }

    HLC_STACK_FREE(map);
    free(keys);
  }

  return EXIT_SUCCESS;
}
