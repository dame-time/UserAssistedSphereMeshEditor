#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include <Scalar.hpp>
#include <Vector3.hpp>
#include <Versor4.hpp>
#include <Matrix4.hpp>
#include <Point4.hpp>
#include <Quaternion.hpp>
#include <Math.hpp>

#include <iostream>
#include <cmath>
#include <stdexcept>

namespace Math 
{
    template<typename K>
    struct Vector4Coordinates
    {
        K x;
        K y;
        K z;
        K w;
    };

    class Quaternion;
    class Vector3;
    class Versor4;
    class Point4;

    class Vector4
    {
        public:
            Vector4Coordinates<Scalar> coordinates;

            Vector4();
            Vector4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w);
            Vector4(const Vector2& vector, const Scalar& z, const Scalar& w);
            Vector4(const Vector3& vector, const Scalar& w);
            Vector4(const Vector4& vector);

            [[nodiscard]] Scalar dot(const Vector4& vector) const;
            [[nodiscard]] Vector4 componentWise(const Vector4& vector) const;

            [[nodiscard]] Scalar magnitude() const;
            [[nodiscard]] Scalar squareMagnitude() const;

            [[nodiscard]] Vector4 normalized() const;
            void normalize();

            Scalar operator [] (const short& i) const;
            Scalar& operator [] (const short& i);
        
            // This operator * is the dot product
            Scalar operator * (const Vector4& vector) const;
            Vector4 operator + (const Vector4& vector) const;
            Vector4 operator - (const Vector4& vector) const;
            bool operator == (const Vector4& vector) const;

            Vector4 operator - () const;
            Vector4 operator * (const Scalar& k) const;
            Vector4 operator / (const Scalar& k) const;

            void operator += (const Vector4& vector);
            void operator -= (const Vector4& vector);
            void operator *= (const Scalar& k);
            void operator /= (const Scalar& k);

            Vector3 xyz();

            [[nodiscard]] Scalar angleBetween (const Vector4& vector) const;

            [[nodiscard]] Vector4 lerp(const Vector4& vector, const Scalar& t) const;
            [[nodiscard]] Vector4 lerp(const Vector4 &v1, const Vector4 &v2, const Scalar &t) const;

            [[nodiscard]] Quaternion toQuaternion() const;

            [[nodiscard]] bool isZero() const;
            [[nodiscard]] bool areEquals(const Vector4& vector) const;
        
            [[nodiscard]] Vector3 truncateToVector3() const;

            [[nodiscard]] Versor4 asVersor4() const;
            [[nodiscard]] Point4 asPoint4() const;

            void print() const;
    };
}

#endif
