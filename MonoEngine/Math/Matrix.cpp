//
//  Matrix.cpp
//  MonoOSX
//
//  Created by Niklas Damberg on 26/11/14.
//
//

#include "Matrix.h"
#include "Vector2f.h"
#include <cmath>

void math::Identity(Matrix& matrix)
{
    matrix.data[0] = 1;
    matrix.data[1] = 0;
    matrix.data[2] = 0;
    matrix.data[3] = 0;

    matrix.data[4] = 0;
    matrix.data[5] = 1;
    matrix.data[6] = 0;
    matrix.data[7] = 0;

    matrix.data[8] = 0;
    matrix.data[9] = 0;
    matrix.data[10] = 1;
    matrix.data[11] = 0;

    matrix.data[12] = 0;
    matrix.data[13] = 0;
    matrix.data[14] = 0;
    matrix.data[15] = 1;
}

void math::Translate(Matrix& matrix, const Vector2f& vector)
{
    matrix.data[12] += vector.mX;
    matrix.data[13] += vector.mY;
    //matrix.data[14] += 0.0f;

    // The elements of the matrix are stored as column major order.
    // |  0  4  8 12 |
    // |  1  5  9 13 |
    // |  2  6 10 14 |
    // |  3  7 11 15 |

    /*
    const float x = vector.mX;
    const float y = vector.mY;

    matrix.m00 += matrix.m03 * x;
    matrix.m10 += matrix.m13 * x;
    matrix.m20 += matrix.m23 * x;
    matrix.m30 += matrix.m33 * x;

    matrix.m01 += matrix.m03 * y;
    matrix.m11 += matrix.m13 * y;
    matrix.m21 += matrix.m23 * y;
    matrix.m31 += matrix.m33 * y;
     */


    /*
    m[0] += m[3] * x;   m[4] += m[7] * x;   m[8] += m[11]* x;   m[12]+= m[15]* x;
    m[1] += m[3] * y;   m[5] += m[7] * y;   m[9] += m[11]* y;   m[13]+= m[15]* y;
    m[2] += m[3] * z;   m[6] += m[7] * z;   m[10]+= m[11]* z;   m[14]+= m[15]* z;
     */
}

void math::Position(Matrix& matrix, const Vector2f& position)
{
    matrix.data[12] = position.mX;
    matrix.data[13] = position.mY;
    //matrix.data[14] += 0.0f;
}

void math::RotateZ(Matrix& matrix, float radians)
{
    const float sine = std::sin(radians);
    const float cosine = std::cos(radians);

    const float m0 = matrix.data[0];
    const float m4 = matrix.data[4];
    const float m8 = matrix.data[8];
    const float m12 = matrix.data[12];

    const float m1 = matrix.data[1];
    const float m5 = matrix.data[5];
    const float m9 = matrix.data[9];
    const float m13 = matrix.data[13];

    matrix.data[0] = m0 * cosine + m1 * -sine;
    matrix.data[1] = m0 * sine   + m1 * cosine;

    matrix.data[4] = m4 * cosine + m5 * -sine;
    matrix.data[5] = m4 * sine   + m5 * cosine;

    matrix.data[8] = m8 * cosine + m9 * -sine;
    matrix.data[9] = m8 * sine   + m9 * cosine;

    matrix.data[12] = m12 * cosine + m13 * -sine;
    matrix.data[13] = m12 * sine   + m13 * cosine;
}

void math::ScaleXY(Matrix& matrix, const Vector2f& scale)
{
    // The elements of the matrix are stored as column major order.
    // |  0  4  8 12 |
    // |  1  5  9 13 |
    // |  2  6 10 14 |
    // |  3  7 11 15 |

    matrix.data[0] *= scale.mX;
    matrix.data[4] *= scale.mX;
    matrix.data[8] *= scale.mX;
    matrix.data[12] *= scale.mX;

    matrix.data[1] *= scale.mY;
    matrix.data[5] *= scale.mY;
    matrix.data[9] *= scale.mY;
    matrix.data[13] *= scale.mY;

    //matrix.data[2] *= scale.mZ;
    //matrix.data[6] *= scale.mZ;
    //matrix.data[10] *= scale.mZ;
    //matrix.data[14] *= scale.mZ;
}

