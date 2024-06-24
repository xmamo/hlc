#include "reassign.h"

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

static bool hlc_no_reassign(void* target, const void* source, const struct hlc_Reassign_trait* trait, void* context) {
  (void)target;
  (void)source;
  (void)trait;
  (void)context;
  return true;
}

static const hlc_Reassign_trait hlc_no_reassign_trait = {
  .reassign = hlc_no_reassign,
};

const hlc_Reassign_instance hlc_no_reassign_instance = {
  .trait = &hlc_no_reassign_trait,
  .context = NULL,
};

HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(schar, signed char);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(short, short);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(int, int);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(long, long);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(llong, long long);

HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(uchar, unsigned char);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(ushort, unsigned short);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(uint, unsigned);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(ulong, unsigned long);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(ullong, unsigned long long);

HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(float, float);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(double, double);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(ldouble, long double);

HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(size, size_t);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(ptrdiff, ptrdiff_t);

HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(char, char);
HLC_DEFINE_PRIMITIVE_REASSIGN_INSTANCE(wchar, wchar_t);
