//
//  RenderableMesh.h
//  CustomRenderer
//
//  Created by Davide Paollilo on 21/06/23.
//

#ifndef RenderableMesh_h
#define RenderableMesh_h

#include <Shader.hpp>

#include <Vector3.hpp>
#include <Quaternion.hpp>

#include <vector>
#include <string>
#include <cfloat>

namespace Renderer {
    struct Face
    {
        int i, j, k;

        Face() : i(0), j(0), k(0) {};
        Face(int a, int b, int c) : i(a), j(b), k(c) {};
    };

    struct Vertex
    {
        Math::Vector3 position;
        Math::Vector3 normal;
        Math::Vector3 color;
        
        Math::Vector2 curvature;

        Vertex() : position(Math::Vector3()), normal(Math::Vector3(1, 0, 0)) {}
        Vertex(const Math::Vector3& p, const Math::Vector3& n) : position(p), normal(n) {}
    };

    struct AABB
    {
        Math::Vector3 minCorner = Math::Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        Math::Vector3 maxCorner = Math::Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        
        void addPoint(const Math::Vector3& p)
        {
            for (int i = 0; i < 3; i++)
                if (p[i] > maxCorner[i])
                    maxCorner[i] = p[i];
            
            for (int i = 0; i < 3; i++)
                if (p[i] < minCorner[i])
                    minCorner[i] = p[i];
        }
        
        Math::Vector3 BDD() const
        {
            return maxCorner - minCorner;
        }
    };

    class RenderableMesh {
        private:
            int _ID;
        
            bool isPickable;
        
            Math::Matrix4 model;
            Shader* shader;
        
            GLuint VAO, VBO, EBO;
        
            Math::Vector3 wireframeColor;
            bool wireframeColorSetted = false;
        
            void setup();
            void updateGPUVertexData();
        
            void updateBBOX();
        
            void generateUUID();
        
            Math::Scalar getMeshRadius();
        
            std::vector<Face> getVertexAdjacentFaces(int vertexIndex);
            std::vector<Vertex> getAdjacentVertices(int vertexIndex);
        
            Math::Scalar distance(const Math::Vector3& p0, const Math::Vector3& p1);
            Math::Scalar getAngleOfVertexAtFace(const Face& f, const Vertex& v);
            Math::Scalar getCotAlpha(Vertex v, Vertex adjVertex);
            Math::Scalar getCotBeta(Vertex v, Vertex adjVertex);
        
            void computeVerticesCurvatureIGL();
        
            void computeVerticesCurvature();
        
        public:
            AABB bbox;
            std::string path;
        
            std::vector<Vertex> vertices;
            std::vector<Face> faces;
        
            bool isWireframe;
            bool isFilled;
            bool isBlended;
        
            RenderableMesh(const std::string& pathToLoadFrom, Shader* shader);
            RenderableMesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces, Shader* shader);
        
            RenderableMesh& operator = (const RenderableMesh& other) {
                this->vertices = other.vertices;
                this->faces = other.faces;
                
                this->model = other.model;
                this->shader = other.shader;
                
                this->VAO = other.VAO;
                this->VBO = other.VBO;
                this->EBO = other.EBO;
                
                this->_ID = other._ID;
                this->isWireframe = other.isWireframe;
                this->bbox = other.bbox;
            }
        
            int getID();
        
            Math::Matrix4 getModel();
            Math::Vector3 getCentroid();
        
            void scale(const Math::Vector3& scale);
            void translate(const Math::Vector3& translate);
            
            void scaleUniform(const Math::Scalar& scaleValue);
            void translateUniform(const Math::Scalar& translateValue);
        
            Math::Vector3 getPosition();
            Math::Vector3 getScale();
        
            Math::Vector3 getCurrentScale();
            Math::Vector3 getCurrentTranslation();
        
            Math::Scalar getBoundingSphereRadius();
        
            void setWireframe(bool isWireframeActive);
            void setFilled(bool isFilledActive);
            void setBlended(bool isBlendedActive);
        
            void setColors(const std::vector<Math::Vector3>& color);
            void setUniformColor(Math::Vector3 color);
        
            void setWireframeColor(const Math::Vector3& color);
        
            void render();
    };
}

#endif /* RenderableMesh_h */
