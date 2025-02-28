//HappyEngine Copyright (C) 2011 - 2014  Evil Interactive
//
//This file is part of HappyEngine.
//
//    HappyEngine is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    HappyEngine is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with HappyEngine.  If not, see <http://www.gnu.org/licenses/>.
#include "HappyPCH.h" 

#include "mat44.h"
#include "mat33.h"

#include "ICamera.h"

namespace he {

mat44::mat44(): m_Matrix(physx::PxVec4(1.0f, 1.0f, 1.0f, 1.0f))
{
}
mat44::mat44(const mat44& other): m_Matrix(other.m_Matrix)
{
}
mat44& mat44::operator=(const mat44& other)
{
    m_Matrix = other.m_Matrix;
    return *this;
}

mat44::mat44(const physx::PxMat44& mat): m_Matrix(mat)
{
}
mat44::mat44(const vec4& col0, const vec4& col1, const vec4& col2, const vec4& col3):
    m_Matrix(physx::PxVec4(col0.x, col0.y, col0.z, col0.w),
             physx::PxVec4(col1.x, col1.y, col1.z, col1.w),
             physx::PxVec4(col2.x, col2.y, col2.z, col2.w),
             physx::PxVec4(col3.x, col3.y, col3.z, col3.w))
{
}
mat44::mat44( float _00, float _01, float _02, float _03,
              float _10, float _11, float _12, float _13,
              float _20, float _21, float _22, float _23,
              float _30, float _31, float _32, float _33 ) : 
m_Matrix(physx::PxVec4(_00, _10, _20, _30),
         physx::PxVec4(_01, _11, _21, _31),
         physx::PxVec4(_02, _12, _22, _32),
         physx::PxVec4(_03, _13, _23, _33))
{
}

mat44::~mat44()
{
}

//static constructors
mat44 mat44::createPerspectiveRH(float fov, float viewWidth, float viewHeight, float zNear, float zFar)
{
    return createPerspectiveRH(fov, viewWidth / viewHeight, zNear, zFar);
}

he::mat44 mat44::createPerspectiveRH( float fov, float aspectRatio, float zNear, float zFar )
{
    float _11 = cosf(fov * 0.5f) / sinf(fov * 0.5f);
    float _00 = _11 * aspectRatio;
    return mat44(
        _00, 0.0f, 0.0f                            , 0.0f,
        0.0f,  _11, 0.0f                            , 0.0f,
        0.0f, 0.0f, (zFar) / (zFar - zNear)         , (zFar * zNear) / (zFar - zNear),
        0.0f, 0.0f, -1.0f                           , 0.0f);
}

mat44 mat44::createPerspectiveLH(float fov, float viewWidth, float viewHeight, float zNear, float zFar)
{
    return createPerspectiveLH(fov, viewWidth / viewHeight, zNear, zFar);
}

he::mat44 mat44::createPerspectiveLH( float fov, float aspectRatio, float zNear, float zFar )
{
    float _11 = cosf(fov * 0.5f) / sinf(fov * 0.5f);
    float _00 = _11 / aspectRatio;
    return mat44(
        _00, 0.0f, 0.0f                     , 0.0f,
        0.0f,  _11, 0.0f                     , 0.0f,
        0.0f, 0.0f, (zFar) / (zFar - zNear)  , -(zFar * zNear) / (zFar - zNear),
        0.0f, 0.0f,  1.0f                     , 0.0f);
}

mat44 mat44::createOrthoLH(float left, float right, float top, float bottom, float zNear, float zFar)
{
    return mat44(
        2.0f / (right - left),                    0.0f,                    0.0f,   (left + right) / (left - right),
                         0.0f,   2.0f / (top - bottom),                    0.0f,   (top + bottom) / (bottom - top),
                         0.0f,                    0.0f,   1.0f / (zFar - zNear),            zNear / (zNear - zFar),
                         0.0f,                    0.0f,                    0.0f,                             1.0f);
}
mat44 mat44::createOrthoRH(float left, float right, float top, float bottom, float zNear, float zFar)
{
    return mat44(
        2.0f / (right - left),                    0.0f,                    0.0f,   (left + right) / (left - right),
                         0.0f,   2.0f / (top - bottom),                    0.0f,   (top + bottom) / (bottom - top),
                         0.0f,                    0.0f,   1.0f / (zNear - zFar),            zNear / (zNear - zFar),
                         0.0f,                    0.0f,                    0.0f,                             1.0f);
}
mat44 mat44::createLookAtLH(const vec3& eye, const vec3& target, const vec3& up)
{
    vec3 zaxis(normalize(target - eye));
    vec3 xaxis(normalize(cross(zaxis, normalize(up))));
    vec3 yaxis(cross(xaxis, zaxis));

    return mat44(
           xaxis.x,  xaxis.y,  xaxis.z, -dot(xaxis, eye),
           yaxis.x,  yaxis.y,  yaxis.z, -dot(yaxis, eye),
           zaxis.x,  zaxis.y,  zaxis.z, -dot(zaxis, eye),
              0.0f,     0.0f,     0.0f,            1.0f);     
}
mat44 mat44::createLookAtRH(const vec3& eye, const vec3& target, const vec3& up)
{
    vec3 zaxis(normalize(eye - target));
    vec3 xaxis(normalize(cross(normalize(up), zaxis)));
    vec3 yaxis(cross(zaxis, xaxis));

    return mat44(
           -xaxis.x,  -xaxis.y,  -xaxis.z,  dot(xaxis, eye),
            yaxis.x,   yaxis.y,   yaxis.z, -dot(yaxis, eye),
           -zaxis.x,  -zaxis.y,  -zaxis.z, -dot(zaxis, eye),
              0.0f,     0.0f,     0.0f,            1.0f);     
}
mat44 mat44::createTranslation(const vec3& translation)
{
    return mat44(1.0f, 0.0f, 0.0f, translation.x,
                  0.0f, 1.0f, 0.0f, translation.y,
                  0.0f, 0.0f, 1.0f, translation.z,
                  0.0f, 0.0f, 0.0f, 1.0f);

}
mat44 mat44::createRotation(const vec3& axis, float radians)
{
    float cosAlpha(cosf(radians));
    float sinAlpha(sinf(radians));

    return mat44(
        cosAlpha + sqr(axis.x) * (1 - cosAlpha),                    axis.x * axis.y * (1 - cosAlpha) - axis.z * sinAlpha,       axis.x * axis.z * (1 - cosAlpha) + axis.y * sinAlpha,       0.0f,
        axis.x * axis.y * (1 - cosAlpha) + axis.z * sinAlpha,       cosAlpha + sqr(axis.y) * (1 - cosAlpha),                    axis.y * axis.z * (1 - cosAlpha) - axis.x * sinAlpha,       0.0f,
        axis.x * axis.z * (1 - cosAlpha) - axis.y * sinAlpha,       axis.y * axis.z * (1 - cosAlpha) + axis.x * sinAlpha,       cosAlpha + sqr(axis.z) * (1 - cosAlpha),                    0.0f,
        0.0f,                                                       0.0f,                                                       0.0f,                                                       1.0f);
}
mat44 mat44::createScale(const vec3& scale)
{
    return mat44(scale.x, 0.0f, 0.0f,    0.0f,
                  0.0f, scale.y, 0.0f,    0.0f,
                  0.0f,    0.0f, scale.z, 0.0f,
                  0.0f,    0.0f, 0.0f,    1.0f);
}
mat44 mat44::createScale(float xScale, float yScale, float zScale)
{
    return mat44(xScale,  0.0f,   0.0f,  0.0f,
                  0.0f,  yScale,   0.0f,  0.0f,
                  0.0f,    0.0f, zScale,  0.0f,
                  0.0f,    0.0f,   0.0f,  1.0f);
}
mat44 mat44::createScale(float scale)
{
    return mat44(scale,  0.0f,  0.0f,  0.0f,
                  0.0f,  scale,  0.0f,  0.0f,
                  0.0f,   0.0f, scale,  0.0f,
                  0.0f,   0.0f,  0.0f,  1.0f);
}
mat44 mat44::createBillboard( const gfx::ICamera* pCam )
{
    const mat44& v(pCam->getView());
    return mat44(v(0, 0),  v(1, 0),  v(2, 0),  0.0f,
                 v(0, 1),  v(1, 1),  v(2, 1),  0.0f,
                 v(0, 2),  v(1, 2),  v(2, 2),  0.0f,
                    0.0f,     0.0f,     0.0f,  1.0f);
}

he::mat44 mat44::createWorld( const vec3& position, const vec3& forward, const vec3& up )
{ 
    vec3 xaxis(normalize(cross(up, forward)));

    //return mat44(
    //    xaxis.x,        xaxis.y,        xaxis.z,        0,
    //    up.x,           up.y,           up.z,           0,
    //    forward.x,      forward.y,      forward.z,      0,
    //    position.x,     position.y,     position.z,     1.0f); 
    return mat44(
        xaxis.x,        up.x,           forward.x,      position.x,
        xaxis.y,        up.y,           forward.y,      position.y,
        xaxis.z,        up.z,           forward.z,      position.z,
        0,                 0,                   0,      1.0f);
}

he::mat44 mat44::createWorld( const vec3& translation, const mat33& rotation, const vec3& scale )
{
    return he::mat44(  rotation(0, 0) * scale.x, rotation(0, 1) * scale.y, rotation(0, 2) * scale.z, translation.x,
                       rotation(1, 0) * scale.x, rotation(1, 1) * scale.y, rotation(1, 2) * scale.z, translation.y,
                       rotation(2, 0) * scale.x, rotation(2, 1) * scale.y, rotation(2, 2) * scale.z, translation.z,
                                              0,                        0,                        0,             1);
}

mat44 mat44::operator*(const mat44& mat) const
{
    return mat44(m_Matrix * mat.m_Matrix);
}
mat44 mat44::operator*(float scale) const
{
    return mat44(m_Matrix * scale);
}
mat44& mat44::operator*=( float scale )
{
    m_Matrix *= scale;
    return *this;
}

mat44& mat44::operator*=( const mat44& mat )
{
    m_Matrix = m_Matrix * mat.m_Matrix;
    return *this;
}

vec3 mat44::operator*(const vec3& vec) const
{
    return vec3(m_Matrix.transform(physx::PxVec3(vec.x, vec.y, vec.z)));
}
vec4 mat44::operator*(const vec4& vec) const
{
    return vec4(m_Matrix.transform(physx::PxVec4(vec.x, vec.y, vec.z, vec.w)));
}

const float* mat44::toFloatArray() const
{
    return &m_Matrix.column0.x;
}

const physx::PxMat44& mat44::getPhyicsMatrix() const
{
    return m_Matrix;
}

//http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
mat44 mat44::inverse() const
{
    const float& m00(m_Matrix.column0.x);
    const float& m10(m_Matrix.column0.y);
    const float& m20(m_Matrix.column0.z);
    const float& m30(m_Matrix.column0.w);

    const float& m01(m_Matrix.column1.x);
    const float& m11(m_Matrix.column1.y);
    const float& m21(m_Matrix.column1.z);
    const float& m31(m_Matrix.column1.w);

    const float& m02(m_Matrix.column2.x);
    const float& m12(m_Matrix.column2.y);
    const float& m22(m_Matrix.column2.z);
    const float& m32(m_Matrix.column2.w);

    const float& m03(m_Matrix.column3.x);
    const float& m13(m_Matrix.column3.y);
    const float& m23(m_Matrix.column3.z);
    const float& m33(m_Matrix.column3.w);

    mat44 mtxInverse(
        m12*m23*m31 - m13*m22*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 + m11*m22*m33,
        m03*m22*m31 - m02*m23*m31 - m03*m21*m32 + m01*m23*m32 + m02*m21*m33 - m01*m22*m33,
        m02*m13*m31 - m03*m12*m31 + m03*m11*m32 - m01*m13*m32 - m02*m11*m33 + m01*m12*m33,
        m03*m12*m21 - m02*m13*m21 - m03*m11*m22 + m01*m13*m22 + m02*m11*m23 - m01*m12*m23,

        m13*m22*m30 - m12*m23*m30 - m13*m20*m32 + m10*m23*m32 + m12*m20*m33 - m10*m22*m33,
        m02*m23*m30 - m03*m22*m30 + m03*m20*m32 - m00*m23*m32 - m02*m20*m33 + m00*m22*m33,
        m03*m12*m30 - m02*m13*m30 - m03*m10*m32 + m00*m13*m32 + m02*m10*m33 - m00*m12*m33,
        m02*m13*m20 - m03*m12*m20 + m03*m10*m22 - m00*m13*m22 - m02*m10*m23 + m00*m12*m23,

        m11*m23*m30 - m13*m21*m30 + m13*m20*m31 - m10*m23*m31 - m11*m20*m33 + m10*m21*m33,
        m03*m21*m30 - m01*m23*m30 - m03*m20*m31 + m00*m23*m31 + m01*m20*m33 - m00*m21*m33,
        m01*m13*m30 - m03*m11*m30 + m03*m10*m31 - m00*m13*m31 - m01*m10*m33 + m00*m11*m33,
        m03*m11*m20 - m01*m13*m20 - m03*m10*m21 + m00*m13*m21 + m01*m10*m23 - m00*m11*m23,

        m12*m21*m30 - m11*m22*m30 - m12*m20*m31 + m10*m22*m31 + m11*m20*m32 - m10*m21*m32,
        m01*m22*m30 - m02*m21*m30 + m02*m20*m31 - m00*m22*m31 - m01*m20*m32 + m00*m21*m32,
        m02*m11*m30 - m01*m12*m30 - m02*m10*m31 + m00*m12*m31 + m01*m10*m32 - m00*m11*m32,
        m01*m12*m20 - m02*m11*m20 + m02*m10*m21 - m00*m12*m21 - m01*m10*m22 + m00*m11*m22
    );
    mtxInverse *= 1.0f / getDeterminant();

    return mtxInverse;
}
float mat44::getDeterminant() const
{
    const float& m00(m_Matrix.column0.x);
    const float& m10(m_Matrix.column0.y);
    const float& m20(m_Matrix.column0.z);
    const float& m30(m_Matrix.column0.w);

    const float& m01(m_Matrix.column1.x);
    const float& m11(m_Matrix.column1.y);
    const float& m21(m_Matrix.column1.z);
    const float& m31(m_Matrix.column1.w);

    const float& m02(m_Matrix.column2.x);
    const float& m12(m_Matrix.column2.y);
    const float& m22(m_Matrix.column2.z);
    const float& m32(m_Matrix.column2.w);

    const float& m03(m_Matrix.column3.x);
    const float& m13(m_Matrix.column3.y);
    const float& m23(m_Matrix.column3.z);
    const float& m33(m_Matrix.column3.w);

    return m03*m12*m21*m30 - m02*m13*m21*m30 - m03*m11*m22*m30 + m01*m13*m22*m30 +
           m02*m11*m23*m30 - m01*m12*m23*m30 - m03*m12*m20*m31 + m02*m13*m20*m31 +
           m03*m10*m22*m31 - m00*m13*m22*m31 - m02*m10*m23*m31 + m00*m12*m23*m31 +
           m03*m11*m20*m32 - m01*m13*m20*m32 - m03*m10*m21*m32 + m00*m13*m21*m32 +
           m01*m10*m23*m32 - m00*m11*m23*m32 - m02*m11*m20*m33 + m01*m12*m20*m33 +
           m02*m10*m21*m33 - m00*m12*m21*m33 - m01*m10*m22*m33 + m00*m11*m22*m33;
}

float mat44::operator()( int row, int column ) const
{
    return m_Matrix(row, column);
}

float& mat44::operator()( int row, int column )
{
    return m_Matrix(row, column);
}

bool mat44::operator==(const mat44& other) const
{
    return  m_Matrix.column0 == other.m_Matrix.column0 && 
            m_Matrix.column1 == other.m_Matrix.column1 &&
            m_Matrix.column2 == other.m_Matrix.column2 &&
            m_Matrix.column3 == other.m_Matrix.column3;
}
bool mat44::operator!=(const mat44& other) const
{
    return !operator==(other);
}

void mat44::getTranslationComponent( vec3& translation ) const
{
    translation.x = m_Matrix.column3.x; 
    translation.y = m_Matrix.column3.y;
    translation.z = m_Matrix.column3.z;
}
void mat44::getRotationComponent( mat33& rotation ) const
{
    for (uint8 row(0); row < 3; ++row)
    for (uint8 col(0); col < 3; ++col)
        rotation(row, col) = m_Matrix(row, col);
}

void mat44::transpose()
{
    m_Matrix = m_Matrix.getTranspose();
}


//Static
const mat44 mat44::Identity = mat44(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    0, 0, 0, 1);

} //end namespace
