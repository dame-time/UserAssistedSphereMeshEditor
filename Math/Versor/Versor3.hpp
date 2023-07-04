#ifndef __VERSOR3_H__
#define __VERSOR3_H__

#include <Vector3.hpp>
#include <Vector2.hpp>
#include <Scalar.hpp>

namespace Math
{
    class Vector2;
    class Vector3;

    template<typename K>
    struct Versor3Coordinates
    {
        K x;
        K y;
        K z;
    };

    class Versor3 {
        private:
            Versor3Coordinates<Scalar> _coordinates;
        
        public:
            const Versor3Coordinates<Scalar>& coordinates;

            Versor3(const Scalar& x, const Scalar& y, const Scalar& z);
            Versor3(const Vector2& vector2, const Scalar& z);
            Versor3(const Vector3& vector);
            Versor3(const Versor3& versor);
            Versor3();

            static Scalar dot(const Versor3& v1, const Versor3& v2);
            static Scalar dot(const Versor3& v1, const Vector3& v2);
            static Scalar dot(const Vector3& v1, const Versor3& v2);

            Scalar operator [] (const int& i) const;
            Scalar operator * (const Vector3& vector) const;
            Scalar operator * (const Versor3& versor) const;
            Versor3 operator + () const;
            Vector3 operator + (const Vector3& vector) const;
            Vector3 operator + (const Versor3& vector) const;
            Versor3 operator - () const;
            Vector3 operator - (const Vector3& vector) const;
            Vector3 operator - (const Versor3& vector) const;
            Vector3 operator * (const Scalar& k) const;
            Vector3 operator / (const Scalar& k) const;

            void nlerp(const Versor3& v, const Scalar& t);

            Vector3 asVector3() const;
    };
} // namespace Math


#endif // __VERSOR3_H__