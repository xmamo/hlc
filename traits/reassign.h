#ifndef HLC_TRAITS_REASSIGN_H
#define HLC_TRAITS_REASSIGN_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#include "../api.h"

typedef struct hlc_Reassign_trait {
  bool (*reassign)(void* target, const void* source, const struct hlc_Reassign_trait* trait, void* context);
} hlc_Reassign_trait;

typedef struct hlc_Reassign_instance {
  const hlc_Reassign_trait* trait;
  void* context;
} hlc_Reassign_instance;

static inline bool hlc_reassign(void* target, const void* source, hlc_Reassign_instance instance) {
  return instance.trait->reassign(target, source, instance.trait, instance.context);
}

#define HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(t_name, t) const hlc_Reassign_instance hlc_##t_name##_reassign_instance

#define HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(t_name, t)           \
  static bool hlc_##t_name##_reassign(                              \
    void* _target,                                                  \
    const void* _source,                                            \
    const hlc_Reassign_trait* instance,                             \
    void* context                                                   \
  ) {                                                               \
    t* target = _target;                                            \
    const t* source = _source;                                      \
    (void)instance;                                                 \
    (void)context;                                                  \
                                                                    \
    assert(target != NULL);                                         \
    assert(source != NULL);                                         \
    *target = *source;                                              \
    return true;                                                    \
  }                                                                 \
                                                                    \
  static const hlc_Reassign_trait hlc_##t_name##_reassign_trait = { \
    .reassign = hlc_##t_name##_reassign,                            \
  };                                                                \
                                                                    \
  const hlc_Reassign_instance hlc_##t_name##_reassign_instance = {  \
    .trait = &hlc_##t_name##_reassign_trait,                        \
    .context = NULL,                                                \
  }

extern HLC_API const hlc_Reassign_instance hlc_no_reassign_instance;

extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(schar, signed char);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(short, short);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(int, int);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(long, long);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(llong, long long);

extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(uchar, unsigned char);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(ushort, unsigned short);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(uint, unsigned);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(ulong, unsigned long);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(ullong, unsigned long long);

extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(float, float);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(double, double);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(ldouble, long double);

extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(size, size_t);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(ptrdiff, ptrdiff_t);

extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(char, char);
extern HLC_API HLC_DECLARE_PRIMITIVE_REASSIGN_INSTANCE(wchar, wchar_t);

#endif
