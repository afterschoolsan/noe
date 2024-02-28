#ifndef NOMATH_H_
#define NOMATH_H_

#ifndef CLITERAL
    #ifdef __cplusplus
        #define CLITERAL(T) T
    #else
        #define CLITERAL(T) (T)
    #endif
#endif

#ifndef NOMATH_TYPES
#define NOMATH_TYPES
typedef struct Vector2 {
    float x, y;
} Vector2;

typedef struct Vector3 {
    float x, y, z;
} Vector3;

typedef struct Vector4 {
    float x, y, z, w;
} Vector4;

typedef union Matrix {
    float elements[4*4];
    Vector4 rows[4];
} Matrix;
#endif // NOMATH_TYPES

Matrix MatrixNew(float eye);
Matrix MatrixOrthographic(float left, float right, float bottom, float top, float far, float near);
Matrix MatrixPerspective(float fovRadians, float aspectRatio, float near, float far);
Matrix MatrixLookAt(void);
Matrix MatrixTranslate(void);

#endif // NOMATH_H_

#ifdef NOMATH_IMPLEMENTATION

#include <math.h>

Matrix MatrixNew(float eye)
{
    Matrix result = CLITERAL(Matrix){0};
    result.elements[0] = eye;
    result.elements[5] = eye;
    result.elements[10] = eye;
    result.elements[15] = eye;
    return result;
}

Matrix MatrixOrthographic(float left, float right, float bottom, float top, float far, float near)
{
    Matrix result = MatrixNew(1.0f);
    float lr = 1.0f / (left - right);
    float bt = 1.0f / (bottom - top);
    float nf = 1.0f / (near - far);

    result.elements[0] = -2.0f * lr;
    result.elements[5] = -2.0f * bt;
    result.elements[10] = 2.0f * nf;
    result.elements[12] = (left + right) * lr;
    result.elements[13] = (top + bottom) * bt;
    result.elements[14] = (far + near) * nf;
    return result;
}

Matrix MatrixPerspective(float fovRadians, float aspectRatio, float near, float far)
{
    Matrix res = MatrixNew(1.0f);
    float halfTanFOV = tan(fovRadians/2);
    res.elements[0] = 1.0f / (aspectRatio * halfTanFOV);
    res.elements[5] = 1.0f / halfTanFOV;
    res.elements[10] = -((far + near) / (far - near));
    res.elements[11] = -1.0f;
    res.elements[14] = -((2.0f * far * near) / (far - near));
    return res;
}
#endif // NOMATH_IMPLEMENTATION
