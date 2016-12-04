#include "Component.h"

#include "Entity.h"

namespace Flux {
    class AttachedTo : public Component {
    public:
        AttachedTo(Entity* parent) : parent(parent) { }

        Entity* parent;
    };
}
