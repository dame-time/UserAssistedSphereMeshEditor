#include <Point3.hpp>

namespace Math {

    Point3::Point3(const Scalar& x, const Scalar& y, const Scalar& z)
    {
        this->coordinates.x = x;
        this->coordinates.y = y;
        this->coordinates.z = z;
    }

    Point3::Point3(const Point3& point)
    {
        this->coordinates.x = point.coordinates.x;
        this->coordinates.y = point.coordinates.y;
        this->coordinates.z = point.coordinates.z;
    }

    Point3::Point3(const Vector3& vector)
    {
        this->coordinates.x = vector.coordinates.x;
        this->coordinates.y = vector.coordinates.y;
        this->coordinates.z = vector.coordinates.z;
    }

    Point3::Point3()
    {
        this->coordinates.x = 0;
        this->coordinates.y = 0;
        this->coordinates.z = 0;
    }

    Vector3 Point3::operator + (const Point3& point) const
    {
        return Vector3(this->coordinates.x + point.coordinates.x, this->coordinates.y + point.coordinates.y, this->coordinates.z + point.coordinates.z);
    }

    Point3 Point3::operator + (const Vector3& vector) const
    {
        return Point3(this->coordinates.x + vector.coordinates.x, this->coordinates.y + vector.coordinates.y, this->coordinates.z + vector.coordinates.z);
    }

    Point3 Point3::operator + (const Versor3& versor) const
    {
        return Point3(this->coordinates.x + versor.coordinates.x, this->coordinates.y + versor.coordinates.y, this->coordinates.z + versor.coordinates.z);
    }

    Vector3 Point3::operator - (const Point3& point) const
    {
        return Vector3(this->coordinates.x - point.coordinates.x, this->coordinates.y - point.coordinates.y, this->coordinates.z - point.coordinates.z);
    }

    Point3 Point3::operator - (const Vector3& vector) const
    {
        return Point3(this->coordinates.x - vector.coordinates.x, this->coordinates.y - vector.coordinates.y, this->coordinates.z - vector.coordinates.z);
    }

    Point3 Point3::operator - (const Versor3& versor) const
    {
        return Point3(this->coordinates.x - versor.coordinates.x, this->coordinates.y - versor.coordinates.y, this->coordinates.z - versor.coordinates.z);
    }

    Point3 Point3::operator * (const Scalar& k) const
    {
        return Point3(this->coordinates.x * k, this->coordinates.y * k, this->coordinates.z * k);
    }

    Point3 Point3::operator / (const Scalar& k) const
    {
        return Point3(this->coordinates.x / k, this->coordinates.y / k, this->coordinates.z / k);
    }

    Scalar Point3::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        case 2:
            return this->coordinates.z;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector3 the index might be either 0, 1 or 2");
    }

    Scalar& Point3::operator [] (const int& i)
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        case 2:
            return this->coordinates.z;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector3 the index might be either 0, 1 or 2");
    }

    Point3 Point3::lerp(const Point3& p, const Scalar& t) const
    {
        return Point3(*this * (1 - t) + p * t);
    }

    void Point3::lerp(const Point3& p, const Scalar& t)
    {
        auto tmp = Point3(*this * (1 - t) + p * t);

        this->coordinates.x = tmp.coordinates.x;
        this->coordinates.y = tmp.coordinates.y;
        this->coordinates.z = tmp.coordinates.z;
    }

    Vector3 Point3::asVector3() const
    {
        return Vector3(this->coordinates.x, this->coordinates.y, this->coordinates.z);
    }

    void Point3::print() const
    {
        std::cout << "Point3: ( " << this->coordinates.x <<  ", " << this->coordinates.y << ", " << this->coordinates.z <<" )" << std::endl;
    }

}