#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include "../SphereMesh.hpp"

#include <Math.hpp>

#include <YAMLUtils.hpp>

#include <filesystem>
#include <algorithm>
#include <cassert>
#include <cmath>

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
        auto vertices = mesh->vertices;
        auto faces = mesh->faces;
        
        this->sphereShader = shader;
        renderType = RenderType::SPHERES;
        
        BDDSize = mesh->bbox.BDD().magnitude();
        initializeEPSILON();

        initializeSphereMeshTriangles(faces);
        initializeSpheres(vertices, 0.01f * BDDSize);
        
        computeSpheresProperties(vertices);
        updateSpheres();

        initialSpheres = sphere;
        
        clearSphereMesh();
        initializeEdgeQueue();
    }

    void SphereMesh::initializeEPSILON()
    {
//       EPSILON = 0.0275 * BDDSize;
//        EPSILON = 0.04 * BDDSize; // CAMEL & FOOT
        EPSILON = 0.095 * BDDSize; // BUNNY
//        EPSILON = 0.035 * BDDSize; // HAND
    }

    void SphereMesh::setEpsilon(const Math::Scalar& e)
    {
        auto newEPSILON = Math::Math::clamp01(e);
        EPSILON = newEPSILON * BDDSize;
    }

    int SphereMesh::getPerSphereVertexCount() {
        return perSphereVertices;
    }

    int SphereMesh::getRenderCalls() {
        return renderCalls;
    }

    void SphereMesh::resetRenderCalls() {
        renderCalls = 0;
    }

    int SphereMesh::getTriangleSize() {
        return triangle.size();
    }

    int SphereMesh::getEdgeSize() {
        return edge.size();
    }

    SphereMesh& SphereMesh::operator = (const SphereMesh& sm)
    {
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
        for (int i = 0; i < faces.size(); i++)
            triangle.push_back(Triangle(faces[i].i, faces[i].j, faces[i].k));
    }

    void SphereMesh::initializeSpheres(const std::vector<Vertex>& vertices, Math::Scalar initialRadius)
    {
        for (int i = 0; i < vertices.size(); i++)
            sphere.push_back(Sphere(vertices[i], initialRadius));
    }

    void SphereMesh::initializeEdgeQueue()
    {
        for (int i = 0; i < sphere.size(); ++i) {
            for (int j = i + 1; j < sphere.size(); ++j) {
                if (i == j || (sphere[i].center - sphere[j].center).squareMagnitude() > EPSILON * EPSILON)
                    continue;

                CollapsableEdge edge(sphere[i], sphere[j], i, j);
                edge.updateError();
                
                edge.queueIdI = 0;
                edge.queueIdJ = 0;
                
                if (edge.idxI > edge.idxJ)
                    edge.updateEdge(edge.j, edge.i, edge.idxJ, edge.idxI);
                
                edgeQueue.push(edge);
            }
        }
    }

    void SphereMesh::updateEdgeQueue(const CollapsableEdge& collapsedEdge)
    {
        auto edgeI = collapsedEdge.idxI;
        auto edgeJ = collapsedEdge.idxJ;
        
        for (int i = 0; i < sphere.size(); i++)
        {
            if (i != edgeI && (sphere[i].center - sphere[edgeI].center).squareMagnitude() <= EPSILON * EPSILON)
            {
                CollapsableEdge newEdge = CollapsableEdge(sphere[i], sphere[edgeI], i, edgeI);
                
                newEdge.queueIdI = 0;
                if (i == edgeJ)
                    newEdge.queueIdI = collapsedEdge.queueIdJ + 1;
                
                newEdge.queueIdJ = collapsedEdge.queueIdJ + 1;
                
                if (newEdge.idxI > newEdge.idxJ)
                {
                    newEdge.updateEdge(sphere[edgeI], sphere[i], newEdge.idxJ, newEdge.idxI);
                    
                    newEdge.queueIdJ = 0;
                    if (i == edgeJ)
                        newEdge.queueIdJ = collapsedEdge.queueIdJ + 1;
                    
                    newEdge.queueIdI = collapsedEdge.queueIdI + 1;
                }
                
                edgeQueue.push(newEdge);
            }
            
            if (i != edgeJ && (sphere[i].center - sphere[edgeJ].center).squareMagnitude() <= EPSILON * EPSILON)
            {
                CollapsableEdge newEdge = CollapsableEdge(sphere[i], sphere[edgeJ], i, edgeJ);
                
                newEdge.queueIdI = 0;
                if (i == edgeI)
                    newEdge.queueIdI = collapsedEdge.queueIdI + 1;
                
                newEdge.queueIdJ = collapsedEdge.queueIdJ + 1;
                
                if (newEdge.idxI > newEdge.idxJ)
                {
                    newEdge.updateEdge(sphere[edgeJ], sphere[i], newEdge.idxJ, newEdge.idxI);
                    
                    newEdge.queueIdJ = 0;
                    if (i == edgeI)
                        newEdge.queueIdJ = collapsedEdge.queueIdI + 1;
                    
                    newEdge.queueIdI = collapsedEdge.queueIdJ + 1;
                }
                
                edgeQueue.push(newEdge);
            }
        }
    }

    RenderType SphereMesh::getRenderType() {
        return this->renderType;
    }

    void SphereMesh::setRenderType(const RenderType &renderType) {
        this->renderType = renderType;
    }

    void SphereMesh::buildEdgeConnectivity() {
        edgeConnectivity.clear();
        
        for (int i = 0; i < sphere.size(); i++) {
            std::vector<bool> row;
            for (int j = 0; j < sphere.size(); j++)
                row.push_back(false);

            edgeConnectivity.push_back(row);
        }
    }

    void SphereMesh::buildTriangleConnectivity() {
        triangleConnectivity.clear();
        
        for (int i = 0; i < sphere.size(); i++) {
            std::vector<std::vector<bool>> x;
            for (int j = 0; j < sphere.size(); j++) {
                std::vector<bool> y;
                
                for (int k = 0; k < sphere.size(); k++)
                    y.push_back(false);
                
                x.push_back(y);
            }

            triangleConnectivity.push_back(x);
        }
    }

    void SphereMesh::clearEdges() {
        for (int i = 0; i < edge.size(); i++)
            if (edge[i].i > edge[i].j) {
                int tmp = edge[i].i;
                edge[i].i = edge[i].j;
                edge[i].j = tmp;
            }
        
        for (int i = 0; i < edge.size(); i++)
            edgeConnectivity[edge[i].i][edge[i].j] = true;
        
        edge.clear();
        for (int i = 0; i < edgeConnectivity.size(); i++)
            for (int j = 0; j < edgeConnectivity[i].size(); j++)
                if (edgeConnectivity[i][j])
                    edge.push_back(Edge(i, j));
    }

    void SphereMesh::clearTriangles() {
        for (int i = 0; i < triangle.size(); i++) {
            int min = std::min(triangle[i].i, std::min(triangle[i].j, triangle[i].k));
            int max = std::max(triangle[i].i, std::max(triangle[i].j, triangle[i].k));
            
            if (triangle[i].i != min && triangle[i].i != max) triangle[i] = Triangle(min, triangle[i].i, max);
            if (triangle[i].j != min && triangle[i].j != max) triangle[i] = Triangle(min, triangle[i].j, max);
            if (triangle[i].k != min && triangle[i].k != max) triangle[i] = Triangle(min, triangle[i].k, max);
        }
        
        for (int i = 0; i < triangle.size(); i++) {
            edgeConnectivity[triangle[i].i][triangle[i].j] = false;
            edgeConnectivity[triangle[i].i][triangle[i].k] = false;
            edgeConnectivity[triangle[i].j][triangle[i].k] = false;
            triangleConnectivity[triangle[i].i][triangle[i].j][triangle[i].k] = true;
        }
        
        triangle.clear();
        for (int i = 0; i < triangleConnectivity.size(); i++)
            for (int j = 0; j < triangleConnectivity[i].size(); j++)
                for (int k = 0; k < triangleConnectivity[i][j].size(); k++)
                    if (triangleConnectivity[i][j][k])
                        triangle.push_back(Triangle(i, j, k));
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

    CollapsableEdge SphereMesh::getBestCollapseFast()
    {
        if (edgeQueue.size() < 1)
            return CollapsableEdge();
        
        CollapsableEdge topEdge = edgeQueue.top((int)sphere.size());
        
        if (topEdge.idxI == -1 || topEdge.idxJ == -1)
            return CollapsableEdge();
        
        edgeQueue.pop();
        
        if (topEdge.i.region.intervals.size() == 0 || topEdge.j.region.intervals.size() == 0)
            std::cerr << "Selected sphere has an invalid region!" << std::endl;
        
        return topEdge;
    }

    CollapsableEdge SphereMesh::getBestCollapseBruteForce()
    {
        if (sphere.size() <= 1)
            return CollapsableEdge();
        
        Math::Scalar minErorr = DBL_MAX;
        CollapsableEdge bestEdge = CollapsableEdge(sphere[0], sphere[1], -1, -1);
        
        for (int i = 0; i < sphere.size(); i++)
            for (int j = i + 1; j < sphere.size(); j++)
            {
                if (i == j || (sphere[i].center - sphere[j].center).squareMagnitude() > EPSILON * EPSILON)
                    continue;
                
                CollapsableEdge candidateEdge = CollapsableEdge(sphere[i], sphere[j], i, j);
                
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
            return CollapsableEdge();
        
        return bestEdge;
    }

    CollapsableEdge SphereMesh::getBestCollapseInConnectivity()
    {
        if (triangle.size() < 1 && edge.size() < 1)
            return CollapsableEdge();
        
        Math::Scalar minErorr = DBL_MAX;
        CollapsableEdge bestEdge;
        
        for (int i = 0; i < triangle.size(); i++)
        {
            int v1 = triangle[i].i;
            int v2 = triangle[i].j;
            int v3 = triangle[i].k;
            
            CollapsableEdge e1 = CollapsableEdge(sphere[v1], sphere[v2], v1, v2);
            CollapsableEdge e2 = CollapsableEdge(sphere[v1], sphere[v3], v1, v3);
            CollapsableEdge e3 = CollapsableEdge(sphere[v2], sphere[v3], v2, v3);
            
            CollapsableEdge bestInTriangle;
            if (e1.error <= e2.error && e1.error <= e3.error)
                bestInTriangle = e1;
            else if (e2.error <= e1.error && e2.error <= e3.error)
                bestInTriangle = e2;
            else
                bestInTriangle = e3;
            
            if (bestInTriangle.error < minErorr)
            {
                bestEdge = bestInTriangle;
                minErorr = bestEdge.error;
            }
        }
        
        for (int i = 0; i < edge.size(); i++)
        {
            int v1 = edge[i].i;
            int v2 = edge[i].j;
            
            CollapsableEdge candidateEdge = CollapsableEdge(sphere[v1], sphere[v2], v1, v2);
            if (candidateEdge.error < minErorr)
            {
                bestEdge = candidateEdge;
                minErorr = bestEdge.error;
            }
        }
        
        if (bestEdge.idxI == -1 || bestEdge.idxJ == -1)
            return CollapsableEdge();
        
        if (bestEdge.idxI > bestEdge.idxJ)
            bestEdge.updateEdge(bestEdge.j, bestEdge.i, bestEdge.idxJ, bestEdge.idxI);
        
        return bestEdge;
    }

    void SphereMesh::computeSpheresProperties(const std::vector<Vertex>& vertices)
    {
        const Math::Scalar sigma = 1.0;
        
        for (int j = 0; j < triangle.size(); j++)
        {
            int i0 = triangle[j].i;
            int i1 = triangle[j].j;
            int i2 = triangle[j].k;
            
            Math::Vector3 v0 = vertices[i0].position;
            Math::Vector3 v1 = vertices[i1].position;
            Math::Vector3 v2 = vertices[i2].position;
            // Getting neighbor face normal and centroid in order to add the face to our sphere
//            Math::Vector3 centroid = getTriangleCentroid(v0, v1, v2);
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

            sphere[i0].region.addVertex(vertices[i0].position);
            sphere[i0].region.addVertex(vertices[i1].position);
            sphere[i0].region.addVertex(vertices[i2].position);

            sphere[i1].region.addVertex(vertices[i0].position);
            sphere[i1].region.addVertex(vertices[i1].position);
            sphere[i2].region.addVertex(vertices[i2].position);

            sphere[i2].region.addVertex(vertices[i0].position);
            sphere[i2].region.addVertex(vertices[i1].position);
            sphere[i2].region.addVertex(vertices[i2].position);
        }
    }

    void SphereMesh::updateSpheres()
    {
        for (int i = 0; i < sphere.size(); i++)
        {
            sphere[i].quadric *= (1/sphere[i].quadricWeights);

            auto result = sphere[i].quadric.minimizer();
            sphere[i].center = result.toQuaternion().immaginary;
            sphere[i].radius = result.coordinates.w;
        }
    }

    Math::Vector3 SphereMesh::getTriangleCentroid(const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3)
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

        sphere.push_back(Sphere(Math::Vector3(), 1));
        sphere.push_back(Sphere(Math::Vector3(3, 1.5f, 0.0f), 1.5f));
        sphere.push_back(Sphere(Math::Vector3(3, -1.5f, 0.0f), 0.7f));
        sphere.push_back(Sphere(Math::Vector3(6, 3, -.5f), 1));
        sphere.push_back(Sphere(Math::Vector3(6, -3, .5f), 1.2));

        triangle.push_back(Triangle(0, 1, 2));

        edge.push_back(Edge(1, 3));
        edge.push_back(Edge(2, 4));
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
            Math::Vector3 point = Math::lerp<Math::Vector3>(p0, p1, t);
            
            renderSphere(point, BDDSize * 0.002, color);
            
            t += 0.05;
        }
    }

    void SphereMesh::renderOneLine(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& color, int spheresPerEdge, Math::Scalar sphereSize) {
        Math::Scalar t = 0.0;
        Math::Scalar cyclesIncrement = 1.0 / (Math::Scalar)spheresPerEdge;
        
        while (t < 1.0) {
            Math::Vector3 point = Math::lerp<Math::Vector3>(p0, p1, t);
            
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
        for (int i = 0; i < triangle.size(); i++)
            this->drawSpheresOverTriangle(triangle[i]);

        for (int i = 0; i < edge.size(); i++)
            this->drawSpheresOverEdge(edge[i]);
    }

    void SphereMesh::renderWithNSpherePerEdge(int n, Math::Scalar rescaleRadii, Math::Scalar minRadiiScale)
    {
        for (int i = 0; i < triangle.size(); i++)
            this->drawSpheresOverTriangle(triangle[i], n, rescaleRadii, minRadiiScale);

        for (int i = 0; i < edge.size(); i++)
            this->drawSpheresOverEdge(edge[i], n, rescaleRadii, minRadiiScale);
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
            
            vertexCount = vertices.size();

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }
        
        perSphereVertices = vertexCount;

        sphereShader->use();
        sphereShader->setVec3("center", center);
//        sphereShader->setFloat("radius", radius);
        sphereShader->setVec3("material.ambient", color);
        sphereShader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
        sphereShader->setVec3("material.specular", Math::Vector3(0, 0, 0));
        sphereShader->setFloat("material.shininess", 0);
        sphereShader->setFloat("radius", radius);
//        sphereShader->setVec3("color", color);

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
                    for (int edge = 0; edge < 3; edge++) {
                        int i1 = faces[i + edge];
                        int i2 = faces[i + (edge + 1) % 3];
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
                        // Add the midpoint to the vertices and store the index in mid[edge]
                        vertices.insert(vertices.end(), midpoint, midpoint + 3);
                        mid[edge] = vertices.size() / 3 - 1;
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
                vertexCount = vertices.size();
            }

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
        sphereShader->setFloat("radius", radius);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);
