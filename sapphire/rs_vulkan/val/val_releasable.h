#ifndef SAPPHIRE_VAL_RELEASABLE_H
#define SAPPHIRE_VAL_RELEASABLE_H

class ValInstance;

class ValReleasable {
public:
    // Allows this object to release Vulkan resources associated with it
    virtual void release(ValInstance *p_val_instance);
};

#endif
