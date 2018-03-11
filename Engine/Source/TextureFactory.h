#pragma once

#include "Texture.h"

namespace Flux {
    Cubemap createShadowCubemap(const uint resolution);
    Texture2D createShadowMap(const uint width, const uint height);

    Texture2D createHdrTexture(const uint width, const uint height);
    Texture2D createLdrTexture(const uint width, const uint height);

    Cubemap createEmptyCubemap(const uint resolution);
    Texture2D createEmptyDepthMap(const uint width, const uint height);
}
