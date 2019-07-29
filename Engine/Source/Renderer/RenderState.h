#pragma once

#include <GDT/Vector3f.h>
#include <GDT/Matrix4f.h>
#include <GDT/Shader.h>

#include <glad/glad.h>

#include <vector>
#include <unordered_map>

using GDT::Vector3f;
using GDT::Matrix4f;
using GDT::ShaderProgram;

namespace Flux {
    class Framebuffer;
    class Shader;
    class Entity;
    class Transform;
    class Camera;

    enum Capability {
        BLENDING = GL_BLEND,
        FACE_CULLING = GL_CULL_FACE,
        DEPTH_TEST = GL_DEPTH_TEST,
        STENCIL_TEST = GL_STENCIL_TEST,
        POLYGON_OFFSET = GL_POLYGON_OFFSET_FILL
    };

    struct CapabilitySet
    {
    public:
        void addCapability(Capability capability, bool enabled)
        {
            capabilities.push_back(std::pair<Capability, bool>(capability, enabled));
        }

        std::vector<std::pair<Capability, bool>> getSet()
        {
            return capabilities;
        }

    private:
        std::vector<std::pair<Capability, bool>> capabilities;
    };

    class RenderState {
    public:
        RenderState();

        void enable(Capability capability);
        void disable(Capability capability);
        void require(CapabilitySet capabilitySet);
        void setClearColor(float r, float g, float b, float a);
        
        void drawQuad() const;
        void setCamera(ShaderProgram& shader, Entity& camera);
        void setCamera(ShaderProgram& shader, Transform& t, Camera& cam);

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        static GLuint quadVao;

        static const Framebuffer* currentFramebuffer;

        static GLuint getActiveTexture();
        static void setActiveTexture(unsigned int textureUnit);
        static void bindTexture(GLenum target, GLuint texture);

        static std::vector<unsigned int> textureUnits;
        
    private:
        Vector3f clearColor;

        static unsigned int activeTextureUnit;

        std::unordered_map<Capability, bool> capabilityMap;
    };
}
