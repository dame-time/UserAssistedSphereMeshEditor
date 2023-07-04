#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <Scalar.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Matrix3.hpp>
#include <Matrix4.hpp>
#include "../Matrix/Matrix4.hpp"
#include <Math.hpp>

#include <iostream>
#include <cmath>

#include <stdexcept>
#define DBL_EPSILON 2.2204460492503131e-16

namespace Math
{
    class Vector4;
    class Matrix4;

    class Quaternion
    {
        private:
            Vector3 _immaginary;
            Scalar _real;
        
        public:
            Vector3& immaginary;
            Scalar& real;

            Quaternion();
            Quaternion(const Scalar& a, const Scalar& b, const Scalar& c, const Scalar& d);
            Quaternion(const Vector3& immaginary, const Scalar& real);
            Quaternion(const Vector4& quaternion);
            Quaternion(const Quaternion& quaternion);

            static Quaternion Identity();

            static Quaternion AxisAngleToQuaternion(const Degree& angle, const Vector3& axis);
            
            static void QuaternionToAxisAngle(const Quaternion& quaternion, Scalar& angle, Vector3& axis);

            static Quaternion EulerAnglesToQuaternion(const Vector3& eulerAngles);

            static Vector3 QuaternionToEulerAngles(const Quaternion& quaternion);

            static Quaternion Matrix3ToQuaternion(const Matrix3& matrix);

            static Quaternion Matrix4ToQuaternion(const Matrix4& matrix);

            static Quaternion matToQuatCast(Matrix4& matrix);

            static Quaternion matToQuatCast(Matrix3& matrix);
            static Quaternion matToQuatCubemapCast(Matrix4& matrix);

            static Matrix3 QuaternionToMatrx3(const Quaternion& quaternion);

            static Vector4 AsVector4(const Quaternion& quaternion);

            static Quaternion RotationBetweenVectors(const Vector3& start, const Vector3& destination);

            Quaternion fromMatrix(const Matrix3& matrix) const;
            Quaternion fromEulerAngles(const Vector3& eulerAngles) const;
            Quaternion fromAxisAngle(const Degree& angle, const Vector3& axis) const;

            Matrix3 toMatrix3() const;
            Matrix4 toMatrix4() const;
            Vector3 toEulerAngles() const;
            void toAxisAngle(Degree& angle, Vector3& axis) const;

            void conjugate();
            Quaternion conjugated() const;

            Scalar squareMagnitude() const;
            Scalar magnitude() const;

            void invert();
            Quaternion inverse() const;

            // Returns a quaternion IN A DIFFERENT CONFIGURATION
            // q[0] -> q.w, q[1] -> q.x, ....
            Scalar operator [] (const short& i) const;
            Scalar& operator [] (const short& i);

            Quaternion operator /(const Scalar& k) const;
            Quaternion operator *(const Scalar& k) const;

            void operator /=(const Scalar& k);
            void operator *=(const Scalar& k);
            void operator +=(const Quaternion& quaternion);
            void operator -=(const Quaternion& quaternion);

            void operator = (const Quaternion& quaternion);

            bool operator ==(const Quaternion& quaternion) const;
            bool operator !=(const Quaternion& quaternion) const;

            // Rotate the vector passed as input by this quaternion
            Vector3 rotateVectorByThisQuaternion(const Vector3& vectorToRotate) const;

            void addScaledVector(const Vector3& vec, Scalar scale);

            Vector4 asVector4() const;

            Quaternion normalized() const;

            void normalize();

            static Vector3 normalizeAngles(Vector3& angles);
            static Scalar normalizeAngle(Scalar angle);

            void print() const;
    };

    Quaternion operator *(const Quaternion& a, const Quaternion& b);
}

#endif