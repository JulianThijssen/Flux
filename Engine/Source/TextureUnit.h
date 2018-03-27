#pragma once
#ifndef TEXTURE_UNIT_H
#define TEXTURE_UNIT_H

namespace Flux {
    class TextureUnit {
    public:
        static const unsigned int ALBEDO = 0;
        static const unsigned int NORMAL = 1;
        static const unsigned int ROUGHNESS = 2;
        static const unsigned int METALNESS = 3;
        static const unsigned int STENCIL = 4;
        static const unsigned int EMISSION = 5;
        static const unsigned int IRRADIANCE = 6;
        static const unsigned int PREFILTER = 7;
        static const unsigned int SCALEBIAS = 8;
        static const unsigned int NOISE = 9;

        static const unsigned int TEXTURE = 0;
        static const unsigned int BLOOM = 1;

        static const unsigned int POSITION = 2;
        static const unsigned int DEPTH = 3;

        static const unsigned int SHADOW = 9;

        static const unsigned int TEXTURE0 = 0;
        static const unsigned int TEXTURE1 = 1;
        static const unsigned int TEXTURE2 = 2;
        static const unsigned int TEXTURE3 = 3;
        static const unsigned int TEXTURE4 = 4;
        static const unsigned int TEXTURE5 = 5;
        static const unsigned int TEXTURE6 = 6;
        static const unsigned int TEXTURE7 = 7;
        static const unsigned int TEXTURE8 = 8;
    };
}


#endif /* TEXTURE_UNIT_H */
