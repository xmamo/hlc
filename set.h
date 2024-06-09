#ifndef HLC_SET_H
#define HLC_SET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "api.h"
#include "layout.h"
#include "traits/assign.h"
#include "traits/compare.h"
#include "traits/delete.h"

typedef struct hlc_Set hlc_Set;
HLC_API extern const hlc_Layout hlc_set_layout;

HLC_API void hlc_set_make(hlc_Set* set, hlc_Layout value_layout, hlc_Compare_trait value_compare_instance);
HLC_API size_t hlc_set_count(const hlc_Set* set);
HLC_API bool hlc_set_insert(hlc_Set* set, const void* value, const hlc_Assign_trait* value_assign_instance);
HLC_API bool hlc_set_remove(hlc_Set* set, const void* value, const hlc_Delete_trait* value_delete_instance);
HLC_API void hlc_set_dot(const hlc_Set* set, FILE* stream);

#endif
