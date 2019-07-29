#pragma once

#include "Exception.h"
#include "OpenGL.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace GDT
{
    struct ShaderLoadingException : public ErrorMessageException
    {
        using ErrorMessageException::ErrorMessageException;
    };

    class Vector3f;
    class Matrix4f;

    enum ShaderType
    {
        VERTEX,
        GEOMETRY,
        FRAGMENT,
        COMPUTE
    };

    class Shader
    {
        friend class ShaderProgram;

    public:
        /**
         * Creates a new shader object of the given type
         *
         * @param type The type this shader should be
         */
        Shader(ShaderType type);

        /**
         * Returns whether the shader has compiled successfully.
         * If this returns false, compile() might not have
         * been called yet, or it might have failed.
         *
         * @return true if shader compiled successfully
         */
        bool isCompiled() const;

        /**
         * Loads shader source from the given file path
         *
         * @param path The path of the shader file
         * @return true if the shader could be loaded from the file
         */
        bool loadFromFile(std::string path);

        /**
         * Compiles the shader
         */
        void compile();

        /**
         * Frees any GPU handle related this shader
         */
        void destroy();

        std::string getInfoLog();
    private:
        void create();

        ShaderType _type;

        bool _isCreated;
        bool _isCompiled;

        GLuint _handle;
    };

    class ShaderProgram
    {
    public:
        ShaderProgram();

        void addShader(ShaderType type, std::string path);
        void build();
        void loadFromFile(std::string vertexPath, std::string fragmentPath);
        std::string getError();

        bool isLinked();
        bool isValidated();

        void create();
        void bind();
        void release();
        void destroy();

        void uniform1i(const char* name, int i);
        void uniform1ui(const char* name, unsigned int i);
        void uniform1iv(const char* name, int count, int* values);
        void uniform2i(const char* name, int v0, int v1);
        void uniform2ui(const char* name, unsigned int v0, unsigned int v1);
        void uniform1f(const char* name, float value);
        void uniform1fv(const char* name, int count, float* values);
        void uniform2f(const char* name, float v0, float v1);
        void uniform3f(const char* name, float v0, float v1, float v2);
        void uniform3f(const char* name, const Vector3f& v);
        void uniform3fv(const char* name, int count, Vector3f* values);
        void uniform4f(const char* name, float v0, float v1, float v2, float v3);
        void uniformMatrix4f(const char* name, const Matrix4f& m);

    private:
        void link();
        void validate();
        void attach(const Shader& shader);
        void detachAll();

        std::string getInfoLog();
        GLint getUniformLocation(const char* name);

    private:
        bool _isCreated;
        bool _isLinked;
        bool _isValidated;

        std::vector<std::string> errorLog;
        std::vector<Shader> _attachedShaders;
        std::unordered_map<std::string, int> _locationMap;

        GLuint _handle;
    };
}
