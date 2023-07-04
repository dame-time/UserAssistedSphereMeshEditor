#include "../Matrix4.hpp"

namespace Math
{
    Matrix4::Matrix4()
    {
        data[0] = 1, data[1] = 0, data[2] = 0, data[3] = 0,
        data[4] = 0, data[5] = 1, data[6] = 0, data[7] = 0,
        data[8] = 0, data[9] = 0, data[10] = 1, data[11] = 0,
        data[12] = 0, data[13] = 0, data[14] = 0, data[15] = 1;
    }

    Matrix4::Matrix4(const Scalar& value)
    {
        data[0] = value, data[1] = value, data[2] = value, data[3] = value,
        data[4] = value, data[5] = value, data[6] = value, data[7] = value,
        data[8] = value, data[9] = value, data[10] = value, data[11] = value,
        data[12] = value, data[13] = value, data[14] = value, data[15] = value;
    }

    Matrix4::Matrix4(const Matrix4& mat)
    {
        data[0] = mat.data[0], data[1] = mat.data[1], data[2] = mat.data[2], data[3] = mat.data[3],
        data[4] = mat.data[4], data[5] = mat.data[5], data[6] = mat.data[6], data[7] = mat.data[7],
        data[8] = mat.data[8], data[9] = mat.data[9], data[10] = mat.data[10], data[11] = mat.data[11],
        data[12] = mat.data[12], data[13] = mat.data[13], data[14] = mat.data[14], data[15] = mat.data[15];
    }

    Matrix4::Matrix4(const Scalar& v1, const Scalar& v2, const Scalar& v3, const Scalar& v4,
                    const Scalar& v5, const Scalar& v6, const Scalar& v7, const Scalar& v8,
                    const Scalar& v9, const Scalar& v10, const Scalar& v11, const Scalar& v12,
                    const Scalar& v13, const Scalar& v14, const Scalar& v15, const Scalar& v16)
    {
        data[0] = v1, data[1] = v2, data[2] = v3, data[3] = v4,
        data[4] = v5, data[5] = v6, data[6] = v7, data[7] = v8,
        data[8] = v9, data[9] = v10, data[10] = v11, data[11] = v12,
        data[12] = v13, data[13] = v14, data[14] = v15, data[15] = v16;
    }

    Matrix4::Matrix4(const Vector4& vec1, const Vector4& vec2, const Vector4& vec3, const Vector4& vec4)
    {
        data[0] = vec1[0];
        data[1] = vec1[1];
        data[2] = vec1[2];
        data[3] = vec1[3];

        data[4] = vec2[0];
        data[5] = vec2[1];
        data[6] = vec2[2];
        data[7] = vec2[3];

        data[8] = vec3[0];
        data[9] = vec3[1];
        data[10] = vec3[2];
        data[11] = vec3[3];

        data[12] = vec4[0];
        data[13] = vec4[1];
        data[14] = vec4[2];
        data[15] = vec4[3];
    }

    void Matrix4::setZero()
    {
        data[0] = 0, data[1] = 0, data[2] = 0, data[3] = 0,
        data[4] = 0, data[5] = 0, data[6] = 0, data[7] = 0,
        data[8] = 0, data[9] = 0, data[10] = 0, data[11] = 0,
        data[12] = 0, data[13] = 0, data[14] = 0, data[15] = 0;
    }

