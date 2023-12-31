#pragma once

#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>

#include <RenderableMesh.hpp>
#include <Quadric.hpp>
#include <Region.hpp>
#include <Sphere.hpp>
#include <CollapsableEdge.hpp>

//#include <UpdatableFibonacciPQ.hpp>
#include <UpdatablePQ.hpp>

#include <vector>
#include <string>
#include <cfloat>

namespace Renderer
{
    struct Triangle
    {
        int i, j, k;

        Triangle() : i(0), j(0), k(0) {}
        Triangle(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {}
    };

    struct Edge
    {
        int i, j;

        Edge() : i(0), j(0) {}
        Edge(int first, int second)
        {
            i = first;
            j = second;
        }
    };

    enum RenderType {
        SPHERES,
        BILLBOARDS
    };

    class SphereMesh
    {
        private:
            Math::Scalar EPSILON;
        
            std::vector<Sphere> initialSpheres;
            UpdatablePQ edgeQueue;
            
            std::vector<Triangle> triangle;
            std::vector<Edge> edge;
        
            int perSphereVertices;
            int renderCalls;
        
            RenderableMesh* referenceMesh;
        
            Math::Scalar BDDSize;
            
            Shader* sphereShader;
        
            RenderType renderType;
            
            std::vector<std::vector<bool>> edgeConnectivity;
            std::vector<std::vector<std::vector<bool>>> triangleConnectivity;
        
            void initializeEPSILON();
            
            void initializeSphereMeshTriangles(const std::vector<Face>& Faces);
            void initializeSpheres(const std::vector<Vertex>& vertices, Math::Scalar initialRadius);
            
            void computeSpheresProperties(const std::vector<Vertex>& vertices);
            void updateSpheres();
            void initializeEdgeQueue();
        
            void updateCollapseCosts(const Sphere& newSphere, int i, int j);
            void recalculateCollapseCosts(int edgeIndexToErase, const Sphere& newSphere, int i, int j);
        
            CollapsableEdge getBestCollapseBruteForce();
            CollapsableEdge getBestCollapseFast();
            CollapsableEdge getBestCollapseInConnectivity();
        
            Sphere collapseEdgeIntoSphere(const CollapsableEdge& edgeToCollapse);
            
            void updateEdgesAfterCollapse(int i, int j);
            void updateTrianglessAfterCollapse(int i, int j);
            void removeDegenerates();
            void updateEdgeQueue(const CollapsableEdge& collapsedEdge);
            
            void drawSpheresOverEdge(const Edge &e, int nSpheres = 4, Math::Scalar rescaleRadii = 1.0, Math::Scalar minRadiiScale = 0.3);
            void drawSpheresOverTriangle(const Triangle& t, int nSpheres = 4, Math::Scalar size = 1.0, Math::Scalar minRadiiScale = 0.3);
            
            Math::Vector3 getTriangleCentroid(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
            Math::Vector3 getTriangleNormal(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
        
            void checkSphereIntersections(Sphere& s);
        
            void renderOneSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color, int spheresPerEdge, Math::Scalar sphereSize);
        
            void renderOneBillboardSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
        
            void buildEdgeConnectivity();
            void buildTriangleConnectivity();
        
            void clearEdges();
            void clearTriangles();
            void clearSphereMesh();
        
            Math::Scalar getContainedRadiusOfSphere(const Sphere& s);
        
            void renderSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
        
        public:
            std::vector<Sphere> sphere;
        
            SphereMesh(const SphereMesh& sm);
            SphereMesh(RenderableMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius = 0.1f);
        
            SphereMesh& operator = (const SphereMesh& sm);
        
            void setEpsilon(const Math::Scalar& e);
        
            int getPerSphereVertexCount();
            int getRenderCalls();
            int getTriangleSize();
            int getEdgeSize();
        
            void resetRenderCalls();
            
            void constructTest();
        
            RenderType getRenderType();
            void setRenderType(const RenderType& renderType);
            
            void renderSelectedSpheresOnly();
            void renderFastSelectedSpheresOnly();
            void render();
            void renderWithNSpherePerEdge(int n, Math::Scalar rescaleRadii = 1.0, Math::Scalar minRadiiScale = 0.3);
            void renderSpheresOnly();
            void renderConnectivity();
            void renderConnectivity(int spheresPerEdge, Math::Scalar sphereSize);
        
            void renderSphereVertices(int i);
            void clearRenderedSphereVertices();
            
            bool collapse(int sphereIndexA, int sphereIndexB);
            
            bool collapseSphereMesh();
            bool collapseSphereMesh(int n);
            bool collapseSphereMeshFast();
            bool collapseSphereMeshFast(int n);
        
            Sphere getSelectedVertexSphere(int sphereIndex);
            void resizeSphereVertex(int sphereIndex, Math::Scalar newSize);
            void translateSphereVertex(int sphereIndex, Math::Vector3& translation);
        
            void loadFromYaml(const std::string& path);
        
            void saveYAML(const std::string& path = ".", const std::string& fileName = "SphereMesh.yaml");
            void saveTXT(const std::string& path = ".", const std::string& fileName = "SphereMesh.txt");
        
            void addEdge(int selectedSphereID);
            void addTriangle(int sphereA, int sphereB);
        
            void removeSphere(int selectedSphereID);
            
            void clear();
        
            void reset();
    };
}
