#include "gmath.h"
#include <math.h>

Matrix4x4 getPerspectiveProjection(float fov, float aspect, float zNear, float zFar)
{
    Matrix4x4 mat = {0};
    float tanHalfFov = tanf(fov / 2.0f);

    mat.m[0][0] = 1.0f / (aspect * tanHalfFov);
    mat.m[1][1] = 1.0f / tanHalfFov;
    mat.m[2][2] = -(zFar + zNear) / (zFar - zNear);
    mat.m[2][3] = -1.0f;
    mat.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);

    return mat;
}

Matrix4x4 getRotationMatrixX(float angle)
{
    Matrix4x4 mat = {0};

    mat.m[0][0] = 1.0f;
    mat.m[1][1] = cosf(angle);
    mat.m[1][2] = -sinf(angle);
    mat.m[2][1] = sinf(angle);
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1.0f;

    return mat;
}

// Rotation around the Y axis
Matrix4x4 getRotationMatrixY(float angle)
{
    Matrix4x4 mat = {0};

    mat.m[0][0] = cosf(angle);
    mat.m[0][2] = sinf(angle);
    mat.m[1][1] = 1.0f;
    mat.m[2][0] = -sinf(angle);
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1.0f;

    return mat;
}

// Rotation around the Z axis
Matrix4x4 getRotationMatrixZ(float angle)
{
    Matrix4x4 mat = {0};

    mat.m[0][0] = cosf(angle);
    mat.m[0][1] = -sinf(angle);
    mat.m[1][0] = sinf(angle);
    mat.m[1][1] = cosf(angle);
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;

    return mat;
}

Matrix4x4 getTranslationMatrix(float x, float y, float z)
{
    Matrix4x4 mat = {0};

    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;

    mat.m[0][3] = x;
    mat.m[1][3] = y;
    mat.m[2][3] = z;

    return mat;
}

Matrix4x4 multiplyMatrix4x4(Matrix4x4 a, Matrix4x4 b)
{
    Matrix4x4 result = {0};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

    return result;
}

Vec4 multiplyMatrixVec4(const Matrix4x4* mat, const Vec4* vec)
{
    Vec4 result;

    result.x = mat->m[0][0] * vec->x + mat->m[0][1] * vec->y + mat->m[0][2] * vec->z + mat->m[0][3] * vec->w;
    result.y = mat->m[1][0] * vec->x + mat->m[1][1] * vec->y + mat->m[1][2] * vec->z + mat->m[1][3] * vec->w;
    result.z = mat->m[2][0] * vec->x + mat->m[2][1] * vec->y + mat->m[2][2] * vec->z + mat->m[2][3] * vec->w;
    result.w = mat->m[3][0] * vec->x + mat->m[3][1] * vec->y + mat->m[3][2] * vec->z + mat->m[3][3] * vec->w;

    return result;
}

Vec4 applyPerspectiveProjection(const Matrix4x4 *projMatrix, const Vec4 *vertex)
{
    Vec4 transformed = multiplyMatrixVec4(projMatrix, vertex);

    // Perform perspective divide
    if (transformed.w != 0.0f)
    {
        transformed.x /= transformed.w;
        transformed.y /= transformed.w;
        transformed.z /= transformed.w;
    }

    return transformed;
}