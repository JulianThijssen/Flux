#include "TextureFactory.h"

#include "TextureUnit.h"

namespace Flux {
    Cubemap createShadowCubemap(const uint resolution)
    {
        Cubemap shadowMap;

        shadowMap.create();
        shadowMap.bind(TextureUnit::TEXTURE0);
        shadowMap.setWrapping(BORDER, BORDER, BORDER);
        shadowMap.setSampling(LINEAR, LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        // Set the border color to 1 so samples outside of the shadow map have the furthest depth
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, color);

        for (int i = 0; i < 6; i++) {
            shadowMap.setFace(i);
            shadowMap.setData(resolution, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }

        shadowMap.release();

        return shadowMap;
    }

    Texture2D createShadowMap(const uint width, const uint height)
    {
        Texture2D shadowMap;
        shadowMap.create();
        shadowMap.bind(TextureUnit::TEXTURE0);

        shadowMap.setWrapping(BORDER, BORDER);
        shadowMap.setSampling(LINEAR, LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        // Set the border color to 1 so samples outside of the shadow map have the furthest depth
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        shadowMap.setData(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        shadowMap.release();

        return shadowMap;
    }

    Texture2D createHdrTexture(const uint width, const uint height)
    {
        Texture2D hdrTexture;
        hdrTexture.create();
        hdrTexture.bind(TextureUnit::TEXTURE0);

        hdrTexture.setWrapping(CLAMP, CLAMP);
        hdrTexture.setSampling(LINEAR, LINEAR);

        hdrTexture.setData(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, nullptr);

        hdrTexture.release();

        return hdrTexture;
    }

    Texture2D createLdrTexture(const uint width, const uint height)
    {
        Texture2D ldrTexture;
        ldrTexture.create();
        ldrTexture.bind(TextureUnit::TEXTURE0);

        ldrTexture.setWrapping(CLAMP, CLAMP);
        ldrTexture.setSampling(NEAREST, NEAREST);

        ldrTexture.setData(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        ldrTexture.release();

        return ldrTexture;
    }

    Cubemap createEmptyCubemap(const uint resolution)
    {
        Cubemap cubemap;

        return cubemap;
    }

    Texture2D createEmptyDepthMap(const uint width, const uint height)
    {
        Texture2D depthMap;
        depthMap.create();
        depthMap.bind(TextureUnit::TEXTURE0);
        depthMap.setData(width, height, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
        depthMap.setWrapping(CLAMP, CLAMP);
        depthMap.release();

        return depthMap;
    }
}
