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
		this->neighbourSpheres = other.neighbourSpheres;
    }

    Sphere::Sphere(Vertex& vertex, int vertexIdx, Math::Scalar k)
    {
		init(vertex, vertexIdx, k);
    }

    Sphere::Sphere(const Math::Vector3& center, Math::Scalar radius)
    {
        this->center = center;
        this->radius = radius;
        
        this->color = Math::Vector3(1, 0, 0);
        
        generateUUID();
    }
	
	void Sphere::init (Vertex& vertex, int vertexIdx, Math::Scalar k)
	{
		quadric = Quadric::initializeQuadricFromVertex(vertex, k) * 1e-6;
		
		auto result = quadric.minimizer(k);
		this->center = result.toQuaternion().immaginary;
		this->radius = result.coordinates.w;
		
		this->color = Math::Vector3(1, 0, 0);
		
		this->quadricWeights = 1e-6;
		
		generateUUID();
		
		vertex.referenceSphere = this->getID();
		this->vertices.insert(vertexIdx);
	}
	
	void Sphere::initTHIERY (Renderer::Vertex &vertex)
	{
		quadric = Quadric(); // Removing "OUR" regularizer
		this->quadricWeights = 0;
		region.setAsPoint(vertex.position);
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
	
    void Sphere::addVertex(Vertex& vertex, int index)
    {
		vertex.referenceSphere = this->getID();
        vertices.insert(index);
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

    bool Sphere::containsVertex(const Math::Vector3& vertex)
    {
        return std::pow(center[0] - vertex[0], 2) + std::pow(center[1] - vertex[1], 2) + std::pow(center[2] - vertex[2], 2) <= (radius * radius);
    }
	
	void Sphere::addNeighbourSphere (int sphereIndex)
	{
		neighbourSpheres.insert(sphereIndex);
	}
}