    void Matrix4::operator*=(const Matrix4& mat)
    {
        Scalar temp1, temp2, temp3, temp4;
        temp1 = data[0] * mat.data[0] + data[1] * mat.data[4] + data[2] * mat.data[8] + data[3] * mat.data[12];
        temp2 = data[0] * mat.data[1] + data[1] * mat.data[5] + data[2] * mat.data[9] + data[3] * mat.data[13];
        temp3 = data[0] * mat.data[2] + data[1] * mat.data[6] + data[2] * mat.data[10] + data[3] * mat.data[14];
        temp4 = data[0] * mat.data[3] + data[1] * mat.data[7] + data[2] * mat.data[11] + data[3] * mat.data[15];

        data[0] = temp1;
        data[1] = temp2;
        data[2] = temp3;
        data[3] = temp4;

        temp1 = data[4] * mat.data[0] + data[5] * mat.data[4] + data[6] * mat.data[8] + data[7] * mat.data[12];
        temp2 = data[4] * mat.data[1] + data[5] * mat.data[5] + data[6] * mat.data[9] + data[7] * mat.data[13];
        temp3 = data[4] * mat.data[2] + data[5] * mat.data[6] + data[6] * mat.data[10] + data[7] * mat.data[14];
        temp4 = data[4] * mat.data[3] + data[5] * mat.data[7] + data[6] * mat.data[11] + data[7] * mat.data[15];

        data[4] = temp1;
        data[5] = temp2;
        data[6] = temp3;
        data[7] = temp4;

        temp1 = data[8] * mat.data[0] + data[9] * mat.data[4] + data[10] * mat.data[8] + data[11] * mat.data[12];
        temp2 = data[8] * mat.data[1] + data[9] * mat.data[5] + data[10] * mat.data[9] + data[11] * mat.data[13];
        temp3 = data[8] * mat.data[2] + data[9] * mat.data[6] + data[10] * mat.data[10] + data[11] * mat.data[14];
        temp4 = data[8] * mat.data[3] + data[9] * mat.data[7] + data[10] * mat.data[11] + data[11] * mat.data[15];

        data[8] = temp1;
        data[9] = temp2;
        data[10] = temp3;
        data[11] = temp4;

        temp1 = data[12] * mat.data[0] + data[13] * mat.data[4] + data[14] * mat.data[8] + data[15] * mat.data[12];
        temp2 = data[12] * mat.data[1] + data[13] * mat.data[5] + data[14] * mat.data[9] + data[15] * mat.data[13];
        temp3 = data[12] * mat.data[2] + data[13] * mat.data[6] + data[14] * mat.data[10] + data[15] * mat.data[14];
        temp4 = data[12] * mat.data[3] + data[13] * mat.data[7] + data[14] * mat.data[11] + data[15] * mat.data[15];

        data[12] = temp1;
        data[13] = temp2;
        data[14] = temp3;
        data[15] = temp4;
    }

    void Matrix4::operator*=(const Scalar& value)
    {
        for(unsigned int i = 0; i < 16; ++i)
        {
            data[i] *= value;
        }
    }

    void Matrix4::operator+=(const Matrix4& mat)
    {
        for(unsigned int i = 0; i < 16; ++i)
        {
            data[i] += mat.data[i];
        }
    }

    void Matrix4::operator-=(const Matrix4& mat)
    {
        for(unsigned int i = 0; i < 16; ++i)
        {
            data[i] -= mat.data[i];
        }
    }

    void Matrix4::operator/=(const Scalar& dividend)
    {
        *this = (Matrix4(*this) / dividend);
    }

    Matrix4 Matrix4::operator*(const Matrix4& mat) const
    {
        Matrix4 result = Matrix4(*this);
        result *= mat;
        return result;
    }

    Matrix4 Matrix4::operator*(const Scalar& value) const
    {
        Matrix4 result = Matrix4(*this);
        result *= value;
        return result;
    }

    Matrix4 Matrix4::operator+(const Matrix4& mat) const
    {
        Matrix4 result = Matrix4(*this);
        result += mat;
        return result;
    }

    Matrix4 Matrix4::operator-(const Matrix4& mat) const
    {
        Matrix4 result = Matrix4(*this);
        result -= mat;
        return result;
    }

    Matrix4 Matrix4::operator-() const
    {
        return Matrix4(-data[0], -data[1], -data[2], -data[3],
                       -data[4], -data[5], -data[6], -data[7],
                       -data[8], -data[9], -data[10], -data[11],
                       -data[12], -data[13], -data[14], -data[15]);
    }

