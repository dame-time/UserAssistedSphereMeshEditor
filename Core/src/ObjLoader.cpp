#include <ObjLoader.hpp>

#include <fstream>
#include <sstream>

namespace Renderer {
    ObjLoader::ObjLoader() {
        
    }

    bool ObjLoader::loadOBJ(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                Math::Vector3 vertex;
                iss >> vertex.coordinates.x >> vertex.coordinates.y >> vertex.coordinates.z;
                vertices.push_back(vertex);
                colors.push_back(Math::Vector3(1.0, 1.0, 1.0));
            }
            else if (type == "vn") {
                Math::Vector3 normal;
                iss >> normal.coordinates.x >> normal.coordinates.y >> normal.coordinates.z;
                normals.push_back(normal);
            }
            else if (type == "f") {
                std::vector<unsigned int> faceIndices;
                std::string vertexData;
                while (iss >> vertexData) {
                    std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                    std::istringstream vData(vertexData);
                    unsigned int iVertex, iTex, iNormal;
                    vData >> iVertex >> iTex >> iNormal;

                    indices.push_back(iVertex - 1);
                }
            }
        }

        return true;
    }
}
