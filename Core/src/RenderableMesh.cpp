//
//  RenderableMesh.cpp
//  CustomRenderer
//
//  Created by Davide Paollilo on 21/06/23.
//

#include <glad/glad.h>

#include <RenderableMesh.hpp>

#include <ObjLoader.hpp>

#include <Vector4.hpp>
#include <Matrix4.hpp>

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include <igl/principal_curvature.h>

#include <random>
#include <unordered_map>

namespace Renderer {
    RenderableMesh::RenderableMesh(const std::string& pathToLoadFrom, Shader* s) :  shader(s) {
        ObjLoader loader = ObjLoader();
        path = pathToLoadFrom;
        
        if (!loader.loadOBJ(pathToLoadFrom)) {
            std::cerr << "ERROR LOADING THE OBJ MODEL" << std::endl;
            return;
        }
        
        for (int i = 0; i < loader.vertices.size(); i++) {
            Vertex v = Vertex();
            
            v.position = loader.vertices[i];
            v.normal = loader.normals[i];
            v.color = loader.colors[i];
            v.curvature = Math::Vector2();
            
            this->vertices.push_back(v);
        }
        
        for (int i = 0; i < loader.indices.size(); i += 3)
            this->faces.push_back(Face(loader.indices[i], loader.indices[i + 1], loader.indices[i + 2]));
        
        setup();
        
        computeVerticesCurvatureIGL();
        
        generateUUID();
        updateBBOX();
        
        this->setBlended(true);
    }