    Matrix4 Matrix4::operator/(const Scalar& d) const
    {
        Matrix4 res = Matrix4(*this);
        
        for (int i = 0; i < 16; i++)
            res.data[i] /= d;
        
        return res;
    }

    void Matrix4::operator=(const Matrix4& mat)
    {
        for(unsigned int i = 0; i < 16; ++i)
        {
            data[i] = mat.data[i];
        }
    }

    bool Matrix4::operator==(const Matrix4& other) const {
        for (int i = 0; i < 16; i++) {
            if (data[i] != other.data[i]) {
                return false;
            }
        }
        return true;
    }

    void Matrix4::setInverse(const Matrix4& mat)
    {
        Scalar inv[16], det;
        int i;

        inv[0] = mat.data[5]  * mat.data[10] * mat.data[15] - 
                mat.data[5]  * mat.data[11] * mat.data[14] - 
                mat.data[9]  * mat.data[6]  * mat.data[15] + 
                mat.data[9]  * mat.data[7]  * mat.data[14] +
                mat.data[13] * mat.data[6]  * mat.data[11] - 
                mat.data[13] * mat.data[7]  * mat.data[10];

        inv[4] = -mat.data[4]  * mat.data[10] * mat.data[15] + 
                mat.data[4]  * mat.data[11] * mat.data[14] + 
                mat.data[8]  * mat.data[6]  * mat.data[15] - 
                mat.data[8]  * mat.data[7]  * mat.data[14] - 
                mat.data[12] * mat.data[6]  * mat.data[11] + 
                mat.data[12] * mat.data[7]  * mat.data[10];

        inv[8] = mat.data[4]  * mat.data[9] * mat.data[15] - 
                mat.data[4]  * mat.data[11] * mat.data[13] - 
                mat.data[8]  * mat.data[5] * mat.data[15] + 
                mat.data[8]  * mat.data[7] * mat.data[13] + 
                mat.data[12] * mat.data[5] * mat.data[11] - 
                mat.data[12] * mat.data[7] * mat.data[9];

        inv[12] = -mat.data[4]  * mat.data[9] * mat.data[14] + 
                mat.data[4]  * mat.data[10] * mat.data[13] +
                mat.data[8]  * mat.data[5] * mat.data[14] - 
                mat.data[8]  * mat.data[6] * mat.data[13] - 
                mat.data[12] * mat.data[5] * mat.data[10] + 
                mat.data[12] * mat.data[6] * mat.data[9];

        inv[1] = -mat.data[1]  * mat.data[10] * mat.data[15] + 
                mat.data[1]  * mat.data[11] * mat.data[14] + 
                mat.data[9]  * mat.data[2] * mat.data[15] - 
                mat.data[9]  * mat.data[3] * mat.data[14] - 
                mat.data[13] * mat.data[2] * mat.data[11] + 
                mat.data[13] * mat.data[3] * mat.data[10];

        inv[5] = mat.data[0]  * mat.data[10] * mat.data[15] - 
                mat.data[0]  * mat.data[11] * mat.data[14] - 
                mat.data[8]  * mat.data[2] * mat.data[15] + 
                mat.data[8]  * mat.data[3] * mat.data[14] + 
                mat.data[12] * mat.data[2] * mat.data[11] - 
                mat.data[12] * mat.data[3] * mat.data[10];

        inv[9] = -mat.data[0]  * mat.data[9] * mat.data[15] + 
                mat.data[0]  * mat.data[11] * mat.data[13] + 
                mat.data[8]  * mat.data[1] * mat.data[15] - 
                mat.data[8]  * mat.data[3] * mat.data[13] - 
                mat.data[12] * mat.data[1] * mat.data[11] + 
                mat.data[12] * mat.data[3] * mat.data[9];

        inv[13] = mat.data[0]  * mat.data[9] * mat.data[14] - 
                mat.data[0]  * mat.data[10] * mat.data[13] - 
                mat.data[8]  * mat.data[1] * mat.data[14] + 
                mat.data[8]  * mat.data[2] * mat.data[13] + 
                mat.data[12] * mat.data[1] * mat.data[10] - 
                mat.data[12] * mat.data[2] * mat.data[9];

        inv[2] = mat.data[1]  * mat.data[6] * mat.data[15] - 
                mat.data[1]  * mat.data[7] * mat.data[14] - 
                mat.data[5]  * mat.data[2] * mat.data[15] + 
                mat.data[5]  * mat.data[3] * mat.data[14] + 
                mat.data[13] * mat.data[2] * mat.data[7] - 
                mat.data[13] * mat.data[3] * mat.data[6];

        inv[6] = -mat.data[0]  * mat.data[6] * mat.data[15] + 
                mat.data[0]  * mat.data[7] * mat.data[14] + 
                mat.data[4]  * mat.data[2] * mat.data[15] - 
                mat.data[4]  * mat.data[3] * mat.data[14] - 
                mat.data[12] * mat.data[2] * mat.data[7] + 
                mat.data[12] * mat.data[3] * mat.data[6];

        inv[10] = mat.data[0]  * mat.data[5] * mat.data[15] - 
                mat.data[0]  * mat.data[7] * mat.data[13] - 
                mat.data[4]  * mat.data[1] * mat.data[15] + 
                mat.data[4]  * mat.data[3] * mat.data[13] + 
                mat.data[12] * mat.data[1] * mat.data[7] - 
                mat.data[12] * mat.data[3] * mat.data[5];

        inv[14] = -mat.data[0]  * mat.data[5] * mat.data[14] + 
                mat.data[0]  * mat.data[6] * mat.data[13] + 
                mat.data[4]  * mat.data[1] * mat.data[14] - 
                mat.data[4]  * mat.data[2] * mat.data[13] - 
                mat.data[12] * mat.data[1] * mat.data[6] + 
                mat.data[12] * mat.data[2] * mat.data[5];

        inv[3] = -mat.data[1] * mat.data[6] * mat.data[11] + 
                mat.data[1] * mat.data[7] * mat.data[10] + 
                mat.data[5] * mat.data[2] * mat.data[11] - 
                mat.data[5] * mat.data[3] * mat.data[10] - 
                mat.data[9] * mat.data[2] * mat.data[7] + 
                mat.data[9] * mat.data[3] * mat.data[6];

        inv[7] = mat.data[0] * mat.data[6] * mat.data[11] - 
                mat.data[0] * mat.data[7] * mat.data[10] - 
                mat.data[4] * mat.data[2] * mat.data[11] + 
                mat.data[4] * mat.data[3] * mat.data[10] + 
                mat.data[8] * mat.data[2] * mat.data[7] - 
                mat.data[8] * mat.data[3] * mat.data[6];

        inv[11] = -mat.data[0] * mat.data[5] * mat.data[11] + 
                mat.data[0] * mat.data[7] * mat.data[9] + 
                mat.data[4] * mat.data[1] * mat.data[11] - 
                mat.data[4] * mat.data[3] * mat.data[9] - 
                mat.data[8] * mat.data[1] * mat.data[7] + 
                mat.data[8] * mat.data[3] * mat.data[5];

        inv[15] = mat.data[0] * mat.data[5] * mat.data[10] - 
                mat.data[0] * mat.data[6] * mat.data[9] - 
                mat.data[4] * mat.data[1] * mat.data[10] + 
                mat.data[4] * mat.data[2] * mat.data[9] + 
                mat.data[8] * mat.data[1] * mat.data[6] - 
                mat.data[8] * mat.data[2] * mat.data[5];

        det = mat.data[0] * inv[0] + mat.data[1] * inv[4] + mat.data[2] * inv[8] + mat.data[3] * inv[12];

        if (det == 0)
            throw std::invalid_argument("Determinant is zero, therefore inverse matrix doesn't exist");

        det = 1.0f / det;

        for (i = 0; i < 16; i++)
            data[i] = inv[i] * det;
    }

