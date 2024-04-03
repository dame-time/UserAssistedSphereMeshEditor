#pragma once

//#define MEASURE_EPSILON_MAX
#define LOG_TOUCH_OF_SPHERES

#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>

#include <TriMesh.hpp>
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
            TemporalValidityQueue edgeQueue;
			std::string lastCollapseDuration;
			
			int performedOperations{0};
			int numberOfActiveSpheres {0};
            
            std::unordered_set<Triangle> triangle;
            std::unordered_set<Edge> edge;
        
            int perSphereVertices{};
            int renderCalls{};
        
            TriMesh* referenceMesh;
        
            Math::Scalar BDDSize;
            
            Shader* sphereShader;
        
            RenderType renderType {RenderType::BILLBOARDS};
	    
	        std::unordered_map<int, int> sphereMapper;
            
            void initializeSphereMeshTriangles(const std::vector<Face>& Faces);
            void initializeSpheres(std::vector<Vertex>& vertices, Math::Scalar initialRadius);
            
            void computeSpheresProperties(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);
            void updateSpheres();
            void initializeEdgeQueue();
			
			void updateConnectivityAfterCollapses();
            
            void drawSpheresOverEdge(const Edge &e, int nSpheres = 4, Math::Scalar rescaleRadii = 1.0, Math::Scalar minRadiiScale = 0.3);
            void drawSpheresOverTriangle(const Triangle& t, int nSpheres = 4, Math::Scalar size = 1.0, Math::Scalar minRadiiScale = 0.3);
	    
	        [[maybe_unused]] static Math::Vector3 getTriangleCentroid(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
            static Math::Vector3 getTriangleNormal(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3);
        
            void renderOneSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color);
            void renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color, int spheresPerEdge, Math::Scalar sphereSize);
        
            void renderOneBillboardSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
        
            void renderSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color);
			
			void updateNeighborsOf(Sphere& s);
		
			bool engulfsAnything(EdgeCollapse& e);
			void execute(const EdgeCollapse& e);
			void addPotentialCollapse(int i, int j);
			
			bool isOutOfDate(const EdgeCollapse& e);
			void updateCost(EdgeCollapse& e);
			
			bool debugCheckNoLoops(); // Check that in the graphs there are no loops
			
			void extendSpheresNeighboursOneStep();
			
		    // ONLY FOR IMPLEMENTATION OF THIERY-ET-AL-2013
		    static bool normalTest(const Vertex& v, const Vertex& v1);
			void addGeometricallyCloseNeighbours(Math::Scalar epsilon);
        
        public:
            std::vector<TimedSphere> timedSpheres;
			
			bool IMPLEMENT_THIERY_2013{false};
		
			int alias(int alias);
			Sphere& currentSphere(int id) { return timedSpheres[alias(id)].sphere; }
			bool isTimedSphereAlive(int id);
        
            SphereMesh(const SphereMesh& sm);
            SphereMesh(TriMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius = 0.1f);
        
            SphereMesh& operator = (const SphereMesh& sm);
        
            [[nodiscard]] int getPerSphereVertexCount() const;
            [[nodiscard]] int getRenderCalls() const;
            int getTriangleSize();
            int getEdgeSize();
        
            void resetRenderCalls();
        
            RenderType getRenderType();
            void setRenderType(const RenderType& renderType);
			
            void render();
            void renderWithNSpherePerEdge(int n, Math::Scalar rescaleRadii = 1.0, Math::Scalar minRadiiScale = 0.3);
            void renderSpheresOnly();
            void renderConnectivity();
            void renderConnectivity(int spheresPerEdge, Math::Scalar sphereSize);
        
            void renderSphereVertices(int i);
            
            int collapse(int sphereIndexA, int sphereIndexB);
            
            bool collapseSphereMesh();
            bool collapseSphereMesh(int n);
			
			[[nodiscard]] int getTimedSphereSize() const;
        
            void loadFromYaml(const std::string& path);
        
            void saveYAML(const std::string& path = ".", const std::string& fileName = "SphereMesh.yaml");
            void saveTXT(const std::string& path = ".", const std::string& fileName = "SphereMesh.txt");
            void saveTXTToAutoPath();
        
            void addEdge(int selectedSphereID);
            void addTriangle(int sphereA, int sphereB);
        
            void removeSphere(int selectedSphereID);
			void resetSphereMesh();
            
            void clear();
    };
	
	inline bool includes(std::vector<int>& A, int B)
	{
		return std::find(A.begin(), A.end(), B) != A.end();
	}
}
