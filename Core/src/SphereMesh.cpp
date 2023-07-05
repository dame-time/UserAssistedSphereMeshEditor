#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#include "../SphereMesh.hpp"

#include <Math.hpp>

//#include <YAMLUtils.hpp>

#include <filesystem>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Renderer {
    SphereMesh::SphereMesh(const SphereMesh& sm) : referenceMesh(sm.referenceMesh)
    {
        BDDSize = sm.BDDSize;
        
        sphere = sm.sphere;
        initialSpheres = sm.initialSpheres;
        
        triangle = sm.triangle;
        edge = sm.edge;
        
        renderedSpheres = sm.renderedSpheres;
        renderedSphereVertexMeshes = sm.renderedSphereVertexMeshes;
    }

    SphereMesh::SphereMesh(RenderableMesh* mesh, Math::Scalar vertexSphereRadius) : referenceMesh(mesh)
    {
        auto vertices = mesh->vertices;
        auto faces = mesh->faces;
        
//        for (auto v : mesh.vertices)
//            v.position.print();
//
//        for (auto f : mesh.faces)
//            std::cout << f.i << ", " << f.j << ", " << f.k << std::endl;
        
        BDDSize = mesh->bbox.BDD().magnitude();

        initializeSphereMeshTriangles(faces);
        initializeSpheres(vertices, 0.01f * BDDSize);
        
        computeSpheresProperties(vertices);
        updateSpheres();

        initialSpheres = sphere;
    }

    SphereMesh& SphereMesh::operator = (const SphereMesh& sm)
    {
        BDDSize = sm.BDDSize;
        
        sphere = sm.sphere;
        initialSpheres = sm.initialSpheres;
        
        triangle = sm.triangle;
        edge = sm.edge;
        
        renderedSpheres = sm.renderedSpheres;
        renderedSphereVertexMeshes = sm.renderedSphereVertexMeshes;
        
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

    CollapsableEdge SphereMesh::getBestCollapseBruteForce()
    {
        const Math::Scalar EPSILON = 0.05 * BDDSize;
        
        assert(sphere.size() > 1);
        
        Math::Scalar minErorr = DBL_MAX;
        CollapsableEdge bestEdge = CollapsableEdge(sphere[0], sphere[1], -1, -1);
        
        for (int i = 0; i < sphere.size(); i++)
        {
            for (int j = i + 1; j < sphere.size(); j++)
            {
                if (i == j || (sphere[i].center - sphere[j].center).magnitude() > EPSILON)
                    continue;
                
                CollapsableEdge candidateEdge = CollapsableEdge(sphere[i], sphere[j], i, j);
                if (candidateEdge.error < minErorr)
                {
                    bestEdge = CollapsableEdge(sphere[i], sphere[j], i, j);
                    minErorr = candidateEdge.error;
                }
            }
        }
        
        assert(bestEdge.idxI != -1 && bestEdge.idxJ != -1);
        
        if (bestEdge.idxI > bestEdge.idxJ)
            bestEdge.updateEdge(bestEdge.j, bestEdge.i, bestEdge.idxJ, bestEdge.idxI);
        
        return bestEdge;
    }

    CollapsableEdge SphereMesh::getBestCollapseInConnectivity()
    {
        const Math::Scalar EPSILON = DBL_MAX;
        
        assert(triangle.size() > 0 || edge.size() > 0);
        
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
        
        assert(bestEdge.idxI != -1 && bestEdge.idxJ != -1);
        
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

    void SphereMesh::drawSpheresOverEdge(const Edge &e, int ns)
    {
        int nSpheres = ns;

        const Math::Vector3 color = Math::Vector3(0.1, 0.7, 1);

        for (int i = 1; i < nSpheres - 1; i++)
        {
            RenderableMesh& s = referenceMesh->addSubSphere(false);
            
            s.scaleUniform(Math::lerp(sphere[e.i].radius, sphere[e.j].radius, i * 1.0f / (nSpheres - 1)));
            s.translate(Math::lerp(sphere[e.i].center, sphere[e.j].center, i * 1.0f / (nSpheres - 1)));
            
//            s.setUniformColor(Math::Vector3(0, 0, 1));
            
            renderedSpheres.push_back(s.getID());
        }
    }

    void SphereMesh::drawSpheresOverTriangle(const Triangle& e, int ns)
    {
        Math::Vector3 color = Math::Vector3(0.1f, 0.7f, 1);

        int nSpheres = ns;

        for (int i = 0; i < nSpheres; i++)
            for (int j = 0; j < nSpheres - i; j++)
            {
                int k = nSpheres - 1 - i - j;

                if (i == nSpheres - 1 || j == nSpheres - 1 || k == nSpheres - 1) continue;

                Math::Scalar ci = i * 1.0 / (nSpheres - 1);
                Math::Scalar cj = j * 1.0 / (nSpheres - 1);
                Math::Scalar ck = k * 1.0 / (nSpheres - 1);
                
                RenderableMesh& s = referenceMesh->addSubSphere();
                
                s.scaleUniform(sphere[e.i].radius * ci + sphere[e.j].radius * cj + sphere[e.k].radius * ck);
                s.translate(sphere[e.i].center * ci + sphere[e.j].center * cj + sphere[e.k].center * ck);
                
//                s.setUniformColor(Math::Vector3(0, 0, 1));
                
                renderedSpheres.push_back(s.getID());
            }
    }

    void SphereMesh::renderEdge(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Vector3& color)
    {
//        TODO: Add a way to render lines instead of spheres
        double i = 0;
        
        while (i < 1.0)
        {
            auto position = Math::lerp<Math::Vector3>(p1, p2, i);
            auto& s = referenceMesh->addSubSphere(false);
            
            s.scaleUniform(0.075);
            s.translate(position);
            
            s.setUniformColor(color);
            
            renderedEdgesSpheres.push_back(&s);
            
            i += 0.1;
        }
    }

    void SphereMesh::renderConnectivity()
    {
        clearRenderedEdges();
        
        const Math::Vector3 color = Math::Vector3(1, 1, 0);
        for (Triangle& t : triangle)
        {
            renderEdge(sphere[t.i].center, sphere[t.j].center, color);
            renderEdge(sphere[t.i].center, sphere[t.k].center, color);
            renderEdge(sphere[t.j].center, sphere[t.j].center, color);
        }
        
        for (Edge& e : edge)
            renderEdge(sphere[e.i].center, sphere[e.j].center, color);
    }

    void SphereMesh::clearRenderedEdges()
    {
        auto& subspheres = referenceMesh->subSpheres;
        auto& edgesSpheres = renderedEdgesSpheres;
        std::vector<RenderableMesh> tempSpheres;

        for (auto& sphere : subspheres) {
            bool found = false;

            for (auto it = edgesSpheres.begin(); it != edgesSpheres.end(); ++it) {
                if (sphere.getID() == (*it)->getID()) {
                    found = true;
                    edgesSpheres.erase(it);
                    break;
                }
            }

            if (!found) {
                tempSpheres.push_back(std::move(sphere));
            }
        }

        subspheres = std::move(tempSpheres);
    }

    void SphereMesh::render()
    {
        for (int i = 0; i < triangle.size(); i++)
            this->drawSpheresOverTriangle(triangle[i]);

        for (int i = 0; i < edge.size(); i++)
            this->drawSpheresOverEdge(edge[i]);

        renderSpheresOnly();
    }

    void SphereMesh::renderWithNSpherePerEdge(int n)
    {
        for (int i = 0; i < triangle.size(); i++)
            this->drawSpheresOverTriangle(triangle[i], n);

        for (int i = 0; i < edge.size(); i++)
            this->drawSpheresOverEdge(edge[i], n);
        
        renderSpheresOnly();
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
            
            RenderableMesh& s = referenceMesh->addSubSphere();
//            this->renderedSphereVertexMeshes.push_back(s);
            
            s.scaleUniform(sphere[i].radius);
            s.translate(sphere[i].center);
            
//            sphere[i].quadric.print();
//            std::cout << sphere[i].radius << std::endl;
//            sphere[i].center.print();
            
            sphere[i].renderedMeshID = s.getID();
            
            renderedSpheres.push_back(s.getID());
        }
    }

    void SphereMesh::renderSelectedSpheresOnly()
    {
        Math::Vector3 color = Math::Vector3(0, 1, 0);
        
        CollapsableEdge e = getBestCollapseBruteForce();
        
        if (e.i.radius > 0)
        {
            // The point is inside the triangle
            RenderableMesh& s = referenceMesh->addSubSphere();

            auto r = e.i.radius;
            if (r > e.i.region.directionalWidth)
                r = e.i.region.directionalWidth;
            
            s.scaleUniform(r + 0.01f);
            s.translate(e.i.center);
            
            e.i.renderedMeshID = s.getID();
            
            renderedSpheres.push_back(s.getID());
        }
        
        if (e.j.radius > 0)
        {
            // The point is inside the triangle
            RenderableMesh& s = referenceMesh->addSubSphere();

            auto r = e.j.radius;
            if (r > e.j.region.directionalWidth)
                r = e.j.region.directionalWidth;
            
            s.scaleUniform(r + 0.01f);
            s.translate(e.j.center);
            
            e.j.renderedMeshID = s.getID();
            
            renderedSpheres.push_back(s.getID());
        }
    }

    void SphereMesh::renderFastSelectedSpheresOnly()
    {
        Math::Vector3 color = Math::Vector3(0, 1, 0);
        
        CollapsableEdge e = getBestCollapseInConnectivity();
        
        if (e.i.radius > 0)
        {
            // The point is inside the triangle
            RenderableMesh& s = referenceMesh->addSubSphere();
//            this->renderedSphereVertexMeshes.push_back(s);

            auto r = e.i.radius;
            if (r > e.i.region.directionalWidth)
                r = e.i.region.directionalWidth;
            
            s.scaleUniform(r + 0.01f);
            s.translate(e.i.center);
            
            e.i.renderedMeshID = s.getID();
            
            renderedSpheres.push_back(s.getID());
        }
        
        if (e.j.radius > 0)
        {
            // The point is inside the triangle
            RenderableMesh& s = referenceMesh->addSubSphere();
//            this->renderedSphereVertexMeshes.push_back(s);

            auto r = e.j.radius;
            if (r > e.j.region.directionalWidth)
                r = e.j.region.directionalWidth;
            
            s.scaleUniform(r + 0.01f);
            s.translate(e.j.center);
            
            e.j.renderedMeshID = s.getID();
            
            renderedSpheres.push_back(s.getID());
        }
    }

    void SphereMesh::renderSphereVertices(int i)
    {
        int size = sphere.size();
        
        for (int idx = 0; idx < size; idx++)
            if (sphere[idx].renderedMeshID == i)
            {
                sphere[idx].renderAssociatedVertices(*referenceMesh, 0.02 * BDDSize);
                renderedSphereVertexMeshes.push_back(idx);
            }
                
    }

    void SphereMesh::clearRenderedSphereVertices()
    {
        for (int idx = 0; idx < renderedSphereVertexMeshes.size(); idx++)
            sphere[renderedSphereVertexMeshes[renderedSphereVertexMeshes.size() - 1 - idx]].clearRenderedSpheres(*referenceMesh);

        renderedSphereVertexMeshes.clear();
    }

    void SphereMesh::colorSelectedSphere(int i)
    {
        // TODO: Implement this
//        for (int idx = 0; idx < i; idx++)
//            if (sphere[idx].renderedMeshID == i)
//            {
//                Eigen::MatrixXd C(this->viewer.data_list[sphere[idx].renderedMeshID].F.rows(), 3);
//                C.rowwise() = Eigen::RowVector3d(0, 0, 1);
//
//                std::cout << "0. error of sphere -> " << sphere[idx].quadric.evaluateSQEM(sphere[idx].quadric.minimizer()) << std::endl;
//
//                this->viewer.data_list[sphere[idx].renderedMeshID].set_colors(C);
//                return;
//            }
    }
//
//    void SphereMesh::resetColorOfSpheres()
//    {
//        for (int idx = 0; idx < sphere.size(); idx++)
//        {
//            Eigen::MatrixXd C(this->viewer.data_list[sphere[idx].renderedMeshID].F.rows(), 3);
//            C.rowwise() = Eigen::RowVector3d(1, 0, 0);
//
//            this->viewer.data_list[sphere[idx].renderedMeshID].set_colors(C);
//        }
//    }
//

    void SphereMesh::clearRenderedMeshes()
    {
        referenceMesh->clearSubSpheres();
        renderedSpheres.clear();
        renderedSphereVertexMeshes.clear();
    }

    Sphere SphereMesh::collapseEdgeIntoSphere(const CollapsableEdge& edgeToCollapse)
    {
        int i = edgeToCollapse.idxI;
        int j = edgeToCollapse.idxJ;
        
        Sphere collapsedSphereA = sphere[i];
        Sphere collapsedSphereB = sphere[j];
        
        Sphere newSphere = Sphere();
        newSphere.addQuadric(collapsedSphereA.getSphereQuadric());
        newSphere.addQuadric(collapsedSphereB.getSphereQuadric());
        
//      FIXME: This is done only for rendering
        for (int k = 0; k < sphere[i].vertices.size(); k++)
            newSphere.addVertex(sphere[i].vertices[k]);
        
        for (int k = 0; k < sphere[j].vertices.size(); k++)
            newSphere.addVertex(sphere[j].vertices[k]);
//
        
        newSphere.region.join(collapsedSphereA.region);
        newSphere.region.join(collapsedSphereB.region);
        
        return newSphere;
    }

    void SphereMesh::collapseSphereMesh()
    {
        CollapsableEdge e = getBestCollapseBruteForce();
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        checkSphereIntersections(newSphere);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();

        sphere.pop_back();
        
        updateEdgesAfterCollapse(e.idxI, e.idxJ);
        updateTrianglessAfterCollapse(e.idxI, e.idxJ);
        
        std::cout << sphere.size() << std::endl;

        removeDegenerates();
    }

    void SphereMesh::collapseSphereMeshFast()
    {
        CollapsableEdge e = getBestCollapseInConnectivity();
        Sphere newSphere = collapseEdgeIntoSphere(e);
        
        sphere[e.idxI] = newSphere;
        sphere[e.idxJ] = sphere.back();

        sphere.pop_back();

        updateEdgesAfterCollapse(e.idxI, e.idxJ);
        updateTrianglessAfterCollapse(e.idxI, e.idxJ);
        
        std::cout << sphere.size() << std::endl;

        removeDegenerates();
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

    void SphereMesh::collapseSphereMesh(int n)
    {
        while (this->sphere.size() > n)
            this->collapseSphereMesh();
    }

    void SphereMesh::collapseSphereMeshFast(int n)
    {
        while (this->sphere.size() > n)
            this->collapseSphereMeshFast();
    }

    void SphereMesh::collapse(int i, int j)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].renderedMeshID == i)
            {
                i = idx;
                break;
            }
        
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].renderedMeshID == j)
            {
                j = idx;
                break;
            }
        
        assert(i != j);
        assert(i < sphere.size());
        assert(j < sphere.size());

        if (i > j)
            std::swap(i, j);

        // Sphere newSphere = sphere[i].lerp(sphere[j], 0.5f);
        
        Sphere newSphere = Sphere();
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
        
        std::cout << sphere.size() << std::endl;

        removeDegenerates();
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
            if (sphere[idx].renderedMeshID == i)
            {
                return sphere[idx];
            }
    }

    void SphereMesh::resizeSphereVertex(int i, Math::Scalar newSize)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].renderedMeshID == i)
            {
                sphere[idx].radius = newSize;
                break;
            }
    }

    void SphereMesh::translateSphereVertex(int i, Math::Vector3& translation)
    {
        for (int idx = 0; idx < sphere.size(); idx++)
            if (sphere[idx].renderedMeshID == i)
            {
                sphere[idx].center += translation;
                break;
            }
    }