    Matrix4 Matrix4::inverse() const
    {
        Matrix4 result;
        result.setInverse(*this);
        return result;
    }

    Matrix4 Matrix4::inverse(const Matrix4& mat)
    {
        Matrix4 result;
        result.setInverse(mat);
        return result;
    }
    
    Matrix4 Matrix4::transposed() const
    {
        Matrix4 result;

        result.data[0] = this->data[0];
        result.data[1] = this->data[4];
        result.data[2] = this->data[8];
        result.data[3] = this->data[12];

        result.data[4] = this->data[1];
        result.data[5] = this->data[5];
        result.data[6] = this->data[9];
        result.data[7] = this->data[13];

        result.data[8] = this->data[2];
        result.data[9] = this->data[6];
        result.data[10] = this->data[10];
        result.data[11] = this->data[14];

        result.data[12] = this->data[3];
        result.data[13] = this->data[7];
        result.data[14] = this->data[11];
        result.data[15] = this->data[15];

        return result;
    }

    void Matrix4::setTopLeftMatrix3(const Matrix3 &mat)
    {
        this->data[0] = mat.data[0];
        this->data[1] = mat.data[1];
        this->data[2] = mat.data[2];

        this->data[4] = mat.data[3];
        this->data[5] = mat.data[4];
        this->data[6] = mat.data[5];

        this->data[8] = mat.data[6];
        this->data[9] = mat.data[7];
        this->data[10] = mat.data[8];
    }

