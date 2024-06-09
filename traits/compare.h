#ifndef HLC_TRAITS_COMPARE_H
#define HLC_TRAITS_COMPARE_H

#include "../api.h"

#include <assert.h>
#include <stddef.h>

/// @brief Trait for types which can compared.
typedef struct hlc_Compare_trait {
  signed char (*compare)(const void* x, const void* y, const struct hlc_Compare_trait* instance);
  void* context;
} hlc_Compare_trait;

/// @pre instance != NULL
static inline signed char hlc_compare(const void* x, const void* y, const hlc_Compare_trait* instance) {
  assert(instance != NULL);
  return instance->compare(x, y, instance);
}

#define HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(t, t_name) const hlc_Compare_trait hlc_##t_name##_compare_instance

#define HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(t, t_name)                                                         \
  static signed char hlc_##t_name##_compare(const void* _x, const void* _y, const hlc_Compare_trait* instance) { \
    const t* x = _x;                                                                                             \
    const t* y = _y;                                                                                             \
    (void)instance;                                                                                              \
                                                                                                                 \
    return *x < *y ? -1 : *x > *y ? +1 : 0;                                                                      \
  }                                                                                                              \
                                                                                                                 \
  const hlc_Compare_trait hlc_##t_name##_compare_instance = {                                                    \
    .compare = hlc_##t_name##_compare,                                                                           \
    .context = NULL,                                                                                             \
  }

extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(signed char, schar);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(short, short);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(int, int);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(long, long);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(long long, llong);

extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(unsigned char, uchar);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(unsigned short, ushort);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(unsigned, uint);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(unsigned long, ulong);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(unsigned long long, ullong);

extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(size_t, size);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(ptrdiff_t, ptrdiff);

extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(char, char);
extern HLC_API HLC_DECLARE_PRIMITIVE_COMPARE_INSTANCE(wchar_t, wchar);

#endif
