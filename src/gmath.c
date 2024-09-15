#include "gmath.h"

#include <math.h>
#include <stdbool.h>

void vec4_normalize(Vec4 *a, Vec4 *out)
{
    float len = a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w;

    if (len > 0)
    {
        len = 1 / sqrt(len);
        a->x *= len;
        a->y *= len;
        a->z *= len;
        a->w *= len;
    }
}

void invert_matrix4x4(const Matrix4x4 *mat, Matrix4x4 *result)
{
    float a = mat->m[0][0], b = mat->m[1][0], c = mat->m[2][0], d = mat->m[3][0];
    float e = mat->m[0][1], f = mat->m[1][1], g = mat->m[2][1], h = mat->m[3][1];
    float i = mat->m[0][2], j = mat->m[1][2], k = mat->m[2][2], l = mat->m[3][2];
    float m = mat->m[0][3], n = mat->m[1][3], o = mat->m[2][3], p = mat->m[3][3];

    float kp_lo = k * p - l * o;
    float jp_ln = j * p - l * n;
    float jo_kn = j * o - k * n;
    float ip_lm = i * p - l * m;
    float io_km = i * o - k * m;
    float in_jm = i * n - j * m;

    float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
    float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
    float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
    float a14 = -(e * jo_kn - f * io_km + g * in_jm);

    float det = a * a11 + b * a12 + c * a13 + d * a14;

    if (fabsf(det) < __FLT_EPSILON__)
    {
        return;
    }

    float invDet = 1.0f / det;

    result->m[0][0] = a11 * invDet;
    result->m[0][1] = a12 * invDet;
    result->m[0][2] = a13 * invDet;
    result->m[0][3] = a14 * invDet;

    float gp_ho = g * p - h * o;
    float fp_hn = f * p - h * n;
    float fo_gn = f * o - g * n;
    float ep_hm = e * p - h * m;
    float eo_gm = e * o - g * m;
    float en_fm = e * n - f * m;

    result->m[1][0] = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
    result->m[1][1] = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
    result->m[1][2] = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
    result->m[1][3] = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

    result->m[2][0] = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
    result->m[2][1] = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
    result->m[2][2] = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
    result->m[2][3] = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

    float gl_hk = g * l - h * k;
    float fl_hj = f * l - h * j;
    float fk_gj = f * k - g * j;
    float el_hi = e * l - h * i;
    float ek_gi = e * k - g * i;
    float ej_fi = e * j - f * i;

    result->m[3][0] = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
    result->m[3][1] = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
    result->m[3][2] = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
    result->m[3][3] = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;
}

void transpose_matrix4x4(Matrix4x4 *in, Matrix4x4 *out)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out->m[i][j] = in->m[j][i];
        }
    }
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

double dot(Vec4 *a, Vec4 *b)
{
    double result = 0.0;

    result += a->x * b->x;
    result += a->y * b->y;
    result += a->z * b->z;

    return result;
}

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

Vec4 multiplyMatrixVec4(const Matrix4x4 *mat, const Vec4 *vec)
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

    if (transformed.w != 0.0f)
    {
        transformed.x /= transformed.w;
        transformed.y /= transformed.w;
        transformed.z /= transformed.w;
    }

    return transformed;
}

Vec4 vec4_lerp(Vec4 *v1, Vec4 *v2, float t)
{
    Vec4 result;
    result.x = v1->x + t * (v2->x - v1->x);
    result.y = v1->y + t * (v2->y - v1->y);
    result.z = v1->z + t * (v2->z - v1->z);
    result.w = v1->w + t * (v2->w - v1->w);
    return result;
}

Vec4 vec4_add(Vec4 *a, Vec4 *b)
{
    return (Vec4){a->x + b->x, a->y + b->y, a->z + b->z, a->w + b->w};
}

Vec4 vec4_sub(Vec4 *a, Vec4 *b)
{
    return (Vec4){a->x - b->x, a->y - b->y, a->z - b->z, a->w - b->w};
}

Vec4 vec4_mul(Vec4 *a, Vec4 *b)
{
    return (Vec4){a->x * b->x, a->y * b->y, a->z * b->z, a->w * b->w};
}

Vec4 vec4_mul_scalar(Vec4 *a, float scalar)
{
    return (Vec4){a->x * scalar, a->y * scalar, a->z * scalar, a->w * scalar};
}

Vec4 vec4_div(Vec4 *a, Vec4 *b)
{
    return (Vec4){a->x / b->x, a->y / b->y, a->z / b->z, a->w / b->w};
}

Vec4 vec4_cross(Vec4 *a, Vec4 * b)
{
    return (Vec4){
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x};
}

Vec4 vec4_neg(Vec4 *a)
{
    return (Vec4){-a->x, -a->y, -a->z, -a->w};
}

Vec4 vec4_min(const Vec4 *a, const Vec4 *b)
{
    Vec4 result;
    result.x = (a->x < b->x) ? b->x : b->x;
    result.y = (a->y < b->y) ? b->y : b->y;
    result.z = (a->z < b->z) ? b->z : b->z;
    result.w = (a->w < b->w) ? b->w : b->w;
    return result;
}

Vec4 vec4_max(const Vec4 *a, const Vec4 *b)
{
    Vec4 result;
    result.x = (a->x > b->x) ? b->x : b->x;
    result.y = (a->y > b->y) ? b->y : b->y;
    result.z = (a->z > b->z) ? b->z : b->z;
    result.w = (a->w > b->w) ? b->w : b->w;
    return result;
}