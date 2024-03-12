#pragma once

#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>

#include <RenderableMesh.hpp>
#include <Quadric.hpp>
#include <Region.hpp>
#include <EdgeCollapse.hpp>
#include <TimedSphere.hpp>
#include <HashDefinitions.hpp>

#ifdef USE_FIB_QUEUE
#include <UpdatableFibonacciPQ.hpp>
#elif USE_INDEX_QUEUE
#include <UpdatablePQ.hpp>
#else
#include <TemporalValidityQueue.hpp>
#endif

#include <vector>
#include <unordered_set>
#include <string>
#include <cfloat>
#include <chrono>
#include <functional>

namespace Renderer
{
	struct Pair
	{
		int i, j;

		Pair() : i(0), j(0) {}
		Pair(int a, int b) : i(a), j(b) {}
	};

    enum RenderType {
        SPHERES,
        BILLBOARDS
    };

    class SphereMesh
    {
        private:
            Math::Scalar EPSILON{};
	    
	        std::vector<TimedSphere> initialSpheres;
            TemporalValidityQueue edgeQueue;
			
			int timedSphereSize {0};
            
            std::unordered_set<Triangle> triangle;
            std::unordered_set<Edge> edge;
        
            int perSphereVertices{};
            int renderCalls{};
        
            RenderableMesh* referenceMesh;
        
            Math::Scalar BDDSize;
            
            Shader* sphereShader;
        
            RenderType renderType {RenderType::BILLBOARDS};
            
            std::vector<std::vector<bool>> edgeConnectivity;
            std::vector<std::vector<std::vector<bool>>> triangleConnectivity;
			std::vector<std::vector<bool>> triangleEdgeConnectivity;
	    
	        std::unordered_map<int, int> sphereMapper;
        
            void initializeEPSILON();
            
            void initializeSphereMeshTriangles(const std::vector<Face>& Faces);
            void initializeSpheres(std::vector<Vertex>& vertices, Math::Scalar initialRadius);
            
            void computeSpheresProperties(const std::vector<Vertex>& vertices);
            void updateSpheres();
            void initializeEdgeQueue();
        
            EdgeCollapse getBestCollapseBruteForce();
            EdgeCollapse getBestCollapseFast();
            EdgeCollapse getBestCollapseInConnectivity();
        
            Sphere collapseEdgeIntoSphere(EdgeCollapse& edgeToCollapse);
            
            void updateEdgesAfterCollapse(int i, int j);
            void updateTrianglesAfterCollapse(int i, int j);
            void removeDegenerates();
            void updateEdgeQueue(const EdgeCollapse& collapsedEdge);
            
            void drawSpheresOverEdge(const Edge &e, int nSpheres = 4, Math::Scalar rescaleRadii = 1.0, Math::Scalar minRadiiScale = 0.3);
            void drawSpheresOverTriangle(const Triangle& t, int nSpheres = 4, Math::Scalar size = 1.0, Math::Scalar minRadiiScale = 0.3);
	    
	    [[maybe_unused]] static Math::Vector3 getTriangleCentroid(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
            static Math::Vector3 getTriangleNormal(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
        
            void renderOneSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color, int spheresPerEdge, Math::Scalar sphereSize);
        
            void renderOneBillboardSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
        
            void buildEdgeConnectivity();
            void buildTriangleConnectivity();
        
            void clearEdges();
            void clearTriangles();
        
            Math::Scalar getContainedRadiusOfSphere(const Sphere& s);
        
            void renderSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
        
        public:
            std::vector<TimedSphere> timedSpheres;
        
            SphereMesh(const SphereMesh& sm);
            SphereMesh(RenderableMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius = 0.1f);
        
            SphereMesh& operator = (const SphereMesh& sm);
        
            void setEpsilon(const Math::Scalar& e);
        
            [[nodiscard]] int getPerSphereVertexCount() const;
            [[nodiscard]] int getRenderCalls() const;
            int getTriangleSize();
            int getEdgeSize();
        
            void resetRenderCalls();
        
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
            
            int collapse(int sphereIndexA, int sphereIndexB);
            
            bool collapseSphereMesh();
            bool collapseSphereMesh(int n);
            bool collapseSphereMeshFast();
            bool collapseSphereMeshFast(int n);
			
			[[nodiscard]] int getTimedSphereSize() const;
        
            void loadFromYaml(const std::string& path);
        
            void saveYAML(const std::string& path = ".", const std::string& fileName = "SphereMesh.yaml");
            void saveTXT(const std::string& path = ".", const std::string& fileName = "SphereMesh.txt");
        
            void addEdge(int selectedSphereID);
            void addTriangle(int sphereA, int sphereB);
        
            void removeSphere(int selectedSphereID);
            
            void clear();
	        void clearSphereMesh();
        
            void reset();
    };
}
