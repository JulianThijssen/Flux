#include "Shader.h"
#include "Model.h"

namespace Flux {
    class Renderer {
    public:
        Renderer() : shader(0), model() { }
        void create();
        void update();

    private:
        Shader shader;
        Model model;
    };
}
