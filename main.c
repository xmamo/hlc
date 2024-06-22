#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __has_include
  #if __has_include(<crtdbg.h>)
    #include <crtdbg.h>
  #endif
#endif

#include "layout.h"
#include "map.h"
#include "random.h"
#include "set.h"
#include "stack.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/destroy.h"


static void shuffle(hlc_Random* random, int* xs, size_t count) {
  assert(xs != NULL);

  for (size_t i = 0; i + 1 < count; ++i) {
    size_t j = hlc_random_size_in(random, i, count - 1);
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
  #ifdef __has_include
    #if __has_include(<crtdbg.h>)
      _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
      _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
      _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    #endif
  #endif

  hlc_Random* random = HLC_STACK_ALLOCATE(hlc_random_layout.size);
  hlc_random_create(random);

  puts("Testing hlc_Set:");

  for (size_t i = 1; i <= ITERATIONS; ++i) {
    printf("\tIteration %zu\n", i);

    int* elements = malloc(sizeof(int) * COUNT);
    assert(elements != NULL);

    for (size_t j = 0; j < COUNT; ++j) {
      elements[j] = (int)(j + 1);
    }

    hlc_Set* set = HLC_STACK_ALLOCATE(hlc_set_layout.size);

    hlc_set_create(
      set,
      HLC_LAYOUT_OF(int),
      hlc_int_compare_instance,
      hlc_int_assign_instance,
      hlc_no_destroy_instance
    );

    shuffle(random, elements, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_set_insert(set, &elements[j]);
      bool contains = hlc_set_contains(set, &elements[j]);
      assert(ok && contains);
    }

    shuffle(random, elements, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_set_remove(set, &elements[j]);
      bool contains = hlc_set_contains(set, &elements[j]);
      assert(ok && !contains);
    }

    HLC_STACK_FREE(set);
    free(elements);
  }

  puts("Testing hlc_Map:");

  for (size_t i = 1; i <= ITERATIONS; ++i) {
    printf("\tIteration %zu\n", i);

    int* keys = malloc(sizeof(int) * COUNT);
    assert(keys != NULL);

    for (size_t j = 0; j < COUNT; ++j) {
      keys[j] = (int)(j + 1);
    }

    hlc_Map* map = HLC_STACK_ALLOCATE(hlc_map_layout.size);

    hlc_map_create(
      map,
      HLC_LAYOUT_OF(int),
      HLC_LAYOUT_OF(double),
      hlc_int_compare_instance,
      hlc_int_assign_instance,
      hlc_no_destroy_instance,
      hlc_double_assign_instance,
      hlc_no_destroy_instance
    );

    shuffle(random, keys, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      double value = -keys[j];
      bool ok = hlc_map_insert(map, &keys[j], &value);
      bool contains = hlc_map_contains(map, &keys[j]);
      const double* lookup = hlc_map_lookup(map, &keys[j]);
      assert(ok && contains && lookup != NULL && *lookup == value);
    }

    shuffle(random, keys, COUNT);

    for (size_t j = 0; j < COUNT; ++j) {
      bool ok = hlc_map_remove(map, &keys[j]);
      bool contains = hlc_map_contains(map, &keys[j]);
      const double* lookup = hlc_map_lookup(map, &keys[j]);
      assert(ok && !contains && lookup == NULL);
    }

    HLC_STACK_FREE(map);
    free(keys);
  }

  HLC_STACK_FREE(random);

  return EXIT_SUCCESS;
}
