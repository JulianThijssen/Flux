#pragma once
#ifndef ATTACHED_TO_H
#define ATTACHED_TO_H

#include "Component.h"

namespace Flux {
    class AttachedTo : public Component {
    public:
        AttachedTo(uint32_t parentId) : parentId(parentId) { }

        uint32_t parentId;
    };
}

#endif /* ATTACHED_TO_H */
