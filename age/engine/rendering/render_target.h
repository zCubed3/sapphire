#ifndef AGE_RENDER_TARGET_H
#define AGE_RENDER_TARGET_H

namespace AGE {
    // Abstraction over windows and various other types of attachments
    class RenderTarget {
    public:
        enum ClearFlags {
            CLEAR_DEPTH = 1,
            CLEAR_COLOR = 2
        };
        
        int clear_flags = ClearFlags::CLEAR_COLOR | ClearFlags::CLEAR_DEPTH;
    };
}

#endif
