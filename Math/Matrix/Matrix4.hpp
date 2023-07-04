#ifndef MATRIX4_HPP
#define MATRIX4_HPP

#define _USE_MATH_DEFINES
#include "../Scalar.hpp"
#include "../Vector/Vector3.hpp"
#include "../Vector/Vector4.hpp"
#include "Matrix3.hpp"
#include <Quaternion.hpp>

#include <limits>

namespace Math 
{
    class Quaternion;
    
    class Matrix4
    {
    public:
        Scalar data[4 * 4];

        Matrix4();
        Matrix4(const Scalar& value);
        Matrix4(const Vector4& vec1, const Vector4& vec2, const Vector4& vec3, const Vector4& vec4);
        Matrix4(const Matrix4& mat);
        Matrix4(const Scalar& v1, const Scalar& v2, const Scalar& v3, const Scalar& v4,
                const Scalar& v5, const Scalar& v6, const Scalar& v7, const Scalar& v8,
                const Scalar& v9, const Scalar& v10, const Scalar& v11, const Scalar& v12,
                const Scalar& v13, const Scalar& v14, const Scalar& v15, const Scalar& v16);

        void setZero();

        void operator*=(const Matrix4& mat);
        void operator*=(const Scalar& value);
        void operator+=(const Matrix4& mat);
        void operator-=(const Matrix4& mat);
        void operator/=(const Scalar& dividend);

        Matrix4 operator*(const Matrix4& mat) const;
        Vector4 operator*(const Vector4& vec) const;
        Matrix4 operator*(const Scalar& value) const;
        Matrix4 operator+(const Matrix4& mat) const;
        Matrix4 operator-(const Matrix4& mat) const;
        Matrix4 operator-() const;
        Matrix4 operator/(const Scalar& dividend) const;
        
        void operator=(const Matrix4& mat);
        
        bool operator==(const Matrix4& mat) const;

        void setInverse(const Matrix4& mat);
        Matrix4 inverse() const;
        static Matrix4 inverse(const Matrix4& mat);
        Matrix3 toMatrix3();
        static Matrix3 toMatrix3(const Matrix4& mat);
        Matrix4 transposed() const;

        void setTopLeftMatrix3(const Matrix3& mat);
        void setRowVector(int rowIndex, const Vector4& vector);
        void setColumnVector(int columnIndex, const Vector4& vector);

        //We transform direction vectors not considering the translation part
        Vector3 transformDirection(const Vector3& vec) const;
        Vector3 transformInverseDirection(const Vector3& vec) const;
        Vector3 localToWorldDir(const Vector3& local, const Matrix4& transform) const;
        Vector3 worldToLocalDir(const Vector3& local, const Matrix4& transform) const;

        /**
         * This method performs the inverse transformation assuming that the matrix used
         * has on the 3x3 only the rotation (no shear and scaling, since we'll use the transpose)
         * and on the last column the translation component
        */
        Vector3 transformInverse(const Vector3& vec) const;

        Vector3 transform(const Vector3& vec) const;

        Vector3 getAxisVector(unsigned int index) const;

        // Decomposing a Matrix4 into 3 different components - Scale, Rotate & Translate 
        // in order to be able to break down matrices into engine usable components
        static bool DecomposeMatrixInScaleRotateTranslateComponents(
                                                                        const Matrix4& modelMatrix, 
                                                                        Vector3& scale, 
                                                                        Quaternion& rotation, 
                                                                        Vector3& translation, 
                                                                        Vector3* eulerAnglesRotation = nullptr, 
                                                                        Vector3* skew = nullptr
                                                                    );

        void print() const;
    };
}

#endif