    RenderableMesh::RenderableMesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces, Shader* s) : shader(s) {
        this->vertices = vertices;
        this->faces = faces;
        
        setup();
        
        computeVerticesCurvatureIGL();
        
        generateUUID();
        updateBBOX();
        
        this->setBlended(true);
    }

    void RenderableMesh::computeVerticesCurvatureIGL()
    {
        Eigen::MatrixXd v(vertices.size(), 3);
        Eigen::MatrixXi f(faces.size(), 3);

        for (int i = 0; i < vertices.size(); ++i) {
            v(i, 0) = vertices[i].position.coordinates.x;
            v(i, 1) = vertices[i].position.coordinates.y;
            v(i, 2) = vertices[i].position.coordinates.z;
        }

        for (int i = 0; i < faces.size(); ++i) {
            f(i, 0) = faces[i].i;
            f(i, 1) = faces[i].j;
            f(i, 2) = faces[i].k;
        }
        
        Eigen::MatrixXd PD1, PD2;
        Eigen::VectorXd PV1, PV2;
        
        std::vector<int> bad_vertices;
        
        igl::principal_curvature(v, f, PD1, PD2, PV1, PV2, bad_vertices);
        
        for (int i = 0; i < this->vertices.size(); i++)
            vertices[i].curvature = Math::Vector2(PV1(i, 0), PV2(i, 0));
    }

    void RenderableMesh::computeVerticesCurvature()
    {
        for (int v = 0; v < this->vertices.size(); v++)
        {
            Math::Scalar angleSum = 0;
            Math::Scalar meanCurvature = 0;
            
            std::vector<Face> adjacentFaces = getVertexAdjacentFaces(v);
            std::vector<Vertex> adjacentVertices = getAdjacentVertices(v);
            
            for (const auto& face : adjacentFaces)
                angleSum += getAngleOfVertexAtFace(face, vertices[v]);
            
            for(const auto& adjVertex : adjacentVertices)
            {
                Math::Scalar cotAlpha;
                Math::Scalar cotBeta;
                
                try {
                    cotAlpha = getCotAlpha(vertices[v], adjVertex);  // Compute cotangent of the angle alpha opposite to the edge v-adjVertex
                    cotBeta = getCotBeta(vertices[v], adjVertex);    // Compute cotangent of the angle beta opposite to the edge v-adjVertex
                } catch (const std::exception& e) {
                    cotAlpha = 0;
                    cotBeta = 0;
                }
                
                auto edgeLength = distance(vertices[v].position, adjVertex.position);  // Compute distance between v and adjVertex
                
                meanCurvature += (cotAlpha + cotBeta) * edgeLength;
            }
            
            auto gaussianCurvature = 2 * M_PI - angleSum;
            meanCurvature *= 0.25;
            
            auto k1 = meanCurvature + std::sqrt(meanCurvature * meanCurvature - gaussianCurvature);
            auto k2 = meanCurvature - std::sqrt(meanCurvature * meanCurvature - gaussianCurvature);

            vertices[v].curvature = Math::Vector2(k1, k2);
        }
    }

    Math::Scalar RenderableMesh::distance(const Math::Vector3& p0, const Math::Vector3& p1)
    {
        return std::sqrt(
                             std::pow(p1.coordinates.x - p0.coordinates.x, 2) +
                             std::pow(p1.coordinates.y - p0.coordinates.y, 2) +
                             std::pow(p1.coordinates.z - p0.coordinates.z, 2)
                         );
    }

    Math::Scalar RenderableMesh::getAngleOfVertexAtFace(const Face& f, const Vertex& v)
    {
        Math::Vector3 v1 = vertices[f.i].position;
        Math::Vector3 v2 = vertices[f.j].position;
        Math::Vector3 v3 = vertices[f.k].position;
      
        Math::Vector3 edge1, edge2;
      
        if (v.position.coordinates.x == v1.coordinates.x &&
            v.position.coordinates.y == v1.coordinates.y &&
            v.position.coordinates.z == v1.coordinates.z)
        {
            edge1 = Math::Vector3(v2.coordinates.x - v1.coordinates.x,
                                  v2.coordinates.y - v1.coordinates.y,
                                  v2.coordinates.z - v1.coordinates.z);
            edge2 = Math::Vector3(v3.coordinates.x - v1.coordinates.x,
                                  v3.coordinates.y - v1.coordinates.y,
                                  v3.coordinates.z - v1.coordinates.z);
        }
        else if (v.position.coordinates.x == v2.coordinates.x &&
                   v.position.coordinates.y == v2.coordinates.y &&
                   v.position.coordinates.z == v2.coordinates.z)
        {
            edge1 = Math::Vector3(v1.coordinates.x - v2.coordinates.x,
                                  v1.coordinates.x - v2.coordinates.y,
                                  v1.coordinates.z - v2.coordinates.z);
            edge2 = Math::Vector3(v3.coordinates.x - v2.coordinates.x,
                                  v3.coordinates.y - v2.coordinates.y,
                                  v3.coordinates.z - v2.coordinates.z);
        }
        else
        {
            edge1 = Math::Vector3(v1.coordinates.x - v3.coordinates.x,
                                  v1.coordinates.y - v3.coordinates.y,
                                  v1.coordinates.z - v3.coordinates.z);
            edge2 = Math::Vector3(v2.coordinates.x - v3.coordinates.x,
                                  v2.coordinates.y - v3.coordinates.y,
                                  v2.coordinates.z - v3.coordinates.z);
        }
      
        float dotProduct = edge1.dot(edge2);
        float magnitude1 = edge1.magnitude();
        float magnitude2 = edge2.magnitude();
      
        return std::acos(dotProduct / (magnitude1 * magnitude2));
    }

    Math::Scalar RenderableMesh::getCotAlpha(Vertex v, Vertex adjVertex)
    {
        std::vector<Face> sharingFaces;
        // Find the two faces sharing the edge (v, adjVertex)
        for (const auto& face : faces)
            if (
                    (vertices[face.i].position == v.position &&
                    (vertices[face.j].position == adjVertex.position || vertices[face.k].position == adjVertex.position)) ||
                    (vertices[face.j].position == v.position &&
                    (vertices[face.i].position == adjVertex.position || vertices[face.k].position == adjVertex.position)) ||
                    (vertices[face.k].position == v.position &&
                    (vertices[face.i].position == adjVertex.position || vertices[face.j].position == adjVertex.position))
                )
                sharingFaces.push_back(face);
        
        if (sharingFaces.size() < 2)
            throw std::invalid_argument("Cannot find two face sharing the two vertices v and his adjacent");
        
        Vertex thirdVertex;
        if (vertices[sharingFaces[0].i].position == v.position)
            thirdVertex = vertices[sharingFaces[0].j].position == adjVertex.position ? vertices[sharingFaces[0].k] : vertices[sharingFaces[0].j];
        else if (vertices[sharingFaces[0].j].position == v.position)
            thirdVertex = vertices[sharingFaces[0].i].position == adjVertex.position ? vertices[sharingFaces[0].k] : vertices[sharingFaces[0].i];
        else
            thirdVertex = vertices[sharingFaces[0].i].position == adjVertex.position ? vertices[sharingFaces[0].j] : vertices[sharingFaces[0].i];

        Math::Vector3 vec1 = thirdVertex.position - v.position;
        Math::Vector3 vec2 = adjVertex.position - v.position;

        Math::Scalar dotProduct = vec1.dot(vec2);
        Math::Scalar magnitudeVec1 = vec1.magnitude();
        Math::Scalar magnitudeVec2 = vec2.magnitude();

        Math::Scalar alpha = std::acos(dotProduct / (magnitudeVec1 * magnitudeVec2));
        return 1 / std::tan(alpha);
    }

    Math::Scalar RenderableMesh::getCotBeta(Vertex v, Vertex adjVertex)
    {
        std::vector<Face> sharingFaces;
        // Find the two faces sharing the edge (v, adjVertex)
        for (const auto& face : faces)
            if (
                (vertices[face.i].position == v.position &&
                (vertices[face.j].position == adjVertex.position || vertices[face.k].position == adjVertex.position)) ||
                (vertices[face.j].position == v.position &&
                (vertices[face.i].position == adjVertex.position || vertices[face.k].position == adjVertex.position)) ||
                (vertices[face.k].position == v.position &&
                (vertices[face.i].position == adjVertex.position || vertices[face.j].position == adjVertex.position))
            )
                sharingFaces.push_back(face);

        if (sharingFaces.size() < 2)
            throw std::invalid_argument("Cannot find two faces sharing the two vertices v and his adjacent");

        Vertex thirdVertex;
        if (vertices[sharingFaces[1].i].position == adjVertex.position)
            thirdVertex = vertices[sharingFaces[1].j].position == v.position ? vertices[sharingFaces[1].k] : vertices[sharingFaces[1].j];
        else if (vertices[sharingFaces[1].j].position == adjVertex.position)
            thirdVertex = vertices[sharingFaces[1].i].position == v.position ? vertices[sharingFaces[1].k] : vertices[sharingFaces[1].i];
        else
            thirdVertex = vertices[sharingFaces[1].i].position == v.position ? vertices[sharingFaces[1].j] : vertices[sharingFaces[1].i];

        // Create vectors for calculating beta
        Math::Vector3 vec1 = thirdVertex.position - adjVertex.position;
        Math::Vector3 vec2 = v.position - adjVertex.position;

        Math::Scalar dotProduct = vec1.dot(vec2);
        Math::Scalar magnitudeVec1 = vec1.magnitude();
        Math::Scalar magnitudeVec2 = vec2.magnitude();

        Math::Scalar beta = std::acos(dotProduct / (magnitudeVec1 * magnitudeVec2));
        
        return 1 / std::tan(beta);
    }

    std::vector<Face> RenderableMesh::getVertexAdjacentFaces(int vertexIndex)
    {
        std::vector<Face> adjacentFaces;

        for (const auto& face : faces)
            if (face.i == vertexIndex || face.j == vertexIndex || face.k == vertexIndex)
                adjacentFaces.push_back(face);
        
        return adjacentFaces;
    }

    std::vector<Vertex> RenderableMesh::getAdjacentVertices(int vertexIndex)
    {
        std::vector<Vertex> adjacentVertices;
        std::unordered_map<int, bool> addedIndices;
        
        for (const auto& face : faces)
        {
            if (face.i == vertexIndex || face.j == vertexIndex || face.k == vertexIndex)
            {
                if (face.i != vertexIndex && addedIndices.find(face.i) == addedIndices.end())
                {
                    adjacentVertices.push_back(vertices[face.i]);
                    addedIndices[face.i] = true;
                }

                if (face.j != vertexIndex && addedIndices.find(face.j) == addedIndices.end())
                {
                    adjacentVertices.push_back(vertices[face.j]);
                    addedIndices[face.j] = true;
                }

                if (face.k != vertexIndex && addedIndices.find(face.k) == addedIndices.end())
                {
                    adjacentVertices.push_back(vertices[face.k]);
                    addedIndices[face.k] = true;
                }
            }
        }

        return adjacentVertices;
    }

    int RenderableMesh::getID() {
        return this->_ID;
    }

    Math::Scalar RenderableMesh::getMeshRadius() {
        Math::Scalar max_distance = 0;

        auto centerOfMass = getCentroid();

        for (int i = 0; i < vertices.size(); i++)
        {
            Math::Scalar dist = (vertices[i].position - centerOfMass).magnitude();
            if (dist > max_distance)
                max_distance = dist;
        }

        Math::Scalar radius = max_distance;

        return radius;
    }

    void RenderableMesh::generateUUID() {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distr(0, 999999);
        
        int random_number = distr(generator);
        
        this->_ID = random_number;
    }

    void RenderableMesh::updateBBOX() {
        bbox = AABB();
        for (int i = 0; i < vertices.size(); i++)
            bbox.addPoint(vertices[i].position);
    }

    Math::Matrix4 RenderableMesh::getModel() {
        return model;
    }

    Math::Vector3 RenderableMesh::getCentroid() {
        Math::Vector3 centroid = Math::Vector3();
        
        for (auto v : this->vertices) {
            centroid.coordinates.x += v.position.coordinates.x;
            centroid.coordinates.y += v.position.coordinates.y;
            centroid.coordinates.z += v.position.coordinates.z;
        }
        
        return Math::Vector3(
                                 centroid.coordinates.x / this->vertices.size(),
                                 centroid.coordinates.y / this->vertices.size(),
                                 centroid.coordinates.z / this->vertices.size()
                             );
    }

    Math::Scalar RenderableMesh::getBoundingSphereRadius() {
        Math::Scalar maxSquaredDistance = 0.0;
        
        Math::Vector3 centroid = getCentroid();
        
        for (const auto& vertex : this->vertices) {
            Math::Scalar dx = vertex.position.coordinates.x - centroid.coordinates.x;
            Math::Scalar dy = vertex.position.coordinates.y - centroid.coordinates.y;
            Math::Scalar dz = vertex.position.coordinates.z - centroid.coordinates.z;
            
            Math::Scalar squaredDistance = dx * dx + dy * dy + dz * dz;
            
            maxSquaredDistance = std::max(maxSquaredDistance, squaredDistance);
        }
        
        return std::sqrt(maxSquaredDistance);
    }

    void RenderableMesh::scale(const Math::Vector3 &scale) {
        this->model.data[0] = scale.coordinates.x;
        this->model.data[5] = scale.coordinates.y;
        this->model.data[10] = scale.coordinates.z;
    }

    void RenderableMesh::translate(const Math::Vector3 &translate) {
        this->model.setColumnVector(3, Math::Vector4(translate, 1));
    }

    void RenderableMesh::scaleUniform(const Math::Scalar& scaleValue) {
        this->scale(Math::Vector3(scaleValue, scaleValue, scaleValue));
    }

    void RenderableMesh::translateUniform(const Math::Scalar& translateValue) {
        this->translate(Math::Vector3(translateValue, translateValue, translateValue));
    }

    Math::Vector3 RenderableMesh::getPosition() {
        return Math::Vector3(this->getModel().data[3], this->getModel().data[7], this->getModel().data[11]);
    }

    Math::Vector3 RenderableMesh::getScale() {
        return Math::Vector3(this->getModel().data[0], this->getModel().data[5], this->getModel().data[10]);
    }

    Math::Vector3 RenderableMesh::getCurrentScale() {
        return Math::Vector3(model.data[0], model.data[5], model.data[10]);
    }

    Math::Vector3 RenderableMesh::getCurrentTranslation() {
        return Math::Vector3(model.data[3], model.data[7], model.data[11]);
    }

    void RenderableMesh::setup() {
        isPickable = false;
        model = Math::Matrix4();
        
        std::vector<float> vertexFloats;
        vertexFloats.reserve(vertices.size() * 6); // 3 for position, 3 for normals

        for (const auto& vertex : vertices) {
            vertexFloats.push_back(vertex.position.coordinates.x);
            vertexFloats.push_back(vertex.position.coordinates.y);
            vertexFloats.push_back(vertex.position.coordinates.z);
            
            vertexFloats.push_back(vertex.normal.coordinates.x);
            vertexFloats.push_back(vertex.normal.coordinates.y);
            vertexFloats.push_back(vertex.normal.coordinates.z);
        }

        std::vector<int> faceIndices;
        faceIndices.reserve(faces.size() * 3);

        for (const auto& face : faces) {
            faceIndices.push_back(face.i);
            faceIndices.push_back(face.j);
            faceIndices.push_back(face.k);
        }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexFloats.size() * sizeof(float), vertexFloats.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndices.size() * sizeof(unsigned int), faceIndices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void RenderableMesh::setColors(const std::vector<Math::Vector3>& colors) {
        // Update colors in CPU-side vertex data
        for (size_t i = 0; i < colors.size() && i < vertices.size(); i++)
            vertices[i].color = colors[i];

        updateGPUVertexData();
    }

    void RenderableMesh::setUniformColor(Math::Vector3 color) {
        for (auto& vertex : vertices)
            vertex.color = color;

        updateGPUVertexData();
    }

    void RenderableMesh::updateGPUVertexData() {
        // Pack vertex data
        std::vector<float> vertexFloats;
        vertexFloats.reserve(vertices.size() * 11);

        for (const auto& vertex : vertices) {
            vertexFloats.push_back(vertex.position.coordinates.x);
            vertexFloats.push_back(vertex.position.coordinates.y);
            vertexFloats.push_back(vertex.position.coordinates.z);
            
            vertexFloats.push_back(vertex.normal.coordinates.x);
            vertexFloats.push_back(vertex.normal.coordinates.y);
            vertexFloats.push_back(vertex.normal.coordinates.z);
        }

        // Update the GPU-side vertex data
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexFloats.size() * sizeof(float), vertexFloats.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    void RenderableMesh::setWireframe(bool isActive) {
        this->isWireframe = isActive;
        
        if (isActive) {
            this->isFilled = false;
            this->isBlended = false;
        }
    }

    void RenderableMesh::setFilled(bool isActive) {
        this->isFilled = isActive;
        
        if (isActive) {
            this->isWireframe = false;
            this->isBlended = false;
        }
    }

    void RenderableMesh::setBlended(bool isActive) {
        this->isBlended = isActive;
        
        if (isActive) {
            this->isWireframe = false;
            this->isFilled = false;
        }
    }

    void RenderableMesh::setWireframeColor(const Math::Vector3& color) {
        wireframeColor = color;
        wireframeColorSetted = true;
    }

    void RenderableMesh::render() {
        shader->use();
        shader->setMat4("model", getModel());
        
        if (isFilled){
            shader->setVec3("material.ambient", vertices[0].color);
            shader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
            shader->setVec3("material.specular", Math::Vector3(0, 0, 0));
            shader->setFloat("material.shininess", 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
            if (!isPickable) {
                glDepthMask(GL_FALSE);
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                glDepthMask(GL_TRUE);
            }
            else {
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }
        }
        else if (isWireframe) {
            Math::Vector3 fillColor = vertices[0].color;
            
            if (wireframeColorSetted)
                this->setUniformColor(wireframeColor);
            
            shader->setVec3("material.ambient", vertices[0].color);
            shader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
            shader->setVec3("material.specular", Math::Vector3(0, 0, 0));
            shader->setFloat("material.shininess", 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_CULL_FACE);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1,-1);
            if (!isPickable) {
                glDepthMask(GL_FALSE);
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                glDepthMask(GL_TRUE);
            }
            else {
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }
            glDisable(GL_POLYGON_OFFSET_LINE);
            this->setUniformColor(fillColor);
        } else if (isBlended) {
            shader->setVec3("material.ambient", vertices[0].color);
            shader->setVec3("material.diffuse", Math::Vector3(0.9, 0.9, 0.9));
            shader->setVec3("material.specular", Math::Vector3(0, 0, 0));
            shader->setFloat("material.shininess", 0);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glUseProgram(0);
            glDisable(GL_BLEND);
        }
    }
}
