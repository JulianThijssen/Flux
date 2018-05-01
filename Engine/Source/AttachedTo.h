#pragma once

#include "Component.h"

namespace Flux {
    class AttachedTo : public Component {
    public:
        AttachedTo(uint32_t parentId) : parentId(parentId) { }

        uint32_t parentId;
    };
}
