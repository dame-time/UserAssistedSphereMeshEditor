#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <Vector3.hpp>

#include <vector>
#include <string>

namespace Renderer {
    class ObjLoader {
        public:
        std::vector<Math::Vector3> vertices;
        std::vector<Math::Vector3> colors;
        std::vector<Math::Vector3> normals;
        std::vector<unsigned int> indices;
        
        ObjLoader();

        bool loadOBJ(const std::string& path);
    };
}

#endif
