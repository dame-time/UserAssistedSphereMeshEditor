//
//  Sphere.hpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include <TriMesh.hpp>
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
            set_of_int vertices;
			set_of_int neighbourSpheres;
        
            Math::Scalar quadricWeights{};
        
            Quadric quadric;
            Region region;

            Math::Vector3 center;
            Math::Scalar radius{};
            Math::Vector3 color;

            Sphere();
            Sphere(const Sphere& sphere);
            Sphere(Vertex& vertex, int vertexIdx, Math::Scalar targetSphereRadius);
            Sphere(const Math::Vector3& center, Math::Scalar radius);
        
			void init(Vertex& vertex, int vertexIdx, Math::Scalar targetSphereRadius);
			void initTHIERY(Vertex& vertex);
            void addVertex(Vertex& vertex, int index);
			void addNeighbourSphere(int sphereIndex);
        
            [[nodiscard]] int getID() const;

            [[nodiscard]] Sphere lerp(const Sphere &s, Math::Scalar t) const;
            bool containsVertex(const Math::Vector3& vertex);
    };
	
	inline void operator += (set_of_int& A, const set_of_int& B){
		A.insert(B.begin(), B.end());
	}
}

#endif /* Sphere_hpp */
