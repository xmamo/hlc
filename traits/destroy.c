#include "destroy.h"

#include <stddef.h>

static void hlc_no_destroy(void* target, const struct hlc_Destroy_trait* trait, void* context) {
  (void)target;
  (void)trait;
  (void)context;
}

static const hlc_Destroy_trait hlc_no_destroy_trait = {
  .destroy = hlc_no_destroy,
};

const hlc_Destroy_instance hlc_no_destroy_instance = {
  .trait = &hlc_no_destroy_trait,
  .context = NULL,
};
