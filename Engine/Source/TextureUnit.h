#pragma once
#ifndef TEXTURE_UNIT_H
#define TEXTURE_UNIT_H

namespace Flux {
    class TextureUnit {
    public:
        static const int ALBEDO = 0;
        static const int NORMAL = 1;
        static const int ROUGHNESS = 2;
        static const int METALNESS = 3;
        static const int STENCIL = 4;
        static const int IRRADIANCE = 5;
        static const int PREFILTER = 6;
        static const int SCALEBIAS = 7;
        static const int NOISE = 8;

        static const int TEXTURE = 0;
        static const int BLOOM = 1;

        static const int POSITION = 2;
        static const int DEPTH = 3;

        static const int SHADOW = 9;

        static const int TEXTURE0 = 0;
        static const int TEXTURE1 = 1;
        static const int TEXTURE2 = 2;
        static const int TEXTURE3 = 3;
    };
}


#endif /* TEXTURE_UNIT_H */
