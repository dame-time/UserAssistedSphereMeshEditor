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
            std::vector<int> renderedSpheres;
        
        public:
            std::vector<Vertex> vertices;
        
            int renderedMeshID;
        
            Math::Scalar quadricWeights;
        
            Quadric quadric;
            Region region;

            Math::Vector3 center;
            Math::Scalar radius;

            Sphere();
            Sphere(const Vertex& vertex, Math::Scalar targetSphereRadius);
            Sphere(const Math::Vector3& center, Math::Scalar radius);

            Quadric getSphereQuadric();

            void addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight = 1.0);
            void addQuadric(const Quadric& q);
        
            void addVertex(const Vertex& vertex);
        
            void renderAssociatedVertices(RenderableMesh& referenceMesh, Math::Scalar sphereSize = 0.01);
            void clearRenderedSpheres(RenderableMesh& referenceMesh);
        
            void operator = (const Sphere& s)
            {
                this->quadric = s.quadric;
                this->region = s.region;
                
                this->center = s.center;
                this->radius = s.radius;
                this->vertices = s.vertices;
            }

            Sphere lerp(const Sphere &s, Math::Scalar t);
    };
}

#endif /* Sphere_hpp */
