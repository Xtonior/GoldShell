#ifndef GMATH_H
#define GMATH_H

typedef struct
{
    float m[3][3];
} Matrix3x3;

typedef struct
{
    float m[4][4];
} Matrix4x4;

typedef struct
{
    float x, y, z;
} Vec3; 

typedef struct
{
    float x, y, z, w;
} Vec4; 

Matrix4x4 getPerspectiveProjection(float fov, float aspect, float zNear, float zFar);
Matrix4x4 getRotationMatrixX(float angle);
Matrix4x4 getRotationMatrixY(float angle);
Matrix4x4 getRotationMatrixZ(float angle);
Matrix4x4 getTranslationMatrix(float x, float y, float z);
Matrix4x4 multiplyMatrix4x4(Matrix4x4 a, Matrix4x4 b);
Vec4 applyPerspectiveProjection(const Matrix4x4 *projMatrix, const Vec4 *vertex);
Vec4 multiplyMatrixVec4(const Matrix4x4 *mat, const Vec4 *vec);

// void create_matrix_3x3(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22);
// void create_matrix_4x4(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);

#endif