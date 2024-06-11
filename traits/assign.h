#ifndef HLC_TRAITS_ASSIGN_H
#define HLC_TRAITS_ASSIGN_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#include "../api.h"

typedef struct hlc_Assign_trait {
  bool (*assign)(void* target, const void* source, const struct hlc_Assign_trait* trait, void* context);
} hlc_Assign_trait;

typedef struct hlc_Assign_instance {
  const hlc_Assign_trait* trait;
  void* context;
} hlc_Assign_instance;

static inline bool hlc_assign(void* target, const void* source, hlc_Assign_instance instance) {
  return instance.trait->assign(target, source, instance.trait, instance.context);
}

#define HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(t, t_name) const hlc_Assign_instance hlc_##t_name##_assign_instance

#define HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(t, t_name)         \
  static bool hlc_##t_name##_assign(                            \
    void* _target,                                              \
    const void* _source,                                        \
    const hlc_Assign_trait* instance,                           \
    void* context                                               \
  ) {                                                           \
    t* target = _target;                                        \
    const t* source = _source;                                  \
    (void)instance;                                             \
    (void)context;                                              \
                                                                \
    assert(target != NULL);                                     \
    assert(source != NULL);                                     \
    *target = *source;                                          \
    return true;                                                \
  }                                                             \
                                                                \
  static const hlc_Assign_trait hlc_##t_name##_assign_trait = { \
    .assign = hlc_##t_name##_assign,                            \
  };                                                            \
                                                                \
  const hlc_Assign_instance hlc_##t_name##_assign_instance = {  \
    .trait = &hlc_##t_name##_assign_trait,                      \
    .context = NULL,                                            \
  }

extern HLC_API const hlc_Assign_instance hlc_no_assign_instance;

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