    void Matrix4::setRowVector(int rowIndex, const Vector4 &vector)
    {
        if (rowIndex > 3 || rowIndex < 0) {
            std::cerr << "In Matrix4::setRowVector(...) you specified a overflown rowIndex -> " << rowIndex << " this should be in range [0, 3]" << std::endl;
            return;
        }

        int index = rowIndex * 4;

        this->data[index] = vector.coordinates.x;
        this->data[index + 1] = vector.coordinates.y;
        this->data[index + 2] = vector.coordinates.z;
        this->data[index + 3] = vector.coordinates.w;
    }

    void Matrix4::setColumnVector(int columnIndex, const Vector4 &vector)
    {
        if (columnIndex > 3 || columnIndex < 0)
        {
            std::cerr << "In Matrix4::setColumnVector(...) you specified a overflown rowIndex -> " << columnIndex << " this should be in range [0, 3]" << std::endl;
            return;
        }

        int index = columnIndex;

        this->data[index] = vector.coordinates.x;
        this->data[index + 4] = vector.coordinates.y;
        this->data[index + 8] = vector.coordinates.z;
        this->data[index + 12] = vector.coordinates.w;
    }

    Matrix3 Matrix4::toMatrix3()
    {
        Matrix3 result;
        result.data[0] = data[0];
        result.data[1] = data[1];
        result.data[2] = data[2];
        result.data[3] = data[4];
        result.data[4] = data[5];
        result.data[5] = data[6];
        result.data[6] = data[8];
        result.data[7] = data[9];
        result.data[8] = data[10];

        return result;
    }

    Matrix3 Matrix4::toMatrix3(const Matrix4& mat)
    {
        Matrix3 result;
        result.data[0] = mat.data[0];
        result.data[1] = mat.data[1];
        result.data[2] = mat.data[2];
        result.data[3] = mat.data[4];
        result.data[4] = mat.data[5];
        result.data[5] = mat.data[6];
        result.data[6] = mat.data[8];
        result.data[7] = mat.data[9];
        result.data[8] = mat.data[10];

        return result;
    }

