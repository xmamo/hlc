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

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(schar, signed char);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(short, short);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(int, int);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(long, long);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(llong, long long);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(uchar, unsigned char);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ushort, unsigned short);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(uint, unsigned);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ulong, unsigned long);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ullong, unsigned long long);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(float, float);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(double, double);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ldouble, long double);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(size, size_t);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(ptrdiff, ptrdiff_t);

HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(char, char);
HLC_DEFINE_PRIMITIVE_ASSIGN_INSTANCE(wchar, wchar_t);
