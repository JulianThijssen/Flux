#include "Shader.h"

class Renderer {
public:
    Renderer() : shader(0) { }
    void create();
    void update();

private:
    Shader shader;
};
