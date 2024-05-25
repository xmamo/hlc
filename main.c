#include <assert.h>
#include <stdlib.h>

#include "set.h"


static int random_in(int min, int max) {
  assert(max >= min);
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}


static void shuffle(double* xs, size_t count) {
  for (size_t i = 0; i + 1 < count; ++i) {
    size_t j = random_in(i, count - 1);
    double t = xs[i];
    xs[i] = xs[j];
    xs[j] = t;
  }
}


int main(void) {
  for (unsigned i = 0; i < 100; ++i) {
    double xs[10000];
    double ys[10000];

    for (size_t j = 0; j < 10000; ++j) {
      xs[j] = j;
      ys[j] = j;
    }

    srand(i);
    shuffle(xs, 10000);
    shuffle(ys, 10000);

    hlc_Set* set = alloca(hlc_set_layout.size);
    hlc_set_make(set);

    for (size_t j = 0; j < 10000; ++j) {
      hlc_set_insert(set, xs[j]);
    }

    assert(hlc_set_count(set) == 10000);

    for (size_t j = 0; j < 10000; ++j) {
      hlc_set_remove(set, ys[j]);
    }

    assert(hlc_set_count(set) == 0);
  }

  return EXIT_SUCCESS;
}
