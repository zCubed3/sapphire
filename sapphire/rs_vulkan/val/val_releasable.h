#ifndef SAPPHIRE_VAL_RELEASABLE_H
#define SAPPHIRE_VAL_RELEASABLE_H

class ValInstance;

class ValReleasable {
public:
    virtual void release(ValInstance *p_val_instance);
};

#endif
