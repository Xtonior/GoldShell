#ifndef GMATH_H
#define GMATH_H

#include <stdbool.h>

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

void vec4_normalize(Vec4 *a, Vec4 *out);
void invert_matrix4x4(const Matrix4x4 *mat, Matrix4x4 *result);
void transpose_matrix4x4(Matrix4x4 *in, Matrix4x4 *out);

int max(int a, int b);
int min(int a, int b);

double dot(Vec4 *a, Vec4 *b);

Matrix4x4 getPerspectiveProjection(float fov, float aspect, float zNear, float zFar);
Matrix4x4 getRotationMatrixX(float angle);
Matrix4x4 getRotationMatrixY(float angle);
Matrix4x4 getRotationMatrixZ(float angle);
Matrix4x4 getTranslationMatrix(float x, float y, float z);
Matrix4x4 multiplyMatrix4x4(Matrix4x4 a, Matrix4x4 b);

Vec4 applyPerspectiveProjection(const Matrix4x4 *projMatrix, const Vec4 *vertex);
Vec4 multiplyMatrixVec4(const Matrix4x4 *mat, const Vec4 *vec);
Vec4 vec4_lerp(Vec4 *v1, Vec4 *v2, float t);
Vec4 vec4_add(Vec4 *a, Vec4 *b);
Vec4 vec4_sub(Vec4 *a, Vec4 *b);
Vec4 vec4_mul(Vec4 *a, Vec4 *b);
Vec4 vec4_mul_scalar(Vec4 *a, float scalar);
Vec4 vec4_div(Vec4 *a, Vec4 *b);
Vec4 vec4_cross(Vec4 *a, Vec4 * b);
Vec4 vec4_neg(Vec4 *a);

Vec4 vec4_min(const Vec4 *a, const Vec4 *b);
Vec4 vec4_max(const Vec4 *a, const Vec4 *b);

#endif