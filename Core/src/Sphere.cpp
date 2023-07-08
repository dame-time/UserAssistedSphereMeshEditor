//
//  Sphere.cpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#include <Sphere.hpp>

#include <random>

namespace Renderer {
    Sphere::Sphere()
    {
        quadric = Quadric();
        region = Region();
        
        this->quadricWeights = 0.0;
        
        generateUUID();
    }

    Sphere::Sphere(const Vertex& vertex, Math::Scalar k)
    {
        quadric = Quadric::initializeQuadricFromVertex(vertex, k) * 1e-6;
        region = Region(vertex.position);

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
        
        this->color = Math::Vector3(1, 0, 0);
        
        this->quadricWeights = 1e-6;
        
        this->vertices.push_back(vertex);
        
        generateUUID();
    }

    Sphere::Sphere(const Math::Vector3& center, Math::Scalar radius)
    {
        this->center = center;
        this->radius = radius;
        
        this->color = Math::Vector3(1, 0, 0);
        
        generateUUID();
    }

    void Sphere::generateUUID()
    {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distr(0, 999999);
        
        int random_number = distr(generator);
        
        this->renderedMeshID = random_number;
    }

    int Sphere::getID()
    {
        return this->renderedMeshID;
    }

    Quadric Sphere::getSphereQuadric()
    {
        return this->quadric;
    }

    void Sphere::addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight)
    {
        this->quadric += (Quadric(centroid, normal) * weight);

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;

         if (this->radius > this->region.directionalWidth)
             this->radius = this->region.directionalWidth;
    }

    void Sphere::addQuadric(const Quadric& q)
    {
        this->quadric += q;

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;

         if (this->radius > this->region.directionalWidth)
             this->radius = this->region.directionalWidth;
    }

    void Sphere::addVertex(const Vertex& vertex)
    {
        vertices.push_back(vertex);
    }

    Sphere Sphere::lerp(const Sphere &s, Math::Scalar t)
    {
        Math::Vector4 origin(this->center, this->radius);
        Math::Vector4 destination(s.center, s.radius);

        auto result = origin.lerp(destination, t);

        return Sphere(result.toQuaternion().immaginary, result.coordinates.w);
    }
}