    Vector4 Matrix4::operator*(const Vector4& vec) const
    {  
        return Vector4(vec.coordinates.x * data[0] +
                       vec.coordinates.y * data[1] + 
                       vec.coordinates.z * data[2] +
                       vec.coordinates.w * data[3],
                       
                       vec.coordinates.x * data[4] +
                       vec.coordinates.y * data[5] + 
                       vec.coordinates.z * data[6] +
                       vec.coordinates.w * data[7],

                       vec.coordinates.x * data[8] +
                       vec.coordinates.y * data[9] + 
                       vec.coordinates.z * data[10] +
                       vec.coordinates.w * data[11],
                       
                       vec.coordinates.x * data[12] +
                       vec.coordinates.y * data[13] + 
                       vec.coordinates.z * data[14] +
                       vec.coordinates.w * data[15]);
    }

    Vector3 Matrix4::transformDirection(const Vector3& vec) const
    {
        return Vector3(
            vec.coordinates.x * data[0] + 
            vec.coordinates.y * data[1] +
            vec.coordinates.z * data[2],

            vec.coordinates.x * data[4] + 
            vec.coordinates.y * data[5] +
            vec.coordinates.z * data[6],

            vec.coordinates.x * data[8] + 
            vec.coordinates.y * data[9] +
            vec.coordinates.z * data[10]
        );
    }

    Vector3 Matrix4::transformInverseDirection(const Vector3& vec) const
    {
        return Vector3(
            vec.coordinates.x * data[0] + 
            vec.coordinates.y * data[4] +
            vec.coordinates.z * data[8],

            vec.coordinates.x * data[1] + 
            vec.coordinates.y * data[5] +
            vec.coordinates.z * data[9],

            vec.coordinates.x * data[2] + 
            vec.coordinates.y * data[6] +
            vec.coordinates.z * data[10]
        );
    }

    Vector3 Matrix4::localToWorldDir(const Vector3& local, const Matrix4 &transform) const
    {
        return transform.transformDirection(local);
    }

    Vector3 Matrix4::worldToLocalDir(const Vector3& world, const Matrix4 &transform) const
    {
        return transform.transformInverseDirection(world);
    }

    Vector3 Matrix4::transformInverse(const Vector3& vec) const
    {
        Vector3 tmp = vec;
        tmp.coordinates.x -= data[3];
        tmp.coordinates.y -= data[7];
        tmp.coordinates.z -= data[11];

        return Vector3(tmp.coordinates.x * data[0] +
                       tmp.coordinates.y * data[4] +
                       tmp.coordinates.z * data[8],
                       
                       tmp.coordinates.x * data[1] +
                       tmp.coordinates.y * data[5] +
                       tmp.coordinates.z * data[9],
                       
                       tmp.coordinates.x * data[2] +
                       tmp.coordinates.y * data[6] +
                       tmp.coordinates.z * data[10]);
    }

    Vector3 Matrix4::transform(const Vector3& vec) const
    {
        Vector4 tmp(vec, 1.0);
        tmp = *this * tmp;
        return tmp.xyz();
    }

    Vector3 Matrix4::getAxisVector(unsigned int index) const
    {
        return Vector3(data[index], data[index + 4], data[index + 8]);
    }

