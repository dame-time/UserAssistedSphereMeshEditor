#include "../Quadric3.hpp"

#include <Quadric.hpp>

namespace Renderer
{
    Quadric3::Quadric3()
    {
        this->A = Math::Matrix3();
        this->b = Math::Vector3();
        this->c = 0;
    }

    Quadric3::Quadric3(const Quadric& q, const Math::Scalar& R)
    {
        auto M = q.getA();
        this->A = M.toMatrix3();
        
        auto b3 = q.getB().truncateToVector3();
        auto M13 = Math::Vector3(M.data[12], M.data[13], M.data[14]);
        this->b = b3 + 2 * R * M13;
        
        auto b1 = q.getB().coordinates.w;
        auto M11 = M.data[15];
        this->c = q.c + R * R * M11 + R * b1;
    }

    Quadric3 Quadric3::operator + (const Quadric3 &quadric)
    {
        Quadric3 result;

        result.A = this->A + quadric.A;
        result.b = this->b + quadric.b;
        result.c = this->c + quadric.c;

        return result;
    }

    Quadric3 Quadric3::operator * (const Math::Scalar &multiplier)
    {
        Quadric3 result;

        result.A = this->A * multiplier;
        result.b = this->b * multiplier;
        result.c = this->c * multiplier;

        return result;
    }

    void Quadric3::operator += (const Quadric3& quadric)
    {
        this->A = this->A + quadric.A;
        this->b = this->b + quadric.b;
        this->c = this->c + quadric.c;
    }

    void Quadric3::operator *= (const Math::Scalar& multiplier)
    {
        this->A = this->A * multiplier;
        this->b = this->b * multiplier;
        this->c = this->c * multiplier;
    }

    Math::Scalar Quadric3::evaluateSQEM (const Math::Vector3 &s) const
    {
        return s.dot(A * s) + b.dot(s) + c;
    }

    Math::Vector3 Quadric3::minimizer() const
    {
        Math::Vector3 result;

        try
        {
            result = A.inverse() * (-b/2);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Matrix has determinat 0 for this quadric3" << std::endl;
        }

        return result;
    }
}
