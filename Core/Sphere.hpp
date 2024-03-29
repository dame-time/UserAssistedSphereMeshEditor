//
//  Sphere.hpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include <RenderableMesh.hpp>
#include <Quadric.hpp>
#include <Region.hpp>

#include <iostream>
#include <vector>
#include <unordered_set>

namespace Renderer {
	typedef std::unordered_set<int> set_of_int;
	
    class Sphere
    {
        private:
            int renderedMeshID{};
        
            void generateUUID();
        
        public:
			bool isDangling{false};
			
            std::vector<Vertex*> vertices;
			set_of_int neighbourSpheres;
        
            Math::Scalar quadricWeights{};
        
            Quadric quadric;
            Region region;

            Math::Vector3 center;
            Math::Scalar radius{};
            Math::Vector3 color;

            Sphere();
            Sphere(const Sphere& sphere);
            Sphere(Vertex& vertex, Math::Scalar targetSphereRadius);
            Sphere(const Math::Vector3& center, Math::Scalar radius);

            [[nodiscard]] Quadric getSphereQuadric() const;

            void addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight = 1.0);
            void addQuadric(const Quadric& q);
        
            [[nodiscard]] bool checkSphereOverPlanarRegion() const;
            void approximateSphereOverPlanarRegion(const Math::Vector3& edge0, const Math::Vector3& edge1);
        
            void addVertex(Vertex& vertex);
			void addNeighbourSphere(int sphereIndex);
			
#ifdef USE_THIEF_SPHERE_METHOD
			int clearNotLinkedVertices();
#endif
        
            void constrainSphere(const Math::Scalar& constrainRadius);
        
            [[nodiscard]] int getID() const;

            [[nodiscard]] Sphere lerp(const Sphere &s, Math::Scalar t) const;
            bool containsVertex(const Math::Vector3& vertex);
    };
	
	inline void operator += (set_of_int& A, const set_of_int& B){
		A.insert(B.begin(), B.end());
	}
}

#endif /* Sphere_hpp */