    bool Matrix4::DecomposeMatrixInScaleRotateTranslateComponents(
                                                                    const Matrix4& modelMatrix, 
                                                                    Vector3& scale, 
                                                                    Quaternion& rotation, 
                                                                    Vector3& translation,
                                                                    Vector3* eulerAnglesRotation, 
                                                                    Vector3* skew
                                                                )
    {
        if (skew == nullptr)
            skew = new Vector3();

        if (eulerAnglesRotation == nullptr)
            eulerAnglesRotation = new Vector3();

        Matrix4 temp(modelMatrix);

        if (temp.data[15] < std::numeric_limits<Scalar>::min())
            return false;

        //---------------------Getting Translation Factor------------------------//
        // Normalizing the matrix
        for (short i = 0; i < 16; i++)
            temp.data[i] /= temp.data[15];

        translation = Vector3(temp.data[12], temp.data[13], temp.data[14]);
        temp.data[12], temp.data[13], temp.data[14] = 0;

        //---------------------Getting Scaling Factor---------------------------//
        Vector3 row[3], pdum;
        Matrix3 temp3x3 = temp.toMatrix3();

        for (short i = 0; i < 3; i++)
            row[i] = Vector3(temp3x3.data[0 + i * 3], temp3x3.data[1 + i * 3], temp3x3.data[2 + i * 3]);

        scale.coordinates.x = row[0].magnitude();

        row[0].normalize();

        skew->coordinates.z = row[0].dot(row[1]);
        row[1] = row[1] * 1 + row[0] * (-skew->coordinates.z);

        scale.coordinates.y = row[1].magnitude();
        row[1].normalize();
        skew->coordinates.z /= scale.coordinates.y;
         
        skew->coordinates.y = row[0].dot(row[2]);
        row[2] = row[2] * 1 + row[0] * (-skew->coordinates.y);
        skew->coordinates.x = row[1].dot(row[2]);
        row[2] = row[2] * 1 + row[1] * (-skew->coordinates.x);

        scale.coordinates.z = row[2].magnitude();
        row[2].normalize();
        skew->coordinates.y /= scale.coordinates.z;
        skew->coordinates.x /= scale.coordinates.z;

        pdum = row[1].cross(row[2]);
        if (row[0].dot(pdum) < 0)
            for (short i = 0; i < 3; i++) 
            {
                scale[i] *= -1.0f;
                row[i] *= -1.0f;
            }
        
        //---------------------Getting Euler Angles------------------------//
        eulerAnglesRotation->coordinates.y = std::asin(-row[0][2]);
        if (std::cos(eulerAnglesRotation->coordinates.y) != 0) 
        {
            eulerAnglesRotation->coordinates.x = std::atan2(row[1][2], row[2][2]);
            eulerAnglesRotation->coordinates.z = std::atan2(row[0][1], row[0][0]);
        }
        else
        {
            eulerAnglesRotation->coordinates.x = std::atan2(-row[2][0], row[1][1]);
            eulerAnglesRotation->coordinates.z = 0;
        }

        //---------------------Getting Quaternion-------------------------//
        int i, j, k = 0;
        Scalar root, trace = row[0].coordinates.x + row[1].coordinates.y + row[2].coordinates.z;
        if (trace > static_cast<Scalar>(0))
        {
            root = std::sqrt(trace + static_cast<Scalar>(1.0));
            rotation.real = static_cast<Scalar>(0.5) * root;
            root = static_cast<Scalar>(0.5) / root;
            rotation.immaginary.coordinates.x = root * (row[1].coordinates.z - row[2].coordinates.y);
            rotation.immaginary.coordinates.y = root * (row[2].coordinates.x - row[0].coordinates.z);
            rotation.immaginary.coordinates.z = root * (row[0].coordinates.y - row[1].coordinates.x);
        }
        else
        {
            static int next[3] = { 1, 2, 0 };
            i = 0;
            if (row[1].coordinates.y > row[0].coordinates.x)
                i = 1;
            if (row[2].coordinates.z > row[i][i])
                i = 2;
            j = next[i];
            k = next[j];

            int offset = 1;

            root = std::sqrt(row[i][i] - row[j][j] - row[k][k] + static_cast<Scalar>(1.0));

            rotation[i + offset] = static_cast<Scalar>(0.5) * root;
            root = static_cast<Scalar>(0.5) / root;
            rotation[j + offset] = root * (row[i][j] + row[j][i]);
            rotation[k + offset] = root * (row[i][k] + row[k][i]);
            rotation.real = root * (row[j][k] - row[k][j]);
        }

        return true;
    }

    void Matrix4::print() const
    {
        for(unsigned int i = 0; i < 16; ++i)
        {
            std::cout << data[i] << " ";
            if(i == 3 || i == 7 || i == 11)
                std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
