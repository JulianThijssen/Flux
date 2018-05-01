#pragma once

#include "Renderer/RenderState.h"
#include "Util/String.h"

#include "Scene.h"
#include "Util/Size.h"

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

        void SetSource(const Texture2D* source) {
            this->source = source;
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

        virtual void Resize(const Size& windowSize) = 0;
        virtual void render(RenderState& renderState, const Scene& scene) = 0;

    protected:
        const Texture2D* source;

    private:
        String name;
        bool enabled = true;
    };
}
