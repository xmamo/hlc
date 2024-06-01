#ifndef HLC_TRAITS_ASSIGN_H
#define HLC_TRAITS_ASSIGN_H

#include "../api.h"

#include <stdbool.h>
#include <stddef.h>

/// @brief Trait for types which can be copied or moved.
typedef struct hlc_Assign_trait {
  bool (*assign)(void* t_target, const void* t_source, const struct hlc_Assign_trait* instance);
  void* context;
} hlc_Assign_trait;

static inline bool hlc_assign(const hlc_Assign_trait* instance, void* t_target, const void* t_source) {
  return instance->assign(t_target, t_source, instance);
}

#define HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(t, t_name) const hlc_Assign_trait hlc_##t_name##_assign_instance

#define HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(t, t_name)                                                                     \
  static bool hlc_##t_name##_assign(void* t_name##_target, const void* t_name##_source, const hlc_Assign_trait* instance) { \
    t* target = t_name##_target;                                                                                            \
    const t* source = t_name##_source;                                                                                      \
    (void)instance;                                                                                                         \
                                                                                                                            \
    *target = *source;                                                                                                      \
    return true;                                                                                                            \
  }                                                                                                                         \
                                                                                                                            \
  const hlc_Assign_trait hlc_##t_name##_assign_instance = {                                                                 \
    .assign = hlc_##t_name##_assign,                                                                                        \
    .context = NULL,                                                                                                        \
  }

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(signed char, schar);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(short, short);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(int, int);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(long, long);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(long long, llong);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(unsigned char, uchar);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(unsigned short, ushort);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(unsigned, uint);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(unsigned long, ulong);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(unsigned long long, ullong);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(float, float);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(double, double);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(long double, ldouble);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(size_t, size);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ptrdiff_t, ptrdiff);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(char, char);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(wchar_t, wchar);

#endif
