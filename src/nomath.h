#ifndef NOMATH_H_
#define NOMATH_H_

#ifndef PI
    #define PI 3.14159265358979323846
#endif
#ifndef FLOAT_EPSILON
    #define FLOAT_EPSILON 1.192092896e-07f
#endif
#ifndef INFINITY
    #define INFINITY (1e30f*1e30f)
#endif

#ifndef SIGN
    #define SIGN(T, a) (((T)(a) > 0) - ((T)(a) < 0))
#endif 

#ifndef ABS
    #define ABS(T, a) (SIGN(T, a) * a)
#endif

#ifndef CLITERAL
    #ifdef __cplusplus
        #define CLITERAL(T) T
    #else
        #define CLITERAL(T) (T)
    #endif
#endif

#ifndef DEG2RAD
#define DEG2RAD(deg) (((PI)/180.0f)*(deg))
#endif

#ifndef RAD2DEG
#define RAD2DEG(rad) (((180.0f/(PI))*(deg))
#endif

#ifndef NOMATH_TYPES
#define NOMATH_TYPES
typedef union Vector2 {
    float elements[2];
    struct {
        union {
            float x, r, s;
        };
        union {
            float y, g, t;
        };
    };
} Vector2;

typedef union Vector3 {
    float elements[3];
    struct {
        union {
            float x, r, s;
        };
        union {
            float y, g, t;
        };
        union {
            float z, b, p;
        };
    };
} Vector3;

typedef union Vector4 {
    float elements[4];
    struct {
        union {
            float x, r, s;
        };
        union {
            float y, g, t;
        };
        union {
            float z, b, p;
        };
        union {
            float w, a, q;
        };
    };
} Vector4;

typedef union Matrix {
    float elements[4*4];
    Vector4 rows[4];
} Matrix;
#endif // NOMATH_TYPES

Vector2 Vector2Add(Vector2 a, Vector2 b);
Vector2 Vector2Sub(Vector2 a, Vector2 b);
float Vector2Dot(Vector2 a, Vector2 b);
float Vector2LengthSqr(Vector2 a);
float Vector2Length(Vector2 a);
Vector2 Vector2Normalize(Vector2 a);

Vector3 Vector3Add(Vector3 a, Vector3 b);
Vector3 Vector3Sub(Vector3 a, Vector3 b);
float Vector3Dot(Vector3 a, Vector3 b);
float Vector3LengthSqr(Vector3 a);
float Vector3Length(Vector3 a);
Vector3 Vector3Normalize(Vector3 a);
Vector3 Vector3Cross(Vector3 a, Vector3 b);

Matrix MatrixCreate(float eye);
Matrix MatrixAdd(Matrix a, Matrix b);
Matrix MatrixSub(Matrix a, Matrix b);
Matrix MatrixDot(Matrix a, Matrix b);

Matrix MatrixOrthographic(float left, float right, float bottom, float top, float far, float near);
Matrix MatrixPerspective(float fovRadians, float aspectRatio, float near, float far);
Matrix MatrixTranslate(Matrix a, Vector3 v3);
Matrix MatrixScale(Matrix a, Vector3 v3);
Matrix MatrixRotateX(Matrix a, float angleRadians);
Matrix MatrixRotateY(Matrix a, float angleRadians);
Matrix MatrixRotateZ(Matrix a, float angleRadians);
Matrix MatrixLookAt(Vector3 pos, Vector3 target, Vector3 up);

#endif // NOMATH_H_

#ifdef NOMATH_IMPLEMENTATION

#include <math.h> // 

Vector2 Vector2Add(Vector2 a, Vector2 b)
{
    return CLITERAL(Vector2){ .x=(a.x + b.x), .y=(a.y + b.y) };
}

Vector2 Vector2Sub(Vector2 a, Vector2 b)
{
    return CLITERAL(Vector2){ .x=(a.x - b.x), .y=(a.y - b.y) };
}

