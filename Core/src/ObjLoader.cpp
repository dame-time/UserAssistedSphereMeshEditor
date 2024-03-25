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
		
		vertices.clear();
		colors.clear();
		normals.clear();
		indices.clear();

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                Math::Vector3 vertex;
                iss >> vertex.coordinates.x >> vertex.coordinates.y >> vertex.coordinates.z;
                vertices.push_back(vertex);
                colors.emplace_back(1.0, 1.0, 1.0);
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
		            vData >> iVertex;
		            faceIndices.push_back(iVertex - 1);
	            }
				
				// Simple triangulation of a face with more than 3 vertices, it's not the best way to do it, but
				// prevents from screwing up the mesh
	            if (faceIndices.size() == 4)
				{
		            indices.push_back(faceIndices[0]);
		            indices.push_back(faceIndices[1]);
		            indices.push_back(faceIndices[2]);
					
		            indices.push_back(faceIndices[0]);
		            indices.push_back(faceIndices[2]);
		            indices.push_back(faceIndices[3]);
					
					std::cerr << "Face with " << faceIndices.size() << " vertices, not supported, please triangulate"
																	   " the mesh."	<< std::endl;
	            }
	            else if (faceIndices.size() == 3)
				{
		            indices.push_back(faceIndices[0]);
		            indices.push_back(faceIndices[1]);
		            indices.push_back(faceIndices[2]);
	            }
				else
	            {
	                std::cerr << "Face with " << faceIndices.size() << " vertices, not supported" << std::endl;
				}
            }
        }

        return true;
    }
}
