#include "../Quadric2.hpp"

#include <Quadric.hpp>

namespace Renderer
{
    Quadric2::Quadric2()
    {
        this->A = Math::Matrix2();
        this->b = Math::Vector2();
        this->c = 0;
    }

    Quadric2::Quadric2(const Quadric& q, const Math::Vector3& u, const Math::Vector3& v)
    {
        this->A = Math::Matrix2();
        
        auto qA1133 = q.getA().toMatrix3();
        auto qA4143 = Math::Vector3(q.getA().data[3], q.getA().data[7], q.getA().data[11]);
        auto qA44 = q.getA().data[15];
        
        auto mu = u - v;
        
        auto mux = mu.coordinates.x;
        auto muy = mu.coordinates.y;
        auto muz = mu.coordinates.z;
        
        auto a = qA1133.data[0];
        auto b = qA1133.data[1];
        auto c = qA1133.data[2];
        auto d = qA1133.data[3];
        auto e = qA1133.data[4];
        auto f = qA1133.data[5];
        auto g = qA1133.data[6];
        auto h = qA1133.data[7];
        auto i = qA1133.data[8];
        
        auto A00 = Math::Vector3(mux * a + muy * b + muz * c, mux * d + muy * e + muz * f, mux * g + muy * h + muz * i).dot(mu);
        auto A01 = qA4143.dot(mu);
        auto A10 = qA4143.dot(mu);
        auto A11 = qA44;
        
        this->A.data[0] = A00;
        this->A.data[1] = A01;
        this->A.data[2] = A10;
        this->A.data[3] = A11;
        
        this->b = Math::Vector2();
        
        auto qb13 = q.getB().toQuaternion().immaginary; // taking only the first 3 components of b with this trick
        auto qb44 = q.getB().coordinates.w;
        
        auto b0 = qb13.dot(mu) - Math::Vector3(mux * a + muy * b + muz * c, mux * d + muy * e + muz * f, mux * g + muy * h + muz * i).dot(mu);
        auto b1 = qb44 - qA4143.dot(mu);
        
        this->b = Math::Vector2(b0, b1);
        
        this->c = q.getC() - qb13.dot(mu) + 0.5 * Math::Vector3(mux * a + muy * b + muz * c, mux * d + muy * e + muz * f, mux * g + muy * h + muz * i).dot(mu);
    }

    Quadric2 Quadric2::operator + (const Quadric2& quadric)
    {
        Quadric2 result;

        result.A = this->A + quadric.A;
        result.b = this->b + quadric.b;
        result.c = this->c + quadric.c;

        return result;
    }

    Quadric2 Quadric2::operator * (const Math::Scalar& multiplier)
    {
        Quadric2 result;

        result.A = this->A * multiplier;
        result.b = this->b * multiplier;
        result.c = this->c * multiplier;

        return result;
    }

    void Quadric2::operator += (const Quadric2& quadric)
    {
        this->A = this->A + quadric.A;
        this->b = this->b + quadric.b;
        this->c = this->c + quadric.c;
    }

    void Quadric2::operator *= (const Math::Scalar& multiplier)
    {
        this->A = this->A * multiplier;
        this->b = this->b * multiplier;
        this->c = this->c * multiplier;
    }

    Math::Vector2 Quadric2::minimizer () const
    {
        Math::Vector2 result;
        
        const Math::Scalar minRadius = 0.01;

        try
        {
            result = A.inverse() * (-b/2);
        }
        catch (const std::exception& e)
        {
            // TODO: check if its better to implement Quadric1 or like this is fine
            std::cerr << "Matrix has determinat 0 for this quadric" << std::endl;
            
            auto Ahat = 0.5 * this->A.data[3];
            auto bhat = (this->A.data[1] / 4) + (this->A.data[2] / 4) + this->b.coordinates.y;
            
            auto radius = (-bhat / 2) / Ahat;
            return Math::Vector2(0.5, radius);
        }
        
        return result;
    }

    Math::Vector2 Quadric2::constrainR (const Math::Scalar& radius) const
    {
        auto Ahat = 0.5 * this->A.data[0] * this->A.data[2];
        auto bhat = 0.5 * radius * this->A.data[1] - this->b.coordinates.x;
        
        auto lambda = (-bhat / 2) / Ahat;
        return Math::Vector2(lambda, radius);
    }

    void Quadric2::print ()
    {
        this->A.print();
        this->b.print();
        std::cout << this->c << std::endl;
    }
}
