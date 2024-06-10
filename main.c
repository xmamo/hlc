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


static void shuffle(int* xs, int count) {
  for (int i = 0; i + 1 < count; ++i) {
    int j = random_in(i, count - 1);
    int t = xs[i];
    xs[i] = xs[j];
    xs[j] = t;
  }
}


#define N (10)
#define M (10000)


int main(void) {
  puts("Testing hlc_Set...");

  for (size_t i = 0; i < N; ++i) {
    printf("i = %zu\n", i);

    int xs[M];
    int ys[M];

    for (int j = 0; j < M; ++j) {
      xs[j] = j + 1;
      ys[j] = j + 1;
    }

    srand(i);
    shuffle(xs, M);
    shuffle(ys, M);

    hlc_Set* set = HLC_STACK_ALLOCATE(hlc_set_layout.size);
    assert(set != NULL);

    hlc_set_make(set, HLC_LAYOUT_OF(int), hlc_int_compare_instance);

    for (size_t j = 0; j < M; ++j) {
      bool ok = hlc_set_insert(set, &xs[j], &hlc_int_assign_instance);
      assert(ok);
    }

    for (size_t j = 0; j < M; ++j) {
      bool ok = hlc_set_remove(set, &ys[j], &hlc_int_delete_instance);
      assert(ok);
    }

    HLC_STACK_FREE(set);
  }

  puts("\nTesting hlc_Map...");

  for (size_t i = 0; i < N; ++i) {
    printf("i = %zu\n", i);

    int xs[M];
    int ys[M];

    for (int j = 0; j < M; ++j) {
      xs[j] = j + 1;
      ys[j] = j + 1;
    }

    srand(i);
    shuffle(xs, M);
    shuffle(ys, M);

    hlc_Map* map = HLC_STACK_ALLOCATE(hlc_map_layout.size);
    assert(map != NULL);

    hlc_map_make(map, HLC_LAYOUT_OF(int), hlc_int_compare_instance, HLC_LAYOUT_OF(double));

    for (size_t j = 0; j < M; ++j) {
      double value = -xs[j];
      bool ok = hlc_map_insert(map, &xs[j], &value, &hlc_int_assign_instance, &hlc_double_assign_instance);
      const double* lookup = hlc_map_lookup(map, &xs[j]);
      assert(ok && lookup != NULL && *lookup == value);
    }

    for (size_t j = 0; j < M; ++j) {
      bool ok = hlc_map_remove(map, &ys[j], &hlc_int_delete_instance, &hlc_double_delete_instance);
      const double* lookup = hlc_map_lookup(map, &ys[j]);
      assert(ok && lookup == NULL);
    }

    HLC_STACK_FREE(map);
  }

  return EXIT_SUCCESS;
}
