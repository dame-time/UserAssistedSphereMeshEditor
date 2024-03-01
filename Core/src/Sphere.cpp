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

    Sphere::Sphere(const Sphere& other)
    {
        this->renderedMeshID = other.renderedMeshID;
        this->vertices = other.vertices;
        this->quadricWeights = other.quadricWeights;
        this->quadric = other.quadric;
        this->region = other.region;
        this->center = other.center;
        this->radius = other.radius;
        this->color = other.color;
		this->isDangling = other.isDangling;
    }

    Sphere::Sphere(Vertex& vertex, Math::Scalar k)
    {
        quadric = Quadric::initializeQuadricFromVertex(vertex, k) * 1e-6;
        region = Region(vertex.position);

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
        
        this->color = Math::Vector3(1, 0, 0);
        
        this->quadricWeights = 1e-6;
        
        generateUUID();
		
		vertex.referenceSphere = this->getID();
		this->vertices.push_back(&vertex);
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

    int Sphere::getID() const
    {
        return this->renderedMeshID;
    }

    Quadric Sphere::getSphereQuadric() const
    {
        return this->quadric;
    }

    void Sphere::addFace(const Math::Vector3& centroid, const Math::Vector3& normal, Math::Scalar weight)
    {
        this->quadric += (Quadric(centroid, normal) * weight);

        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
    }

    void Sphere::addQuadric(const Quadric& q)
    {
        this->quadric += q;
        
        auto result = quadric.minimizer();
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
    }

    void Sphere::constrainSphere(const Math::Scalar& constrainRadius)
    {
        if (this->radius <= constrainRadius)
            return;
        
        auto result = this->quadric.constrainR(constrainRadius);
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
    }

    bool Sphere::checkSphereOverPlanarRegion() const
    {
        return this->center == Math::Vector3(-1, -1, -1) && this->radius == 0;
    }

    void Sphere::approximateSphereOverPlanarRegion(const Math::Vector3& edge0, const Math::Vector3& edge1)
    {
        auto result = this->quadric.constrainIntoVector(edge0, edge1, region.directionalWidth);
        this->center = result.toQuaternion().immaginary;
        this->radius = result.coordinates.w;
    }

    void Sphere::addVertex(Vertex& vertex)
    {
		vertex.referenceSphere = this->getID();
        vertices.push_back(&vertex);
    }
	
#ifdef USE_THIEF_SPHERE_METHOD
	int Sphere::clearNotLinkedVertices()
	{
		auto referenceSphere = -1;
		
		for (auto it = vertices.begin(); it != vertices.end();)
			if ((*it)->referenceSphere != this->getID())
			{
				if (vertices.size() == 1)
					referenceSphere = (*it)->referenceSphere;
				
				it = vertices.erase(it);
			}
			else
				++it;
			
		isDangling = vertices.empty();
		return referenceSphere;
	}
#endif

    Sphere Sphere::lerp(const Sphere &s, Math::Scalar t) const
    {
        Math::Vector4 origin(this->center, this->radius);
        Math::Vector4 destination(s.center, s.radius);

        auto result = origin.lerp(destination, t);

        return {result.toQuaternion().immaginary, result.coordinates.w};
    }

    bool Sphere::intersectsVertex(const Math::Vector3& vertex)
    {
        return std::pow(center[0] - vertex[0], 2) + std::pow(center[1] - vertex[1], 2) + std::pow(center[2] - vertex[2], 2) <= (radius * radius);
    }
}
