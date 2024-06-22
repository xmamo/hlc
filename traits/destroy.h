#ifndef HLC_TRAITS_DESTROY_H
#define HLC_TRAITS_DESTROY_H

#include "../api.h"

typedef struct hlc_Destroy_trait {
  void (*destroy)(void* target, const struct hlc_Destroy_trait* trait, void* context);
} hlc_Destroy_trait;

typedef struct hlc_Destroy_instance {
  const hlc_Destroy_trait* trait;
  void* context;
} hlc_Destroy_instance;

static inline void hlc_destroy(void* target, hlc_Destroy_instance instance) {
  instance.trait->destroy(target, instance.trait, instance.context);
}

extern HLC_API const hlc_Destroy_instance hlc_no_destroy_instance;

#endif
