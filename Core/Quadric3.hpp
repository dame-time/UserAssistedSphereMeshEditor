#pragma once

#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Matrix4.hpp>

#include <Math.hpp>
#include <Quaternion.hpp>

#include <iostream>

namespace Renderer
{
    class Quadric;

    class Quadric3
    {
        private:
            Math::Matrix3 A;
            Math::Vector3 b;
            Math::Scalar c;
        
        public:
            Quadric3();
            Quadric3(const Quadric& q, const Math::Scalar& constR);
        
            Math::Matrix3 getA() const
            {
                return A;
            }
        
            Math::Vector3 getB() const
            {
                return b;
            }
        
            Math::Scalar getC() const
            {
                return c;
            }

            Quadric3 operator + (const Quadric3& quadric);
            Quadric3 operator * (const Math::Scalar& multiplier);

            void operator += (const Quadric3& quadric);
            void operator *= (const Math::Scalar& multiplier);
        
            Math::Scalar evaluateSQEM (const Math::Vector3& sphereOrigin) const;
            Math::Vector3 minimizer () const;
        
            friend std::ostream& operator<<(std::ostream& os, const Quadric3& q);
            void print();
    };
}
