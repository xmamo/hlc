#ifndef HLC_TRAITS_DELETE_H
#define HLC_TRAITS_DELETE_H

#include "../api.h"

#include <assert.h>
#include <stddef.h>

/// @brief Trait for object deletion.
typedef struct hlc_Delete_trait {
  void (*delete)(void* target, const struct hlc_Delete_trait* instance);
  void* context;
} hlc_Delete_trait;

/// @pre instance != NULL
static inline void hlc_delete(void* target, const hlc_Delete_trait* instance) {
  assert(instance != NULL);
  instance->delete(target, instance);
}

#define HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(t, t_name) const hlc_Delete_trait hlc_##t_name##_delete_instance

#define HLC_DEFINE_PRIMITIVE_DELETE_INSTANCE(t, t_name)                               \
  static void hlc_##t_name##_delete(void* target, const hlc_Delete_trait* instance) { \
    (void)target;                                                                     \
    (void)instance;                                                                   \
  }                                                                                   \
                                                                                      \
  const hlc_Delete_trait hlc_##t_name##_delete_instance = {                           \
    .delete = hlc_##t_name##_delete,                                                  \
    .context = NULL,                                                                  \
  }

extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(signed char, schar);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(short, short);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(int, int);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(long, long);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(long long, llong);

extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(unsigned char, uchar);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(unsigned short, ushort);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(unsigned, uint);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(unsigned long, ulong);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(unsigned long long, ullong);

extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(float, float);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(double, double);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(long double, ldouble);

extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(size_t, size);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(ptrdiff_t, ptrdiff);

extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(char, char);
extern HLC_API HLC_DECLARE_PRIMITIVE_DELETE_INSTANCE(wchar_t, wchar);

#endif
