#ifndef __VERSOR4_H__
#define __VERSOR4_H__

#include <Vector4.hpp>
#include <Vector3.hpp>
#include <Vector2.hpp>
#include <Scalar.hpp>

namespace Math
{
    class Vector2;
    class Vector3;
    class Vector4;
    
    template<typename K>
    struct Versor4Coordinates
    {
        K x;
        K y;
        K z;
        K w;
    };

    class Versor4 {
        private:
            Versor4Coordinates<Scalar> _coordinates;
        
        public:
            const Versor4Coordinates<Scalar>& coordinates;

            Versor4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w);
            Versor4(const Vector2& vector2, const Scalar& z, const Scalar& w);
            Versor4(const Vector3& vector3, const Scalar& w);
            Versor4(const Vector4& vector);
            Versor4(const Versor4& versor);
            Versor4();

            static Scalar dot(const Versor4& v1, const Versor4& v2);
            static Scalar dot(const Versor4& v1, const Vector4& v2);
            static Scalar dot(const Vector4& v1, const Versor4& v2);

            Scalar operator [] (const int& i) const;
            Scalar operator * (const Vector4& vector) const;
            Scalar operator * (const Versor4& versor) const;
            Versor4 operator + () const;
            Vector4 operator + (const Vector4& vector) const;
            Vector4 operator + (const Versor4& vector) const;
            Versor4 operator - () const;
            Vector4 operator - (const Vector4& vector) const;
            Vector4 operator - (const Versor4& vector) const;
            Vector4 operator * (const Scalar& k) const;
            Vector4 operator / (const Scalar& k) const;

            void nlerp(const Versor4& v, const Scalar& t);

            Vector4 asVector4() const;
    };
} // namespace Math


#endif // __VERSOR4_H__