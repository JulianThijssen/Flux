#pragma once
#ifndef RENDER_PHASE_H
#define RENDER_PHASE_H

#include "String.h"

#include "Scene.h"

#include "nvToolsExt.h"

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

        virtual void render(const Scene& scene) = 0;

    private:
        String name;
        bool enabled = true;
    };
}


#endif /* RENDER_PHASE_H */
