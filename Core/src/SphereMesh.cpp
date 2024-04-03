#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include "../SphereMesh.hpp"

#include <Math.hpp>

#include <YAMLUtils.hpp>
#include <ScopeTimer.hpp>

#include <omp.h>

#include <filesystem>
#include <cmath>


// TODO: Define a stop criteria for the collapsing of the timedSpheres-mesh (error of the quadrics)
// TODO: Define a way to avoid using the EPSILON/improve its usage
// TODO: Implement a link function for the spheres, when clicking two spheres I can link them, also with three, in
//  the first case I generate a new edge, in the second case I generate a new triangle, I need to discard the
//  operations if the spheres are already connected, but I want to hande the transform of an edge into a triangle
//  selecting a new timedSpheres and I want to handle the split of a triangle into three edges
// TODO: Implement the function to unlink two or three spheres

namespace Renderer {
    SphereMesh::SphereMesh(const SphereMesh& sm) : referenceMesh(sm.referenceMesh)
    {
        BDDSize = sm.BDDSize;
	    
	    timedSpheres = sm.timedSpheres;
		
        triangle = sm.triangle;
        edge = sm.edge;
        
        sphereShader = sm.sphereShader;
        renderType = RenderType::BILLBOARDS;
	    
	    initializeEdgeQueue();
    }

    SphereMesh::SphereMesh(TriMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius) : referenceMesh(mesh)
    {
        this->sphereShader = shader;
        renderType = RenderType::BILLBOARDS;
        
        BDDSize = mesh->bbox.BDD().magnitude();

        initializeSphereMeshTriangles(mesh->faces);
        initializeSpheres(mesh->vertices, 0.01 * BDDSize);
        
        computeSpheresProperties(mesh->vertices, mesh->faces);
        updateSpheres();
		
	    for (const Triangle& t : triangle)
	    {
		    timedSpheres[t.i].sphere.addNeighbourSphere(t.j);
		    timedSpheres[t.i].sphere.addNeighbourSphere(t.k);
		    timedSpheres[t.j].sphere.addNeighbourSphere(t.i);
		    timedSpheres[t.j].sphere.addNeighbourSphere(t.k);
		    timedSpheres[t.k].sphere.addNeighbourSphere(t.i);
		    timedSpheres[t.k].sphere.addNeighbourSphere(t.j);
	    }
		
		if (IMPLEMENT_THIERY_2013)
			addGeometricallyCloseNeighbours(0.05 * BDDSize);
		else
		{
	        extendSpheresNeighboursOneStep();
	        extendSpheresNeighboursOneStep();
		}
        initializeEdgeQueue();
    }
	
	void SphereMesh::resetSphereMesh()
	{
		timedSpheres.clear();
		triangle.clear();
		edge.clear();
		edgeQueue.clear();
		sphereMapper.clear();
		
		performedOperations = 0;
		numberOfActiveSpheres = 0;
		
		initializeSphereMeshTriangles(referenceMesh->faces);
		initializeSpheres(referenceMesh->vertices, 0.01 * BDDSize);
		
		computeSpheresProperties(referenceMesh->vertices, referenceMesh->faces);
		updateSpheres();
		
		for (const Triangle& t : triangle)
		{
			timedSpheres[t.i].sphere.addNeighbourSphere(t.j);
			timedSpheres[t.i].sphere.addNeighbourSphere(t.k);
			timedSpheres[t.j].sphere.addNeighbourSphere(t.i);
			timedSpheres[t.j].sphere.addNeighbourSphere(t.k);
			timedSpheres[t.k].sphere.addNeighbourSphere(t.i);
			timedSpheres[t.k].sphere.addNeighbourSphere(t.j);
		}
		
		if (IMPLEMENT_THIERY_2013)
			addGeometricallyCloseNeighbours(0.05 * BDDSize);
		else
		{
			extendSpheresNeighboursOneStep();
			extendSpheresNeighboursOneStep();
		}
		initializeEdgeQueue();
	}
	
	void SphereMesh::addGeometricallyCloseNeighbours(Math::Scalar epsilon)
	{
		for (int i = 0; i < timedSpheres.size(); i++)
		{
			for (int j = i + 1; j < timedSpheres.size(); j++)
			{
				bool check = (timedSpheres[i].sphere.center - timedSpheres[j].sphere.center).magnitude()
				             - (timedSpheres[i].sphere.radius + timedSpheres[j].sphere.radius) <= epsilon;
				Vertex& vi = referenceMesh->vertices[*timedSpheres[i].sphere.vertices.begin()];
				Vertex& vj = referenceMesh->vertices[*timedSpheres[j].sphere.vertices.begin()];
				if (check && normalTest(vi, vj))
				{
					timedSpheres[i].sphere.neighbourSpheres.insert(j);
					timedSpheres[j].sphere.neighbourSpheres.insert(i);
				}
			}
		}
	}
	
