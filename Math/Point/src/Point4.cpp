#include <Point4.hpp>

namespace Math {

    Point4::Point4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w)
    {
        this->coordinates.x = x;
        this->coordinates.y = y;
        this->coordinates.z = z;
        this->coordinates.w = w;
    }

    Point4::Point4(const Point4& point)
    {
        this->coordinates.x = point.coordinates.x;
        this->coordinates.y = point.coordinates.y;
        this->coordinates.z = point.coordinates.z;
        this->coordinates.w = point.coordinates.w;
    }

    Point4::Point4(const Vector4& vector)
    {
        this->coordinates.x = vector.coordinates.x;
        this->coordinates.y = vector.coordinates.y;
        this->coordinates.z = vector.coordinates.z;
        this->coordinates.w = vector.coordinates.w;
    }

    Point4::Point4()
    {
        this->coordinates.x = 0;
        this->coordinates.y = 0;
        this->coordinates.z = 0;
        this->coordinates.w = 0;
    }

    Vector4 Point4::operator + (const Point4& point) const
    {
        return Vector4(
            this->coordinates.x + point.coordinates.x, 
            this->coordinates.y + point.coordinates.y, 
            this->coordinates.z + point.coordinates.z, 
            this->coordinates.w + point.coordinates.w
        );
    }

    Point4 Point4::operator + (const Vector4& vector) const
    {
        return Point4(this->coordinates.x + vector.coordinates.x, 
            this->coordinates.y + vector.coordinates.y, 
            this->coordinates.z + vector.coordinates.z, 
            this->coordinates.w + vector.coordinates.w
        );
    }

    Point4 Point4::operator + (const Versor4& versor) const
    {
        return Point4(
            this->coordinates.x + versor.coordinates.x, 
            this->coordinates.y + versor.coordinates.y, 
            this->coordinates.z + versor.coordinates.z, 
            this->coordinates.w + versor.coordinates.w
        );
    }

    Vector4 Point4::operator - (const Point4& point) const
    {
        return Vector4(
            this->coordinates.x - point.coordinates.x, 
            this->coordinates.y - point.coordinates.y, 
            this->coordinates.z - point.coordinates.z, 
            this->coordinates.w - point.coordinates.w
        );
    }

    Point4 Point4::operator - (const Vector4& vector) const
    {
        return Point4(
            this->coordinates.x - vector.coordinates.x, 
            this->coordinates.y - vector.coordinates.y, 
            this->coordinates.z - vector.coordinates.z, 
            this->coordinates.w - vector.coordinates.w
        );
    }

    Point4 Point4::operator - (const Versor4& versor) const
    {
        return Point4(
            this->coordinates.x - versor.coordinates.x, 
            this->coordinates.y - versor.coordinates.y, 
            this->coordinates.z - versor.coordinates.z, 
            this->coordinates.w - versor.coordinates.w
        );
    }

    Point4 Point4::operator * (const Scalar& k) const
    {
        return Point4(this->coordinates.x * k, this->coordinates.y * k, this->coordinates.z * k, this->coordinates.w * k);
    }

    Point4 Point4::operator / (const Scalar& k) const
    {
        return Point4(this->coordinates.x / k, this->coordinates.y / k, this->coordinates.z / k, this->coordinates.w / k);
    }

    Scalar Point4::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        case 2:
            return this->coordinates.z;
        case 3:
            return this->coordinates.w;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector4 the index might be either 0, 1, 2 or 3");
    }

    Scalar& Point4::operator [] (const int& i)
    {
        switch (i)
        {
        case 0:
            return this->coordinates.x;
        case 1:
            return this->coordinates.y;
        case 2:
            return this->coordinates.z;
        case 3:
            return this->coordinates.w;
        default:
            break;
        }

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector4 the index might be either 0, 1, 2 or 3");
    }

    Point4 Point4::lerp(const Point4& p, const Scalar& t) const
    {
        return Point4(*this * (1 - t) + p * t);
    }

    void Point4::lerp(const Point4& p, const Scalar& t)
    {
        auto tmp = Point4(*this * (1 - t) + p * t);

        this->coordinates.x = tmp.coordinates.x;
        this->coordinates.y = tmp.coordinates.y;
        this->coordinates.z = tmp.coordinates.z;
        this->coordinates.w = tmp.coordinates.w;
    }

    Vector4 Point4::asVector4() const
    {
        return Vector4(this->coordinates.x, this->coordinates.y, this->coordinates.z, this->coordinates.w);
    }

    void Point4::print() const
    {
        std::cout << "Point4: ( " << this->coordinates.x <<  ", " << this->coordinates.y << ", " << this->coordinates.z << ", " << this->coordinates.w <<" )" << std::endl;
    }

}