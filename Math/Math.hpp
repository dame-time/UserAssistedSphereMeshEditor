#ifndef MATH_HPP
#define MATH_HPP

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <cmath>
#include <iostream>
#include <Vector3.hpp>

namespace Math
{
#ifdef LOW_PRECISON_MATH
	typedef float Scalar;
#else
    typedef double Scalar;
#endif

    const Scalar EPSILON = 1e-10;

    class Math
    {
        public:
            static Scalar radiansToDegreeAngle(const Scalar& angle) {
                return angle * (180 / (M_PI));
            }

            static Scalar degreeToRandiansAngle(const Scalar& angle) {
                return angle * ((M_PI) / 180);
            }
            
            static Scalar getAngleInRadiansFromCos(const Scalar& cosine) {
                return std::acos(cosine);
            }

            static Scalar getAngleInRadiansFromSin(const Scalar& sine) {
                return std::asin(sine);
            }

            static Scalar areEquals(const Scalar& a, const Scalar& b) {
                return std::abs(a - b) < EPSILON;
            }

            static Scalar clamp(const Scalar& start, const Scalar& end, const Scalar& value) {
                return value < start ? start : value > end ? end : value;
            }

            static Scalar clamp(const int& start, const int& end, const int& value) {
                return value < start ? start : value > end ? end : value;
            }

            static Scalar clamp01(const Scalar& value) {
                return value < 0 ? 0 : value > 1 ? 1 : value;
            }

            static Scalar clamp01(const int& value) {
                return value < 0 ? 0 : value > 1 ? 1 : value;
            }

            static Scalar inverseSqrt(const Scalar& value)
            {
                return static_cast<Scalar>(1) / static_cast<Scalar>(std::sqrt(static_cast<double>(value)));
            }

            // Linear interpolation: where t = 0 = a and t = 1 = b
            static Scalar lerp(const Scalar& a, const Scalar& b, const Scalar& t)
            {
                return (1.0f - t) * a + t * b;
            }

            // Linear interpolation: where t = a = 0 and t = b = 1
            static Scalar inverseLerp(const Scalar& a, const Scalar& b, const Scalar& t)
            {
                return (t - a) / (b - a);
            }

            static Scalar scalarAbs(const Scalar& sc)
            {
                return std::fabs(sc);
            }

            static Scalar scalarPow(const Scalar& sc, const Scalar& dt)
            {
                return std::powf(sc, dt);
            }

            static Scalar scalarSqrt(const Scalar& sc)
            {
                return std::sqrtf(sc);
            }

            static Scalar getPI()
            {
                return M_PI;
            }

            static Scalar sign(const Scalar& sc)
            {
                return (sc > 0.0f) ? 1.0f : ((sc < 0.0f) ? -1.0f : 0.0f);
            }

            static Scalar inverseScalarSqrt(const Scalar& sc)
            {
                union
                {
                    Scalar f;
                    uint32_t i;
                } conv{};
                Scalar x2;
                const Scalar threehalfs = 1.5f;

                x2 = sc * 0.5f;
                conv.f = sc;
                conv.i  = 0x5f3759df - ( conv.i >> 1 );
                conv.f  = conv.f * ( threehalfs - ( x2 * conv.f * conv.f ) );
                return conv.f;
                
            }
    };
}

#endif