	void SphereMesh::updateNeighborsOf(Sphere& s)
	{
		set_of_int newNeighbors;
		
		int sphereAlias = alias(sphereMapper[s.getID()]);
		for (int i : s.neighbourSpheres)
		{
			int j = alias(i);
			if (j != sphereAlias)
				newNeighbors.insert(j);
		}
		
		s.neighbourSpheres = newNeighbors;
	}
	
	int SphereMesh::alias(int i)
	{
		int j = timedSpheres[i].alias;
		
		if (i == j) return i;
		
		return timedSpheres[i].alias = alias(j);
	}
	
	void SphereMesh::extendSpheresNeighboursOneStep()
	{
		std::vector<set_of_int> originalFriends(timedSpheres.size());
		for (int i = 0; i < timedSpheres.size(); i++)
			originalFriends[i] = timedSpheres[i].sphere.neighbourSpheres;
		
		for (TimedSphere& s : timedSpheres)
			for (int j : s.sphere.neighbourSpheres)
				s.sphere.neighbourSpheres += originalFriends[j];
		
		for (int i = 0; i < timedSpheres.size(); i++)
			timedSpheres[i].sphere.neighbourSpheres.erase(i);
	}
	
	bool SphereMesh::isTimedSphereAlive(int id)
	{
		return timedSpheres[id].alias == id;
	}

    int SphereMesh::getPerSphereVertexCount() const {
        return perSphereVertices;
    }

    int SphereMesh::getRenderCalls() const {
        return renderCalls;
    }

    void SphereMesh::resetRenderCalls() {
        renderCalls = 0;
    }

    int SphereMesh::getTriangleSize() {
        return (int)triangle.size();
    }

    int SphereMesh::getEdgeSize() {
        return (int)edge.size();
    }
	
	SphereMesh& SphereMesh::operator = (const SphereMesh& sm) {
		if (this == &sm)
			return *this;
		
		BDDSize = sm.BDDSize;
		timedSpheres = sm.timedSpheres;
		triangle = sm.triangle;
		edge = sm.edge;
		
		return *this;
	}
	
	void SphereMesh::initializeSphereMeshTriangles(const std::vector<Face>& faces)
    {
        triangle.reserve(faces.size());
        
        for (auto face : faces)
            triangle.insert(Triangle(face.i, face.j, face.k));
    }

    void SphereMesh::initializeSpheres(std::vector<Vertex>& vertices, Math::Scalar initialRadius)
    {
		timedSpheres.clear();
        timedSpheres.reserve(vertices.size());
		
	    for (int i = 0; i < vertices.size(); i++)
	    {
			auto newSphere = Sphere(vertices[i], i, initialRadius);
			
			if (IMPLEMENT_THIERY_2013)
				newSphere.initTHIERY(vertices[i]);
			
		    timedSpheres.emplace_back(newSphere, timedSpheres.size(), performedOperations);
			
			sphereMapper[newSphere.getID()] = static_cast<int>(timedSpheres.size()) - 1;
		}
	    
	    numberOfActiveSpheres = static_cast<int>(timedSpheres.size());
    }
	
	void SphereMesh::addPotentialCollapse(int i, int j)
	{
		EdgeCollapse e = EdgeCollapse(i, j, performedOperations);
		updateCost(e);
		edgeQueue.push(e);
	}

    void SphereMesh::initializeEdgeQueue()
    {
	    edgeQueue = TemporalValidityQueue(timedSpheres, sphereMapper);
		performedOperations = 0;
		numberOfActiveSpheres = static_cast<int>(timedSpheres.size());
		
		for (int i = 0; i < timedSpheres.size(); i++)
			for (int j : timedSpheres[i].sphere.neighbourSpheres)
				if (i > j)
					addPotentialCollapse(i, j);
    }
	
    RenderType SphereMesh::getRenderType() {
        return this->renderType;
    }

    void SphereMesh::setRenderType(const RenderType &selectedRenderType) {
        this->renderType = selectedRenderType;
    }

    void SphereMesh::renderSphere(const Math::Vector3 &center, Math::Scalar radius, const Math::Vector3 &color) {
        ++renderCalls;
        if (renderType == RenderType::SPHERES)
            renderOneSphere(center, radius, color);
        else
            renderOneBillboardSphere(center, radius, color);
    }

    void SphereMesh::computeSpheresProperties(const std::vector<Vertex>& vertices, const std::vector<Face>& faces)
    {
        const Math::Scalar sigma = 1.0;
        
        for (const Face& j : faces)
        {
            int i0 = j.i;
            int i1 = j.j;
            int i2 = j.k;
            
            Math::Vector3 v0 = vertices[i0].position;
            Math::Vector3 v1 = vertices[i1].position;
            Math::Vector3 v2 = vertices[i2].position;
            Math::Vector3 normal = getTriangleNormal(v0, v1, v2);
            
            Math::Scalar area = 0.5 * (v1 - v0).cross(v2 - v0).magnitude();
            Math::Scalar weight = area / 3.0;
            
            Math::Scalar totalK1 = (vertices[i0].curvature[0] + vertices[i1].curvature[0] + vertices[i2]
					.curvature[0]) / 3.0;
            Math::Scalar totalK2 = (vertices[i0].curvature[1] + vertices[i1].curvature[1] + vertices[i2]
					.curvature[1]) / 3.0;
            
            weight *= (1 + sigma * BDDSize * BDDSize * ((totalK1 * totalK1) + (totalK2 * totalK2)));
            
            Quadric q = Quadric(v0, normal) * weight;
	        
	        timedSpheres[i0].sphere.quadric += q;
	        timedSpheres[i1].sphere.quadric += q;
	        timedSpheres[i2].sphere.quadric += q;
	        
	        timedSpheres[i0].sphere.quadricWeights += weight;
	        timedSpheres[i1].sphere.quadricWeights += weight;
	        timedSpheres[i2].sphere.quadricWeights += weight;
        }
    }

