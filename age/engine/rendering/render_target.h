#ifndef AGE_RENDER_TARGET_H
#define AGE_RENDER_TARGET_H

#include <engine/data/color.h>

namespace AGE {
    // Abstraction over windows and various other types of attachments
    class RenderTarget {
    public:
        enum ClearFlags {
            CLEAR_DEPTH = 1,
            CLEAR_COLOR = 2
        };

        enum Dimensions {
            RT_1D,
            RT_2D,
            RT_3D
        };

        int clear_flags = ClearFlags::CLEAR_COLOR | ClearFlags::CLEAR_DEPTH;
        Color color = Color(0.1F, 0.1F, 0.1F, 1.0F);
    };
}

#endif