// TODO: Import YAML and use this function
//    void SphereMesh::saveYAML(const std::string& path, const std::string& fn)
//    {
//        YAML::Emitter out;
//
//        out << YAML::Comment("Sphere Mesh YAML @ author Davide Paolillo");
//        out << YAML::Key << "Start Mesh Resolution" << YAML::Value << initialSpheres.size();
//        out << YAML::Key << "Sphere Mesh Resolution" << YAML::Value << sphere.size();
//        out << YAML::Key << "Spheres" << YAML::Value;
//        out << YAML::BeginSeq;
//            for (int i = 0; i < sphere.size(); i++)
//            {
//                out << YAML::BeginMap;
//                    out << YAML::Key << "Center" << YAML::Value;
//                    YAMLSerializeVector3(out, sphere[i].center);
//                    out << YAML::Key << "Radius" << YAML::Value << sphere[i].radius;
//                    out << YAML::Key << "Quadric" << YAML::Value;
//                    YAMLSerializeQuadric(out, sphere[i].quadric);
//                out << YAML::EndMap;
//            }
//        out << YAML::EndSeq;
//        out << YAML::Key << "Connectivity" << YAML::Value;
//        out << YAML::BeginMap;
//            out << YAML::Key << "Edges" << YAML::Value;
//            out << YAML::BeginSeq;
//                for (int i = 0; i < edge.size(); i++)
//                {
//                    out << YAML::BeginMap;
//                        out << YAML::Key << "E1" << YAML::Value << edge[i].i;
//                        out << YAML::Key << "E2" << YAML::Value << edge[i].j;
//                    out << YAML::EndMap;
//                }
//            out << YAML::EndSeq;
//            out << YAML::Key << "Triangles" << YAML::Value;
//            out << YAML::BeginSeq;
//                for (int i = 0; i < triangle.size(); i++)
//                {
//                    out << YAML::BeginMap;
//                        out << YAML::Key << "T0" << YAML::Value << triangle[i].i;
//                        out << YAML::Key << "T1" << YAML::Value << triangle[i].j;
//                        out << YAML::Key << "T2" << YAML::Value << triangle[i].k;
//                    out << YAML::EndMap;
//                }
//            out << YAML::EndSeq;
//        out << YAML::EndMap;
//
//        namespace fs = std::__fs::filesystem;
//
//        fs::path cwd = fs::current_path();
//
//        std::string separator = std::string(1, fs::path::preferred_separator);
//
//        std::string filePath = fn;
//        std::string folderPath = "." + separator;
//        if (path != ".")
//        {
//            folderPath = path;
//        }
//
//        std::ofstream fout(folderPath + filePath);
//        std::cout << "File location: " << cwd << std::endl;
//        fout << out.c_str();
//        fout.close();
//    }

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
        {
            folderPath = path;
        }
        
        std::ofstream fout(folderPath + filePath);
        std::cout << "File location: " << cwd << std::endl;
        fout << fileContent.c_str();
        fout.close();
    }

    void SphereMesh::reset()
    {
        sphere = initialSpheres;
    }
}
