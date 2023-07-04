#include <Versor4.hpp>

namespace Math {

    Versor4::Versor4() : coordinates(_coordinates)
    {
        this->_coordinates.x = 0;
        this->_coordinates.y = 0;
        this->_coordinates.z = 0;
        this->_coordinates.w = 0;
    }

    Versor4::Versor4(const Vector2& vector2, const Scalar& z, const Scalar& w) : coordinates(_coordinates)
    {
        this->_coordinates.x = vector2.coordinates.x;
        this->_coordinates.y = vector2.coordinates.y;
        this->_coordinates.z = z;
        this->_coordinates.w = w;
    }
    
    Versor4::Versor4(const Vector3& vector3, const Scalar& w) : coordinates(_coordinates)
    {
        this->_coordinates.x = vector3.coordinates.x;
        this->_coordinates.y = vector3.coordinates.y;
        this->_coordinates.z = vector3.coordinates.z;
        this->_coordinates.w = w;
    }

    Versor4::Versor4(const Versor4& versor) : coordinates(_coordinates)
    {
        this->_coordinates.x = versor.coordinates.x;
        this->_coordinates.y = versor.coordinates.y;
        this->_coordinates.z = versor.coordinates.z;
        this->_coordinates.z = versor.coordinates.w;
    }
    
    Versor4::Versor4(const Vector4& vector) : coordinates(_coordinates)
    {
        auto tmp = vector.normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
        this->_coordinates.z = tmp.coordinates.w;
    }
    
    Versor4::Versor4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w) : coordinates(_coordinates)
    {
        auto tmp = Vector4(x, y, z, w).normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
        this->_coordinates.w = tmp.coordinates.w;
    }
    
    Versor4 Versor4::operator - () const
    {
        return Versor4(-coordinates.x, -coordinates.y, -coordinates.z, -coordinates.w);
    }

    Vector4 Versor4::operator - (const Vector4& vector) const
    {
        return Vector4(_coordinates.x - vector.coordinates.x, _coordinates.y - vector.coordinates.y, _coordinates.z - vector.coordinates.z, _coordinates.w - vector.coordinates.w);
    }

    Vector4 Versor4::operator - (const Versor4& versor) const
    {
        return Vector4(_coordinates.x - versor.coordinates.x, _coordinates.y - versor.coordinates.y, _coordinates.z - versor.coordinates.z, _coordinates.w - versor.coordinates.w);
    }

    Vector4 Versor4::operator * (const Scalar& k) const
    {
        return Vector4(coordinates.x * k, coordinates.y * k, coordinates.z * k, coordinates.w * k);
    }

    Vector4 Versor4::operator / (const Scalar& k) const
    {
        return Vector4(coordinates.x / k, coordinates.y / k, coordinates.z / k, coordinates.w / k);
    }

    void Versor4::nlerp(const Versor4& v, const Scalar& t)
    {
        Vector4 v1 = Vector4(coordinates.x, coordinates.y, coordinates.z, coordinates.w);
        Vector4 v2 = Vector4(v.coordinates.x, v.coordinates.y, v.coordinates.z, v.coordinates.w);

        auto tmp = v1.lerp(v2, t).normalized();
        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
        this->_coordinates.w = tmp.coordinates.w;
    }

    Vector4 Versor4::asVector4() const
    {
        return Vector4(coordinates.x, coordinates.y, coordinates.z, coordinates.w);
    }

    Vector4 Versor4::operator + (const Versor4& versor) const
    {
        return Vector4(coordinates.x + versor.coordinates.x, coordinates.y + versor.coordinates.y, coordinates.z + versor.coordinates.z, coordinates.w + versor.coordinates.w);
    }

    Vector4 Versor4::operator + (const Vector4& vector) const
    {
        return Vector4(coordinates.x + vector.coordinates.x, coordinates.y + vector.coordinates.y, coordinates.z + vector.coordinates.z, coordinates.w + vector.coordinates.w);
    }

    Versor4 Versor4::operator + () const
    {
        return *this;
    }

    Scalar Versor4::operator * (const Versor4& versor) const
    {
        return dot(*this, versor);
    }

    Scalar Versor4::operator * (const Vector4& vector) const
    {
        return dot(*this, vector);
    }

    Scalar Versor4::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return coordinates.x;
        case 1:
            return coordinates.y;
        case 2:
            return coordinates.z;
        case 3:
            return coordinates.w;
        default:
            throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Versor4 the index might be either 0, 1, 2 or 3");
        }
    }

    Scalar Versor4::dot(const Vector4& v1, const Versor4& v2)
    {
        return v1.dot(v2.asVector4());
    }

    Scalar Versor4::dot(const Versor4& v1, const Vector4& v2)
    {
        return dot(v2, v1);
    }

    Scalar Versor4::dot(const Versor4& v1, const Versor4& v2)
    {
        return v1.asVector4().dot(v2.asVector4());
    }

}