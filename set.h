#ifndef HLC_SET_H
#define HLC_SET_H

#include <stdbool.h>
#include <stdio.h>

#include "base.h"
#include "layout.h"

typedef struct hlc_Set hlc_Set;
HLC_API extern const hlc_Layout hlc_set_layout;

HLC_API void hlc_set_make(hlc_Set* set);
HLC_API size_t hlc_set_count(const hlc_Set* set);
HLC_API bool hlc_set_insert(hlc_Set* set, double value);
HLC_API bool hlc_set_remove(hlc_Set* set, double value);
HLC_API void hlc_set_print(const hlc_Set* set);
HLC_API void hlc_set_dot(const hlc_Set* set, FILE* stream);

#endif
