#include "val_releasable.h"

#include <rs_vulkan/val/val_instance.h>

void ValReleasable::release(ValInstance *p_val_instance) {
    p_val_instance->await_frame();
}