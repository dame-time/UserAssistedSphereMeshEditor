#include "../Quadric.hpp"

#include <Matrix3.hpp>

#include <Quadric3.hpp>
#include <Quadric2.hpp>

namespace Renderer
{
    Quadric::Quadric ()
    {
        A.setZero();
        b = Math::Vector4(0, 0, 0, 0);
        c = 0;
    }

    Quadric::Quadric (const Math::Vector3 &faceOrigin, const Math::Vector3 &faceNormal)
    {
        A.setZero();
        A.setTopLeftMatrix3(faceNormal.outer(faceNormal));
        A.setRowVector(3, Math::Vector4(faceNormal, 1));
        A.setColumnVector(3, Math::Vector4(faceNormal, 1));
        
        Math::Vector4 omogeneousFaceNormalPoint = Math::Vector4(faceNormal, 1);
        b = omogeneousFaceNormalPoint * (-2 * faceNormal.dot(faceOrigin));

        c = faceNormal.dot(faceOrigin) * faceNormal.dot(faceOrigin);
    }

    Quadric Quadric::operator + (const Quadric &quadric)
    {
        Quadric result;

        result.A = this->A + quadric.A;
        result.b = this->b + quadric.b;
        result.c = this->c + quadric.c;

        return result;
    }

    Quadric Quadric::operator * (const Math::Scalar &multiplier)
    {
        Quadric result;

        result.A = this->A * multiplier;
        result.b = this->b * multiplier;
        result.c = this->c * multiplier;

        return result;
    }

    void Quadric::operator += (const Quadric& quadric)
    {
        this->A = this->A + quadric.A;
        this->b = this->b + quadric.b;
        this->c = this->c + quadric.c;
    }

    void Quadric::operator *= (const Math::Scalar& multiplier)
    {
        this->A = this->A * multiplier;
        this->b = this->b * multiplier;
        this->c = this->c * multiplier;
    }

    Math::Scalar Quadric::evaluateSQEM (const Math::Vector4 &s) const
    {
        return s.dot(A * s) + b.dot(s) + c;
    }

    Math::Vector4 Quadric::minimizer()
    {
        Math::Vector4 result;

        try
        {
            result = A.inverse() * (-b/2);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Matrix has determinat 0 for this quadric" << std::endl;
            return Math::Vector4(-1, -1, -1, 0);
        }
        
        // TODO: MARCO Check if the new method works well
        if (result.coordinates.w < minRadius)
        {
//            std::cout << "IMPOSING NEW RADIUS" << std::endl;
//            Quadric3 q3 = Quadric3(*this, minRadius);
//            auto newOrigin = q3.minimizer();
//            result = Math::Vector4(newOrigin, minRadius);
            const int newQuadricWeight = 1000;

            Quadric q = Quadric();

            q.A = Math::Matrix4(0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 1);
            q.b = Math::Vector4(0, 0, 0, -minRadius * 2);
            q.c = minRadius;

            auto newQ = *this + (q * newQuadricWeight);

            result = newQ.A.inverse() * (-newQ.b/2);
        }

        return result;
    }

    Math::Vector4 Quadric::constrainIntoVector(const Math::Vector3& u, const Math::Vector3& v, const Math::Scalar& radius)
    {
        auto r = radius;
        
//        if (radius < minRadius)
//            r = minRadius;
        
        Quadric2 q2 = Quadric2(*this, u, v);
        auto minimizer = q2.minimizer();
        
        if (minimizer.coordinates.y > r)
            minimizer = q2.constrainR(r);
        
        auto mu = u - v;
        auto newOrigin = u + (minimizer.coordinates.x * mu);
        auto newRadius = minimizer.coordinates.y;
        
        return Math::Vector4(newOrigin, newRadius);
    }

    Math::Vector4 Quadric::constrainR(const Math::Scalar& radius)
    {
        auto r = radius;
        
//        if (radius < minRadius)
//            r = minRadius;
        
        // Using Quadric3 to find the new center
        Quadric3 q3 = Quadric3(*this, r);
        auto newOrigin = q3.minimizer();
        
        return Math::Vector4(newOrigin, radius);
    }

    void Quadric::addQuadricToTargetRadius(const Math::Scalar& t)
    {
        const int newQuadricWeight = 1000;

        Quadric q = Quadric();

        q.A = Math::Matrix4(0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1);
        q.b = Math::Vector4(0, 0, 0, -t * 2);
        q.c = t;

        *this = *this + q * newQuadricWeight;
    }

    void Quadric::print ()
    {
        this->A.print();
        this->b.print();
        std::cout << this->c << std::endl;
    }
}
