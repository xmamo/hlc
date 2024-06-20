// hlc_Random implements MT19937 (http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/ARTICLES/mt.pdf).

#include "random.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "layout.h"


#define HLC_RANDOM_W 32
#define HLC_RANDOM_N 624
#define HLC_RANDOM_M 397
#define HLC_RANDOM_R 31
#define HLC_RANDOM_A 0x9908B0DF
#define HLC_RANDOM_U 11
#define HLC_RANDOM_S 7
#define HLC_RANDOM_B 0x9D2C5680
#define HLC_RANDOM_T 15
#define HLC_RANDOM_C 0xEFC60000
#define HLC_RANDOM_L 18

static_assert(HLC_RANDOM_W >= 2 && HLC_RANDOM_W <= 32, "HLC_RANDOM_W >= 2 && HLC_RANDOM_W <= 32");
static_assert(HLC_RANDOM_N >= 1, "HLC_RANDOM_N >= 1");
static_assert(HLC_RANDOM_M >= 1 && HLC_RANDOM_M <= HLC_RANDOM_N, "HLC_RANDOM_M >= 1 && HLC_RANDOM_M <= HLC_RANDOM_N");
static_assert(HLC_RANDOM_R >= 0 && HLC_RANDOM_R < HLC_RANDOM_W, "HLC_RANDOM_R >= 0 && HLC_RANDOM_R < HLC_RANDOM_W");
static_assert(HLC_RANDOM_U >= 0, "HLC_RANDOM_U >= 0");
static_assert(HLC_RANDOM_S >= 0, "HLC_RANDOM_S >= 0");
static_assert(HLC_RANDOM_T >= 0, "HLC_RANDOM_T >= 0");
static_assert(HLC_RANDOM_L >= 0, "HLC_RANDOM_L >= 0");

#define HLC_RANDOM_HI ((unsigned long)((~0ULL << HLC_RANDOM_R) & ~(~0ULL << HLC_RANDOM_W)))
#define HLC_RANDOM_LO ((unsigned long)(~(~0ULL << HLC_RANDOM_R) & ~(~0ULL << HLC_RANDOM_W)))


struct hlc_Random {
  unsigned long x[HLC_RANDOM_N];
  size_t i;
};


const hlc_Layout hlc_random_layout = {
  .size = offsetof(hlc_Random, i) + sizeof(size_t),
  .alignment = alignof(hlc_Random),
};


void hlc_random_make(hlc_Random* random) {
  time_t t = time(NULL);

  if (t != (time_t)-1) {
    double diff = t >= 0 ? difftime(t, 0) : difftime(0, t);

    if (isfinite(diff)) {
      assert(diff >= 0);
      hlc_random_make_with(random, (unsigned long)fmod(diff, pow(2.0, CHAR_BIT * sizeof(unsigned long))));
      return;
    }
  }

  hlc_random_make_with(random, 0);
}


void hlc_random_make_with(hlc_Random* random, unsigned long seed) {
  assert(random != NULL);

  random->x[0] = seed;
  random->x[0] &= (unsigned long)~(~0ULL << HLC_RANDOM_W);

  for (size_t i = 1; i < HLC_RANDOM_N; ++i) {
    random->x[i] = ((size_t)1812433253 * (random->x[i - 1] ^ (random->x[i - 1] >> (HLC_RANDOM_W - 2))) + i);
    random->x[i] &= (unsigned long)~(~0ULL << HLC_RANDOM_W);
  }

  random->i = 0;
}


/// @brief Produces a random number of HLC_RANDOM_W bits.
static unsigned long hlc_random_next(hlc_Random* random) {
  assert(random != NULL);

  unsigned long y = (random->x[random->i] & HLC_RANDOM_HI) | (random->x[(random->i + 1) % HLC_RANDOM_N] & HLC_RANDOM_LO);
  y = random->x[(random->i + HLC_RANDOM_M) % HLC_RANDOM_N] ^ (y >> 1) ^ ((y & 1) != 0 ? HLC_RANDOM_A : 0);

  random->x[random->i] = y;
  random->i = (random->i + 1) % HLC_RANDOM_N;

  y ^= y >> HLC_RANDOM_U;
  y ^= (y << HLC_RANDOM_S) & HLC_RANDOM_B;
  y ^= (y << HLC_RANDOM_T) & HLC_RANDOM_C;
  y ^= y >> HLC_RANDOM_L;

  return y;
}


#define HLC_DEFINE_RANDOM_UNSIGNED_IN(ut_name, ut)                   \
  ut hlc_random_##ut_name##_in(hlc_Random* random, ut min, ut max) { \
    assert(max >= min);                                              \
                                                                     \
    ut mask = 0;                                                     \
    size_t bits = 0;                                                 \
                                                                     \
    while (mask < max - min) {                                       \
      mask = (mask << 1) | 1;                                        \
      bits += 1;                                                     \
    }                                                                \
                                                                     \
    ut n;                                                            \
                                                                     \
    do {                                                             \
      n = hlc_random_next(random);                                   \
                                                                     \
      for (size_t i = HLC_RANDOM_W; i < bits; i += HLC_RANDOM_W) {   \
        n = (n << HLC_RANDOM_W) | hlc_random_next(random);           \
      }                                                              \
                                                                     \
      n &= mask;                                                     \
    } while (n > max - min);                                         \
                                                                     \
    return min + n;                                                  \
  }


HLC_DEFINE_RANDOM_UNSIGNED_IN(uchar, unsigned char)
HLC_DEFINE_RANDOM_UNSIGNED_IN(ushort, unsigned short)
HLC_DEFINE_RANDOM_UNSIGNED_IN(uint, unsigned)
HLC_DEFINE_RANDOM_UNSIGNED_IN(ulong, unsigned long)
HLC_DEFINE_RANDOM_UNSIGNED_IN(ullong, unsigned long long)
HLC_DEFINE_RANDOM_UNSIGNED_IN(size, size_t)
