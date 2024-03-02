#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include "../SphereMesh.hpp"

#include <Math.hpp>

#include <YAMLUtils.hpp>
#include <ScopeTimer.hpp>

#include <omp.h>

#include <filesystem>
#include <algorithm>
#include <cmath>

// TODO: Define a stop criteria for the collapsing of the sphere-mesh (error of the quadrics)
// TODO: Define a way to avoid using the EPSILON/improve its usage
// TODO: Implement a link function for the spheres, when clicking two spheres I can link them, also with three, in
//  the first case I generate a new edge, in the second case I generate a new triangle, I need to discard the
//  operations if the spheres are already connected, but I want to hande the transform of an edge into a triangle
//  selecting a new sphere and I want to handle the split of a triangle into three edges
// TODO: Implement the function to unlink two or three spheres
namespace Renderer {
    SphereMesh::SphereMesh(const SphereMesh& sm) : referenceMesh(sm.referenceMesh)
    {
        BDDSize = sm.BDDSize;
        initializeEPSILON();
        
        sphere = sm.sphere;
        initialSpheres = sm.initialSpheres;
        
        triangle = sm.triangle;
        edge = sm.edge;
        
        sphereShader = sm.sphereShader;
        renderType = RenderType::SPHERES;
        
        clearSphereMesh();
        initializeEdgeQueue();
    }

    SphereMesh::SphereMesh(RenderableMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius) : referenceMesh(mesh)
    {
        this->sphereShader = shader;
        renderType = RenderType::SPHERES;
        
        BDDSize = mesh->bbox.BDD().magnitude();
        initializeEPSILON();

        initializeSphereMeshTriangles(mesh->faces);
        initializeSpheres(mesh->vertices, 0.01f * BDDSize);
        
        computeSpheresProperties(mesh->vertices);
        updateSpheres();

        initialSpheres.reserve(sphere.size());
        initialSpheres = sphere;
        
        clearSphereMesh();
        initializeEdgeQueue();
    }

    void SphereMesh::initializeEPSILON()
    {
//       EPSILON = 0.0275 * BDDSize;
        EPSILON = 0.04 * BDDSize; // CAMEL & FOOT
//        EPSILON = 0.095 * BDDSize; // BUNNY
//        EPSILON = 0.035 * BDDSize; // HAND
    }

    void SphereMesh::setEpsilon(const Math::Scalar& e)
    {
        EPSILON = e * BDDSize;
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
		sphere = sm.sphere;
		initialSpheres = sm.initialSpheres;
		triangle = sm.triangle;
		edge = sm.edge;
		edgeConnectivity = sm.edgeConnectivity;
		triangleConnectivity = sm.triangleConnectivity;
		EPSILON = sm.EPSILON;
		
		return *this;
	}
	
	void SphereMesh::initializeSphereMeshTriangles(const std::vector<Face>& faces)
    {
        triangle.reserve(faces.size());
        
        for (auto face : faces)
            triangle.emplace_back(face.i, face.j, face.k);
    }

    void SphereMesh::initializeSpheres(std::vector<Vertex>& vertices, Math::Scalar initialRadius)
    {
        sphere.reserve(vertices.size());
	    
	    for (auto& vertex : vertices)
	    {
			auto newSphere = Sphere(vertex, initialRadius);
			
		    sphere.emplace_back(newSphere);
			
			sphereMapper[newSphere.getID()] = static_cast<int>(sphere.size()) - 1;
		}
    }

    void SphereMesh::initializeEdgeQueue()
    {
        int sphereSize = (int)sphere.size();
        std::queue<EdgeCollapse> localQueues[omp_get_max_threads()];

        #pragma omp parallel default(none) shared(sphere, sphereSize, localQueues, edgeQueue)
        {
            int tid = omp_get_thread_num();

            #pragma omp for schedule(dynamic, 64)
            for (int i = 0; i < sphereSize; ++i) {
                for (int j = i + 1; j < sphereSize; ++j) {
					// FIXME: In here I don't want to exclude the collapses of the spheres that are linked by an edge
					//  or by a triangle
                    if ((sphere[i].center - sphere[j].center).squareMagnitude() <= EPSILON * EPSILON) {
                        EdgeCollapse e(sphere[i], sphere[j], i, j);
                        e.updateError();
	                    e.queueIdI = 0;
	                    e.queueIdJ = 0;

                        if (e.idxI > e.idxJ)
                            e.updateEdge(e.j, e.i, e.idxJ, e.idxI);

                        localQueues[tid].push(e);
                    }
                }
            }
        }
	    
	    for (int i = 0; i < omp_get_max_threads(); ++i)
		    while (!localQueues[i].empty())
		    {
			    edgeQueue.push(localQueues[i].front());
			    localQueues[i].pop();
		    }
	    
    }

