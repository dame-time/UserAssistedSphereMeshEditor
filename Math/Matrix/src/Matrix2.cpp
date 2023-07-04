#include "../Matrix2.hpp"

namespace Math{
    Matrix2::Matrix2()
    {
        data[0] = data[2] = 1;
        data[1] = data[3] = 0;
    }

    Matrix2::Matrix2(const Scalar& v1, const Scalar& v2,
                     const Scalar& v3, const Scalar& v4)
    {
        data[0] = v1;
        data[1] = v2;
        data[2] = v3;
        data[3] = v4;
    }

    Matrix2::Matrix2(const Vector2& vec1, const Vector2& vec2)
    {
        data[0] = vec1.coordinates.x;
        data[1] = vec1.coordinates.y;
        data[2] = vec2.coordinates.x;
        data[3] = vec2.coordinates.y;
    }

    Matrix2::Matrix2(const Matrix2& mat)
    {
        data[0] = mat.data[0];
        data[1] = mat.data[1];
        data[2] = mat.data[2];
        data[3] = mat.data[3];
    }

    void Matrix2::operator*=(const Matrix2& mat)
    {
        Scalar temp1, temp2, temp3, temp4;
        temp1 = (data[0] * mat.data[0]) + (data[1] * mat.data[2]);
        temp2 = (data[0] * mat.data[1]) + (data[1] * mat.data[3]);
        temp3 = (data[2] * mat.data[0]) + (data[3] * mat.data[2]);
        temp4 = (data[2] * mat.data[1]) + (data[3] * mat.data[3]);
        data[0] = temp1;
        data[1] = temp2;
        data[2] = temp3;
        data[3] = temp4;

    }

    void Matrix2::operator*=(const Scalar& value)
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            data[i] *= value;
        }
    }

    void Matrix2::operator+=(const Matrix2& mat)
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            data[i] += mat.data[i];
        }
    }

    void Matrix2::operator-=(const Matrix2& mat)
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            data[i] -= mat.data[i];
        }
    }

    Vector2 Matrix2::operator*(const Vector2& vec) const
    {
        return Vector2(
            (data[0] * vec.coordinates.x) + (data[1] * vec.coordinates.y),
            (data[2] * vec.coordinates.x) + (data[3] * vec.coordinates.y)
        );
    }

    Matrix2 Matrix2::operator*(const Matrix2& mat) const
    {
        Matrix2 result = Matrix2(*this);
        result *= mat;
        return result;
    }

    Matrix2 Matrix2::operator*(const Scalar& value) const
    {
        Matrix2 result = Matrix2(*this);
        result *= value;
        return result;
    }

    Matrix2 Matrix2::operator+(const Matrix2& mat) const
    {
        Matrix2 result = Matrix2(*this);
        result += mat;
        return result;
    }

    Matrix2 Matrix2::operator-(const Matrix2& mat) const
    {
        Matrix2 result = Matrix2(*this);
        result -= mat;
        return result;
    }

    Matrix2 Matrix2::operator-() const
    {
        return Matrix2(-data[0], -data[1], -data[2], -data[3]);
    }

    void Matrix2::operator=(const Matrix2& mat)
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            data[i] = mat.data[i];
        }
    }

    bool Matrix2::operator==(const Matrix2& mat) const
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            if(data[i] != mat.data[i])
                return false;
        }
        return true;
    }

    void Matrix2::setInverse(const Matrix2& mat)
    {
        Scalar det = mat.data[0] * mat.data[3] - mat.data[1] * mat.data[2];

        if(det == (Scalar)0.0f)
        {
            throw std::invalid_argument("Determinant is zero, therefore inverse matrix doesn't exist");
        }

        Scalar inverseDet = 1/det;

        Scalar tmp = mat.data[0];

        data[0] = mat.data[3] * inverseDet;
        data[1] = -mat.data[1] * inverseDet;
        data[2] = -mat.data[2] * inverseDet;
        data[3] = tmp * inverseDet;
    }

    Matrix2 Matrix2::inverse() const
    {
        Matrix2 result;
        result.setInverse(*this);
        return result;
    }

    Matrix2 Matrix2::inverse(const Matrix2& mat)
    {
        Matrix2 result;
        result.setInverse(mat);
        return result;
    }

    void Matrix2::setTranspose(const Matrix2& mat)
    {
        data[0] = mat.data[0];
        data[1] = mat.data[2];
        data[2] = mat.data[1];
        data[3] = mat.data[3];
    }

    Matrix2 Matrix2::transpose() const
    {
        Matrix2 result;
        result.setTranspose(*this);
        return result;
    }

    Matrix2 Matrix2::transpose(const Matrix2& mat)
    {
        Matrix2 result;
        result.setTranspose(mat);
        return result;
    }

    void Matrix2::print() const
    {
        for(unsigned int i = 0; i < 4; ++i)
        {
            std::cout << data[i] << " ";
            if(i == 1)
                std::cout << std::endl;
        }
    }
}