void math::Transpose(Matrix& matrix)
{
    std::swap(matrix.data[1], matrix.data[4]);
    std::swap(matrix.data[2], matrix.data[8]);
    std::swap(matrix.data[3], matrix.data[12]);

    std::swap(matrix.data[6], matrix.data[9]);
    std::swap(matrix.data[7], matrix.data[13]);

    std::swap(matrix.data[11], matrix.data[14]);
}


void math::operator *= (Matrix& left, const Matrix& right)
{
    const float m0 = left.data[0];
    const float m1 = left.data[1];
    const float m2 = left.data[2];
    const float m3 = left.data[3];

    const float m4 = left.data[4];
    const float m5 = left.data[5];
    const float m6 = left.data[6];
    const float m7 = left.data[7];

    const float m8  = left.data[8];
    const float m9  = left.data[9];
    const float m10 = left.data[10];
    const float m11 = left.data[11];

    const float m12 = left.data[12];
    const float m13 = left.data[13];
    const float m14 = left.data[14];
    const float m15 = left.data[15];

    left.data[0] = m0 * right.data[0] + m4 * right.data[1] + m8  * right.data[2] + m12 * right.data[3];
    left.data[1] = m1 * right.data[0] + m5 * right.data[1] + m9  * right.data[2] + m13 * right.data[3];
    left.data[2] = m2 * right.data[0] + m6 * right.data[1] + m10 * right.data[2] + m14 * right.data[3];
    left.data[3] = m3 * right.data[0] + m7 * right.data[1] + m11 * right.data[2] + m15 * right.data[3];

    left.data[4] = m0 * right.data[4] + m4 * right.data[5] + m8  * right.data[6] + m12 * right.data[7];
    left.data[5] = m1 * right.data[4] + m5 * right.data[5] + m9  * right.data[6] + m13 * right.data[7];
    left.data[6] = m2 * right.data[4] + m6 * right.data[5] + m10 * right.data[6] + m14 * right.data[7];
    left.data[7] = m3 * right.data[4] + m7 * right.data[5] + m11 * right.data[6] + m15 * right.data[7];

    left.data[8]  = m0 * right.data[8] + m4 * right.data[9] + m8  * right.data[10] + m12 * right.data[11];
    left.data[9]  = m1 * right.data[8] + m5 * right.data[9] + m9  * right.data[10] + m13 * right.data[11];
    left.data[10] = m2 * right.data[8] + m6 * right.data[9] + m10 * right.data[10] + m14 * right.data[11];
    left.data[11] = m3 * right.data[8] + m7 * right.data[9] + m11 * right.data[10] + m15 * right.data[11];

    left.data[12] = m0 * right.data[12] + m4 * right.data[13] + m8  * right.data[14] + m12 * right.data[15];
    left.data[13] = m1 * right.data[12] + m5 * right.data[13] + m9  * right.data[14] + m13 * right.data[15];
    left.data[14] = m2 * right.data[12] + m6 * right.data[13] + m10 * right.data[14] + m14 * right.data[15];
    left.data[15] = m3 * right.data[12] + m7 * right.data[13] + m11 * right.data[14] + m15 * right.data[15];
}

math::Matrix math::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    math::Matrix matrix;

    matrix.data[0]  =  2.0f / right - left;
    matrix.data[5]  =  2.0f / top - bottom;
    matrix.data[10] = -2.0f / far - near;
    matrix.data[12]  = - (right + left) / (right - left);
    matrix.data[13]  = - (top + bottom) / (top - bottom);
    matrix.data[14] = - (far + near) / (far - near);

    return matrix;
}