    void SphereMesh::updateEdgeQueue(const EdgeCollapse& collapsedEdge)
    {
        auto& edgeI = collapsedEdge.idxI;
        auto& edgeJ = collapsedEdge.idxJ;
        
        const auto epsilonSquared = EPSILON * EPSILON;
        const auto size = sphere.size();

        auto updateEdges = [&](int i, int edgeIdx, int queueIdOffset) {
            if (i != edgeIdx && (sphere[i].center - sphere[edgeIdx].center).squareMagnitude() <= epsilonSquared) {
                EdgeCollapse newEdge = EdgeCollapse(sphere[i], sphere[edgeIdx], i, edgeIdx);

                newEdge.queueIdI = 0;
                newEdge.queueIdJ = queueIdOffset;

                if (newEdge.idxI > newEdge.idxJ) {
                    std::swap(newEdge.idxI, newEdge.idxJ);
                    std::swap(newEdge.queueIdI, newEdge.queueIdJ);
                }
                
                newEdge.updateError();
                edgeQueue.push(newEdge);
            }
        };

        for (int i = 0; i < size; ++i) {
            updateEdges(i, edgeI, collapsedEdge.queueIdJ + 1);
            updateEdges(i, edgeJ, collapsedEdge.queueIdI + 1);
        }
    }

    RenderType SphereMesh::getRenderType() {
        return this->renderType;
    }

    void SphereMesh::setRenderType(const RenderType &selectedRenderType) {
        this->renderType = selectedRenderType;
    }

    void SphereMesh::buildEdgeConnectivity() {
        size_t size = sphere.size();
        edgeConnectivity.clear();
        edgeConnectivity.reserve(size);

        std::vector<bool> row(size, false);

        for (size_t i = 0; i < size; ++i) {
            edgeConnectivity.push_back(row);
        }
    }

    void SphereMesh::buildTriangleConnectivity() {
        size_t size = sphere.size();
        triangleConnectivity.clear();
        triangleConnectivity.reserve(size);

        std::vector<bool> innerRow(size, false);
        std::vector<std::vector<bool>> innerMatrix(size, innerRow);

        for (size_t i = 0; i < size; ++i) {
            triangleConnectivity.push_back(innerMatrix);
        }
    }

    void SphereMesh::clearEdges() {
        // Loop to swap and set edgeConnectivity
        for (auto & i : edge) {
            if (i.i > i.j) {
                std::swap(i.i, i.j); // In-place swap
            }
            edgeConnectivity[i.i][i.j] = true;
        }

        edge.clear();
        size_t eSize = edgeConnectivity.size();
        for (size_t i = 0; i < eSize; ++i) {
            for (size_t j = 0; j < edgeConnectivity[i].size(); ++j) {
                if (edgeConnectivity[i][j]) {
                    edge.emplace_back(i, j);
                }
            }
        }
    }

    void SphereMesh::clearTriangles() {
        // Precompute sizes
	    auto triangleSize = (int)triangle.size();
        auto connectivitySize = (int)triangleConnectivity.size();

        // First loop
        #pragma omp parallel for schedule(dynamic, 64) default(none) shared(triangle, edgeConnectivity, triangleConnectivity, triangleSize, connectivitySize)
        for (int i = 0; i < triangleSize; ++i) {
            auto& tri = triangle[i];
            int min = std::min({tri.i, tri.j, tri.k});
            int max = std::max({tri.i, tri.j, tri.k});
            
            if (tri.i != min && tri.i != max) tri = Triangle(min, tri.i, max);
            if (tri.j != min && tri.j != max) tri = Triangle(min, tri.j, max);
            if (tri.k != min && tri.k != max) tri = Triangle(min, tri.k, max);
        }

        // Second loop
        #pragma omp parallel for schedule(dynamic, 64) default(none) shared(triangle, edgeConnectivity, triangleConnectivity, triangleSize, connectivitySize)
        for (int i = 0; i < triangleSize; ++i) {
            auto& tri = triangle[i];
            edgeConnectivity[tri.i][tri.j] = false;
            edgeConnectivity[tri.i][tri.k] = false;
            edgeConnectivity[tri.j][tri.k] = false;
            triangleConnectivity[tri.i][tri.j][tri.k] = true;
        }

        // Clear existing triangles
        triangle.clear();

        // Third loop
        #pragma omp parallel default(none) shared(triangle, edgeConnectivity, triangleConnectivity, triangleSize, connectivitySize)
        {
            std::vector<Triangle> local_triangle;
            #pragma omp for nowait
            for (int i = 0; i < connectivitySize; ++i) {
                for (int j = 0; j < connectivitySize; ++j) {
                    for (int k = 0; k < connectivitySize; ++k) {
                        if (triangleConnectivity[i][j][k]) {
                            local_triangle.emplace_back(i, j, k);
                        }
                    }
                }
            }
            #pragma omp critical
            triangle.insert(triangle.end(), local_triangle.begin(), local_triangle.end());
        }
    }

    void SphereMesh::clearSphereMesh() {
        buildEdgeConnectivity();
        buildTriangleConnectivity();

        clearTriangles();
        clearEdges();
    }

    void SphereMesh::renderSphere(const Math::Vector3 &center, Math::Scalar radius, const Math::Vector3 &color) {
        ++renderCalls;
        if (renderType == RenderType::SPHERES)
            renderOneSphere(center, radius, color);
        else
            renderOneBillboardSphere(center, radius, color);
    }
	
