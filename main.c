#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
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


#define N (100)
#define M (10000)


int main(void) {
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
      assert(ok && hlc_set_count(set) == j + 1);
    }

    for (size_t j = 0; j < M; ++j) {
      bool ok = hlc_set_remove(set, &ys[j], &hlc_int_delete_instance);
      assert(ok && hlc_set_count(set) == M - (j + 1));
    }

    HLC_STACK_FREE(set);
  }

  return EXIT_SUCCESS;
}
