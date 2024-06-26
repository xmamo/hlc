#include "compare.h"

#include <stddef.h>
#include <wchar.h>

HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(schar, signed char);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(short, short);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(int, int);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(long, long);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(llong, long long);

HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(uchar, unsigned char);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(ushort, unsigned short);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(uint, unsigned);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(ulong, unsigned long);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(ullong, unsigned long long);

HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(size, size_t);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(ptrdiff, ptrdiff_t);

HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(char, char);
HLC_DEFINE_PRIMITIVE_COMPARE_INSTANCE(wchar, wchar_t);
