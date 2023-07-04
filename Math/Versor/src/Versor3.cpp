#include <Versor3.hpp>

namespace Math {

    Versor3::Versor3() : coordinates(_coordinates)
    {
        this->_coordinates.x = 0;
        this->_coordinates.y = 0;
        this->_coordinates.z = 0;
    }

    Versor3::Versor3(const Vector2& vector2, const Scalar& z) : coordinates(_coordinates)
    {
        this->_coordinates.x = vector2.coordinates.x;
        this->_coordinates.y = vector2.coordinates.y;
        this->_coordinates.z = z;
    }
    
    Versor3::Versor3(const Versor3& versor) : coordinates(_coordinates)
    {
        this->_coordinates.x = versor.coordinates.x;
        this->_coordinates.y = versor.coordinates.y;
        this->_coordinates.z = versor.coordinates.z;
    }
    
    Versor3::Versor3(const Vector3& vector) : coordinates(_coordinates)
    {
        auto tmp = vector.normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
    }
    
    Versor3::Versor3(const Scalar& x, const Scalar& y, const Scalar& z) : coordinates(_coordinates)
    {
        auto tmp = Vector3(x, y, z).normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
    }
    
    Versor3 Versor3::operator - () const
    {
        return Versor3(-coordinates.x, -coordinates.y, -coordinates.z);
    }

    Vector3 Versor3::operator - (const Vector3& vector) const
    {
        return Vector3(_coordinates.x - vector.coordinates.x, _coordinates.y - vector.coordinates.y, _coordinates.z - vector.coordinates.z);
    }

    Vector3 Versor3::operator - (const Versor3& versor) const
    {
        return Vector3(_coordinates.x - versor.coordinates.x, _coordinates.y - versor.coordinates.y, _coordinates.z - versor.coordinates.z);
    }

    Vector3 Versor3::operator * (const Scalar& k) const
    {
        return Vector3(coordinates.x * k, coordinates.y * k, coordinates.z * k);
    }

    Vector3 Versor3::operator / (const Scalar& k) const
    {
        return Vector3(coordinates.x / k, coordinates.y / k, coordinates.z / k);
    }

    void Versor3::nlerp(const Versor3& v, const Scalar& t)
    {
        Vector3 v1 = Vector3(coordinates.x, coordinates.y, coordinates.z);
        Vector3 v2 = Vector3(v.coordinates.x, v.coordinates.y, v.coordinates.z);

        auto tmp = v1.lerp(v2, t).normalized();
        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
        this->_coordinates.z = tmp.coordinates.z;
    }

    Vector3 Versor3::asVector3() const
    {
        return Vector3(coordinates.x, coordinates.y, coordinates.z);
    }

    Vector3 Versor3::operator + (const Versor3& versor) const
    {
        return Vector3(coordinates.x + versor.coordinates.x, coordinates.y + versor.coordinates.y, coordinates.z + versor.coordinates.z);
    }

    Vector3 Versor3::operator + (const Vector3& vector) const
    {
        return Vector3(coordinates.x + vector.coordinates.x, coordinates.y + vector.coordinates.y, coordinates.z + vector.coordinates.z);
    }

    Versor3 Versor3::operator + () const
    {
        return *this;
    }

    Scalar Versor3::operator * (const Versor3& versor) const
    {
        return dot(*this, versor);
    }

    Scalar Versor3::operator * (const Vector3& vector) const
    {
        return dot(*this, vector);
    }

    Scalar Versor3::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return coordinates.x;
        case 1:
            return coordinates.y;
        case 2:
            return coordinates.z;
        default:
            throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Versor3 the index might be either 0, 1 or 2");
        }
    }

    Scalar Versor3::dot(const Vector3& v1, const Versor3& v2)
    {
        return v1.dot(v2.asVector3());
    }

    Scalar Versor3::dot(const Versor3& v1, const Vector3& v2)
    {
        return dot(v2, v1);
    }

    Scalar Versor3::dot(const Versor3& v1, const Versor3& v2)
    {
        return v1.asVector3().dot(v2.asVector3());
    }

}