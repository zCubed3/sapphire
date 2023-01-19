#ifndef AGE_RENDER_SERVER_H
#define AGE_RENDER_SERVER_H

namespace AGE {
    class RenderServer {
    protected:
        RenderServer* singleton = nullptr;

    public:
        virtual const char* get_name() = 0;

        virtual void initialize() = 0;
    };
}

#endif
