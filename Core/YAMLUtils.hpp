//
//  YAMLUtils.h
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#ifndef YAMLUtils_h
#define YAMLUtils_h

#include <yaml-cpp/yaml.h>

#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Matrix4.hpp>
#include <Quadric.hpp>

#include <string>

struct ShaderPath {
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
};

inline void YAMLSerializeVector3(YAML::Emitter& out, const Math::Vector3& vector) {
    out << YAML::BeginMap;
    out << YAML::Key << "X" << YAML::Value << vector.coordinates.x;
    out << YAML::Key << "Y" << YAML::Value << vector.coordinates.y;
    out << YAML::Key << "Z" << YAML::Value << vector.coordinates.z;
    out << YAML::EndMap;
}

inline void YAMLSerializeVector4(YAML::Emitter& out, const Math::Vector4& vector) {
    out << YAML::BeginMap;
    out << YAML::Key << "X" << YAML::Value << vector.coordinates.x;
    out << YAML::Key << "Y" << YAML::Value << vector.coordinates.y;
    out << YAML::Key << "Z" << YAML::Value << vector.coordinates.z;
    out << YAML::Key << "W" << YAML::Value << vector.coordinates.w;
    out << YAML::EndMap;
}

inline void YAMLSerializeMatrix4(YAML::Emitter& out, const Math::Matrix4& matrix) {
    out << YAML::BeginMap;
    out << YAML::Key << "0" << YAML::Value << matrix.data[0];
    out << YAML::Key << "1" << YAML::Value << matrix.data[1];
    out << YAML::Key << "2" << YAML::Value << matrix.data[2];
    out << YAML::Key << "3" << YAML::Value << matrix.data[3];
    out << YAML::Key << "4" << YAML::Value << matrix.data[4];
    out << YAML::Key << "5" << YAML::Value << matrix.data[5];
    out << YAML::Key << "6" << YAML::Value << matrix.data[6];
    out << YAML::Key << "7" << YAML::Value << matrix.data[7];
    out << YAML::Key << "8" << YAML::Value << matrix.data[8];
    out << YAML::Key << "9" << YAML::Value << matrix.data[9];
    out << YAML::Key << "10" << YAML::Value << matrix.data[10];
    out << YAML::Key << "11" << YAML::Value << matrix.data[11];
    out << YAML::Key << "12" << YAML::Value << matrix.data[12];
    out << YAML::Key << "13" << YAML::Value << matrix.data[13];
    out << YAML::Key << "14" << YAML::Value << matrix.data[14];
    out << YAML::Key << "15" << YAML::Value << matrix.data[15];
    out << YAML::EndMap;
}

inline void YAMLSerializeQuadric(YAML::Emitter& out, Renderer::Quadric& q) {
    out << YAML::BeginMap;
    out << YAML::Key << "A" << YAML::Value;
    YAMLSerializeMatrix4(out, q.getA());
    out << YAML::Key << "b" << YAML::Value;
    YAMLSerializeVector4(out, q.getB());
    out << YAML::Key << "c" << YAML::Value << q.getC();
    out << YAML::EndMap;
}

inline std::string getYAMLRenderableMeshPath(const std::string& path) {
    std::ifstream stream(path);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    
    return data["Reference Mesh"].as<std::string>();
}

inline ShaderPath getYAMLSphereShaderPath(const std::string& path) {
    ShaderPath shaderPath;
    
    std::ifstream stream(path);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    
    shaderPath.vertexShaderPath = data["Rendering Shader"]["Vertex Shader"].as<std::string>();
    shaderPath.fragmentShaderPath = data["Rendering Shader"]["Fragment Shader"].as<std::string>();
    
    return shaderPath;
}

namespace YAML {
    template<>
    struct convert<Math::Vector3> {
        static bool decode(const Node& node, Math::Vector3& rhs) {
            rhs.coordinates.x = node["X"].as<Math::Scalar>();
            rhs.coordinates.y = node["Y"].as<Math::Scalar>();
            rhs.coordinates.z = node["Z"].as<Math::Scalar>();
            
            return true;
        }
    };

    template<>
    struct convert<Math::Matrix4> {
        static bool decode(const Node& node, Math::Matrix4& rhs) {
            rhs.data[0] = node["0"].as<Math::Scalar>();
            rhs.data[1] = node["1"].as<Math::Scalar>();
            rhs.data[2] = node["2"].as<Math::Scalar>();
            rhs.data[3] = node["3"].as<Math::Scalar>();
            rhs.data[4] = node["4"].as<Math::Scalar>();
            rhs.data[5] = node["5"].as<Math::Scalar>();
            rhs.data[6] = node["6"].as<Math::Scalar>();
            rhs.data[7] = node["7"].as<Math::Scalar>();
            rhs.data[8] = node["8"].as<Math::Scalar>();
            rhs.data[9] = node["9"].as<Math::Scalar>();
            rhs.data[10] = node["10"].as<Math::Scalar>();
            rhs.data[11] = node["11"].as<Math::Scalar>();
            rhs.data[12] = node["12"].as<Math::Scalar>();
            rhs.data[13] = node["13"].as<Math::Scalar>();
            rhs.data[14] = node["14"].as<Math::Scalar>();
            rhs.data[15] = node["15"].as<Math::Scalar>();
            
            return true;
        }
    };

    template<>
    struct convert<Math::Vector4> {
        static bool decode(const Node& node, Math::Vector4& rhs) {
            rhs.coordinates.x = node["X"].as<Math::Scalar>();
            rhs.coordinates.y = node["Y"].as<Math::Scalar>();
            rhs.coordinates.z = node["Z"].as<Math::Scalar>();
            rhs.coordinates.w = node["W"].as<Math::Scalar>();
            
            return true;
        }
    };

    template<>
    struct convert<Renderer::Quadric> {
        static bool decode(const Node& node, Renderer::Quadric& rhs) {
            rhs.A = node["A"].as<Math::Matrix4>();
            rhs.b = node["b"].as<Math::Vector4>();
            rhs.c = node["c"].as<Math::Scalar>();
            
            return true;
        }
    };
}


#endif /* YAMLUtils_h */