    void SphereMesh::updateSpheres()
    {
        for (TimedSphere& i : timedSpheres)
        {
            i.sphere.quadric *= (1/i.sphere.quadricWeights);
			
            Math::Vector4 result = i.sphere.quadric.minimizer(0.001);
            i.sphere.center = result.toQuaternion().immaginary;
            i.sphere.radius = result.coordinates.w;
        }
    }
	
	[[maybe_unused]] Math::Vector3 SphereMesh::getTriangleCentroid(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3)
    {
        Math::Vector3 centroid;

        centroid[0] = (v1[0] + v2[0] + v3[0]) / 3.0f;
        centroid[1] = (v1[1] + v2[1] + v3[1]) / 3.0f;
        centroid[2] = (v1[2] + v2[2] + v3[2]) / 3.0f;

        return centroid;
    }

    Math::Vector3 SphereMesh::getTriangleNormal(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3)
    {
        return (v2 - v1).cross(v3 - v1).normalized();
    }

    void SphereMesh::clear ()
    {
        timedSpheres.clear();
        triangle.clear();
        edge.clear();
    }

    void SphereMesh::drawSpheresOverEdge(const Edge &e, int ns, Math::Scalar rescaleRadii, Math::Scalar minRadiiScale)
    {
        int nSpheres = ns;

        const Math::Vector3 color = Math::Vector3(0.1, 0.7, 1);

        for (int i = 1; i < nSpheres - 1; i++)
            renderSphere(Math::lerp(timedSpheres[e.i].sphere.center, timedSpheres[e.j].sphere.center, i * 1.0 / (nSpheres - 1)),
                            Math::lerp(
                                       0.001,
                                       Math::lerp(timedSpheres[e.i].sphere.radius, timedSpheres[e.j].sphere.radius, i * 1.0 / (nSpheres - 1)),
                                       rescaleRadii
                                       ),
                            color);
    }

    void SphereMesh::drawSpheresOverTriangle(const Triangle& e, int ns, Math::Scalar size, Math::Scalar minRadiiScale)
    {
        Math::Vector3 color = Math::Vector3(0.7f, 0.1f, 1);

        int nSpheres = ns;

        for (int i = 0; i < nSpheres; i++)
            for (int j = 0; j < nSpheres - i; j++)
            {
                int k = nSpheres - 1 - i - j;

                if (i == nSpheres - 1 || j == nSpheres - 1 || k == nSpheres - 1) continue;

                Math::Scalar ci = i * 1.0 / (nSpheres - 1);
                Math::Scalar cj = j * 1.0 / (nSpheres - 1);
                Math::Scalar ck = k * 1.0 / (nSpheres - 1);
                
                Math::Vector3 origin = Math::Vector3(timedSpheres[e.i].sphere.center * ci + timedSpheres[e.j].sphere.center * cj + timedSpheres[e.k].sphere.center * ck);
                Math::Scalar radius = Math::lerp(0.001, timedSpheres[e.i].sphere.radius * ci + timedSpheres[e.j]
				.sphere.radius * cj + timedSpheres[e.k].sphere.radius * ck, size);
                
                renderSphere(origin, radius, color);
            }
    }

