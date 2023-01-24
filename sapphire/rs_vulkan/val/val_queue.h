#ifndef SAPPHIRE_VAL_QUEUE_H
#define SAPPHIRE_VAL_QUEUE_H

#include <vulkan/vulkan.h>

struct ValQueue {
    enum QueueType {
        QUEUE_TYPE_GRAPHICS,
        QUEUE_TYPE_TRANSFER,

#ifdef SDL_SUPPORT
        QUEUE_TYPE_PRESENT,
#endif

        // TODO: More queue types
    };

    QueueType type;
    uint32_t family;
    VkQueue queue;
    VkCommandPool pool;
};

#endif
