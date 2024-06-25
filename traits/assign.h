#ifndef HLC_TRAITS_ASSIGN_H
#define HLC_TRAITS_ASSIGN_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#include "../api.h"

typedef struct hlc_Assign_trait {
  bool (*assign)(void* target, const void* source, const struct hlc_Assign_trait* trait, void* context);
  bool (*reassign)(void* target, const void* source, const struct hlc_Assign_trait* trait, void* context);
} hlc_Assign_trait;

typedef struct hlc_Assign_instance {
  const hlc_Assign_trait* trait;
  void* context;
} hlc_Assign_instance;

static inline bool hlc_assign(void* target, const void* source, hlc_Assign_instance instance) {
  return instance.trait->assign(target, source, instance.trait, instance.context);
}

static inline bool hlc_reassign(void* target, const void* source, hlc_Assign_instance instance) {
  return instance.trait->reassign(target, source, instance.trait, instance.context);
}

#define HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(t_name, t) const hlc_Assign_instance hlc_##t_name##_assign_instance

#define HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(t_name, t)         \
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
  static bool hlc_##t_name##_reassign(                          \
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
    .reassign = hlc_##t_name##_reassign,                        \
  };                                                            \
                                                                \
  const hlc_Assign_instance hlc_##t_name##_assign_instance = {  \
    .trait = &hlc_##t_name##_assign_trait,                      \
    .context = NULL,                                            \
  }

extern HLC_API const hlc_Assign_instance hlc_no_assign_instance;

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(schar, signed char);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(short, short);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(int, int);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(long, long);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(llong, long long);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(uchar, unsigned char);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ushort, unsigned short);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(uint, unsigned);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ulong, unsigned long);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ullong, unsigned long long);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(float, float);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(double, double);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ldouble, long double);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(size, size_t);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(ptrdiff, ptrdiff_t);

extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(char, char);
extern HLC_API HLC_DECLARE_PRIMITIVE_ASSIGN_INSTANCE(wchar, wchar_t);

#endif
