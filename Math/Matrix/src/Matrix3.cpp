#include <Matrix3.hpp>

#include <Vector3.hpp>
#include <Matrix2.hpp>

namespace Math
{
    Matrix3::Matrix3()
    {
        data[0] = 1, data[1] = 0, data[2] = 0,
        data[3] = 0, data[4] = 1, data[5] = 0,
        data[6] = 0, data[7] = 0; data[8] = 1;
    }

    Matrix3::Matrix3(const Scalar& v1, const Scalar& v2, const Scalar& v3,
                     const Scalar& v4, const Scalar& v5, const Scalar& v6,
                     const Scalar& v7, const Scalar& v8, const Scalar& v9)
    {
        data[0] = v1, data[1] = v2, data[2] = v3,
        data[3] = v4, data[4] = v5, data[5] = v6,
        data[6] = v7, data[7] = v8, data[8] = v9;
    }

    Matrix3::Matrix3(Vector3& vec1, Vector3& vec2, Vector3& vec3)
    {
        data[0] = vec1.coordinates.x;
        data[1] = vec1.coordinates.y;
        data[2] = vec1.coordinates.z;
        data[3] = vec2.coordinates.x;
        data[4] = vec2.coordinates.y;
        data[5] = vec2.coordinates.z;
        data[6] = vec3.coordinates.x;
        data[7] = vec3.coordinates.y;
        data[8] = vec3.coordinates.z;
    }

    Matrix3::Matrix3(const Matrix2& mat)
    {
        data[0] = mat.data[0];
        data[1] = mat.data[1];
        data[2] = Scalar(0.0f);
        data[3] = mat.data[2];
        data[4] = mat.data[3];
        data[5] = Scalar(0.0f);
        data[6] = Scalar(0.0f);
        data[7] = Scalar(0.0f);
        data[8] = Scalar(0.0f);
    }

    Matrix3::Matrix3(const Matrix3& mat)
    {
        data[0] = mat.data[0];
        data[1] = mat.data[1];
        data[2] = mat.data[2];
        data[3] = mat.data[3];
        data[4] = mat.data[4];
        data[5] = mat.data[5];
        data[6] = mat.data[6];
        data[7] = mat.data[7];
        data[8] = mat.data[8];
    }

    void Matrix3::operator*=(const Matrix3& mat)
    {
        Scalar temp1, temp2, temp3;
        temp1 = data[0] * mat.data[0] + data[1] * mat.data[3] + data[2] * mat.data[6];
        temp2 = data[0] * mat.data[1] + data[1] * mat.data[4] + data[2] * mat.data[7];
        temp3 = data[0] * mat.data[2] + data[1] * mat.data[5] + data[2] * mat.data[8];
        data[0] = temp1;
        data[1] = temp2;
        data[2] = temp3;

        temp1 = data[3] * mat.data[0] + data[4] * mat.data[3] + data[5] * mat.data[6];
        temp2 = data[3] * mat.data[1] + data[4] * mat.data[4] + data[5] * mat.data[7];
        temp3 = data[3] * mat.data[2] + data[4] * mat.data[5] + data[5] * mat.data[8];
        data[3] = temp1;
        data[4] = temp2;
        data[5] = temp3;

        temp1 = data[6] * mat.data[0] + data[7] * mat.data[3] + data[8] * mat.data[6];
        temp2 = data[6] * mat.data[1] + data[7] * mat.data[4] + data[8] * mat.data[7];
        temp3 = data[6] * mat.data[2] + data[7] * mat.data[5] + data[8] * mat.data[8];
        data[6] = temp1;
        data[7] = temp2;
        data[8] = temp3;
    }

