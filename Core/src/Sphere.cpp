//
//  Sphere.cpp
//  Thesis
//
//  Created by Davide Paollilo on 10/04/23.
//

#include <Sphere.hpp>

namespace Renderer {
    Sphere::Sphere()
    {
        quadric = Quadric();
        region = Region();
        
        this->quadricWeights = 0.0;
    }

    Sphere::Sphere(const Vertex& vertex, Math::Scalar k)
    {
        quadric = Quadric::initializeQuadricFromVertex(vertex, k) * 1e-6;
        region = Region(vertex.position);

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
        
        this->quadricWeights = 1e-6;
        
        this->vertices.push_back(vertex);
    }

    Sphere::Sphere(const Math::Vector3& center, Math::Scalar radius)
    {
        this->center = center;
        this->radius = radius;
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

    void Sphere::renderAssociatedVertices(RenderableMesh& referenceMesh, Math::Scalar sphereSize)
    {
        const Math::Vector3 color = Math::Vector3(0, 1, 0);

        for (int i = 0; i < vertices.size(); i++)
        {
            // The point is inside the triangle
            RenderableMesh& s = referenceMesh.addSubSphere();
//            referenceMesh.renderedSphereVertexMeshes.push_back(s);
            
            s.setUniformColor(Math::Vector3(0, 1, 0));

            s.scaleUniform(sphereSize);
            s.translate(vertices[i].position);

            renderedSpheres.push_back(s.getID());
        }
    }

    void Sphere::clearRenderedSpheres(RenderableMesh& referenceMesh)
    {
        std::vector<RenderableMesh> tempSpheres;

        for (auto& sphere : referenceMesh.subSpheres) {
            bool found = false;

            for (auto it = renderedSpheres.begin(); it != renderedSpheres.end(); ++it) {
                if (sphere.getID() == *it) {
                    found = true;
                    renderedSpheres.erase(it);
                    break;
                }
            }

            if (!found) {
                tempSpheres.push_back(std::move(sphere));
            }
        }

        referenceMesh.subSpheres = std::move(tempSpheres);
    }

    Sphere Sphere::lerp(const Sphere &s, Math::Scalar t)
    {
        Math::Vector4 origin(this->center, this->radius);
        Math::Vector4 destination(s.center, s.radius);

        auto result = origin.lerp(destination, t);

        return Sphere(result.toQuaternion().immaginary, result.coordinates.w);
    }
}
