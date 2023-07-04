#ifndef __POINT3_H__
#define __POINT3_H__

#include <Scalar.hpp>
#include <Vector3.hpp>
#include <Versor3.hpp>

#include <iostream>
#include <stdexcept>

namespace Math
{
    class Vector3;
    class Versor3;

    template<typename K>
    struct Point3Coordinates
    {
        K x;
        K y;
        K z;
    };

    class Point3 {
        public:
            Point3Coordinates<Scalar> coordinates;

            Point3(const Scalar& x, const Scalar& y, const Scalar& z);
            Point3(const Point3& point);
            Point3(const Vector3& vector);
            Point3();

            Vector3 operator + (const Point3& point) const;
            Point3 operator + (const Vector3& point) const;
            Point3 operator + (const Versor3& point) const;
            Vector3 operator - (const Point3& point) const;
            Point3 operator - (const Vector3& point) const;
            Point3 operator - (const Versor3& point) const;
            Point3 operator * (const Scalar& k) const;
            Point3 operator / (const Scalar& k) const;
            Scalar operator [] (const int& i) const;
            Scalar& operator [] (const int& i);

            Point3 lerp(const Point3& p, const Scalar& t) const;
            void lerp(const Point3& p, const Scalar& t);

            Vector3 asVector3() const;

            void print() const;
    };
} // namespace Math


#endif // __POINT3_H__