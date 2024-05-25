#ifndef HLC_SET_H
#define HLC_SET_H

#include <stdbool.h>
#include <stdio.h>

#include "avl.h"
#include "layout.h"

typedef struct hlc_Set hlc_Set;
extern const hlc_Layout hlc_set_layout;

void hlc_set_make(hlc_Set* set);
size_t hlc_set_count(const hlc_Set* set);
bool hlc_set_insert(hlc_Set* set, double value);
bool hlc_set_remove(hlc_Set* set, double value);
void hlc_set_print(const hlc_Set* set);
void hlc_set_dot(const hlc_Set* set, FILE* stream);

#endif
