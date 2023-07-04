#ifndef MATRIX2_HPP
#define MATRIX2_HPP

#include "../Scalar.hpp"
#include "../Vector/Vector2.hpp"

#include <iostream>

namespace Math 
{
    class Vector2;
    
    class Matrix2
    {
    public:
        Scalar data[2 * 2];
        Matrix2();
        Matrix2(const Scalar& v1, const Scalar& v2,
                const Scalar& v3, const Scalar& v4);
        Matrix2(const Vector2& vec1, const Vector2& vec2);
        Matrix2(const Matrix2& mat);

        Matrix2 operator*(const Matrix2& mat) const;
        Vector2 operator*(const Vector2& vec) const;
        Matrix2 operator*(const Scalar& value) const;
        Matrix2 operator+(const Matrix2& mat) const;
        Matrix2 operator-(const Matrix2& mat) const;
        Matrix2 operator-() const;
        void operator=(const Matrix2& mat);

        void operator*=(const Matrix2& mat);
        void operator*=(const Scalar& value);
        void operator+=(const Matrix2& mat);
        void operator-=(const Matrix2& mat);

        bool operator==(const Matrix2& mat) const;

        void setInverse(const Matrix2& mat);
        Matrix2 inverse() const;
        static Matrix2 inverse(const Matrix2& mat);

        void setTranspose(const Matrix2& mat);
        Matrix2 transpose() const;
        static Matrix2 transpose(const Matrix2& mat);

        void print() const;

    };
}

#endif