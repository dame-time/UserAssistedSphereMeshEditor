#ifndef __POINT2_H__
#define __POINT2_H__

#include <Scalar.hpp>
#include <Vector2.hpp>
#include <Versor2.hpp>

#include <iostream>
#include <stdexcept>

namespace Math
{
    class Vector2;
    class Versor2;
    
    template<typename K>
    struct Point2Coordinates
    {
        K x;
        K y;
    };

    class Point2 {
        public:
            Point2Coordinates<Scalar> coordinates;

            Point2(const Scalar& x, const Scalar& y);
            Point2(const Point2& point);
            Point2(const Vector2& vector);
            Point2();

            Vector2 operator + (const Point2& point) const;
            Point2 operator + (const Vector2& vector) const;
            Point2 operator + (const Versor2& versor) const;
            Vector2 operator - (const Point2& point) const;
            Point2 operator - (const Vector2& vector) const;
            Point2 operator - (const Versor2& versor) const;
            Point2 operator * (const Scalar& k) const;
            Point2 operator / (const Scalar& k) const;
            Scalar operator [] (const int& i) const;
            Scalar& operator [] (const int& i);

            Point2 lerp(const Point2& p, const Scalar& t) const;
            void lerp(const Point2& p, const Scalar& t);

            Vector2 asVector2() const;

            void print() const;
    };
} // namespace Math


#endif // __POINT2_H__