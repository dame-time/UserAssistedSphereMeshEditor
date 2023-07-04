#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <Scalar.hpp>
#include <Math.hpp>
#include <Vector3.hpp>
#include <Versor2.hpp>
#include <Point2.hpp>

#include <iostream>
#include <cmath>
#include <stdexcept>

namespace Math
{
    template<typename K>
    struct Vector2Coordinates
    {
        K x;
        K y;
    };

    // Forward declaration
    class Vector3;
    class Versor2;
    class Point2;

    class Vector2
    {
        public:
            Vector2Coordinates<Scalar> coordinates;

            Vector2();
            Vector2(Scalar x, Scalar y);
            Vector2(const Vector2& vector);
            Vector2(const Versor2& versor);

            Scalar dot(const Vector2& vector) const;
            Vector3 cross(const Vector2& vector) const;
            Vector2 componentWise(const Vector2& vector) const;

            Scalar magnitude() const;
            Scalar squareMagnitude() const;
            Vector2 normalized() const;
            void normalize();

            Scalar operator [] (const short& i) const; // Readonly acess to coordinates
            Scalar& operator [] (const short& i); // Read & Write acess to coordinates

            Scalar operator * (const Vector2& vector) const;
            Vector2 operator + (const Vector2& vector) const;
            Vector2 operator - (const Vector2& vector) const;
            bool operator == (const Vector2& vector) const;

            Vector2 operator - () const;
            Vector2 operator * (const Scalar& k) const;
            Vector2 operator / (const Scalar& k) const;

            void operator += (const Vector2& vector);
            void operator -= (const Vector2& vector);
            void operator *= (const Scalar& k);
            void operator /= (const Scalar& k);

            Scalar angleBetween (const Vector2& vector) const;

            Vector2 lerp(const Vector2& vector, const Scalar& t) const;
            
            bool isZero() const;
            bool areEquals(const Vector2& vector) const;

            Versor2 asVersor2() const;
            Point2 asPoint2() const;

            void print() const;
    };
}

#endif