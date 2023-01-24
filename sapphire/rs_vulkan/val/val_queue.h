#ifndef SAPPHIRE_VAL_QUEUE_H
#define SAPPHIRE_VAL_QUEUE_H

#include <vulkan/vulkan.h>

class ValInstance;

struct ValQueue {
    enum QueueType {
        QUEUE_TYPE_GRAPHICS,
        QUEUE_TYPE_TRANSFER,

#ifdef SDL_SUPPORT
        QUEUE_TYPE_PRESENT,
#endif

        // TODO: More queue types
    };

    bool create_pool(ValInstance* p_val_instance);
    VkCommandBuffer allocate_buffer(ValInstance *p_val_instance);

    QueueType type;
    uint32_t family;
    VkQueue vk_queue;
    VkCommandPool vk_pool;
};

#endif
