#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <Scalar.hpp>
#include <Vector2.hpp>
#include <Versor3.hpp>
#include <Point3.hpp>
#include <Math.hpp>

#include <stdexcept>

namespace Math
{
    template<typename K>
    struct Vector3Coordinates
    {
        K x;
        K y;
        K z;
    };

    class Vector2;
    class Versor3;
    class Point3;
    class Matrix3;
    class Matrix4;
    class Vector4;

    class Vector3
    {
        public:
            Vector3Coordinates<Scalar> coordinates;

            Vector3();
            Vector3(const Scalar& x, const Scalar& y, const Scalar& z);
            Vector3(const Vector2& vector, const Scalar& z);
            Vector3(const Vector3& vector);
            Vector3(const Scalar* vector);
        
            static Vector3 cross(const Vector3& vector1, const Vector3& vector2);

            static Scalar dot(const Vector3& vector1, const Vector3& vector2);

            static Vector3 up();
            static Vector3 down();
            static Vector3 right();
            static Vector3 left();
            static Vector3 forward();
            static Vector3 backward();
        
            static Vector3 unProject(Vector3 wincoord, Matrix4 view, Matrix4 projection, Vector4 viewport);

            Scalar dot(const Vector3& vector) const;
            Vector3 cross(const Vector3& vector) const;
            Vector3 componentWise(const Vector3& vector) const;
            Matrix3 outer(const Vector3& vector) const;

            Vector3 componentWiseMinimum(const Vector3& vector) const;
            Vector3 componentWiseMaximum(const Vector3& vector) const;

            Scalar magnitude() const;
            Scalar squareMagnitude() const;
            Vector3 normalized() const;
            void normalize();
            static Vector3 normalize(const Vector3& vec);

            Scalar operator [] (const short& i) const;
            Scalar& operator [] (const short& i);

            Scalar operator * (const Vector3& vector) const;
            Vector3 operator + (const Vector3& vector) const;
            Vector3 operator - (const Vector3& vector) const;
            bool operator == (const Vector3& vector) const;
            Vector3 operator - () const;
            Vector3 operator * (const Scalar& k) const;
            Vector3 operator / (const Scalar& k) const;

            void operator += (const Vector3& vector);
            void operator -= (const Vector3& vector);
            void operator *= (const Scalar& k);
            void operator /= (const Scalar& k);

            static Scalar distance(const Vector3& vector1, const Vector3& vector2);
            Scalar angleBetween (const Vector3& vector) const;

            Vector3 lerp(const Vector3& vector, const Scalar& t) const;
            Vector3 lerp(const Vector3& vector1, const Vector3 vector2, const Scalar& t) const;
            
            bool isZero() const;
            void clear();

            void addScaledVector(const Vector3& vec, Scalar scale);

            Versor3 asVersor3() const;
            Point3 asPoint3() const;
            Scalar* asScalarVector() const;

            bool areEquals(const Vector3& vector) const;
            void print() const;
    };

    Vector3 operator * (const Scalar& k, const Vector3& vector);
    template<class T>
    inline T lerp (const T& a, const T& b, Scalar t)
    {
        return (1 - t) * a + t * b;
    }
}

namespace std
{
    template <>
    struct hash<Math::Vector3>
    {
            size_t operator()(const Math::Vector3 &v) const
            {
                return std::hash<Math::Scalar>()(v.coordinates.x) ^ std::hash<Math::Scalar>()(v.coordinates.y) ^ std::hash<Math::Scalar>()(v.coordinates.z);
            }
    };
}

#endif