float Vector2Dot(Vector2 a, Vector2 b)
{
    return ((a.x*b.x) + (a.y*b.y));
}

float Vector2LengthSqr(Vector2 a)
{
    return ((a.x*a.x) + (a.y*a.y));
}

float Vector2Length(Vector2 a)
{
    return sqrtf(Vector2LengthSqr(a));
}

Vector2 Vector2Normalize(Vector2 a)
{
    float length = Vector2Length(a);
    if (length == 0.0f) length = 1.0f;
    float ilength = 1.0f/length;
    a.x *= ilength;
    a.y *= ilength;
    return a;
}

Vector3 Vector3Add(Vector3 a, Vector3 b)
{
    return CLITERAL(Vector3){ .x=(a.x + b.x), .y=(a.y + b.y), .z=(a.z + b.z), };
}

Vector3 Vector3Sub(Vector3 a, Vector3 b)
{
    return CLITERAL(Vector3){ .x=(a.x - b.x), .y=(a.y - b.y), .z=(a.z - b.z) };
}

float Vector3Dot(Vector3 a, Vector3 b)
{
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}

float Vector3LengthSqr(Vector3 a)
{
    return ((a.x*a.x) + (a.y*a.y) +(a.z*a.z));
}

float Vector3Length(Vector3 a)
{
    return sqrtf(Vector3LengthSqr(a));
}

Vector3 Vector3Normalize(Vector3 a)
{
    float length = Vector3Length(a);
    if (length == 0.0f) length = 1.0f;
    float ilength = 1.0f/length;
    a.x *= ilength;
    a.y *= ilength;
    a.z *= ilength;
    return a;
}

Vector3 Vector3Cross(Vector3 a, Vector3 b)
{
    return CLITERAL(Vector3) {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };
}

Vector4 Vector4Add(Vector4 a, Vector4 b)
{
    return CLITERAL(Vector4){ .x=(a.x + b.x), .y=(a.y + b.y), .z=(a.z + b.z), .w=(a.w + b.w) };
}

Vector4 Vector4Sub(Vector4 a, Vector4 b)
{
    return CLITERAL(Vector4){ .x=(a.x - b.x), .y=(a.y - b.y), .z=(a.z - b.z), .w=(a.w - b.w) };
}

float Vector4Dot(Vector4 a, Vector4 b)
{
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w));
}

float Vector4LengthSqr(Vector4 a)
{
    return ((a.x*a.x) + (a.y*a.y) + (a.z*a.z) + (a.w*a.w));
}

float Vector4Length(Vector4 a)
{
    return sqrtf(Vector4LengthSqr(a));
}

Vector4 Vector4Normalize(Vector4 a)
{
    float length = Vector4Length(a);
    if (length == 0.0f) length = 1.0f;
    float ilength = 1.0f/length;
    a.x *= ilength;
    a.y *= ilength;
    a.z *= ilength;
    a.w *= ilength;
    return a;
}

Matrix MatrixCreate(float eye)
{
    Matrix result = CLITERAL(Matrix){0};
    result.elements[0] = eye;
    result.elements[5] = eye;
    result.elements[10] = eye;
    result.elements[15] = eye;
    return result;
}

Matrix MatrixAdd(Matrix a, Matrix b)
{
    for(int i=0; i<16; ++i)
        a.elements[i] += b.elements[i];
    return a;
}

Matrix MatrixSub(Matrix a, Matrix b)
{
    for(int i=0; i<16; ++i)
        a.elements[i] -= b.elements[i];
    return a;
}

Matrix MatrixDot(Matrix a, Matrix b)
{
    Matrix result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {            
            result.elements[i * 4 + j] = 0.0f;
            for(int k = 0; k < 4; ++k) {
                result.elements[i * 4 + j] += 
                    a.elements[i * 4 + k] *
                    b.elements[k * 4 + j];
            }
        }
    }
    return result;
}

