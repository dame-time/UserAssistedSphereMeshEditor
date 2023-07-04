#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>

#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Matrix2.hpp>
#include <Matrix3.hpp>
#include <Matrix4.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Renderer
{
    class Shader
    {
    public:
        std::string vertexShaderPath, fragmentShaderPath;

        unsigned int ID;
        
        Shader();
        // constructor generates the shader on the fly
        Shader(const char* vertexPath, const char* fragmentPath);

        void assignShadersPath(const char* vertexPath, const char* fragmentPath);
        Shader* newShaderAtPath(const char* vertexPath, const char* fragmentPath);
        // activate the shader
        // ------------------------------------------------------------------------
        void use() const;
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string &name, bool value) const;
        // ------------------------------------------------------------------------
        void setInt(const std::string &name, int value) const;
        // ------------------------------------------------------------------------
        void setFloat(const std::string &name, float value) const;
        // ------------------------------------------------------------------------
        void setVec2(const std::string &name, const Math::Vector2& value) const;
        void setVec2(const std::string &name, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const Math::Vector3& value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const Math::Vector4& value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w) const;
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const Math::Matrix2& mat) const;
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const Math::Matrix3& mat) const;
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const Math::Matrix4& mat) const;

    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(GLuint shader, std::string type);
    };
}
#endif
