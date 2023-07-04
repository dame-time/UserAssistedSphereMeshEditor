#ifndef __POINT4_H__
#define __POINT4_H__

#include <Scalar.hpp>
#include <Vector4.hpp>
#include <Versor4.hpp>

#include <iostream>
#include <stdexcept>

namespace Math
{
    class Vector4;
    class Versor4;
    
    template<typename K>
    struct Point4Coordinates
    {
        K x;
        K y;
        K z;
        K w;
    };

    class Point4 {
        public:
            Point4Coordinates<Scalar> coordinates;

            Point4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w);
            Point4(const Point4& point);
            Point4(const Vector4& vector);
            Point4();

            Vector4 operator + (const Point4& point) const;
            Point4 operator + (const Vector4& point) const;
            Point4 operator + (const Versor4& point) const;
            Vector4 operator - (const Point4& point) const;
            Point4 operator - (const Vector4& point) const;
            Point4 operator - (const Versor4& point) const;
            Point4 operator * (const Scalar& k) const;
            Point4 operator / (const Scalar& k) const;
            Scalar operator [] (const int& i) const;
            Scalar& operator [] (const int& i);

            Point4 lerp(const Point4& p, const Scalar& t) const;
            void lerp(const Point4& p, const Scalar& t);

            Vector4 asVector4() const;

            void print() const;
    };
} // namespace Math


#endif // __POINT4_H__