Matrix MatrixOrthographic(float left, float right, float bottom, float top, float far, float near)
{
    Matrix result = MatrixCreate(1.0f);
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
    Matrix res = MatrixCreate(1.0f);
    float halfTanFOV = tan(fovRadians/2);
    res.elements[0] = 1.0f / (aspectRatio * halfTanFOV);
    res.elements[5] = 1.0f / halfTanFOV;
    res.elements[10] = -((far + near) / (far - near));
    res.elements[11] = -1.0f;
    res.elements[14] = -((2.0f * far * near) / (far - near));
    return res;
}

Matrix MatrixTranslate(Matrix a, Vector3 v3)
{
    Matrix transformation_matrix = MatrixCreate(1.0f);
    transformation_matrix.elements[12]  = v3.x;
    transformation_matrix.elements[13]  = v3.y;
    transformation_matrix.elements[14] = v3.z;
    return MatrixDot(a, transformation_matrix);
}

Matrix MatrixScale(Matrix a, Vector3 v3)
{
    Matrix transformation_matrix = MatrixCreate(1.0f);
    transformation_matrix.elements[0] = v3.x;
    transformation_matrix.elements[5] = v3.y;
    transformation_matrix.elements[10] = v3.z;

    return MatrixDot(a, transformation_matrix);
}

Matrix MatrixRotateX(Matrix a, float angleRadians)
{
    Matrix transformation_matrix = MatrixCreate(1.0f);
    float s = sin(angleRadians);
    float c = cos(angleRadians);
    transformation_matrix.elements[5] = c;
    transformation_matrix.elements[6] = s;
    transformation_matrix.elements[9] = -s;
    transformation_matrix.elements[10] = c;
    return MatrixDot(a, transformation_matrix);
}

Matrix MatrixRotateY(Matrix a, float angleRadians)
{
    Matrix transformation_matrix = MatrixCreate(1.0f);
    float s = sin(angleRadians);
    float c = cos(angleRadians);

    transformation_matrix.elements[0] = c;
    transformation_matrix.elements[2] = -s;
    transformation_matrix.elements[8] = s;
    transformation_matrix.elements[10] = c;

    return MatrixDot(a, transformation_matrix);
}

Matrix MatrixRotateZ(Matrix a, float angleRadians)
{
    Matrix transformation_matrix = MatrixCreate(1.0f);
    float s = sin(angleRadians);
    float c = cos(angleRadians);

    transformation_matrix.elements[0] = c;
    transformation_matrix.elements[1] = -s;
    transformation_matrix.elements[4] = s;
    transformation_matrix.elements[5] = c;

    return MatrixDot(a, transformation_matrix);
}

Matrix MatrixLookAt(Vector3 pos, Vector3 target, Vector3 up)
{
    Matrix result = MatrixCreate(1.0f);
    Vector3 z_axis = Vector3Normalize(Vector3Sub(target, pos)); // direction vector
    Vector3 x_axis = Vector3Normalize(Vector3Cross(z_axis, up)); // right vector
    Vector3 y_axis = Vector3Cross(x_axis, z_axis);

    result.elements[0] = x_axis.x;
    result.elements[1] = y_axis.x;
    result.elements[2] = -z_axis.x;
    result.elements[3] = 0;
    result.elements[4] = x_axis.y;
    result.elements[5] = y_axis.y;
    result.elements[6] = -z_axis.y;
    result.elements[7] = 0;
    result.elements[8] = x_axis.z;
    result.elements[9] = y_axis.z;
    result.elements[10] = -z_axis.z;
    result.elements[11] = 0;
    result.elements[12] = -Vector3Dot(x_axis, pos);
    result.elements[13] = -Vector3Dot(y_axis, pos);
    result.elements[14] =  Vector3Dot(z_axis, pos);
    result.elements[15] = 1.0f;

    return result;
}

#endif // NOMATH_IMPLEMENTATION
