#include <Vector2.hpp>

namespace Math {
    Vector2::Vector2() {
        coordinates = Vector2Coordinates<Scalar>();
        this->coordinates.x = (Scalar) 0.0;
        this->coordinates.y = (Scalar) 0.0;
    }

    Vector2::Vector2(Scalar x, Scalar y) {
        coordinates = Vector2Coordinates<Scalar>();
        this->coordinates.x = x;
        this->coordinates.y = y;
    }

    Vector2::Vector2(const Vector2& vector) {
        coordinates = Vector2Coordinates<Scalar>();
        this->coordinates.x = vector.coordinates.x;
        this->coordinates.y = vector.coordinates.y;
    }

    Vector2::Vector2(const Versor2& versor)
    {
        this->coordinates.x = versor.coordinates.x;
        this->coordinates.y = versor.coordinates.y;
    }

    Scalar Vector2::dot(const Vector2& vector) const {
        return this->coordinates.x * vector.coordinates.x +
                this->coordinates.y * vector.coordinates.y;
    }

    Vector3 Vector2::cross(const Vector2& vector) const {
        return Vector3(this->coordinates.x, this->coordinates.y, (Scalar) 0.0)
                .cross(Vector3(vector.coordinates.x, vector.coordinates.y, (Scalar) 0.0));
    }

    Vector2 Vector2::componentWise(const Vector2& vector) const {
        return Vector2(
            this->coordinates.x * vector.coordinates.x,
            this->coordinates.y * vector.coordinates.y
        );
    }

    Scalar Vector2::magnitude() const {
        return std::sqrt(this->squareMagnitude());
    }

    Scalar Vector2::squareMagnitude() const {
        return (this->coordinates.x * this->coordinates.x) + (this->coordinates.y * this->coordinates.y);
    }

    Vector2 Vector2::normalized() const {
        return *this / this->magnitude();
    }

    void Vector2::normalize() {
        *this /= this->magnitude();
    }

    Scalar Vector2::operator [] (const short& i) const {
        if (i == 0)
            return this->coordinates.x;
        if (i == 1)
            return this->coordinates.y;

        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector2 the index might be either 0 or 1");
    }

    Scalar& Vector2::operator [] (const short& i) {
        if (i == 0)
            return this->coordinates.x;
        if (i == 1)
            return this->coordinates.y;
        
        throw std::invalid_argument("INDEX_OUT_OF_RANGE::in Vector2 the index might be either 0 or 1");
    }

    Scalar Vector2::operator * (const Vector2& vector) const {
        return this->dot(vector);
    }

    Vector2 Vector2::operator + (const Vector2& vector) const {
        return Vector2(this->coordinates.x + vector.coordinates.x, 
            this->coordinates.y + vector.coordinates.y);
    }

    Vector2 Vector2::operator - (const Vector2& vector) const {
            return Vector2(this->coordinates.x - vector.coordinates.x, 
                this->coordinates.y - vector.coordinates.y);
    }

    bool Vector2::operator == (const Vector2& vector) const {
        return this->areEquals(vector);
    }

    Vector2 Vector2::operator - () const {
        return Vector2(-this->coordinates.x, -this->coordinates.y);
    }

    Vector2 Vector2::operator * (const Scalar& k) const {
        return Vector2(this->coordinates.x * k, 
                this->coordinates.y * k);
    }

    Vector2 Vector2::operator / (const Scalar& k) const {
        return Vector2(this->coordinates.x / k, 
                this->coordinates.y / k);
    }

    void Vector2::operator += (const Vector2& vector) {
        this->coordinates.x += vector.coordinates.x;
        this->coordinates.y += vector.coordinates.y;
    }

    void Vector2::operator -= (const Vector2& vector) {
        this->coordinates.x -= vector.coordinates.x;
        this->coordinates.y -= vector.coordinates.y;
    }

    void Vector2::operator *= (const Scalar& k) {
        this->coordinates.x *= k;
        this->coordinates.y *= k;
    }

    void Vector2::operator /= (const Scalar& k) {
        this->coordinates.x /= k;
        this->coordinates.y /= k;
    }

    Scalar Vector2::angleBetween (const Vector2& vector) const {
        return Math::radiansToDegreeAngle(std::acos(this->normalized() * vector.normalized()));
    }

    Vector2 Vector2::lerp(const Vector2& vector, const Scalar& t) const {
        return *this * (1 - t) + vector * t;
    }

    bool Vector2::isZero() const {
        return this->squareMagnitude() == 0;
    }

    bool Vector2::areEquals(const Vector2& vector) const{
        return (*this - vector).isZero();
    }

    Versor2 Vector2::asVersor2() const
    {
        return Versor2(*this);
    }

    Point2 Vector2::asPoint2() const
    {
        return Point2(*this);
    }

    void Vector2::print() const {
        std::cout << "( " << this->coordinates.x << ", " << this->coordinates.y << " )" << std::endl;
    }
}