    EdgeCollapse SphereMesh::getBestCollapseFast()
    {
        if (edgeQueue.size() < 1)
            return {};
        
        EdgeCollapse topEdge = edgeQueue.top((int)sphere.size());
        edgeQueue.extractTop();
        
        if (topEdge.idxI == -1 || topEdge.idxJ == -1)
            return {};
		
		while (!topEdge.isCheckedAgainstIncorporatedVertices)
		{
		    Sphere newSphere = Sphere();
		    newSphere.color = Math::Vector3(1, 0, 0);
		    newSphere.addQuadric(sphere[topEdge.idxI].getSphereQuadric());
		    newSphere.addQuadric(sphere[topEdge.idxJ].getSphereQuadric());
			
			auto startError = topEdge.error;
			
			// TODO: Check this problem: when I do that maybe a sphere exist in this moment, but not in the future so
			//  store in a separate map where all the old sphere collapse into, and then when I get an edge that has
			//  already been checked against the incorporated vertices, I want to recheck if the spheres of the chain
			//  still exists, if so then perfect, otherwise I want to recompute the error of the edge
			for (auto& vertex : referenceMesh->vertices)
				if (newSphere.intersectsVertex(vertex.position))
				{
					if (vertex.referenceSphere == topEdge.i.getID() || vertex.referenceSphere == topEdge.j.getID())
						continue;
					
					topEdge.addSphereCollapseToChain(sphere[sphereMapper[vertex.referenceSphere]]);
				}
			
			topEdge.isCheckedAgainstIncorporatedVertices = true;
			topEdge.updateError();
			
			if (topEdge.error == startError)
			{
				edgeQueue.increaseEdgeCollapseTimestamp(topEdge);
				return topEdge;
			}
			
			edgeQueue.push(topEdge);
			topEdge = edgeQueue.top((int)sphere.size());
			edgeQueue.extractTop();
			
			if (topEdge.idxI == -1 || topEdge.idxJ == -1)
				return {};
		}
	    
	    edgeQueue.increaseEdgeCollapseTimestamp(topEdge);
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
        while (!topEdge.isErrorCorrectionQuadricSet)
        {
            Sphere newSphere = Sphere();
            newSphere.color = Math::Vector3(1, 0, 0);
            newSphere.addQuadric(sphere[topEdge.idxI].getSphereQuadric());
            newSphere.addQuadric(sphere[topEdge.idxJ].getSphereQuadric());
	        
	        #ifdef ENABLE_REGION_BOUND
            newSphere.region = sphere[topEdge.idxI].region;
            newSphere.region.join(sphere[topEdge.idxJ].region);
			#endif
            
            auto startError = topEdge.error;
            for (auto& vertex : referenceMesh->vertices)
                if (newSphere.intersectsVertex(vertex.position))
                {
                    bool isAlreadyContained = false;
                    for (auto& ownVertex : sphere[topEdge.idxI].vertices)
                    {
                        if (ownVertex->position == vertex.position)
                        {
                            isAlreadyContained = true;
                            break;
                        }
                    }
                    
                    if (isAlreadyContained)
                        break;
                    
                    for (auto& ownVertex : sphere[topEdge.idxJ].vertices)
                        if (ownVertex->position == vertex.position)
                        {
                            isAlreadyContained = true;
                            break;
                        }
                    
                    if (isAlreadyContained)
                        break;
                    
                    topEdge.updateCorrectionErrorQuadric(Quadric::initializeQuadricFromVertex(vertex, 0.1 * BDDSize));
					topEdge.incorporatedVertices.emplace_back(&vertex);
                }
            topEdge.updateError();
            topEdge.isErrorCorrectionQuadricSet = true;
            
            if (topEdge.error == startError)
                return topEdge;
            
            edgeQueue.push(topEdge);
            topEdge = edgeQueue.top((int)sphere.size());
            edgeQueue.pop();
            
            if (topEdge.idxI == -1 || topEdge.idxJ == -1)
                return {};
        }
	    #endif
        
        return topEdge;
    }

    EdgeCollapse SphereMesh::getBestCollapseBruteForce()
    {
        if (sphere.size() <= 1)
            return {};
        
        Math::Scalar minErorr = DBL_MAX;
        EdgeCollapse bestEdge = EdgeCollapse(sphere[0], sphere[1], -1, -1);
        
        for (int i = 0; i < sphere.size(); i++)
            for (int j = i + 1; j < sphere.size(); j++)
            {
                if (i == j || (sphere[i].center - sphere[j].center).squareMagnitude() > EPSILON * EPSILON)
                    continue;
                
                EdgeCollapse candidateEdge = EdgeCollapse(sphere[i], sphere[j], i, j);
                
                if (candidateEdge.idxI > candidateEdge.idxJ)
                    candidateEdge.updateEdge(candidateEdge.j, candidateEdge.i, candidateEdge.idxJ, candidateEdge.idxI);
                
                candidateEdge.updateError();
                
                if (candidateEdge.error < minErorr)
                {
                    bestEdge = candidateEdge;
                    minErorr = candidateEdge.error;
                }
            }
        
        if (bestEdge.idxI == -1 || bestEdge.idxJ == -1)
            return {};
        
        return bestEdge;
    }

