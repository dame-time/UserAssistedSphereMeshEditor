#pragma once

#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Matrix4.hpp>

#include <Math.hpp>
#include <Quaternion.hpp>

#include <RenderableMesh.hpp>

#include <iostream>

namespace Renderer
{
    class Quadric
    {
        public:
            Math::Matrix4 A;
            Math::Vector4 b;
            Math::Scalar c;
            
            Quadric();
            Quadric(const Math::Vector3& faceOrigin, const Math::Vector3& faceNormal);
            Quadric(const Quadric& other)
            {
                this->A = other.A;
                this->b = other.b;
                this->c = other.c;
            }

            static Quadric initializeQuadricFromVertex(const Vertex& vertex, Math::Scalar targetSphereRadius = 1.0f)
            {
                Quadric q;

                // (p * I * p - 2 * t * p + t * t) * weight -> con t sfera target, ovvero mi sposto sulla normale nella direzione negativa t = (vertex.position - k * vertex.normal, k) -> (sfera)
                Math::Vector3 n = vertex.normal;

                Math::Vector4 t = Math::Vector4(vertex.position - targetSphereRadius * n, targetSphereRadius);

                q.A = Math::Matrix4();
                q.b = (-t) * 2;
                q.c = t.dot(t);

                return  q;
            }
        
            Math::Matrix4 getA() const
            {
                return A;
            }
        
            Math::Vector4 getB() const
            {
                return b;
            }
        
            Math::Scalar getC() const
            {
                return c;
            }

            Quadric operator + (const Quadric& quadric);
            Quadric operator * (const Math::Scalar& multiplier);

            void operator += (const Quadric& quadric);
            void operator *= (const Math::Scalar& multiplier);

            Math::Scalar evaluateSQEM (const Math::Vector4& sphere) const;
            Math::Vector4 minimizer ();
            
            Math::Vector4 constrainIntoVector(const Math::Vector3& start, const Math::Vector3& end, const Math::Scalar& radius);
            Math::Vector4 constrainR(const Math::Scalar& radius);
        
            void addQuadricToTargetRadius(const Math::Scalar& t);

            void print ();
    };
}
