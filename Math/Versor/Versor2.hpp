#ifndef __VERSOR2_H__
#define __VERSOR2_H__

#include <Vector2.hpp>
#include <Scalar.hpp>

#include <stdexcept>

namespace Math
{
    class Vector2;
    
    template<typename K>
    struct Versor2Coordinates
    {
        K x;
        K y;
    };

    class Versor2 {
        private:
            Versor2Coordinates<Scalar> _coordinates;
        
        public:
            const Versor2Coordinates<Scalar>& coordinates;

            Versor2(const Scalar& x, const Scalar& y);
            Versor2(const Vector2& vector);
            Versor2(const Versor2& versor);
            Versor2();

            static Scalar dot(const Versor2& v1, const Versor2& v2);
            static Scalar dot(const Versor2& v1, const Vector2& v2);
            static Scalar dot(const Vector2& v1, const Versor2& v2);

            Scalar operator [] (const int& i) const;
            Scalar operator * (const Vector2& vector) const;
            Scalar operator * (const Versor2& versor) const;
            Versor2 operator + () const;
            Vector2 operator + (const Vector2& vector) const;
            Vector2 operator + (const Versor2& vector) const;
            Versor2 operator - () const;
            Vector2 operator - (const Vector2& vector) const;
            Vector2 operator - (const Versor2& vector) const;
            Vector2 operator * (const Scalar& k) const;
            Vector2 operator / (const Scalar& k) const;

            void nlerp(const Versor2& v, const Scalar& t);

            Vector2 asVector2() const;
    };
} // namespace Math

#endif // __VERSOR2_H__