    EdgeCollapse SphereMesh::getBestCollapseInConnectivity()
    {
        if (triangle.empty() && edge.empty())
            return {};
        
        Math::Scalar minError = DBL_MAX;
        EdgeCollapse bestEdge;
        
        for (auto & i : triangle)
        {
            int v1 = i.i;
            int v2 = i.j;
            int v3 = i.k;
            
            EdgeCollapse e1 = EdgeCollapse(sphere[v1], sphere[v2], v1, v2);
            EdgeCollapse e2 = EdgeCollapse(sphere[v1], sphere[v3], v1, v3);
            EdgeCollapse e3 = EdgeCollapse(sphere[v2], sphere[v3], v2, v3);
            
            EdgeCollapse bestInTriangle;
            if (e1.error <= e2.error && e1.error <= e3.error)
                bestInTriangle = e1;
            else if (e2.error <= e1.error && e2.error <= e3.error)
                bestInTriangle = e2;
            else
                bestInTriangle = e3;
            
            if (bestInTriangle.error < minError)
            {
                bestEdge = bestInTriangle;
	            minError = bestEdge.error;
            }
        }
        
        for (auto & i : edge)
        {
            int v1 = i.i;
            int v2 = i.j;
            
            EdgeCollapse candidateEdge = EdgeCollapse(sphere[v1], sphere[v2], v1, v2);
            if (candidateEdge.error < minError)
            {
                bestEdge = candidateEdge;
	            minError = bestEdge.error;
            }
        }
        
        if (bestEdge.idxI == -1 || bestEdge.idxJ == -1)
            return {};
        
        if (bestEdge.idxI > bestEdge.idxJ)
            bestEdge.updateEdge(bestEdge.j, bestEdge.i, bestEdge.idxJ, bestEdge.idxI);
        
        return bestEdge;
    }

