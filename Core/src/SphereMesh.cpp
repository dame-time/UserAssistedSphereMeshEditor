#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include "../SphereMesh.hpp"

#include <Math.hpp>

#include <YAMLUtils.hpp>
#include <ScopeTimer.hpp>

#include <omp.h>

#include <filesystem>
#include <algorithm>
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
        initializeEPSILON();
	    
	    timedSpheres = sm.timedSpheres;
        initialSpheres = sm.initialSpheres;
		
        triangle = sm.triangle;
        edge = sm.edge;
        
        sphereShader = sm.sphereShader;
        renderType = RenderType::BILLBOARDS;
        
        clearSphereMesh();
        initializeEdgeQueue();
    }

    SphereMesh::SphereMesh(RenderableMesh* mesh, Shader* shader, Math::Scalar vertexSphereRadius) : referenceMesh(mesh)
    {
        this->sphereShader = shader;
        renderType = RenderType::BILLBOARDS;
        
        BDDSize = mesh->bbox.BDD().magnitude();
        initializeEPSILON();

        initializeSphereMeshTriangles(mesh->faces);
        initializeSpheres(mesh->vertices, 0.01f * BDDSize);
        
        computeSpheresProperties(mesh->vertices);
        updateSpheres();

		// TODO: update this so also the connectivity reset after a rest, right now only the array is resetted, but
		//  not the connectivity
        initialSpheres.reserve(timedSpheres.size());
		for (auto& s : timedSpheres)
			initialSpheres.emplace_back(s.sphere);
        
        clearSphereMesh();
        initializeEdgeQueue();
    }

	// FIXME: This function is useless, I need to find a way to avoid this EPSILON
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
		
		edgeQueue.setQueueDirty();
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
		initialSpheres = sm.initialSpheres;
		triangle = sm.triangle;
		edge = sm.edge;
		edgeConnectivity = sm.edgeConnectivity;
		triangleConnectivity = sm.triangleConnectivity;
		triangleEdgeConnectivity = sm.triangleEdgeConnectivity;
		EPSILON = sm.EPSILON;
		
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
        timedSpheres.reserve(vertices.size());
	    
	    for (auto& vertex : vertices)
	    {
			auto newSphere = Sphere(vertex, initialRadius);
			
		    timedSpheres.emplace_back(newSphere);
			
			sphereMapper[newSphere.getID()] = static_cast<int>(timedSpheres.size()) - 1;
		}
		
		timedSphereSize = static_cast<int>(timedSpheres.size());
    }

    void SphereMesh::initializeEdgeQueue()
    {
        int sphereSize = (int)timedSpheres.size();
	    edgeQueue = TemporalValidityQueue(timedSpheres);
		
        std::queue<EdgeCollapse> localQueues[omp_get_max_threads()];

        #pragma omp parallel default(none) shared(timedSpheres, sphereSize, localQueues, edgeQueue)
        {
            int tid = omp_get_thread_num();

            #pragma omp for schedule(dynamic, 64)
            for (int i = 0; i < sphereSize; ++i) {
                for (int j = i + 1; j < sphereSize; ++j) {
					// FIXME: In here I don't want to exclude the collapses of the spheres that are linked by an edge
					//  or by a triangle
                    if ((timedSpheres[i].sphere.center - timedSpheres[j].sphere.center).squareMagnitude() <= EPSILON * EPSILON) {
                        EdgeCollapse e(timedSpheres[i], timedSpheres[j], i, j);
                        e.updateError();

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
        const auto size = timedSpheres.size();

        auto updateEdges = [&](int i, int edgeIdx, int queueIdOffset) {
			auto minIndex = std::min(i, edgeIdx);
			auto maxIndex = std::max(i, edgeIdx);
			
			auto checkIndexNotTheSame = i != edgeIdx;
			auto checkBothSpheresAreActive = timedSpheres[i].isActive && timedSpheres[edgeIdx].isActive;
			auto checkInRange = (timedSpheres[i].sphere.center - timedSpheres[edgeIdx].sphere.center).squareMagnitude()
			                            <= epsilonSquared;
			auto checkEdgesConnected = edgeConnectivity[minIndex][maxIndex]
					|| triangleEdgeConnectivity[minIndex][maxIndex] || triangleEdgeConnectivity[maxIndex][minIndex];
			
			auto checkInRangeOrConnected = checkInRange || checkEdgesConnected;
			
            if (checkIndexNotTheSame && checkInRangeOrConnected && checkBothSpheresAreActive) {
                EdgeCollapse newEdge = EdgeCollapse(timedSpheres[i], timedSpheres[edgeIdx], i, edgeIdx);
                
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
        size_t size = timedSpheres.size();
        edgeConnectivity.clear();
        edgeConnectivity.reserve(size);
		
		triangleEdgeConnectivity.clear();
		triangleEdgeConnectivity.reserve(size);

        std::vector<bool> row(size, false);

        for (size_t i = 0; i < size; ++i) {
            edgeConnectivity.push_back(row);
			triangleEdgeConnectivity.push_back(row);
        }
    }

    void SphereMesh::buildTriangleConnectivity() {
        size_t size = timedSpheres.size();
        triangleConnectivity.clear();
        triangleConnectivity.reserve(size);

        std::vector<bool> innerRow(size, false);
        std::vector<std::vector<bool>> innerMatrix(size, innerRow);

        for (size_t i = 0; i < size; ++i)
            triangleConnectivity.push_back(innerMatrix);
    }

    void SphereMesh::clearEdges() {
        // Here  I initialize the edgeConnectivity matrix to my edges, by considering the first index as the lower,
		// and the second index as the higher, this is just a convention
        for (auto & i : edge) {
            auto minIndex = std::min(i.i, i.j);
			auto maxIndex = std::max(i.i, i.j);
			
            edgeConnectivity[minIndex][maxIndex] = true;
        }

		// TODO: Check if this code is useful, I think it is not
		// Here I rebuild the edge data structure, by basically setting all the edges, so they have the minor index
		// as the first and the higher as the right
        edge.clear();
		
        size_t eSize = edgeConnectivity.size();
        for (size_t i = 0; i < eSize; ++i)
            for (size_t j = 0; j < edgeConnectivity[i].size(); ++j)
                if (edgeConnectivity[i][j])
                    edge.insert(Edge(static_cast<int>(i), static_cast<int>(j)));
    }

    void SphereMesh::clearTriangles() {
	    auto triangleSize = (int)triangle.size();
        auto connectivitySize = (int)triangleConnectivity.size();
		triangle.end();
		
		// Here I just reorder the indices of the triangles in ascending order, just a convention of the triangle
		// connectivity matrix that I use
		std::vector<Triangle> trianglesToRemove;
		for (auto tri : triangle)
		{
			int min = std::min({tri.i, tri.j, tri.k});
			int max = std::max({tri.i, tri.j, tri.k});
			int mid = tri.i + tri.j + tri.k - min - max;
			
			edgeConnectivity[min][mid] = false;
			edgeConnectivity[min][max] = false;
			edgeConnectivity[mid][max] = false;
			
			triangleEdgeConnectivity[min][mid] = true;
			triangleEdgeConnectivity[min][max] = true;
			triangleEdgeConnectivity[mid][max] = true;
			
			triangleConnectivity[min][mid][max] = true;
			
			if (tri.i != min || tri.j != mid || tri.k != max)
				trianglesToRemove.emplace_back(min, mid, max);
		}
		
		for (auto tri : trianglesToRemove)
		{
			triangle.erase(tri);
			
			auto min = std::min({tri.i, tri.j, tri.k});
			auto max = std::max({tri.i, tri.j, tri.k});
			auto mid = tri.i + tri.j + tri.k - min - max;
			
			auto newTri = Triangle(min, mid, max);
			triangle.insert(newTri);
		}
		
        triangle.clear();
		
        #pragma omp parallel default(none) shared(triangle, edgeConnectivity, triangleConnectivity, triangleSize, connectivitySize)
        {
            std::vector<Triangle> local_triangle;
            #pragma omp for nowait
            for (int i = 0; i < connectivitySize; ++i)
                for (int j = 0; j < connectivitySize; ++j)
                    for (int k = 0; k < connectivitySize; ++k)
                        if (triangleConnectivity[i][j][k])
                            local_triangle.emplace_back(i, j, k);

            #pragma omp critical
			triangle.insert(local_triangle.begin(), local_triangle.end());
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
        
        EdgeCollapse topEdge = edgeQueue.top();
		edgeQueue.pop();
        
        if (topEdge.idxI == -1 || topEdge.idxJ == -1)
            return {};
		
		while (!topEdge.isCheckedAgainstIncorporatedVertices)
		{
		    Sphere newSphere = Sphere();
		    newSphere.color = Math::Vector3(1, 0, 0);
		    newSphere.addQuadric(timedSpheres[topEdge.idxI].sphere.getSphereQuadric());
		    newSphere.addQuadric(timedSpheres[topEdge.idxJ].sphere.getSphereQuadric());
			
			auto startError = topEdge.error;
			
			for (auto& vertex : referenceMesh->vertices)
				if (newSphere.intersectsVertex(vertex.position))
				{
					if (vertex.referenceSphere == topEdge.i.sphere.getID()
					|| vertex.referenceSphere == topEdge.j.sphere.getID())
						continue;
					
					topEdge.addSphereCollapseToChain(timedSpheres[sphereMapper[vertex.referenceSphere]]);
				}
			
			topEdge.isCheckedAgainstIncorporatedVertices = true;
			topEdge.updateError();
			
			if (topEdge.error == startError)
			{
				return topEdge;
			}
			
			edgeQueue.push(topEdge);
			topEdge = edgeQueue.top();
			edgeQueue.pop();
			
			if (topEdge.isCheckedAgainstIncorporatedVertices)
				for (auto& chainCollapseSphere: topEdge.chainOfCollapse)
					if (!chainCollapseSphere.second->isActive)
						topEdge.isCheckedAgainstIncorporatedVertices = false;
			
			if (topEdge.idxI == -1 || topEdge.idxJ == -1)
				return {};
		}
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
        while (!topEdge.isErrorCorrectionQuadricSet)
        {
            Sphere newSphere = Sphere();
            newSphere.color = Math::Vector3(1, 0, 0);
            newSphere.addQuadric(timedSpheres[topEdge.idxI].getSphereQuadric());
            newSphere.addQuadric(timedSpheres[topEdge.idxJ].getSphereQuadric());
	        
	        #ifdef ENABLE_REGION_BOUND
            newSphere.region = timedSpheres[topEdge.idxI].region;
            newSphere.region.join(timedSpheres[topEdge.idxJ].region);
			#endif
            
            auto startError = topEdge.error;
            for (auto& vertex : referenceMesh->vertices)
                if (newSphere.intersectsVertex(vertex.position))
                {
                    bool isAlreadyContained = false;
                    for (auto& ownVertex : timedSpheres[topEdge.idxI].vertices)
                    {
                        if (ownVertex->position == vertex.position)
                        {
                            isAlreadyContained = true;
                            break;
                        }
                    }
                    
                    if (isAlreadyContained)
                        break;
                    
                    for (auto& ownVertex : timedSpheres[topEdge.idxJ].vertices)
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
            topEdge = edgeQueue.top((int)timedSpheres.size());
            edgeQueue.pop();
            
            if (topEdge.idxI == -1 || topEdge.idxJ == -1)
                return {};
        }
	    #endif
        
        return topEdge;
    }

    EdgeCollapse SphereMesh::getBestCollapseBruteForce()
    {
        if (timedSpheres.size() <= 1)
            return {};
        
        Math::Scalar minErorr = DBL_MAX;
        EdgeCollapse bestEdge = EdgeCollapse(timedSpheres[0], timedSpheres[1], -1, -1);
        
        for (int i = 0; i < timedSpheres.size(); i++)
            for (int j = i + 1; j < timedSpheres.size(); j++)
            {
                if (i == j || (timedSpheres[i].sphere.center - timedSpheres[j].sphere.center).squareMagnitude() >
		                              (EPSILON * EPSILON) || !timedSpheres[i].isActive || !timedSpheres[j].isActive)
                    continue;
                
                EdgeCollapse candidateEdge = EdgeCollapse(timedSpheres[i], timedSpheres[j], i, j);
	            
	            Sphere newSphere = Sphere();
	            newSphere.color = Math::Vector3(1, 0, 0);
	            newSphere.addQuadric(candidateEdge.i.sphere.getSphereQuadric());
	            newSphere.addQuadric(candidateEdge.j.sphere.getSphereQuadric());
	            
	            for (auto& vertex : referenceMesh->vertices)
		            if (newSphere.intersectsVertex(vertex.position))
		            {
			            if (vertex.referenceSphere == candidateEdge.i.sphere.getID()
			                || vertex.referenceSphere == candidateEdge.j.sphere.getID())
				            continue;
			            
			            candidateEdge.addSphereCollapseToChain(timedSpheres[sphereMapper[vertex.referenceSphere]]);
		            }
                
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
            
            EdgeCollapse e1 = EdgeCollapse(timedSpheres[v1], timedSpheres[v2], v1, v2);
            EdgeCollapse e2 = EdgeCollapse(timedSpheres[v1], timedSpheres[v3], v1, v3);
            EdgeCollapse e3 = EdgeCollapse(timedSpheres[v2], timedSpheres[v3], v2, v3);
            
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
            
            EdgeCollapse candidateEdge = EdgeCollapse(timedSpheres[v1], timedSpheres[v2], v1, v2);
            if (candidateEdge.error < minError)
            {
                bestEdge = candidateEdge;
	            minError = bestEdge.error;
            }
        }
        
        if (bestEdge.idxI == -1 || bestEdge.idxJ == -1)
            return {};
        
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
	        
	        timedSpheres[i0].sphere.quadric += q;
	        timedSpheres[i1].sphere.quadric += q;
	        timedSpheres[i2].sphere.quadric += q;
	        
	        timedSpheres[i0].sphere.quadricWeights += weight;
	        timedSpheres[i1].sphere.quadricWeights += weight;
	        timedSpheres[i2].sphere.quadricWeights += weight;
	        
	        #ifdef ENABLE_REGION_BOUND
            timedSpheres[i0].region.addVertex(vertices[i0].position);
            timedSpheres[i0].region.addVertex(vertices[i1].position);
            timedSpheres[i0].region.addVertex(vertices[i2].position);

            timedSpheres[i1].region.addVertex(vertices[i0].position);
            timedSpheres[i1].region.addVertex(vertices[i1].position);
            timedSpheres[i2].region.addVertex(vertices[i2].position);

            timedSpheres[i2].region.addVertex(vertices[i0].position);
            timedSpheres[i2].region.addVertex(vertices[i1].position);
            timedSpheres[i2].region.addVertex(vertices[i2].position);
	        #endif
        }
    }

    void SphereMesh::updateSpheres()
    {
        for (auto & i : timedSpheres)
        {
            i.sphere.quadric *= (1/i.sphere.quadricWeights);

            Math::Vector4 result = i.sphere.quadric.minimizer();
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
            if (timedSpheres[i].sphere.radius <= 0)
            {
                std::cout << "Sphere [" << i << "] has radius = " << timedSpheres[i].sphere.radius << std::endl;
                continue;
            }
            
            auto r = timedSpheres[i].sphere.radius;
            
			if (timedSpheres[i].isActive)
                renderSphere(timedSpheres[i].sphere.center, r, timedSpheres[i].sphere.color);
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
		
		if (!timedSpheres[e.idxI].isActive || !timedSpheres[e.idxJ].isActive)
			return;
        
        if (e.i.sphere.radius > 0)
        {
            auto r = e.i.sphere.radius;
            
            renderSphere(e.i.sphere.center, r + 0.01f, e.i.sphere.color);
        }
        
        if (e.j.sphere.radius > 0)
        {
            auto r = e.j.sphere.radius;
            
            renderSphere(e.j.sphere.center, r + 0.01f, e.j.sphere.color);
        }
    }

    void SphereMesh::renderFastSelectedSpheresOnly()
    {
        Math::Vector3 color = Math::Vector3(0, 1, 0);
        
        EdgeCollapse e = getBestCollapseInConnectivity();
        if (e.idxI == -1 || e.idxJ == -1)
            return;
	    
	    if (!timedSpheres[e.idxI].isActive || !timedSpheres[e.idxJ].isActive)
		    return;
        
        if (e.i.sphere.radius > 0)
        {
            auto r = e.i.sphere.radius;
            
            renderSphere(e.i.sphere.center, r + 0.01f, e.i.sphere.color);
        }
        
        if (e.j.sphere.radius > 0)
        {
            auto r = e.j.sphere.radius;
            
            renderSphere(e.j.sphere.center, r + 0.01f, e.j.sphere.color);
        }
    }

    void SphereMesh::renderSphereVertices(int i)
    {
        for (auto & idx : timedSpheres)
	        if (idx.sphere.getID() == i && idx.isActive)
		        for (auto &vertex: idx.sphere.vertices)
			        renderSphere(vertex->position, 0.02 * BDDSize, Math::Vector3(0, 1, 0));
    }

    Sphere SphereMesh::collapseEdgeIntoSphere(EdgeCollapse& edgeToCollapse)
    {
        TimedSphere& collapsedSphereA = timedSpheres[edgeToCollapse.idxI];
	    TimedSphere& collapsedSphereB = timedSpheres[edgeToCollapse.idxJ];
        
        Sphere newSphere = Sphere();
        
        newSphere.color = Math::Vector3(1, 0, 0);
	    
	    #ifdef ENABLE_REGION_BOUND
        newSphere.region.join(collapsedSphereA.region);
        newSphere.region.join(collapsedSphereB.region);
	    #endif
        
        newSphere.addQuadric(collapsedSphereA.sphere.getSphereQuadric());
        newSphere.addQuadric(collapsedSphereB.sphere.getSphereQuadric());
	    
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
		newSphere.vertices.reserve(collapsedSphereA.sphere.vertices.size() + collapsedSphereB.sphere.vertices.size());
		#endif
		
	    #ifndef DISABLE_VERTEX_INCLUSION_IN_SPHERES
        for (auto& vertex : collapsedSphereA.sphere.vertices)
            newSphere.addVertex(*vertex);

        for (auto& vertex : collapsedSphereB.sphere.vertices)
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
	    
#ifdef TEST_COLLAPSING_QUEUE
		EdgeCollapse e1 = getBestCollapseBruteForce();
		
		if (e.error != e1.error)
		{
			e.updateError();
			e1.updateError();

			std::cout << "Best collapse fast: (" << e.idxI << ", " << e.idxJ << "); with error: " << e.error << std::endl;
			std::cout << "Best collapse brute force: (" << e1.idxI << ", " << e1.idxJ << "); with error: " << e1.error <<
			std::endl;

			std::cout << "Spheres fast: (" << timedSpheres[e.idxI].creationTime << ", " << timedSpheres[e.idxJ].creationTime
			<< ")" << std::endl;
			std::cout << "Spheres brute: (" << timedSpheres[e1.idxI].creationTime << ", " << timedSpheres[e1.idxJ]
			.creationTime << ")" << std::endl;

			std::cout << "Real fast quadric error: " << (timedSpheres[e.idxI].sphere.getSphereQuadric() +
			timedSpheres[e.idxJ].sphere.getSphereQuadric()).minimum() << std::endl;
		}
#endif

        if (e.idxI == -1 || e.idxJ == -1)
            return false;
        
        Sphere newSphere = collapseEdgeIntoSphere(e);
	    
	    timedSpheres[e.idxI] = TimedSphere(newSphere);
	    timedSpheres[e.idxJ].isActive = false;
		
		sphereMapper.erase(e.i.sphere.getID());
		sphereMapper.erase(e.j.sphere.getID());
	    sphereMapper[newSphere.getID()] = e.idxI;
		
		timedSphereSize--;
  
		// TODO: The edges are updated wrongly, I should call clear mesh at each collapse, but how to do it? (without
		//  compromising performances)
        updateEdgesAfterCollapse(e.idxI, e.idxJ);
	    updateTrianglesAfterCollapse(e.idxI, e.idxJ);
	    
	    int result = e.idxI;
	    for (auto& s : e.chainOfCollapse)
	    {
			if (s.second->isActive)
			{
		        std::cout << "Chain collapsing timedSpheres " << s.second->sphere.getID() << " into " << result <<
				std::endl;
		        result = collapse(result, sphereMapper[s.second->sphere.getID()]);
			}
			else
			{
				std::cout << "Not chain collapsing timedSpheres " << s.second->sphere.getID() << " cos is not active" <<
				std::endl;
			}
	    }
		
        removeDegenerates();
        updateEdgeQueue(e);
	 
#ifdef USE_THIEF_SPHERE_METHOD
		std::vector<Pair> toCollapse;
	    for (auto& s : timedSpheres)
	    {
			auto value = s.clearNotLinkedVertices();
			if (value != -1)
				toCollapse.emplace_back(value, s.getID());
		}
		
		for (auto& c : toCollapse)
		{
			std::cout << "Collapsing parentless timedSpheres " << c.j << " into " << c.i << std::endl;
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
	    
	    timedSpheres[e.idxI] = TimedSphere(newSphere);
	    timedSpheres[e.idxJ].isActive = false;

        updateEdgesAfterCollapse(e.idxI, e.idxJ);
	    updateTrianglesAfterCollapse(e.idxI, e.idxJ);

        removeDegenerates();
        
        return true;
    }

    void SphereMesh::updateEdgesAfterCollapse(int i, int j)
    {
		// Here I just set in all the edges with the dead spheres the index of the newly generated sphere
		std::vector<Edge> toUpdate;
		std::vector<Edge> toRemove;
        for (const auto& e : edge) {
			if (e.i == j || e.j == j)
			{
				auto minIndex = std::min({e.i, e.j});
				auto maxIndex = std::max({e.i, e.j});
				
				edgeConnectivity[minIndex][maxIndex] = false;
			}
			
			if (e.i == j)
			{
				if (e.j != i)
					toUpdate.emplace_back(i, e.j);

				toRemove.emplace_back(e);
			}
			
			if (e.j == j)
			{
				if (e.i != i)
					toUpdate.emplace_back(e.i, i);
				
				toRemove.emplace_back(e);
			}
        }
		
		for (auto& e : toRemove)
			edge.erase(e);
		
		for (auto& e : toUpdate)
			edge.insert(e);
    }

    void SphereMesh::updateTrianglesAfterCollapse(int i, int j)
    {
	    // Here I just set in all the triangles with the dead spheres the index of the newly generated sphere
	    std::vector<Triangle> toUpdate;
	    std::vector<Triangle> toRemove;
        for (const auto& t : triangle) {
			if (t.i == j || t.j == j || t.k == j)
			{
				auto minIndex = std::min({t.i, t.j, t.k});
				auto maxIndex = std::max({t.i, t.j, t.k});
				auto midIndex = t.i + t.j + t.k - minIndex - maxIndex;
				
				triangleConnectivity[minIndex][midIndex][maxIndex] = false;
				triangleEdgeConnectivity[minIndex][midIndex] = false;
				triangleEdgeConnectivity[minIndex][maxIndex] = false;
				triangleEdgeConnectivity[midIndex][maxIndex] = false;
			}
			
			if (t.i == j)
			{
				if (t.j != i && t.k != i)
					toUpdate.emplace_back(i, t.j, t.k);
				else
				{
					auto minIndex = std::min({i, t.j, t.k});
					auto maxIndex = std::max({i, t.j, t.k});
					
					edgeConnectivity[minIndex][maxIndex] = true;
					edge.insert(Edge(minIndex, maxIndex));
				}
				
				toRemove.emplace_back(t);
			}
			
			if (t.j == j)
			{
				if (t.i != i && t.k != i)
					toUpdate.emplace_back(t.i, i, t.k);
				else
				{
					auto minIndex = std::min({t.i, i, t.k});
					auto maxIndex = std::max({t.i, i, t.k});
					
					edgeConnectivity[minIndex][maxIndex] = true;
					edge.insert(Edge(minIndex, maxIndex));
				}
				
				toRemove.emplace_back(t);
			}
			
			if (t.k == j)
			{
				if (t.i != i && t.j != i)
					toUpdate.emplace_back(t.i, t.j, i);
				else
				{
					auto minIndex = std::min({t.i, t.j, i});
					auto maxIndex = std::max({t.i, t.j, i});
					
					edgeConnectivity[minIndex][maxIndex] = true;
					edge.insert(Edge(minIndex, maxIndex));
				}
				
				toRemove.emplace_back(t);
			}
        }
		
		for (auto& t : toRemove)
			triangle.erase(t);
		
		for (auto& t : toUpdate)
			triangle.insert(t);
    }

    bool SphereMesh::collapseSphereMesh(int n)
    {
        while (timedSphereSize > n)
            if(!this->collapseSphereMesh())
                return false;
        
        return true;
    }

    bool SphereMesh::collapseSphereMeshFast(int n)
    {
        while (timedSphereSize > n)
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
        for (int idx = 0; idx < timedSpheres.size(); idx++)
            if (timedSpheres[idx].getID() == i)
            {
                i = idx;
                break;
            }
        
        for (int idx = 0; idx < timedSpheres.size(); idx++)
            if (timedSpheres[idx].getID() == j)
            {
                j = idx;
                break;
            }
#endif
        
        if (i == j || i >= timedSpheres.size() || j >= timedSpheres.size())
            return -1;

        if (i > j)
            std::swap(i, j);
        
        edgeQueue.setQueueDirty();
        
        Sphere newSphere = Sphere();
        newSphere.color = Math::Vector3(1, 0, 0);
        newSphere.addQuadric(timedSpheres[i].sphere.getSphereQuadric());
        newSphere.addQuadric(timedSpheres[j].sphere.getSphereQuadric());
	    
	    #ifndef DISABLE_VERTEX_INCLUSION_IN_SPHERES
        for (auto& vertex : timedSpheres[i].sphere.vertices)
            newSphere.addVertex(*vertex);
        
        for (auto& vertex : timedSpheres[j].sphere.vertices)
            newSphere.addVertex(*vertex);
	    
	    #ifdef USE_THIEF_SPHERE_METHOD
//	    if (edgeToCollapse.isErrorCorrectionQuadricSet)
//		    for (auto& vertex : edgeToCollapse.incorporatedVertices)
//			    newSphere.addVertex(*vertex);
		#endif
		
		#endif
	    
	    #ifdef ENABLE_REGION_BOUND
        newSphere.region = timedSpheres[i].region;
        newSphere.region.join(timedSpheres[j].region);
	    #endif
	    
	    timedSpheres[i] = TimedSphere(newSphere);
	    timedSpheres[j].isActive = false;
		
		timedSphereSize--;
	    
#ifndef USE_NON_MAPPER
	    sphereMapper.erase(idI);
	    sphereMapper.erase(idJ);
	    sphereMapper[newSphere.getID()] = i;
#endif

        updateEdgesAfterCollapse(i, j);
	    updateTrianglesAfterCollapse(i, j);

        removeDegenerates();
//        clearSphereMesh(); TODO: What is this for? Am I that dumb?
        
        return i;
    }

	// TODO: Remove this
    void SphereMesh::removeDegenerates()
    {
//        std::vector<int> degeneratedTriangles;
//		std::vector<Triangle> trianglesToAdd;
//		// Checking if a triangle is degenerated into an edge, or it's a duplicate
//        for (int i = 0; i < triangle.size(); i++)
//        {
//			auto minIndex = std::min({triangle[i].i, triangle[i].j, triangle[i].k});
//			auto maxIndex = std::max({triangle[i].i, triangle[i].j, triangle[i].k});
//			auto midIndex = triangle[i].i + triangle[i].j + triangle[i].k - minIndex - maxIndex;
//
//	        if (triangle[i].i == triangle[i].j || triangle[i].i == triangle[i].k || triangle[i].j == triangle[i].k)
//                degeneratedTriangles.emplace_back(i);
//			else if (minIndex != triangle[i].i || midIndex != triangle[i].j || maxIndex != triangle[i].k)
//	        {
//				trianglesToAdd.emplace_back(minIndex, midIndex, maxIndex);
//				degeneratedTriangles.emplace_back(i);
//			}
//		}
//
//        int reducedSize = 0;
//        for (int degeneratedTriangle : degeneratedTriangles)
//        {
//            Edge newEdge = Edge(0, 0);
//
//            int index = degeneratedTriangle;
//	        int i = triangle[degeneratedTriangle].i;
//	        int j = triangle[degeneratedTriangle].j;
//	        int k = triangle[degeneratedTriangle].k;
//
//			newEdge = i == j ? Edge(i, k) : Edge(i, j);
//			newEdge = newEdge.i > newEdge.j ? Edge(newEdge.j, newEdge.i) : newEdge;
//
//            edge.emplace_back(newEdge);
//
//			auto minIndex = std::min({i, j, k});
//			auto maxIndex = std::max({i, j, k});
//
//			edgeConnectivity[minIndex][maxIndex] = true;
//
//            triangle.erase(triangle.begin() + (degeneratedTriangle - reducedSize));
//            reducedSize++;
//        }
//
//		for (auto& t : trianglesToAdd)
//			triangle.emplace_back(t);
//
//        std::vector<int> degeneratedEdges;
//		std::vector<Edge> edgesToAdd;
//		// Checking if an edge is degenerated into a point, or it's a duplicate
//        for (int i = 0; i < edge.size(); i++)
//        {
//			auto minIndex = std::min({edge[i].i, edge[i].j});
//			auto maxIndex = std::max({edge[i].i, edge[i].j});
//
//	        if (edge[i].i == edge[i].j)
//		        degeneratedEdges.emplace_back(i);
//			else if (edge[i].i != minIndex || edge[i].j != maxIndex)
//	        {
//				edgesToAdd.emplace_back(minIndex, maxIndex);
//				degeneratedEdges.emplace_back(i);
//			}
//		}
//
//        reducedSize = 0;
//        for (int degeneratedEdge : degeneratedEdges)
//        {
//            edge.erase(edge.begin() + (degeneratedEdge - reducedSize));
//            reducedSize++;
//        }
//
//		for (auto& e : edgesToAdd)
//			edge.emplace_back(e);
    }

    void SphereMesh::saveYAML(const std::string& path, const std::string& fn)
    {
        YAML::Emitter out;

        out << YAML::Comment("Sphere Mesh YAML @ author Davide Paolillo");
        out << YAML::BeginMap;
            out << YAML::Key << "Reference Mesh" << YAML::Value << referenceMesh->path;
            out << YAML::Key << "Start Mesh Resolution" << YAML::Value << initialSpheres.size();
            out << YAML::Key << "Sphere Mesh Resolution" << YAML::Value << timedSphereSize;
            out << YAML::Key << "Initial Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (auto & initialSphere : initialSpheres)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, initialSphere.sphere.center);
                        out << YAML::Key << "Radius" << YAML::Value << initialSphere.sphere.radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, initialSphere.sphere.quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, initialSphere.sphere.color);
                    out << YAML::EndMap;
                }
            out << YAML::EndSeq;
            out << YAML::Key << "Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (auto & i : timedSpheres)
                {
					if (!i.isActive)
						continue;
					
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, i.sphere.center);
                        out << YAML::Key << "Radius" << YAML::Value << i.sphere.radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, i.sphere.quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, i.sphere.color);
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
        timedSpheres.clear();
        
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

            initialSpheres.emplace_back(s);
        }
        
        for (const auto& node : data["Spheres"]) {
            Sphere s;
            
            s.center = node["Center"].as<Math::Vector3>();
            s.radius = node["Radius"].as<Math::Scalar>();
            s.quadric = node["Quadric"].as<Renderer::Quadric>();
            s.color = node["Color"].as<Math::Vector3>();

            timedSpheres.emplace_back(Sphere(s));
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

    void SphereMesh::saveTXT(const std::string& path, const std::string& fn)
    {
        std::ostringstream fileContent;
        
        // Stating the count for each type: spheres, triangles, and edges
        fileContent << "Sphere Mesh 1.0" << std::endl;
        fileContent << timedSphereSize;
        fileContent << " " << triangle.size();
        fileContent << " " << edge.size() << std::endl;
        fileContent << "====================" << std::endl;
		
		std::unordered_map<int, int> activeSpheres;
		std::vector<TimedSphere> activeEdges;
		std::vector<TimedSphere> activeTris;
		int idx = 0;
		for (const auto& s : timedSpheres)
			if (s.isActive)
				activeSpheres[s.sphere.getID()] = idx++;
        
        // Saving timedSpheres details
        for (const auto& s : timedSpheres)
			if (s.isActive)
                fileContent << s.sphere.center[0] << " " << s.sphere.center[1] << " "
					<< s.sphere.center[2] << " " << s.sphere.radius << std::endl;

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

    void SphereMesh::reset()
    {
	    timedSpheres = initialSpheres;
    }

    void SphereMesh::addEdge(int selectedSphereID) {
        int selectedSphereIndex = sphereMapper[selectedSphereID];
        auto selectedSphere = timedSpheres[selectedSphereIndex];
        Sphere sphereCopy = Sphere(timedSpheres[selectedSphereIndex].sphere.center, timedSpheres[selectedSphereIndex].sphere.radius);
        sphereCopy.quadric = selectedSphere.sphere.quadric;
        sphereCopy.center += Math::Vector3(0.05, 0.05, 0) * BDDSize;
        
        timedSpheres.emplace_back(Sphere(sphereCopy));
        edge.insert(Edge(selectedSphereIndex, (int)timedSpheres.size() - 1));
        
        clearSphereMesh();
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
        
        timedSpheres.emplace_back(Sphere(sphereCopy));
        triangle.insert(Triangle(idxA, idxB, (int)timedSpheres.size() - 1));
        
        clearSphereMesh();
    }

    void SphereMesh::removeSphere(int selectedSphereID) {
        int selectedSphereIndex = sphereMapper[selectedSphereID];
	    
	    timedSpheres[selectedSphereIndex].isActive = false;
		
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
        
        removeDegenerates();
        clearSphereMesh();
    }
	
	int SphereMesh::getTimedSphereSize () const
	{
		return timedSphereSize;
	}
}
