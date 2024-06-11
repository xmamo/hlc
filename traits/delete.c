#include "delete.h"

#include <stddef.h>

static void hlc_no_delete(void* target, const struct hlc_Delete_trait* trait, void* context) {
  (void)target;
  (void)trait;
  (void)context;
}

static const hlc_Delete_trait hlc_no_delete_trait = {
  .delete = hlc_no_delete,
};

const hlc_Delete_instance hlc_no_delete_instance = {
  .trait = &hlc_no_delete_trait,
  .context = NULL,
};
