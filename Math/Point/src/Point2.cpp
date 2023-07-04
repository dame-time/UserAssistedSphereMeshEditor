#include <Point2.hpp>

namespace Math {
    
    Point2::Point2(const Scalar& x, const Scalar& y)
    {
        this->coordinates.x = x;
        this->coordinates.y = y;
    }

    Point2::Point2(const Point2& point)
    {
        this->coordinates.x = point.coordinates.x;
        this->coordinates.y = point.coordinates.y;
    }

    Point2::Point2(const Vector2& vector)
    {
        this->coordinates.x = vector.coordinates.x;
        this->coordinates.y = vector.coordinates.y;
    }

    Point2::Point2()
    {
        this->coordinates.x = 0;
        this->coordinates.y = 0;
    }

    Vector2 Point2::operator + (const Point2& point) const
    {
        return Vector2(this->coordinates.x + point.coordinates.x, this->coordinates.y + point.coordinates.y);
    }

    Point2 Point2::operator + (const Vector2& vector) const
    {
        return Point2(this->coordinates.x + vector.coordinates.x, this->coordinates.y + vector.coordinates.y);
    }

    Point2 Point2::operator + (const Versor2& versor) const
    {
        return Point2(this->coordinates.x + versor.coordinates.x, this->coordinates.y + versor.coordinates.y);
    }

    Vector2 Point2::operator - (const Point2& point) const
    {
        return Vector2(this->coordinates.x - point.coordinates.x, this->coordinates.y - point.coordinates.y);
    }

    Point2 Point2::operator - (const Vector2& vector) const
    {
        return Point2(this->coordinates.x - vector.coordinates.x, this->coordinates.y - vector.coordinates.y);
    }

    Point2 Point2::operator - (const Versor2& versor) const
    {
        return Point2(this->coordinates.x - versor.coordinates.x, this->coordinates.y - versor.coordinates.y);
    }

    Point2 Point2::operator * (const Scalar& k) const
    {
        return Point2(this->coordinates.x * k, this->coordinates.y * k);
    }

    Point2 Point2::operator / (const Scalar& k) const
    {
        return Point2(this->coordinates.x / k, this->coordinates.y / k);
    }

    Scalar Point2::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector3 the index might be either 0 or 1");
    }

    Scalar& Point2::operator [] (const int& i)
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector3 the index might be either 0 or 1");
    }

    Point2 Point2::lerp(const Point2& p, const Scalar& t) const
    {
        return Point2(*this * (1 - t) + p * t);
    }

    void Point2::lerp(const Point2& p, const Scalar& t)
    {
        auto tmp = Point2(*this * (1 - t) + p * t);

        this->coordinates.x = tmp.coordinates.x;
        this->coordinates.y = tmp.coordinates.y;
    }

    Vector2 Point2::asVector2() const
    {
        return Vector2(this->coordinates.x, this->coordinates.y);
    }

    void Point2::print() const
    {
        std::cout << "Point2: ( " << this->coordinates.x <<  ", " << this->coordinates.y << " )" << std::endl;
    }

}