    void Matrix3::operator*=(const Scalar& value)
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            data[i] *= value;
        }
    }

    Matrix3 Matrix3::operator*(const Matrix3& mat) const
    {
        Matrix3 result = Matrix3(*this);
        result *= mat;
        return result;
    }

    Matrix3 Matrix3::operator*(const Scalar& value) const
    {
        Matrix3 result = Matrix3(*this);
        result *= value;
        return result;
    }

    Vector3 Matrix3::operator*(const Vector3& vec) const
    {
        return Vector3(
            (data[0] * vec.coordinates.x) + (data[1] * vec.coordinates.y) + (data[2] * vec.coordinates.z),
            (data[3] * vec.coordinates.x) + (data[4] * vec.coordinates.y) + (data[5] * vec.coordinates.z),
            (data[6] * vec.coordinates.x) + (data[7] * vec.coordinates.y) + (data[8] * vec.coordinates.z)
        );
    }

    void Matrix3::operator+=(const Matrix3& mat)
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            data[i] += mat.data[i];
        }
    }

    Matrix3 Matrix3::operator+(const Matrix3& mat) const
    {
        Matrix3 result = Matrix3(*this);
        result += mat;
        return result;
    }

    void Matrix3::operator-=(const Matrix3& mat)
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            data[i] -= mat.data[i];
        }
    }

    Matrix3 Matrix3::operator-(const Matrix3& mat) const
    {
        Matrix3 result = Matrix3(*this);
        result -= mat;
        return result;
    }

    Matrix3 Matrix3::operator-() const
    {
        return Matrix3(-data[0], -data[1], -data[2],
                       -data[3], -data[4], -data[5],
                       -data[6], -data[7], -data[8]);
    }

    bool Matrix3::operator==(const Matrix3& mat) const
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            if(data[i] != mat.data[i])
                return false;
        }
        return true;
    }

    void Matrix3::operator=(const Matrix3& mat)
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            data[i] = mat.data[i];
        }
    }

   void Matrix3::setTranspose(const Matrix3& mat)
    {
        data[0] = mat.data[0];
        data[1] = mat.data[3];
        data[2] = mat.data[6];
        data[3] = mat.data[1];
        data[4] = mat.data[4];
        data[5] = mat.data[7];
        data[6] = mat.data[2];
        data[7] = mat.data[5];
        data[8] = mat.data[8];
    }

    Matrix3 Matrix3::transpose() const
    {
        Matrix3 result;
        result.setTranspose(*this);
        return result;
    }

    Matrix3 Matrix3::transpose(const Matrix3& mat)
    {
        Matrix3 result;
        result.setTranspose(mat);
        return result;
    }

    Vector3 Matrix3::transformTranspose(const Vector3& vec)
    {
        return Vector3(vec.coordinates.x * data[0] + vec.coordinates.y * data[3] + vec.coordinates.z * data[6],
                       vec.coordinates.x * data[1] + vec.coordinates.y * data[4] + vec.coordinates.z * data[7],
                       vec.coordinates.x * data[2] + vec.coordinates.y * data[5] + vec.coordinates.z * data[8]);
    }

    void Matrix3::setSkewSymmetric(const Vector3& vec)
    {
        data[0] = data[4] = data[8] = 0;
        data[1] = -vec.coordinates.z;
        data[2] = vec.coordinates.y;
        data[3] = vec.coordinates.z;
        data[5] = -vec.coordinates.x;
        data[6] = -vec.coordinates.y;
        data[7] = vec.coordinates.x;
    }

    void Matrix3::setInertiaTensorCoeffs(Scalar ix, Scalar iy, Scalar iz, Scalar ixy, Scalar ixz, Scalar iyz)
    {
        data[0] = ix;
        data[1] = data[3] = -ixy;
        data[2] = data[6] = -ixz;
        data[4] = iy;
        data[5] = data[7] = -iyz;
        data[8] = iz;
    }

    void Matrix3::setBlockInertiaTensor(const Vector3& halfsizes, Scalar mass)
    {
        Vector3 square = Vector3(halfsizes.coordinates.x * halfsizes.coordinates.x,
                                 halfsizes.coordinates.y * halfsizes.coordinates.y,
                                 halfsizes.coordinates.z * halfsizes.coordinates.z);
        
        setInertiaTensorCoeffs(0.3 * mass * (square.coordinates.y + square.coordinates.z),
                               0.3 * mass * (square.coordinates.x + square.coordinates.z),
                               0.3 * mass * (square.coordinates.x + square.coordinates.y));
    }

    void Matrix3::setComponents(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3)
    {
        data[0] = vec1.coordinates.x;
        data[3] = vec1.coordinates.y;
        data[6] = vec1.coordinates.z;
        
        data[1] = vec2.coordinates.x;
        data[4] = vec2.coordinates.y;
        data[7] = vec2.coordinates.z;

        data[2] = vec3.coordinates.x;
        data[5] = vec3.coordinates.y;
        data[8] = vec3.coordinates.z;
    }

    void Matrix3::setInverse(const Matrix3& mat)
    {
        Scalar temp1 = mat.data[0] * mat.data[4];
        Scalar temp2 = mat.data[0] * mat.data[5];
        Scalar temp3 = mat.data[1] * mat.data[3];
        Scalar temp4 = mat.data[2] * mat.data[3];
        Scalar temp5 = mat.data[1] * mat.data[6];
        Scalar temp6 = mat.data[2] * mat.data[6];

        Scalar det = (temp1 * mat.data[8] - temp2 * mat.data[7] - temp3 * mat.data[8] +
                      temp4 * mat.data[7] + temp5 * mat.data[5] - temp6 * mat.data[4]);
        
        if(det == (Scalar)0.0f)
            throw std::invalid_argument("Determinant is zero, therefore inverse matrix doesn't exist");

        Scalar inverseDet = 1 / det;

        data[0] = (mat.data[4] * mat.data[8] - mat.data[5] * mat.data[7])* inverseDet;
        data[1] = -(mat.data[1] * mat.data[8] - mat.data[2] * mat.data[7]) * inverseDet;
        data[2] = (mat.data[1] * mat.data[5] - mat.data[2] * mat.data[4]) * inverseDet;
        data[3] = -(mat.data[3] * mat.data[8] - mat.data[5] * mat.data[6]) * inverseDet;
        data[4] = (mat.data[0] * mat.data[8] - temp6) * inverseDet;
        data[5] = -(temp2 - temp4) * inverseDet;
        data[6] = (mat.data[3] * mat.data[7] - mat.data[4] * mat.data[6]) * inverseDet;
        data[7] = -(mat.data[0] * mat.data[7] - temp5) * inverseDet;
        data[8] = (temp1 - temp3) * inverseDet;
    }

    Matrix3 Matrix3::inverse() const
    {
        Matrix3 result;
        result.setInverse(*this);
        return result;
    }

    Matrix3 Matrix3::inverse(const Matrix3& mat)
    {
        Matrix3 result;
        result.setInverse(mat);
        return result;
    }

    ArrayOfVector3Matrix Matrix3::asVector3Array() const {
        return Matrix3::AsVector3Array(*this);
    }

    void Matrix3::print() const
    {
        for(unsigned int i = 0; i < 9; ++i)
        {
            std::cout << data[i] << " ";
            if(i == 2 || i == 5)
                std::cout << std::endl;
        }
    }

}
