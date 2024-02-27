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

namespace Renderer {
    class Sphere
    {
        private:
            int renderedMeshID{};
        
            void generateUUID();
        
        public:
            std::vector<Vertex> vertices;
        
            Math::Scalar quadricWeights{};
        
            Quadric quadric;
            Region region;

            Math::Vector3 center;
            Math::Scalar radius{};
            Math::Vector3 color;

            Sphere();
            Sphere(const Sphere& sphere);
            Sphere(const Vertex& vertex, Math::Scalar targetSphereRadius);
            Sphere(const Math::Vector3& center, Math::Scalar radius);

            Quadric getSphereQuadric() const;

            void addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight = 1.0);
            void addQuadric(const Quadric& q);
        
            bool checkSphereOverPlanarRegion() const;
            void approximateSphereOverPlanarRegion(const Math::Vector3& edge0, const Math::Vector3& edge1);
        
            void addVertex(const Vertex& vertex);
        
            void constrainSphere(const Math::Scalar& constrainRadius);
        
            int getID() const;

            Sphere lerp(const Sphere &s, Math::Scalar t) const;
            bool intersectsVertex(const Math::Vector3& vertex);
    };
}

#endif /* Sphere_hpp */