    void SphereMesh::renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color) {
        Math::Scalar t = 0.0;
        
        while (t < 1.0) {
            auto point = Math::lerp<Math::Vector3>(p0, p1, t);
            
            renderSphere(point, BDDSize * 0.002, color);
            
            t += 0.05;
        }
    }

    void SphereMesh::renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color, int spheresPerEdge, Math::Scalar sphereSize) {
        Math::Scalar t = 0.0;
        Math::Scalar cyclesIncrement = 1.0 / (Math::Scalar)spheresPerEdge;
        
        while (t < 1.0) {
            auto point = Math::lerp<Math::Vector3>(p0, p1, t);
            
            renderSphere(point, BDDSize * sphereSize, color);
            
            t += cyclesIncrement;
        }
    }

    void SphereMesh::renderConnectivity()
    {
        const Math::Vector3 color = Math::Vector3(1, 1, 0);
        for (const auto& t : triangle)
        {
            renderOneLine(timedSpheres[t.i].sphere.center, timedSpheres[t.j].sphere.center, color);
            renderOneLine(timedSpheres[t.i].sphere.center, timedSpheres[t.k].sphere.center, color);
            renderOneLine(timedSpheres[t.j].sphere.center, timedSpheres[t.j].sphere.center, color);
        }
        
        for (const auto& e : edge)
            renderOneLine(timedSpheres[e.i].sphere.center, timedSpheres[e.j].sphere.center, color);
    }

    void SphereMesh::renderConnectivity(int spheresPerEdge, Math::Scalar sphereSize) {
        const Math::Vector3 color = Math::Vector3(1, 1, 0);
        for (const auto& t : triangle)
        {
            renderOneLine(timedSpheres[t.i].sphere.center, timedSpheres[t.j].sphere.center, color, spheresPerEdge, sphereSize);
            renderOneLine(timedSpheres[t.i].sphere.center, timedSpheres[t.k].sphere.center, color, spheresPerEdge, sphereSize);
            renderOneLine(timedSpheres[t.j].sphere.center, timedSpheres[t.j].sphere.center, color, spheresPerEdge, sphereSize);
        }
        
        for (const auto& e : edge)
            renderOneLine(timedSpheres[e.i].sphere.center, timedSpheres[e.j].sphere.center, color, spheresPerEdge, sphereSize);
    }

    void SphereMesh::render()
    {
        for (auto i : triangle)
            this->drawSpheresOverTriangle(i);

        for (auto i : edge)
            this->drawSpheresOverEdge(i);
    }

    void SphereMesh::renderWithNSpherePerEdge(int n, Math::Scalar rescaleRadii, Math::Scalar minRadiiScale)
    {
        for (auto i : triangle)
            this->drawSpheresOverTriangle(i, n, rescaleRadii, minRadiiScale);

        for (auto i : edge)
            this->drawSpheresOverEdge(i, n, rescaleRadii, minRadiiScale);
    }

    void SphereMesh::renderOneBillboardSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color)
    {
        static GLuint VAO = 0xBAD, VBO = 0xBAD, EBO = 0xBAD;
        static std::vector<float> vertices;
        static std::vector<unsigned int> indices;
        static int vertexCount = 0;

        if (VAO == 0xBAD)
        {
            vertices = {
                -1.0f,  1.0f,
                 1.0f,  1.0f,
                 1.0f, -1.0f,
                -1.0f, -1.0f
            };

            indices = {
                0, 1, 2,
                2, 3, 0
            };
            
            vertexCount = (int)vertices.size();

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data
			(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }
        
        perSphereVertices = vertexCount;

        sphereShader->use();
        sphereShader->setVec3("center", center);
        sphereShader->setVec3("material.ambient", color);
        sphereShader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
        sphereShader->setVec3("material.specular", Math::Vector3(0, 0, 0));
        sphereShader->setFloat("material.shininess", 0);
        sphereShader->setFloat("radius", static_cast<float>(radius));

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void SphereMesh::renderOneSphere(const Math::Vector3& center, Math::Scalar radius, const Math::Vector3& color) {
        static GLuint VAO = 0xBAD, VBO = 0xBAD , EBO = 0xBAD;
        static std::vector<float> vertices;
        static std::vector<int> faces;
        static int vertexCount = 0;

        if (VAO == 0xBAD)
        {
            // Create an icosahedron
            float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
            std::vector<float> initialVertices = {
                -1,  t,  0,
                 1,  t,  0,
                -1, -t,  0,
                 1, -t,  0,
                 0, -1,  t,
                 0,  1,  t,
                 0, -1, -t,
                 0,  1, -t,
                 t,  0, -1,
                 t,  0,  1,
                -t,  0, -1,
                -t,  0,  1,
            };

            for (int i = 0; i < initialVertices.size(); i += 3) {
                float length = std::sqrt(initialVertices[i]*initialVertices[i] +
                                         initialVertices[i+1]*initialVertices[i+1] +
                                         initialVertices[i+2]*initialVertices[i+2]);
                vertices.push_back(initialVertices[i] / length);
                vertices.push_back(initialVertices[i+1] / length);
                vertices.push_back(initialVertices[i+2] / length);
            }

            faces = {
                0, 11, 5,
                0, 5, 1,
                0, 1, 7,
                0, 7, 10,
                0, 10, 11,
                1, 5, 9,
                5, 11, 4,
                11, 10, 2,
                10, 7, 6,
                7, 1, 8,
                3, 9, 4,
                3, 4, 2,
                3, 2, 6,
                3, 6, 8,
                3, 8, 9,
                4, 9, 5,
                2, 4, 11,
                6, 2, 10,
                8, 6, 7,
                9, 8, 1
            };

            int subdivisions = 2;
            std::vector<int> newFaces;
            for (int level = 0; level < subdivisions; level++) {
                newFaces.clear();
                for (int i = 0; i < faces.size(); i += 3) {
                    // Compute the midpoints of each edge in the triangle and add them to the vertices list
                    int mid[3];
                    for (int e = 0; e < 3; e++) {
                        int i1 = faces[i + e];
                        int i2 = faces[i + (e + 1) % 3];
                        // Compute the midpoint of i1 and i2
                        float midpoint[3] = {
                            (vertices[i1 * 3 + 0] + vertices[i2 * 3 + 0]) / 2,
                            (vertices[i1 * 3 + 1] + vertices[i2 * 3 + 1]) / 2,
                            (vertices[i1 * 3 + 2] + vertices[i2 * 3 + 2]) / 2,
                        };
                        // Normalize the midpoint to create a point on the timedSpheres
                        float length = std::sqrt(midpoint[0] * midpoint[0] + midpoint[1] * midpoint[1] + midpoint[2] * midpoint[2]);
                        midpoint[0] /= length;
                        midpoint[1] /= length;
                        midpoint[2] /= length;
                        // Add the midpoint to the vertices and store the index in mid[e]
                        vertices.insert(vertices.end(), midpoint, midpoint + 3);
                        mid[e] = (int)vertices.size() / 3 - 1;
                    }
                    // Create the four new faces
                    newFaces.insert(newFaces.end(), {
                        faces[i], mid[0], mid[2],
                        faces[i + 1], mid[1], mid[0],
                        faces[i + 2], mid[2], mid[1],
                        mid[0], mid[1], mid[2]
                    });
                }

                faces.swap(newFaces);
                vertexCount = (int)vertices.size();
            }

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), faces.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
        
        perSphereVertices = vertexCount;

        sphereShader->use();
        sphereShader->setVec3("center", center);
        sphereShader->setVec3("material.ambient", color);
        sphereShader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
        sphereShader->setVec3("material.specular", Math::Vector3(0, 0, 0));
        sphereShader->setFloat("material.shininess", 0);
        sphereShader->setFloat("radius", static_cast<float>(radius));

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (int)faces.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void SphereMesh::renderSpheresOnly()
    {
        Math::Vector3 color = Math::Vector3(0, 0, 1);
		
        for (int i = 0; i < timedSpheres.size(); i++)
        {
            if (timedSpheres[i].sphere.radius <= 0 && isTimedSphereAlive(i))
            {
//                std::cout << "Sphere [" << i << "] has radius = " << timedSpheres[i].sphere.radius << std::endl;
                continue;
            }
			
			if (isTimedSphereAlive(i))
                renderSphere(timedSpheres[i].sphere.center, timedSpheres[i].sphere.radius, timedSpheres[i].sphere.color);
        }
    }

    void SphereMesh::renderSphereVertices(int i)
    {
		if (sphereMapper.find(i) == sphereMapper.end())
			return;
		
		int idx = sphereMapper[i];
		for (auto &vertex: timedSpheres[idx].sphere.vertices)
			renderSphere(referenceMesh->vertices[vertex].position, 0.02 * BDDSize, Math::Vector3(0, 1, 0));
    }
	
	void SphereMesh::updateCost(EdgeCollapse& e)
	{
		e.error = Quadric();
		for(int c : e.toCollapse)
			e.error += currentSphere(c).quadric;
		

		if (IMPLEMENT_THIERY_2013)
		{
			e.region.clear();
			for (int c : e.toCollapse)
				e.region.unionWith(currentSphere(c).region);
			
			e.error.getMinimumAndMinimizer(e.cost, e.centerRadius, e.region.getWidth() * (3.0 / 4.0));
		}
		else
		{
			e.error.getMinimumAndMinimizer(e.cost, e.centerRadius);
		}
		e.cost /= (e.toCollapse.size() - 1);
	}
	
	bool SphereMesh::isOutOfDate(const EdgeCollapse& e)
	{
		for (int c : e.toCollapse)
			if (timedSpheres[alias(c)].timestamp > e.timestamp)
				return true;
		
		return false;
	}
	
	bool SphereMesh::debugCheckNoLoops()
	{
		for (int i = 0; i < timedSpheres.size(); i++)
		{
			if (alias(i) != i) continue;
			
			for (int j : timedSpheres[i].sphere.neighbourSpheres)
				if (alias(j) == i)
				{
					std::cerr << "Loop detected between " << i << " and " << j << std::endl;
					return false;
				}
		}

		return true;
	}
	
	bool SphereMesh::normalTest(const Vertex& vA, const Vertex& vB)
	{
		Math::Vector3 directionAB = (vB.position - vA.position);
		
		bool doesAseeB = directionAB.dot(vA.normal) > 0;
		bool doesBseeA = directionAB.dot(vB.normal) < 0;
		
		return !(doesAseeB && doesBseeA);
	}
	
	void SphereMesh::execute(const EdgeCollapse& e)
	{
		performedOperations++;
		numberOfActiveSpheres -= e.toCollapse.size() - 1;
		
		int merged = alias(e.toCollapse.front());
		for (int i : e.toCollapse)
		{
			timedSpheres[i].alias = merged;
			sphereMapper.erase(timedSpheres[i].sphere.getID());
			timedSpheres[merged].sphere.neighbourSpheres += timedSpheres[i].sphere.neighbourSpheres;
			if (merged != i)
				for (auto& vertex : timedSpheres[i].sphere.vertices)
					timedSpheres[merged].sphere.addVertex(referenceMesh->vertices[vertex], vertex);
		}
		
		timedSpheres[merged].sphere.quadric = e.error;
		timedSpheres[merged].sphere.center = e.centerRadius.toQuaternion().immaginary;
		timedSpheres[merged].sphere.radius = e.centerRadius.coordinates.w;
		
		if (IMPLEMENT_THIERY_2013)
			timedSpheres[merged].sphere.region = e.region;
		
		timedSpheres[merged].timestamp = performedOperations;
		sphereMapper[timedSpheres[merged].sphere.getID()] = merged;
		
		updateNeighborsOf(timedSpheres[merged].sphere);
		
		for (int i : timedSpheres[merged].sphere.neighbourSpheres)
			if (i != merged && alias(i) != merged)
				updateNeighborsOf(timedSpheres[i].sphere);
		
		for (int i : timedSpheres[merged].sphere.neighbourSpheres)
			addPotentialCollapse(merged, i);
		
		debugCheckNoLoops();
	}

    bool SphereMesh::collapseSphereMesh()
    {
	    return collapseSphereMesh(numberOfActiveSpheres - 1);
    }
	
	void SphereMesh::updateConnectivityAfterCollapses()
	{
		std::unordered_set<Edge> newEdges;
		std::unordered_set<Triangle> newTriangles;
		for (const Edge& e : edge)
		{
			Edge toInsert = Edge(
					alias(e.i),
					alias(e.j)
			);
			
			if (toInsert.i == toInsert.j)
				continue;
			else
				newEdges.insert(toInsert);
		}
		
		for (const Triangle& t : triangle)
		{
			Triangle toInsert = Triangle(
					alias(t.i),
					alias(t.j),
					alias(t.k)
			);
			
			if (toInsert.i == toInsert.k)
				continue;
			else if (toInsert.i == toInsert.j || toInsert.j == toInsert.k)
				newEdges.insert(Edge(toInsert.i, toInsert.k));
			else
				newTriangles.insert(toInsert);
		}
		
		std::swap(edge, newEdges);
		std::swap(triangle, newTriangles);
	}
	
	bool SphereMesh::engulfsAnything(EdgeCollapse& e)
	{
		Math::Vector3 center = e.centerRadius.truncateToVector3();
		Math::Scalar radius = e.centerRadius.coordinates.w;
		Math::Scalar radiusSquared = radius * radius;
		
		for (int vi = 0; vi < referenceMesh->vertices.size(); vi++)
		{
			Vertex& v = referenceMesh->vertices[vi];
			Math::Scalar distanceSquared = (v.position - center).squareMagnitude();
			int si = alias(vi);
			if (distanceSquared < radiusSquared && !includes(e.toCollapse, si))
			{
				e.toCollapse.emplace_back(si);
				updateCost(e);
				return true;
			}
		}
		
		return false;
	}

    bool SphereMesh::collapseSphereMesh(int n)
    {
	    auto start = std::chrono::high_resolution_clock::now();
	    while (!edgeQueue.empty())
	    {
		    EdgeCollapse e = edgeQueue.top();
		    edgeQueue.pop();
		    
		    if (isOutOfDate(e)) continue;
			
			if (!IMPLEMENT_THIERY_2013)
				if (engulfsAnything(e))
				{
					edgeQueue.push(e);
					continue;
				}
			
		    execute(e);
			
			if (numberOfActiveSpheres <= n) break;
	    }
	    auto stop = std::chrono::high_resolution_clock::now();
	    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		lastCollapseDuration = std::to_string(duration.count() / 1e6);
	 
		
        updateConnectivityAfterCollapses();
		
        return numberOfActiveSpheres <= n;
    }

    int SphereMesh::collapse(int i, int j)
    {
		int aliasI = alias(sphereMapper[i]);
		int aliasJ = alias(sphereMapper[j]);
		
		if (aliasI == aliasJ)
			return aliasI;
		
		EdgeCollapse e = EdgeCollapse(aliasI, aliasJ, performedOperations);
	    updateCost(e);
		
	    execute(e);
	    updateConnectivityAfterCollapses();
		
		return aliasI;
    }
	
	void SphereMesh::saveYAML(const std::string& path, const std::string& fn)
    {
        YAML::Emitter out;

        out << YAML::Comment("Sphere Mesh YAML @ author Davide Paolillo");
        out << YAML::BeginMap;
            out << YAML::Key << "Reference Mesh" << YAML::Value << referenceMesh->path;
            out << YAML::Key << "Sphere Mesh Resolution" << YAML::Value << numberOfActiveSpheres;
            out << YAML::Key << "Performed Operations" << YAML::Value << performedOperations;
            out << YAML::Key << "Number of Active Spheres" << YAML::Value << numberOfActiveSpheres;
            out << YAML::Key << "Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (auto & i : timedSpheres)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, i.sphere.center);
                        out << YAML::Key << "Radius" << YAML::Value << i.sphere.radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, i.sphere.quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, i.sphere.color);
						out << YAML::Key << "Alias" << YAML::Value << i.alias;
						out << YAML::Key << "Neighbours" << YAML::Value;
						out << YAML::BeginSeq;
						for (int j : i.sphere.neighbourSpheres)
							out << j;
						out << YAML::EndSeq;
						out << YAML::Key << "Vertices" << YAML::Value;
						out << YAML::BeginSeq;
						for (auto & vertex : i.sphere.vertices)
							out << vertex;
						out << YAML::EndSeq;
						out << YAML::Key << "Quadric Weights" << YAML::Value << i.sphere.quadricWeights;
                    out << YAML::EndMap;
                }
            out << YAML::EndSeq;
            out << YAML::Key << "Connectivity" << YAML::Value;
            out << YAML::BeginMap;
                out << YAML::Key << "Edges" << YAML::Value;
                out << YAML::BeginSeq;
                    for (auto & i : edge)
                    {
                        out << YAML::BeginMap;
                            out << YAML::Key << "E0" << YAML::Value << i.i;
                            out << YAML::Key << "E1" << YAML::Value << i.j;
                        out << YAML::EndMap;
                    }
                out << YAML::EndSeq;
                out << YAML::Key << "Triangles" << YAML::Value;
                out << YAML::BeginSeq;
                    for (auto & i : triangle)
                    {
                        out << YAML::BeginMap;
                            out << YAML::Key << "T0" << YAML::Value << i.i;
                            out << YAML::Key << "T1" << YAML::Value << i.j;
                            out << YAML::Key << "T2" << YAML::Value << i.k;
                        out << YAML::EndMap;
                    }
                out << YAML::EndSeq;
            out << YAML::EndMap;
        out << YAML::EndMap;

        namespace fs = std::__fs::filesystem;

        fs::path cwd = fs::current_path();

        std::string separator = std::string(1, fs::path::preferred_separator);

        const std::string& filePath = fn;
        std::string folderPath = "." + separator;
        if (path != ".")
            folderPath = path;

        std::ofstream fout(folderPath + filePath);
        std::cout << "File location: " << folderPath + filePath << std::endl;
        fout << out.c_str();
        fout.close();
    }

    void SphereMesh::loadFromYaml(const std::string& path)
    {
        triangle.clear();
        edge.clear();
        timedSpheres.clear();
		sphereMapper.clear();
        
        std::ifstream stream(path);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
		
		performedOperations = data["Performed Operations"].as<int>();
		numberOfActiveSpheres = data["Number of Active Spheres"].as<int>();
		
		int i = 0;
        for (const auto& node : data["Spheres"]) {
            Sphere s;
            
            s.center = node["Center"].as<Math::Vector3>();
            s.radius = node["Radius"].as<Math::Scalar>();
            s.quadric = node["Quadric"].as<Renderer::Quadric>();
            s.color = node["Color"].as<Math::Vector3>();
			s.quadricWeights = node["Quadric Weights"].as<Math::Scalar>();
			s.vertices.clear();
			for (const auto& vertex : node["Vertices"])
				s.addVertex(referenceMesh->vertices[vertex.as<int>()], vertex.as<int>());
			s.neighbourSpheres.clear();
			for (const auto& neighbour : node["Neighbours"])
				s.neighbourSpheres.insert(neighbour.as<int>());

            timedSpheres.emplace_back(Sphere(s), node["Alias"].as<int>(), performedOperations);
			sphereMapper[s.getID()] = i++;
        }
        
        for (const auto& node : data["Connectivity"]["Triangles"]) {
            Triangle t;
            
            t.i = node["T0"].as<int>();
            t.j = node["T1"].as<int>();
            t.k = node["T2"].as<int>();

            triangle.insert(t);
        }
        
        for (const auto& node : data["Connectivity"]["Edges"]) {
            Edge e;
            
            e.i = node["E0"].as<int>();
            e.j = node["E1"].as<int>();
            
            edge.insert(e);
        }
    }
	
	void SphereMesh::saveTXTToAutoPath()
	{
		std::string token;
		std::string last;
		std::istringstream tokenStream(referenceMesh->path);
		
		while (std::getline(tokenStream, token, '/'))
			last = token;
		
		last = token.substr(0, last.size() - 4);
		std::string type = IMPLEMENT_THIERY_2013 ? "THIERY" : "OUR";
		std::string name = last + "_" + type  + "_" + std::to_string(numberOfActiveSpheres) + ".sphere-mesh";
		saveTXT("/Users/davidepaollilo/Desktop/ComparisonSM/", name);
	}

    void SphereMesh::saveTXT(const std::string& path, const std::string& fn)
    {
        std::ostringstream fileContent;
        
        // Stating the count for each type: spheres, triangles, and edges
        fileContent << "Sphere Mesh 2.0" << std::endl;
		fileContent << "Duration: " << lastCollapseDuration << " seconds" << std::endl;
        fileContent << numberOfActiveSpheres;
        fileContent << " " << triangle.size();
        fileContent << " " << edge.size() << std::endl;
        fileContent << "====================" << std::endl;
		
		std::unordered_map<int, int> activeSpheres;
		std::vector<TimedSphere> activeEdges;
		std::vector<TimedSphere> activeTris;
		int idx = 0;
		for (int i = 0; i < timedSpheres.size(); i++)
			if (isTimedSphereAlive(i))
				activeSpheres[currentSphere(i).getID()] = idx++;
        
        // Saving timedSpheres details
        for (int i = 0; i < timedSpheres.size(); i++)
			if (isTimedSphereAlive(i))
                fileContent << currentSphere(i).center[0] << " " << currentSphere(i).center[1] << " "
					<< currentSphere(i).center[2] << " " << currentSphere(i).radius << std::endl;

        // Saving triangle details
        for (const auto& t : triangle)
            fileContent << activeSpheres[timedSpheres[t.i].sphere.getID()] << " " << activeSpheres[timedSpheres[t.j]
			.sphere.getID()] << " " << activeSpheres[timedSpheres[t.k].sphere.getID()] << std::endl;

        // Saving edge details
        for (const auto& e : edge)
            fileContent << activeSpheres[timedSpheres[e.i].sphere.getID()] << " " << activeSpheres[timedSpheres[e.j]
			.sphere.getID()] << std::endl;

        namespace fs = std::__fs::filesystem;

        fs::path cwd = fs::current_path();

        std::string separator = std::string(1, fs::path::preferred_separator);

        const std::string& filePath = fn;
        std::string folderPath = "." + separator;
        if (path != ".")
            folderPath = path;

        std::ofstream fout(folderPath + filePath);
        std::cout << "File location: " << folderPath + filePath << std::endl;
        fout << fileContent.str();
        fout.close();
    }

    void SphereMesh::addEdge(int selectedSphereID) {
        int selectedSphereIndex = sphereMapper[selectedSphereID];
        auto selectedSphere = timedSpheres[selectedSphereIndex];
        Sphere sphereCopy = Sphere(timedSpheres[selectedSphereIndex].sphere.center, timedSpheres[selectedSphereIndex].sphere.radius);
        sphereCopy.quadric = selectedSphere.sphere.quadric;
        sphereCopy.center += Math::Vector3(0.05, 0.05, 0) * BDDSize;
        
        timedSpheres.emplace_back(Sphere(sphereCopy), timedSpheres.size(), performedOperations);
        edge.insert(Edge(selectedSphereIndex, (int)timedSpheres.size() - 1));
    }

    void SphereMesh::addTriangle(int sphereA, int sphereB) {
        int idxA = sphereMapper[sphereA];
        int idxB = sphereMapper[sphereB];
        
        auto selectedA = timedSpheres[idxA];
        auto selectedB = timedSpheres[idxB];
        Sphere sphereCopy = Sphere(Math::lerp<Math::Vector3>(selectedA.sphere.center, selectedB.sphere.center, 0.5),
                Math::lerp(selectedA.sphere.radius, selectedB.sphere.radius, 0.5));
        sphereCopy.quadric = selectedA.sphere.quadric;
        sphereCopy.center += Math::Vector3(0.05, 0.05, 0) * BDDSize;
        
        timedSpheres.emplace_back(Sphere(sphereCopy), timedSpheres.size(), performedOperations);
        triangle.insert(Triangle(idxA, idxB, (int)timedSpheres.size() - 1));
    }

    void SphereMesh::removeSphere(int selectedSphereID) {
        int selectedSphereIndex = sphereMapper[selectedSphereID];
	    
	    timedSpheres[selectedSphereIndex].alias = -1;
		
		for (auto it = triangle.begin(); it != triangle.end();)
			if (it->i == selectedSphereIndex || it->j == selectedSphereIndex || it->k == selectedSphereIndex)
				it = triangle.erase(it);
			else
				++it;
		
		for (auto it = edge.begin(); it != edge.end();)
			if (it->i == selectedSphereIndex || it->j == selectedSphereIndex)
				it = edge.erase(it);
			else
				++it;
		
		sphereMapper.erase(selectedSphereID);
  
		// TODO: Fix this code, now the adding of a sphere is messed up
//        for (auto & i : edge)
//            if (i.i == timedSpheres.size())
//                i.i = selectedSphereIndex;
//            else if (i.j == timedSpheres.size())
//                i.j = selectedSphereIndex;
//
//        for (auto & i : triangle)
//            if (i.i == timedSpheres.size())
//                i.i = selectedSphereIndex;
//            else if (i.j == timedSpheres.size())
//                i.j = selectedSphereIndex;
//            else if (i.k == timedSpheres.size())
//                i.k = selectedSphereIndex;
    }
	
	int SphereMesh::getTimedSphereSize () const
	{
		return numberOfActiveSpheres;
	}
}
