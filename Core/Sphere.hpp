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
            int renderedMeshID;
        
            void generateUUID();
        
        public:
            std::vector<Vertex> vertices;
        
            Math::Scalar quadricWeights;
        
            Quadric quadric;
            Region region;

            Math::Vector3 center;
            Math::Scalar radius;
            Math::Vector3 color;

            Sphere();
            Sphere(const Vertex& vertex, Math::Scalar targetSphereRadius);
            Sphere(const Math::Vector3& center, Math::Scalar radius);

            Quadric getSphereQuadric();

            void addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight = 1.0);
            void addQuadric(const Quadric& q);
        
            void addVertex(const Vertex& vertex);
        
            void operator = (const Sphere& s)
            {
                this->quadric = s.quadric;
                this->region = s.region;
                
                this->center = s.center;
                this->radius = s.radius;
                this->vertices = s.vertices;
            }
        
            int getID();

            Sphere lerp(const Sphere &s, Math::Scalar t);
    };
}

#endif /* Sphere_hpp */
