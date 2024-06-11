#ifndef HLC_TRAITS_DELETE_H
#define HLC_TRAITS_DELETE_H

#include "../api.h"

typedef struct hlc_Delete_trait {
  void (*delete)(void* target, const struct hlc_Delete_trait* trait, void* context);
} hlc_Delete_trait;

typedef struct hlc_Delete_instance {
  const hlc_Delete_trait* trait;
  void* context;
} hlc_Delete_instance;

static inline void hlc_delete(void* target, hlc_Delete_instance instance) {
  instance.trait->delete(target, instance.trait, instance.context);
}

extern HLC_API const hlc_Delete_instance hlc_no_delete_instance;

#endif
