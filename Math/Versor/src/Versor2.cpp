#include <Versor2.hpp>

namespace Math {

    Versor2::Versor2() : coordinates(_coordinates)
    {
        this->_coordinates.x = 0;
        this->_coordinates.y = 0;
    }
    
    Versor2::Versor2(const Versor2& versor) : coordinates(_coordinates)
    {
        this->_coordinates.x = versor.coordinates.x;
        this->_coordinates.y = versor.coordinates.y;
    }
    
    Versor2::Versor2(const Vector2& vector) : coordinates(_coordinates)
    {
        auto tmp = vector.normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
    }
    
    Versor2::Versor2(const Scalar& x, const Scalar& y) : coordinates(_coordinates)
    {
        auto tmp = Vector2(x, y).normalized();

        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
    }
    
    Versor2 Versor2::operator - () const
    {
        return Versor2(-coordinates.x, -coordinates.y);
    }

    Vector2 Versor2::operator - (const Vector2& vector) const
    {
        return Vector2(_coordinates.x - vector.coordinates.x, _coordinates.y - vector.coordinates.y);
    }

    Vector2 Versor2::operator - (const Versor2& versor) const
    {
        return Vector2(_coordinates.x - versor.coordinates.x, _coordinates.y - versor.coordinates.y);
    }

    Vector2 Versor2::operator * (const Scalar& k) const
    {
        return Vector2(coordinates.x * k, coordinates.y * k);
    }

    Vector2 Versor2::operator / (const Scalar& k) const
    {
        return Vector2(coordinates.x / k, coordinates.y / k);
    }

    void Versor2::nlerp(const Versor2& v, const Scalar& t)
    {
        Vector2 v1 = Vector2(coordinates.x, coordinates.y);
        Vector2 v2 = Vector2(v.coordinates.x, v.coordinates.y);

        auto tmp = v1.lerp(v2, t).normalized();
        this->_coordinates.x = tmp.coordinates.x;
        this->_coordinates.y = tmp.coordinates.y;
    }

    Vector2 Versor2::asVector2() const
    {
        return Vector2(coordinates.x, coordinates.y);
    }

    Vector2 Versor2::operator + (const Versor2& versor) const
    {
        return Vector2(coordinates.x + versor.coordinates.x, coordinates.y + versor.coordinates.y);
    }

    Vector2 Versor2::operator + (const Vector2& vector) const
    {
        return Vector2(coordinates.x + vector.coordinates.x, coordinates.y + vector.coordinates.y);
    }

    Versor2 Versor2::operator + () const
    {
        return *this;
    }

    Scalar Versor2::operator * (const Versor2& versor) const
    {
        return dot(*this, versor);
    }

    Scalar Versor2::operator * (const Vector2& vector) const
    {
        return dot(*this, vector);
    }

    Scalar Versor2::operator [] (const int& i) const
    {
        switch (i)
        {
        case 0:
            return coordinates.x;
        case 1:
            return coordinates.y;
        default:
            throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Versor2 the index might be either 0 or 1");
        }
    }

    Scalar Versor2::dot(const Vector2& v1, const Versor2& v2)
    {
        return v1.dot(v2.asVector2());
    }

    Scalar Versor2::dot(const Versor2& v1, const Vector2& v2)
    {
        return dot(v2, v1);
    }

    Scalar Versor2::dot(const Versor2& v1, const Versor2& v2)
    {
        return v1.asVector2().dot(v2.asVector2());
    }

}