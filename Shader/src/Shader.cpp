#include <Shader.hpp>

namespace Renderer
{
    Shader::Shader()
    {

    }

    Shader::Shader(const char* vertexPath, const char* fragmentPath)
    {
        this->vertexShaderPath = std::string(vertexPath);
        this->fragmentShaderPath = std::string(fragmentPath);

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void Shader::assignShadersPath(const char* vertexPath, const char* fragmentPath)
    {
        *this = Shader(vertexPath, fragmentPath);
    }

    Shader* Shader::newShaderAtPath(const char* vertexPath, const char* fragmentPath)
    {
        return new Shader(vertexPath, fragmentPath);
    }

    void Shader::use() const
    {
        glUseProgram(ID);
    }

    void Shader::setBool(const std::string &name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
    // ------------------------------------------------------------------------
    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec2(const std::string &name, const Math::Vector2& value) const
    {
        float v[2] = { static_cast<float>(value.coordinates.x), static_cast<float>(value.coordinates.y) };
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, v);
    }
    void Shader::setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec3(const std::string &name, const Math::Vector3& value) const
    {
        float v[3] = { static_cast<float>(value.coordinates.x), static_cast<float>(value.coordinates.y), static_cast<float>(value.coordinates.z) };
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, v);
    }
    void Shader::setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void Shader::setVec4(const std::string &name, const Math::Vector4& value) const
    {
        float v[4] = { static_cast<float>(value.coordinates.x), static_cast<float>(value.coordinates.y), static_cast<float>(value.coordinates.z), static_cast<float>(value.coordinates.w) };
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, v);
    }
    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void Shader::setMat2(const std::string &name, const Math::Matrix2& mat) const
    {
        float m[2 * 2];
        for (int i = 0; i < 4; i++)
            m[i] = static_cast<float>(mat.data[i]);
        
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, m);
    }
    // ------------------------------------------------------------------------
    void Shader::setMat3(const std::string &name, const Math::Matrix3& mat) const
    {
        float m[3 * 3];
        for (int i = 0; i < 9; i++)
            m[i] = static_cast<float>(mat.data[i]);
        
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, m);
    }
    // ------------------------------------------------------------------------

    void Shader::setMat4(const std::string &name, const Math::Matrix4& mat) const
    {
        float m[4 * 4];
        for (int i = 0; i < 16; i++)
            m[i] = static_cast<float>(mat.data[i]);
        
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, m);
    }

    void Shader::checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}
