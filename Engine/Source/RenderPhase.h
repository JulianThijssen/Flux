#pragma once
#ifndef RENDER_PHASE_H
#define RENDER_PHASE_H

#include "String.h"

namespace Flux {
    class RenderPhase {
    public:
        RenderPhase(const char* name) : RenderPhase(String(name)) { }

        RenderPhase(String name)
        :
            name(name)
        {

        }

        const String& getPassName() {
            return name;
        }

        void enable() {
            enabled = true;
        }

        void disable() {
            enabled = false;
        }

        bool isEnabled() {
            return enabled;
        }

    private:
        String name;
        bool enabled = true;
    };
}


#endif /* RENDER_PHASE_H */
