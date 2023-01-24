#ifndef SAPPHIRE_VAL_LAYER_H
#define SAPPHIRE_VAL_LAYER_H

#include <cstdint>
#include <vector>

// TODO: Make extension sets for certain features? Ex: Raytracing?
struct ValLayer {
    enum LayerFlags {
        LAYER_FLAG_OPTIONAL = 1
    };

    const char* name;
    uint32_t flags;
};

#endif