    void SphereMesh::computeSpheresProperties(const std::vector<Vertex>& vertices)
    {
        const Math::Scalar sigma = 1.0;
        
        for (auto & j : triangle)
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
            
            Math::Scalar totalK1 = 0.33 * vertices[i0].curvature[0] + 0.33 * vertices[i1].curvature[0] + 0.33 * vertices[i2].curvature[0];
            Math::Scalar totalK2 = 0.33 * vertices[i0].curvature[1] + 0.33 * vertices[i1].curvature[1] + 0.33  * vertices[i2].curvature[1];
            
            weight *= (1 + sigma * BDDSize * BDDSize * ((totalK1 * totalK1) + (totalK2 * totalK2)));
            
            Quadric q = Quadric(v0, normal) * weight;
            
            sphere[i0].quadric += q;
            sphere[i1].quadric += q;
            sphere[i2].quadric += q;
            
            sphere[i0].quadricWeights += weight;
            sphere[i1].quadricWeights += weight;
            sphere[i2].quadricWeights += weight;
	        
	        #ifdef ENABLE_REGION_BOUND
            sphere[i0].region.addVertex(vertices[i0].position);
            sphere[i0].region.addVertex(vertices[i1].position);
            sphere[i0].region.addVertex(vertices[i2].position);

            sphere[i1].region.addVertex(vertices[i0].position);
            sphere[i1].region.addVertex(vertices[i1].position);
            sphere[i2].region.addVertex(vertices[i2].position);

            sphere[i2].region.addVertex(vertices[i0].position);
            sphere[i2].region.addVertex(vertices[i1].position);
            sphere[i2].region.addVertex(vertices[i2].position);
	        #endif
        }
    }

    void SphereMesh::updateSpheres()
    {
        for (auto & i : sphere)
        {
            i.quadric *= (1/i.quadricWeights);

            Math::Vector4 result = i.quadric.minimizer();
            i.center = result.toQuaternion().immaginary;
            i.radius = result.coordinates.w;
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

    void SphereMesh::constructTest()
    {
        clear();

        sphere.emplace_back(Math::Vector3(), 1);
        sphere.emplace_back(Math::Vector3(3, 1.5f, 0.0f), 1.5f);
        sphere.emplace_back(Math::Vector3(3, -1.5f, 0.0f), 0.7f);
        sphere.emplace_back(Math::Vector3(6, 3, -.5f), 1);
        sphere.emplace_back(Math::Vector3(6, -3, .5f), 1.2);

        triangle.emplace_back(0, 1, 2);

        edge.emplace_back(1, 3);
        edge.emplace_back(2, 4);
    }

    void SphereMesh::clear ()
    {
        sphere.clear();
        triangle.clear();
        edge.clear();
    }

    void SphereMesh::drawSpheresOverEdge(const Edge &e, int ns, Math::Scalar rescaleRadii, Math::Scalar minRadiiScale)
    {
        int nSpheres = ns;

        const Math::Vector3 color = Math::Vector3(0.1, 0.7, 1);

        for (int i = 1; i < nSpheres - 1; i++)
            renderSphere(Math::lerp(sphere[e.i].center, sphere[e.j].center, i * 1.0 / (nSpheres - 1)),
                            Math::lerp(
                                       0.001,
                                       Math::lerp(sphere[e.i].radius, sphere[e.j].radius, i * 1.0 / (nSpheres - 1)),
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
                
                Math::Vector3 origin = Math::Vector3(sphere[e.i].center * ci + sphere[e.j].center * cj + sphere[e.k].center * ck);
                Math::Scalar radius = Math::lerp(0.001, sphere[e.i].radius * ci + sphere[e.j].radius * cj + sphere[e.k].radius * ck, size);
                
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
        for (Triangle& t : triangle)
        {
            renderOneLine(sphere[t.i].center, sphere[t.j].center, color);
            renderOneLine(sphere[t.i].center, sphere[t.k].center, color);
            renderOneLine(sphere[t.j].center, sphere[t.j].center, color);
        }
        
        for (Edge& e : edge)
            renderOneLine(sphere[e.i].center, sphere[e.j].center, color);
    }

    void SphereMesh::renderConnectivity(int spheresPerEdge, Math::Scalar sphereSize) {
        const Math::Vector3 color = Math::Vector3(1, 1, 0);
        for (Triangle& t : triangle)
        {
            renderOneLine(sphere[t.i].center, sphere[t.j].center, color, spheresPerEdge, sphereSize);
            renderOneLine(sphere[t.i].center, sphere[t.k].center, color, spheresPerEdge, sphereSize);
            renderOneLine(sphere[t.j].center, sphere[t.j].center, color, spheresPerEdge, sphereSize);
        }
        
        for (Edge& e : edge)
            renderOneLine(sphere[e.i].center, sphere[e.j].center, color, spheresPerEdge, sphereSize);
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
                        // Normalize the midpoint to create a point on the sphere
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

        for (int i = 0; i < sphere.size(); i++)
        {
            if (sphere[i].radius <= 0)
            {
                std::cout << "Sphere [" << i << "] has radius = " << sphere[i].radius << std::endl;
                continue;
            }
            
            auto r = sphere[i].radius;
            
            renderSphere(sphere[i].center, r, sphere[i].color);
        }
    }

    Math::Scalar SphereMesh::getContainedRadiusOfSphere(const Sphere& s)
    {
        auto center = s.center;
        Math::Scalar minDistance2 = DBL_MAX;
        
        for (auto & vertex : referenceMesh->vertices)
        {
            auto distance2 = (center - vertex.position).squareMagnitude();
            minDistance2 = std::min(distance2, minDistance2);
        }
        
        return std::sqrt(minDistance2);
    }

    void SphereMesh::renderSelectedSpheresOnly()
    {
        if (edgeQueue.isQueueDirty())
        {
            edgeQueue.clear();
            initializeEdgeQueue();
        }
        
        EdgeCollapse e = getBestCollapseFast();
        if (e.idxI == -1 || e.idxJ == -1)
            return;
        
        if (e.i.radius > 0)
        {
            auto r = e.i.radius;
            
            renderSphere(e.i.center, r + 0.01f, e.i.color);
        }
        
        if (e.j.radius > 0)
        {
            auto r = e.j.radius;
            
            renderSphere(e.j.center, r + 0.01f, e.j.color);
        }
    }

    void SphereMesh::renderFastSelectedSpheresOnly()
    {
        Math::Vector3 color = Math::Vector3(0, 1, 0);
        
        EdgeCollapse e = getBestCollapseInConnectivity();
        if (e.idxI == -1 || e.idxJ == -1)
            return;
        
        if (e.i.radius > 0)
        {
            auto r = e.i.radius;
            
            renderSphere(e.i.center, r + 0.01f, e.i.color);
        }
        
        if (e.j.radius > 0)
        {
            auto r = e.j.radius;
            
            renderSphere(e.j.center, r + 0.01f, e.j.color);
        }
    }

    void SphereMesh::renderSphereVertices(int i)
    {
        for (auto & idx : sphere)
	        if (idx.getID() == i)
		        for (auto &vertex: idx.vertices)
			        renderSphere(vertex->position, 0.02 * BDDSize, Math::Vector3(0, 1, 0));
    }

    Sphere SphereMesh::collapseEdgeIntoSphere(EdgeCollapse& edgeToCollapse)
    {
        Sphere& collapsedSphereA = sphere[edgeToCollapse.idxI];
        Sphere& collapsedSphereB = sphere[edgeToCollapse.idxJ];
        
        Sphere newSphere = Sphere();
        
        newSphere.color = Math::Vector3(1, 0, 0);
	    
	    #ifdef ENABLE_REGION_BOUND
        newSphere.region.join(collapsedSphereA.region);
        newSphere.region.join(collapsedSphereB.region);
	    #endif
        
        newSphere.addQuadric(collapsedSphereA.getSphereQuadric());
        newSphere.addQuadric(collapsedSphereB.getSphereQuadric());
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
		if (edgeToCollapse.isErrorCorrectionQuadricSet)
			newSphere.addQuadric(edgeToCollapse.errorCorrectionQuadric);
	    #endif
	    
	    #ifdef ENABLE_REGION_BOUND
        if (newSphere.checkSphereOverPlanarRegion())
            newSphere.approximateSphereOverPlanarRegion(collapsedSphereA.center, collapsedSphereB.center);
        
        newSphere.constrainSphere(newSphere.region.directionalWidth);
        newSphere.constrainSphere(getContainedRadiusOfSphere(newSphere));
		#endif
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
        newSphere.vertices.reserve(collapsedSphereA.vertices.size() + collapsedSphereB.vertices.size() + edgeToCollapse.incorporatedVertices.size());
		#else
		newSphere.vertices.reserve(collapsedSphereA.vertices.size() + collapsedSphereB.vertices.size());
		#endif
		
	    #ifndef DISABLE_VERTEX_INCLUSION_IN_SPHERES
        for (auto& vertex : collapsedSphereA.vertices)
            newSphere.addVertex(*vertex);

        for (auto& vertex : collapsedSphereB.vertices)
            newSphere.addVertex(*vertex);
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
		if (edgeToCollapse.isErrorCorrectionQuadricSet)
			for (auto& vertex : edgeToCollapse.incorporatedVertices)
				newSphere.addVertex(*vertex);
		#endif
		
		#endif
        
        return newSphere;
    }

    bool SphereMesh::collapseSphereMesh()
    {
        if (edgeQueue.isQueueDirty())
        {
            edgeQueue.clear();
            initializeEdgeQueue();
        }
            
        EdgeCollapse e = getBestCollapseFast();
        
        if (e.idxI == -1 || e.idxJ == -1)
            return false;
        
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();
		
        sphere.pop_back();
		
		sphereMapper.erase(e.i.getID());
		sphereMapper.erase(e.j.getID());
		sphereMapper[sphere[e.idxJ].getID()] = e.idxJ;
	    sphereMapper[newSphere.getID()] = e.idxI;
        
        updateEdgesAfterCollapse(e.idxI, e.idxJ);
	    updateTrianglesAfterCollapse(e.idxI, e.idxJ);
	    
	    int result = e.idxI;
	    for (auto& s : e.chainOfCollapse)
	    {
			if (sphereMapper.find(s->getID()) != sphereMapper.end())
			{
		        std::cout << "Chain collapsing sphere " << s->getID() << " into " << result << std::endl;
		        result = collapse(result, sphereMapper[s->getID()]);
			}
	    }

        removeDegenerates();
        updateEdgeQueue(e);
	 
#ifdef USE_THIEF_SPHERE_METHOD
		std::vector<Pair> toCollapse;
	    for (auto& s : sphere)
	    {
			auto value = s.clearNotLinkedVertices();
			if (value != -1)
				toCollapse.emplace_back(value, s.getID());
		}
		
		for (auto& c : toCollapse)
		{
			std::cout << "Collapsing parentless sphere " << c.j << " into " << c.i << std::endl;
			collapse(c.j, c.i);
		}
#endif
        
        return true;
    }

    bool SphereMesh::collapseSphereMeshFast()
    {
        EdgeCollapse e = getBestCollapseInConnectivity();
        if (e.idxI == -1 || e.idxJ == -1)
            return false;
        
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();

        sphere.pop_back();

        updateEdgesAfterCollapse(e.idxI, e.idxJ);
	    updateTrianglesAfterCollapse(e.idxI, e.idxJ);

        removeDegenerates();
        
        return true;
    }

    void SphereMesh::updateEdgesAfterCollapse(int i, int j)
    {
        int last = (int)sphere.size();
        
        for (Edge& e : edge) {
            if (e.i == j) e.i = i;
            if (e.j == j) e.j = i;
            if (e.i == last && j != last) e.i = j;
            if (e.j == last && j != last) e.j = j;
        }
    }

    void SphereMesh::updateTrianglesAfterCollapse(int i, int j)
    {
        int last = (int)sphere.size();
        
        for (Triangle& t : triangle) {
            if (t.i == j) t.i = i;
            if (t.j == j) t.j = i;
            if (t.k == j) t.k = i;
            if (t.i == last && j != last) t.i = j;
            if (t.j == last && j != last) t.j = j;
            if (t.k == last && j != last) t.k = j;
        }
    }

    bool SphereMesh::collapseSphereMesh(int n)
    {
        while (this->sphere.size() > n)
            if(!this->collapseSphereMesh())
                return false;
        
        return true;
    }

    bool SphereMesh::collapseSphereMeshFast(int n)
    {
        while (this->sphere.size() > n)
            if(!this->collapseSphereMeshFast())
                return false;
        
        return true;
    }

    int SphereMesh::collapse(int i, int j)
    {
		auto idI = i;
		auto idJ = j;
	    
#ifndef USE_NON_MAPPER
		i = sphereMapper[i];
		j = sphereMapper[j];
#endif
	    
#ifdef USE_NON_MAPPER
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == i)
            {
                i = idx;
                break;
            }
        
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == j)
            {
                j = idx;
                break;
            }
#endif
        
        if (i == j || i >= sphere.size() || j >= sphere.size())
            return -1;

        if (i > j)
            std::swap(i, j);
        
        edgeQueue.setQueueDirty();
        
        Sphere newSphere = Sphere();
        newSphere.color = Math::Vector3(1, 0, 0);
        newSphere.addQuadric(sphere[i].getSphereQuadric());
        newSphere.addQuadric(sphere[j].getSphereQuadric());
	    
	    #ifndef DISABLE_VERTEX_INCLUSION_IN_SPHERES
        for (auto& vertex : sphere[i].vertices)
            newSphere.addVertex(*vertex);
        
        for (auto& vertex : sphere[j].vertices)
            newSphere.addVertex(*vertex);
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
//	    if (edgeToCollapse.isErrorCorrectionQuadricSet)
//		    for (auto& vertex : edgeToCollapse.incorporatedVertices)
//			    newSphere.addVertex(*vertex);
		#endif
		
		#endif
	    
	    #ifdef ENABLE_REGION_BOUND
        newSphere.region = sphere[i].region;
        newSphere.region.join(sphere[j].region);
	    #endif

        sphere[i] = newSphere;
        sphere[j] = sphere.back();
        
        sphere.pop_back();
	    
#ifndef USE_NON_MAPPER
	    sphereMapper.erase(idI);
	    sphereMapper.erase(idJ);
	    sphereMapper[newSphere.getID()] = i;
#endif

        updateEdgesAfterCollapse(i, j);
	    updateTrianglesAfterCollapse(i, j);

        removeDegenerates();
        clearSphereMesh();
        
        return i;
    }

    void SphereMesh::removeDegenerates()
    {
        std::vector<int> degeneratedTriangles;
        for (int i = 0; i < triangle.size(); i++)
        {
            if (triangle[i].i == triangle[i].j || triangle[i].i == triangle[i].k || triangle[i].j == triangle[i].k)
                degeneratedTriangles.push_back(i);
        }

        int reducedSize = 0;
        for (int degeneratedTriangle : degeneratedTriangles)
        {
            Edge newEdge = Edge(0, 0);
            int index = degeneratedTriangle;

            if (triangle[index].i == triangle[index].j)
                newEdge = Edge(triangle[index].i, triangle[index].k);
            else if (triangle[index].i == triangle[index].k)
                newEdge = Edge(triangle[index].i, triangle[index].j);
            else if (triangle[index].j == triangle[index].k)
                newEdge = Edge(triangle[index].i, triangle[index].j);
            edge.push_back(newEdge);

            triangle.erase(triangle.begin() + (degeneratedTriangle - reducedSize));
            reducedSize++;
        }

        std::vector<int> degeneratedEdges;
        for (int i = 0; i < edge.size(); i++)
        {
            if (edge[i].i == edge[i].j)
                degeneratedEdges.push_back(i);
        }

        reducedSize = 0;
        for (int degeneratedEdge : degeneratedEdges)
        {
            edge.erase(edge.begin() + (degeneratedEdge - reducedSize));
            reducedSize++;
        }
    }

    Sphere SphereMesh::getSelectedVertexSphere(int i)
    {
        for (const auto & idx : sphere)
            if (idx.getID() == i)
            {
                return idx;
            }
		
	    return {};
    }

    void SphereMesh::resizeSphereVertex(int i, Math::Scalar newSize)
    {
        for (auto & idx : sphere)
            if (idx.getID() == i)
            {
                idx.radius = newSize;
                break;
            }
    }

    void SphereMesh::translateSphereVertex(int i, Math::Vector3& translation)
    {
        for (auto & idx : sphere)
            if (idx.getID() == i)
            {
                idx.center += translation;
                break;
            }
    }

    void SphereMesh::saveYAML(const std::string& path, const std::string& fn)
    {
        YAML::Emitter out;

        out << YAML::Comment("Sphere Mesh YAML @ author Davide Paolillo");
        out << YAML::BeginMap;
            out << YAML::Key << "Reference Mesh" << YAML::Value << referenceMesh->path;
            out << YAML::Key << "Start Mesh Resolution" << YAML::Value << initialSpheres.size();
            out << YAML::Key << "Sphere Mesh Resolution" << YAML::Value << sphere.size();
            out << YAML::Key << "Initial Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (auto & initialSphere : initialSpheres)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, initialSphere.center);
                        out << YAML::Key << "Radius" << YAML::Value << initialSphere.radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, initialSphere.quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, initialSphere.color);
                    out << YAML::EndMap;
                }
            out << YAML::EndSeq;
            out << YAML::Key << "Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (auto & i : sphere)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, i.center);
                        out << YAML::Key << "Radius" << YAML::Value << i.radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, i.quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, i.color);
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
        initialSpheres.clear();
        triangle.clear();
        edge.clear();
        sphere.clear();
        
        std::ifstream stream(path);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        
        for (const auto& node : data["Initial Spheres"]) {
            Sphere s;
            
            s.center = node["Center"].as<Math::Vector3>();
            s.radius = node["Radius"].as<Math::Scalar>();
            s.quadric = node["Quadric"].as<Renderer::Quadric>();
            s.color = node["Color"].as<Math::Vector3>();

            initialSpheres.push_back(s);
        }
        
        for (const auto& node : data["Spheres"]) {
            Sphere s;
            
            s.center = node["Center"].as<Math::Vector3>();
            s.radius = node["Radius"].as<Math::Scalar>();
            s.quadric = node["Quadric"].as<Renderer::Quadric>();
            s.color = node["Color"].as<Math::Vector3>();

            sphere.push_back(s);
        }
        
        for (const auto& node : data["Connectivity"]["Triangles"]) {
            Triangle t;
            
            t.i = node["T0"].as<int>();
            t.j = node["T1"].as<int>();
            t.k = node["T2"].as<int>();

            triangle.push_back(t);
        }
        
        for (const auto& node : data["Connectivity"]["Edges"]) {
            Edge e;
            
            e.i = node["E0"].as<int>();
            e.j = node["E1"].as<int>();
            
            edge.push_back(e);
        }
    }

    void SphereMesh::saveTXT(const std::string& path, const std::string& fn)
    {
        std::ostringstream fileContent;
        
        // Stating the count for each type: spheres, triangles, and edges
        fileContent << "Sphere Mesh 1.0" << std::endl;
        fileContent << sphere.size();
        fileContent << " " << triangle.size();
        fileContent << " " << edge.size() << std::endl;
        fileContent << "====================" << std::endl;
        
        // Saving sphere details
        for (const auto& s : sphere)
        {
            fileContent << s.center[0] << " " << s.center[1] << " " << s.center[2] << " " << s.radius << std::endl;
        }

        // Saving triangle details
        for (const auto& t : triangle)
        {
            fileContent << t.i << " " << t.j << " " << t.k << std::endl;
        }

        // Saving edge details
        for (const auto& e : edge)
        {
            fileContent << e.i << " " << e.j << std::endl;
        }

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

    void SphereMesh::reset()
    {
        sphere = initialSpheres;
    }

    void SphereMesh::addEdge(int selectedSphereID) {
        int selectedSphereIndex = -1;
        
        for (int i = 0; i < sphere.size(); i++)
            if (selectedSphereID == sphere[i].getID()) {
                selectedSphereIndex = i;
                break;
            }
        
        if (selectedSphereIndex == -1)
            return;
        
        auto selectedSphere = sphere[selectedSphereIndex];
        Sphere sphereCopy = Sphere(sphere[selectedSphereIndex].center, sphere[selectedSphereIndex].radius);
        sphereCopy.quadric = selectedSphere.quadric;
        sphereCopy.center += Math::Vector3(0.05, 0.05, 0) * BDDSize;
        
        sphere.push_back(sphereCopy);
        edge.emplace_back(selectedSphereIndex, (int)sphere.size() - 1);
        
        clearSphereMesh();
    }

    void SphereMesh::addTriangle(int sphereA, int sphereB) {
        int idxA = -1;
        int idxB = -1;
        
        for (int i = 0; i < sphere.size(); i++) {
            if (sphereA == sphere[i].getID())
                idxA = i;
            if (sphereB == sphere[i].getID())
                idxB = i;
        }
        
        if (idxA == -1 || idxB == -1)
            return;
        
        auto selectedA = sphere[idxA];
        auto selectedB = sphere[idxB];
        Sphere sphereCopy = Sphere(Math::lerp<Math::Vector3>(selectedA.center, selectedB.center, 0.5), Math::lerp(selectedA.radius, selectedB.radius, 0.5));
        sphereCopy.quadric = selectedA.quadric;
        sphereCopy.center += Math::Vector3(0.05, 0.05, 0) * BDDSize;
        
        sphere.push_back(sphereCopy);
        triangle.emplace_back(idxA, idxB, (int)sphere.size() - 1);
        
        clearSphereMesh();
    }

    void SphereMesh::removeSphere(int selectedSphereID) {
        int selectedSphereIndex = -1;
        
        for (int i = 0; i < sphere.size(); i++)
            if (selectedSphereID == sphere[i].getID()) {
                selectedSphereIndex = i;
                break;
            }
        
        if (selectedSphereIndex == -1)
            return;
        
        sphere[selectedSphereIndex] = sphere.back();
        sphere.pop_back();
        
        for (int i = 0; i < triangle.size();)
            if (triangle[i].i == selectedSphereIndex || triangle[i].j == selectedSphereIndex || triangle[i].k == selectedSphereIndex)
                triangle.erase(triangle.begin() + i);
            else
                ++i;
        
        for (int i = 0; i < edge.size();)
            if (edge[i].i == selectedSphereIndex || edge[i].j == selectedSphereIndex)
                edge.erase(edge.begin() + i);
            else
                ++i;
        
        for (auto & i : edge)
            if (i.i == sphere.size())
                i.i = selectedSphereIndex;
            else if (i.j == sphere.size())
                i.j = selectedSphereIndex;
        
        for (auto & i : triangle)
            if (i.i == sphere.size())
                i.i = selectedSphereIndex;
            else if (i.j == sphere.size())
                i.j = selectedSphereIndex;
            else if (i.k == sphere.size())
                i.k = selectedSphereIndex;
        
        removeDegenerates();
        clearSphereMesh();
    }
}
