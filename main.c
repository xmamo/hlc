#include <stdlib.h>
// #include <time.h>

#include "set.h"

#define N (100)


static int random_in(int min, int max) {
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
  double xs[N];
  double ys[N];

  for (size_t i = 0; i < N; ++i) {
    xs[i] = i;
    ys[i] = i;
  }

  // struct timespec ts;
  // timespec_get(&ts, TIME_UTC);
  // srand(ts.tv_sec);

  srand(0);
  shuffle(xs, N);
  shuffle(ys, N);

  hlc_Set* set = alloca(hlc_set_layout.size);
  hlc_set_make(set);

  for (size_t i = 0; i < N; ++i) {
    hlc_set_insert(set, xs[i]);
  }

  printf("[%zu] ", hlc_set_count(set));
  hlc_set_print(set);
  hlc_set_dot(set, stdout);

  for (size_t i = 0; i <= N; ++i) {
    hlc_set_remove(set, ys[i]);
    printf("After removing %g: [%zu] ", ys[i], hlc_set_count(set));
    hlc_set_print(set);
  }

  hlc_set_dot(set, stdout);

  return EXIT_SUCCESS;
}
