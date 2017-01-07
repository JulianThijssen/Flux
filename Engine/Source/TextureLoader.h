#include "Texture.h"

namespace Flux {
    class Path;

    class TextureLoader {
    public:
        static Texture* loadTexture(Path path);
        static Texture* create(const int width, const int height, const int bpp, const unsigned char* data);
    };
}
