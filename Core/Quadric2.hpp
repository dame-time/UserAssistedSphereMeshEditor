#pragma once

#include <Vector3.hpp>
#include <Vector2.hpp>
#include <Matrix2.hpp>

#include <Math.hpp>
#include <Quaternion.hpp>

#include <iostream>

namespace Renderer
{
    class Quadric;

    class Quadric2
    {
        public:
            Math::Matrix2 A;
            Math::Vector2 b;
            Math::Scalar c;
            
            Quadric2();
            Quadric2(const Quadric& q, const Math::Vector3& u, const Math::Vector3& v);
        
            Math::Matrix2 getA() const
            {
                return A;
            }
        
            Math::Vector2 getB() const
            {
                return b;
            }
        
            Math::Scalar getC() const
            {
                return c;
            }

            Quadric2 operator + (const Quadric2& quadric);
            Quadric2 operator * (const Math::Scalar& multiplier);

            void operator += (const Quadric2& quadric);
            void operator *= (const Math::Scalar& multiplier);

            Math::Vector2 minimizer () const;
            Math::Vector2 constrainR (const Math::Scalar& radius) const;

            void print ();
    };
}