//        glDisable(GL_BLEND);
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
        Math::Scalar minDistance = DBL_MAX;
        
        for (int i = 0; i < referenceMesh->vertices.size(); i++)
        {
            auto distance = (center - referenceMesh->vertices[i].position).squareMagnitude();
            distance = std::sqrt(distance);
            minDistance = distance < minDistance ? distance : minDistance;
        }
        
        return minDistance;
    }

    void SphereMesh::renderSelectedSpheresOnly()
    {
        if (edgeQueue.isQueueDirty())
        {
            edgeQueue.clear();
            initializeEdgeQueue();
        }
        
        CollapsableEdge e = getBestCollapseFast();
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
        
        CollapsableEdge e = getBestCollapseInConnectivity();
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
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == i)
            {
                for (int j = 0; j < sphere[idx].vertices.size(); j++)
                    renderSphere(sphere[idx].vertices[j].position, 0.02 * BDDSize, Math::Vector3(0, 1, 0));
            }
                
    }

    Sphere SphereMesh::collapseEdgeIntoSphere(const CollapsableEdge& edgeToCollapse)
    {
        Sphere collapsedSphereA = sphere[edgeToCollapse.idxI];
        Sphere collapsedSphereB = sphere[edgeToCollapse.idxJ];
        
        Sphere newSphere = Sphere();
        
        newSphere.color = Math::Vector3(1, 0, 0);
        
        newSphere.region.join(collapsedSphereA.region);
        newSphere.region.join(collapsedSphereB.region);
        
        newSphere.addQuadric(collapsedSphereA.getSphereQuadric());
        newSphere.addQuadric(collapsedSphereB.getSphereQuadric());
        
        if (newSphere.checkSphereOverPlanarRegion())
            newSphere.approximateSphereOverPlanarRegion(collapsedSphereA.center, collapsedSphereB.center);
        
        newSphere.constrainSphere(newSphere.region.directionalWidth);
//        newSphere.constrainSphere(getContainedRadiusOfSphere(newSphere));
        
//      FIXME: This is done only for rendering
        for (int k = 0; k < collapsedSphereA.vertices.size(); k++)
            newSphere.addVertex(collapsedSphereA.vertices[k]);
        
        for (int k = 0; k < collapsedSphereB.vertices.size(); k++)
            newSphere.addVertex(collapsedSphereB.vertices[k]);
//
        
        return newSphere;
    }

    bool SphereMesh::collapseSphereMesh()
    {
        if (edgeQueue.isQueueDirty())
        {
            edgeQueue.clear();
            initializeEdgeQueue();
        }
            
        CollapsableEdge e = getBestCollapseFast();
        
        if (e.idxI == -1 || e.idxJ == -1)
            return false;
        
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        checkSphereIntersections(newSphere);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();

        sphere.pop_back();
        
        updateEdgesAfterCollapse(e.idxI, e.idxJ);
        updateTrianglessAfterCollapse(e.idxI, e.idxJ);

        removeDegenerates();
//        clearSphereMesh();
//        std::cout << "Cleared Sphere Mesh" << std::endl;
        updateEdgeQueue(e);
        
        return true;
    }

    bool SphereMesh::collapseSphereMeshFast()
    {
        CollapsableEdge e = getBestCollapseInConnectivity();
        if (e.idxI == -1 || e.idxJ == -1)
            return false;
        
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();

        sphere.pop_back();

        updateEdgesAfterCollapse(e.idxI, e.idxJ);
        updateTrianglessAfterCollapse(e.idxI, e.idxJ);
        
        std::cout << sphere.size() << std::endl;

        removeDegenerates();
        clearSphereMesh();
        
        return true;
    }

    void SphereMesh::updateEdgesAfterCollapse(int i, int j)
    {
        int last = sphere.size();
        
        for (Edge& e : edge) {
            if (e.i == j) e.i = i;
            if (e.j == j) e.j = i;
            if (e.i == last && j != last) e.i = j;
            if (e.j == last && j != last) e.j = j;
        }
    }

    void SphereMesh::updateTrianglessAfterCollapse(int i, int j)
    {
        int last = sphere.size();
        
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

    bool SphereMesh::collapse(int i, int j)
    {
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
        
        if (i == j || i >= sphere.size() || j >= sphere.size())
            return false;

        if (i > j)
            std::swap(i, j);
        
        edgeQueue.setQueueDirty();
        
        Sphere newSphere = Sphere();
        newSphere.color = Math::Vector3(1, 0, 0);
        newSphere.addQuadric(sphere[i].getSphereQuadric());
        newSphere.addQuadric(sphere[j].getSphereQuadric());
        
//      FIXME: This is done only for rendering
        for (int k = 0; k < sphere[i].vertices.size(); k++)
            newSphere.addVertex(sphere[i].vertices[k]);
        
        for (int k = 0; k < sphere[j].vertices.size(); k++)
            newSphere.addVertex(sphere[j].vertices[k]);
//
        
        newSphere.region = sphere[i].region;
        newSphere.region.join(sphere[j].region);

        sphere[i] = newSphere;

        sphere[j] = sphere.back();
        
        sphere.pop_back();

        updateEdgesAfterCollapse(i, j);
        updateTrianglessAfterCollapse(i, j);

        removeDegenerates();
        clearSphereMesh();
        
        return true;
    }

    void SphereMesh::checkSphereIntersections(Sphere& s)
    {
//        for (int i = 0; i < sphere.size(); i++)
//        {
//            auto sphereIntersectionTest = (s.center - sphere[i].center).magnitude() < (s.radius + sphere[i].radius);
//
////            if (sphereIntersectionTest)
////                s.addQuadric(sphere[i].quadric);
//
////            TODO: Improve this in order to have the desired radius
//            auto targetRadius = (s.center - sphere[i].center).magnitude() - sphere[i].radius;
//            if (sphereIntersectionTest)
//                s.radius = targetRadius;
//        }
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
        for (int i = 0; i < degeneratedTriangles.size(); i++)
        {
            Edge newEdge = Edge(0, 0);
            int index = degeneratedTriangles[i];

            if (triangle[index].i == triangle[index].j)
                newEdge = Edge(triangle[index].i, triangle[index].k);
            else if (triangle[index].i == triangle[index].k)
                newEdge = Edge(triangle[index].i, triangle[index].j);
            else if (triangle[index].j == triangle[index].k)
                newEdge = Edge(triangle[index].i, triangle[index].j);
            edge.push_back(newEdge);

            triangle.erase(triangle.begin() + (degeneratedTriangles[i] - reducedSize));
            reducedSize++;
        }

        std::vector<int> degeneratedEdges;
        for (int i = 0; i < edge.size(); i++)
        {
            if (edge[i].i == edge[i].j)
                degeneratedEdges.push_back(i);
        }

        reducedSize = 0;
        for (int i = 0; i < degeneratedEdges.size(); i++)
        {
            edge.erase(edge.begin() + (degeneratedEdges[i] - reducedSize));
            reducedSize++;
        }
    }

    Sphere SphereMesh::getSelectedVertexSphere(int i)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == i)
            {
                return sphere[idx];
            }
    }

    void SphereMesh::resizeSphereVertex(int i, Math::Scalar newSize)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == i)
            {
                sphere[idx].radius = newSize;
                break;
            }
    }

    void SphereMesh::translateSphereVertex(int i, Math::Vector3& translation)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].getID() == i)
            {
                sphere[idx].center += translation;
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
                for (int i = 0; i < initialSpheres.size(); i++)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, initialSpheres[i].center);
                        out << YAML::Key << "Radius" << YAML::Value << initialSpheres[i].radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, initialSpheres[i].quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, initialSpheres[i].color);
                    out << YAML::EndMap;
                }
            out << YAML::EndSeq;
            out << YAML::Key << "Spheres" << YAML::Value;
            out << YAML::BeginSeq;
                for (int i = 0; i < sphere.size(); i++)
                {
                    out << YAML::BeginMap;
                        out << YAML::Key << "Center" << YAML::Value;
                        YAMLSerializeVector3(out, sphere[i].center);
                        out << YAML::Key << "Radius" << YAML::Value << sphere[i].radius;
                        out << YAML::Key << "Quadric" << YAML::Value;
                        YAMLSerializeQuadric(out, sphere[i].quadric);
                        out << YAML::Key << "Color" << YAML::Value;
                        YAMLSerializeVector3(out, sphere[i].color);
                    out << YAML::EndMap;
                }
            out << YAML::EndSeq;
            out << YAML::Key << "Connectivity" << YAML::Value;
            out << YAML::BeginMap;
                out << YAML::Key << "Edges" << YAML::Value;
                out << YAML::BeginSeq;
                    for (int i = 0; i < edge.size(); i++)
                    {
                        out << YAML::BeginMap;
                            out << YAML::Key << "E0" << YAML::Value << edge[i].i;
                            out << YAML::Key << "E1" << YAML::Value << edge[i].j;
                        out << YAML::EndMap;
                    }
                out << YAML::EndSeq;
                out << YAML::Key << "Triangles" << YAML::Value;
                out << YAML::BeginSeq;
                    for (int i = 0; i < triangle.size(); i++)
                    {
                        out << YAML::BeginMap;
                            out << YAML::Key << "T0" << YAML::Value << triangle[i].i;
                            out << YAML::Key << "T1" << YAML::Value << triangle[i].j;
                            out << YAML::Key << "T2" << YAML::Value << triangle[i].k;
                        out << YAML::EndMap;
                    }
                out << YAML::EndSeq;
            out << YAML::EndMap;
        out << YAML::EndMap;

        namespace fs = std::__fs::filesystem;

        fs::path cwd = fs::current_path();

        std::string separator = std::string(1, fs::path::preferred_separator);

        std::string filePath = fn;
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
        std::string fileContent = "#Sphere Mesh TXT \n";
        
        for (int i = 0; i < sphere.size(); i++)
        {
            Math::Vector4 s = Math::Vector4(sphere[i].center[0], sphere[i].center[1], sphere[i].center[2], sphere[i].radius);
            fileContent += "( " + std::to_string(s[0]) + ", " + std::to_string(s[1]) + ", " + std::to_string(s[2]) + ", " + std::to_string(s[3]) + " )\n";
        }
        
        for (int i = 0; i < triangle.size(); i++)
        {
            fileContent += "( " + std::to_string(triangle[i].i) + ", " + std::to_string(triangle[i].j) + ", " + std::to_string(triangle[i].k) + " )\n";
        }

        for (int i = 0; i < edge.size(); i++)
        {
            fileContent += "( " + std::to_string(edge[i].i) + ", " + std::to_string(edge[i].j) + " )\n";
        }
        fileContent += " ==================== \n";
        
        namespace fs = std::__fs::filesystem;
        
        fs::path cwd = fs::current_path();
        
        std::string separator = std::string(1, fs::path::preferred_separator);
        
        std::string filePath = fn;
        std::string folderPath = "." + separator;
        if (path != ".")
            folderPath = path;
        
        std::ofstream fout(folderPath + filePath);
        std::cout << "File location: " << folderPath + filePath << std::endl;
        fout << fileContent.c_str();
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
        edge.push_back(Edge(selectedSphereIndex, sphere.size() - 1));
        
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
        triangle.push_back(Triangle(idxA, idxB, sphere.size() - 1));
        
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
        
        for (int i = 0; i < edge.size(); i++)
            if (edge[i].i == sphere.size())
                edge[i].i = selectedSphereIndex;
            else if (edge[i].j == sphere.size())
                edge[i].j = selectedSphereIndex;
        
        for (int i = 0; i < triangle.size(); i++)
            if (triangle[i].i == sphere.size())
                triangle[i].i = selectedSphereIndex;
            else if (triangle[i].j == sphere.size())
                triangle[i].j = selectedSphereIndex;
            else if (triangle[i].k == sphere.size())
                triangle[i].k = selectedSphereIndex;
        
        removeDegenerates();
        clearSphereMesh();
    }
}
