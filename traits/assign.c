#include "assign.h"

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

static bool hlc_no_assign(void* target, const void* source, const struct hlc_Assign_trait* trait, void* context) {
  (void)target;
  (void)source;
  (void)trait;
  (void)context;
  return true;
}

static const hlc_Assign_trait hlc_no_assign_trait = {
  .assign = hlc_no_assign,
};

const hlc_Assign_instance hlc_no_assign_instance = {
  .trait = &hlc_no_assign_trait,
  .context = NULL,
};

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(signed char, schar);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(short, short);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(int, int);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(long, long);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(long long, llong);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(unsigned char, uchar);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(unsigned short, ushort);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(unsigned, uint);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(unsigned long, ulong);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(unsigned long long, ullong);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(float, float);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(double, double);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(long double, ldouble);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(size_t, size);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ptrdiff_t, ptrdiff);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(char, char);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(wchar_t, wchar);
