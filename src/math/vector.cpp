//
// Lol Engine
//
// Copyright: (c) 2010-2014 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <cstdlib> /* free() */
#include <cstring> /* strdup() */

#include <ostream> /* std::ostream */

#include <lol/main.h>

namespace lol
{

/*
 * Return the determinant of a 2×2 matrix.
 */
static inline float det2(float a, float b,
                         float c, float d)
{
    return a * d - b * c;
}

/*
 * Return the determinant of a 3×3 matrix.
 */
static inline float det3(float a, float b, float c,
                         float d, float e, float f,
                         float g, float h, float i)
{
    return a * (e * i - h * f)
         + b * (f * g - i * d)
         + c * (d * h - g * e);
}

/*
 * Return the cofactor of the (i,j) entry in a 2×2 matrix.
 */
static inline float cofact(mat2 const &m, int i, int j)
{
    float tmp = m[(i + 1) & 1][(j + 1) & 1];
    return ((i + j) & 1) ? -tmp : tmp;
}

/*
 * Return the cofactor of the (i,j) entry in a 3×3 matrix.
 */
static inline float cofact(mat3 const &m, int i, int j)
{
    return det2(m[(i + 1) % 3][(j + 1) % 3],
                m[(i + 2) % 3][(j + 1) % 3],
                m[(i + 1) % 3][(j + 2) % 3],
                m[(i + 2) % 3][(j + 2) % 3]);
}

/*
 * Return the cofactor of the (i,j) entry in a 4×4 matrix.
 */
static inline float cofact(mat4 const &m, int i, int j)
{
    return det3(m[(i + 1) & 3][(j + 1) & 3],
                m[(i + 2) & 3][(j + 1) & 3],
                m[(i + 3) & 3][(j + 1) & 3],
                m[(i + 1) & 3][(j + 2) & 3],
                m[(i + 2) & 3][(j + 2) & 3],
                m[(i + 3) & 3][(j + 2) & 3],
                m[(i + 1) & 3][(j + 3) & 3],
                m[(i + 2) & 3][(j + 3) & 3],
                m[(i + 3) & 3][(j + 3) & 3]) * (((i + j) & 1) ? -1.0f : 1.0f);
}

template<> float determinant(mat2 const &m)
{
    return det2(m[0][0], m[0][1],
                m[1][0], m[1][1]);
}

template<> mat2 inverse(mat2 const &m)
{
    mat2 ret;
    float d = determinant(m);
    if (d)
    {
        d = 1.0f / d;
        for (int j = 0; j < 2; j++)
            for (int i = 0; i < 2; i++)
                ret[j][i] = cofact(m, i, j) * d;
    }
    return ret;
}

template<> float determinant(mat3 const &m)
{
    return det3(m[0][0], m[0][1], m[0][2],
                m[1][0], m[1][1], m[1][2],
                m[2][0], m[2][1], m[2][2]);
}

template<> mat3 inverse(mat3 const &m)
{
    mat3 ret;
    float d = determinant(m);
    if (d)
    {
        d = 1.0f / d;
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 3; i++)
                ret[j][i] = cofact(m, i, j) * d;
    }
    return ret;
}

template<> float determinant(mat4 const &m)
{
    float ret = 0;
    for (int n = 0; n < 4; n++)
        ret += m[n][0] * cofact(m, n, 0);
    return ret;
}

template<> mat4 inverse(mat4 const &m)
{
    mat4 ret;
    float d = determinant(m);
    if (d)
    {
        d = 1.0f / d;
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                ret[j][i] = cofact(m, i, j) * d;
    }
    return ret;
}

#define LOL_PRINTF_TOSTRING(type, ...) \
template<> void type::printf() const        { Log::Debug(__VA_ARGS__); } \
template<> String type::tostring() const    { return String::Printf(__VA_ARGS__); }

LOL_PRINTF_TOSTRING(vec2,   "[ %6.6f %6.6f ]\n", x, y);
LOL_PRINTF_TOSTRING(ivec2,  "[ %i %i ]\n", x, y);
LOL_PRINTF_TOSTRING(cmplx,  "[ %6.6f %6.6f ]\n", x, y);
LOL_PRINTF_TOSTRING(vec3,   "[ %6.6f %6.6f %6.6f ]\n", x, y, z);
LOL_PRINTF_TOSTRING(ivec3,  "[ %i %i %i ]\n", x, y, z);
LOL_PRINTF_TOSTRING(vec4,   "[ %6.6f %6.6f %6.6f %6.6f ]\n", x, y, z, w);
LOL_PRINTF_TOSTRING(ivec4,  "[ %i %i %i %i ]\n", x, y, z, w);
LOL_PRINTF_TOSTRING(quat,   "[ %6.6f %6.6f %6.6f %6.6f ]\n", w, x, y, z);

template<> void mat2::printf() const
{
    mat2 const &p = *this;

    Log::Debug("[ %6.6f %6.6f\n", p[0][0], p[1][0]);
    Log::Debug("  %6.6f %6.6f ]\n", p[0][1], p[1][1]);
}

template<> String mat2::tostring() const
{
    mat2 const &p = *this;

    return String::Printf("[ %6.6f %6.6f\n", p[0][0], p[1][0]) +
           String::Printf("  %6.6f %6.6f ]\n", p[0][1], p[1][1]);
}

template<> void mat3::printf() const
{
    mat3 const &p = *this;

    Log::Debug("[ %6.6f %6.6f %6.6f\n", p[0][0], p[1][0], p[2][0]);
    Log::Debug("  %6.6f %6.6f %6.6f\n", p[0][1], p[1][1], p[2][1]);
    Log::Debug("  %6.6f %6.6f %6.6f ]\n", p[0][2], p[1][2], p[2][2]);
}

template<> String mat3::tostring() const
{
    mat3 const &p = *this;

    return String::Printf("[ %6.6f %6.6f %6.6f\n", p[0][0], p[1][0], p[2][0]) +
           String::Printf("  %6.6f %6.6f %6.6f\n", p[0][1], p[1][1], p[2][1]) +
           String::Printf("  %6.6f %6.6f %6.6f ]\n", p[0][2], p[1][2], p[2][2]);
}

template<> void mat4::printf() const
{
    mat4 const &p = *this;

    Log::Debug("[ %6.6f %6.6f %6.6f %6.6f\n",
               p[0][0], p[1][0], p[2][0], p[3][0]);
    Log::Debug("  %6.6f %6.6f %6.6f %6.6f\n",
               p[0][1], p[1][1], p[2][1], p[3][1]);
    Log::Debug("  %6.6f %6.6f %6.6f %6.6f\n",
               p[0][2], p[1][2], p[2][2], p[3][2]);
    Log::Debug("  %6.6f %6.6f %6.6f %6.6f ]\n",
               p[0][3], p[1][3], p[2][3], p[3][3]);
}

template<> String mat4::tostring() const
{
    mat4 const &p = *this;

    return String::Printf("[ %6.6f %6.6f %6.6f %6.6f\n",
                            p[0][0], p[1][0], p[2][0], p[3][0]) +
           String::Printf("  %6.6f %6.6f %6.6f %6.6f\n",
                            p[0][1], p[1][1], p[2][1], p[3][1]) +
           String::Printf("  %6.6f %6.6f %6.6f %6.6f\n",
                            p[0][2], p[1][2], p[2][2], p[3][2]) +
           String::Printf("  %6.6f %6.6f %6.6f %6.6f ]\n",
                            p[0][3], p[1][3], p[2][3], p[3][3]);
}

template<> std::ostream &operator<<(std::ostream &stream, ivec2 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, icmplx const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, ivec3 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, ivec4 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, iquat const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec2 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, cmplx const &v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec3 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, vec4 const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, quat const &v)
{
    return stream << "(" << v.x << ", " << v.y << ", "
                         << v.z << ", " << v.w << ")";
}

template<> std::ostream &operator<<(std::ostream &stream, mat4 const &m)
{
    stream << "((" << m[0][0] << ", " << m[1][0]
            << ", " << m[2][0] << ", " << m[3][0] << "), ";
    stream << "(" << m[0][1] << ", " << m[1][1]
           << ", " << m[2][1] << ", " << m[3][1] << "), ";
    stream << "(" << m[0][2] << ", " << m[1][2]
           << ", " << m[2][2] << ", " << m[3][2] << "), ";
    stream << "(" << m[0][3] << ", " << m[1][3]
           << ", " << m[2][3] << ", " << m[3][3] << "))";
    return stream;
}

template<> mat3 mat3::scale(float x)
{
    mat3 ret(1.0f);

    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;

    return ret;
}

template<> mat3 mat3::scale(float x, float y, float z)
{
    mat3 ret(1.0f);

    ret[0][0] = x;
    ret[1][1] = y;
    ret[2][2] = z;

    return ret;
}

template<> mat3 mat3::scale(vec3 v)
{
    return scale(v.x, v.y, v.z);
}

template<> mat4 mat4::translate(float x, float y, float z)
{
    mat4 ret(1.0f);
    ret[3][0] = x;
    ret[3][1] = y;
    ret[3][2] = z;
    return ret;
}

template<> mat4 mat4::translate(vec3 v)
{
    return translate(v.x, v.y, v.z);
}

template<> mat2 mat2::rotate(float degrees)
{
    float st = sin(radians(degrees));
    float ct = cos(radians(degrees));

    mat2 ret;

    ret[0][0] = ct;
    ret[0][1] = st;

    ret[1][0] = -st;
    ret[1][1] = ct;

    return ret;
}

template<> mat3 mat3::rotate(float degrees, float x, float y, float z)
{
    float st = sin(radians(degrees));
    float ct = cos(radians(degrees));

    float len = std::sqrt(x * x + y * y + z * z);
    float invlen = len ? 1.0f / len : 0.0f;
    x *= invlen;
    y *= invlen;
    z *= invlen;

    float mtx = (1.0f - ct) * x;
    float mty = (1.0f - ct) * y;
    float mtz = (1.0f - ct) * z;

    mat3 ret;

    ret[0][0] = x * mtx + ct;
    ret[0][1] = x * mty + st * z;
    ret[0][2] = x * mtz - st * y;

    ret[1][0] = y * mtx - st * z;
    ret[1][1] = y * mty + ct;
    ret[1][2] = y * mtz + st * x;

    ret[2][0] = z * mtx + st * y;
    ret[2][1] = z * mty - st * x;
    ret[2][2] = z * mtz + ct;

    return ret;
}

template<> mat3 mat3::rotate(float degrees, vec3 v)
{
    return rotate(degrees, v.x, v.y, v.z);
}

template<> mat3::mat(quat const &q)
{
    float n = norm(q);

    if (!n)
    {
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 3; i++)
                (*this)[i][j] = (i == j) ? 1.f : 0.f;
        return;
    }

    float s = 2.0f / n;

    (*this)[0][0] = 1.0f - s * (q.y * q.y + q.z * q.z);
    (*this)[0][1] = s * (q.x * q.y + q.z * q.w);
    (*this)[0][2] = s * (q.x * q.z - q.y * q.w);

    (*this)[1][0] = s * (q.x * q.y - q.z * q.w);
    (*this)[1][1] = 1.0f - s * (q.z * q.z + q.x * q.x);
    (*this)[1][2] = s * (q.y * q.z + q.x * q.w);

    (*this)[2][0] = s * (q.x * q.z + q.y * q.w);
    (*this)[2][1] = s * (q.y * q.z - q.x * q.w);
    (*this)[2][2] = 1.0f - s * (q.x * q.x + q.y * q.y);
}

template<> mat4::mat(quat const &q)
{
    *this = mat4(mat3(q), 1.f);
}

static inline void MatrixToQuat(quat &that, mat3 const &m)
{
    /* See http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/christian.htm for a version with no branches */
    float t = m[0][0] + m[1][1] + m[2][2];
    if (t > 0)
    {
        that.w = 0.5f * std::sqrt(1.0f + t);
        float s = 0.25f / that.w;
        that.x = s * (m[1][2] - m[2][1]);
        that.y = s * (m[2][0] - m[0][2]);
        that.z = s * (m[0][1] - m[1][0]);
    }
    else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
    {
        that.x = 0.5f * std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
        float s = 0.25f / that.x;
        that.y = s * (m[0][1] + m[1][0]);
        that.z = s * (m[2][0] + m[0][2]);
        that.w = s * (m[1][2] - m[2][1]);
    }
    else if (m[1][1] > m[2][2])
    {
        that.y = 0.5f * std::sqrt(1.0f - m[0][0] + m[1][1] - m[2][2]);
        float s = 0.25f / that.y;
        that.x = s * (m[0][1] + m[1][0]);
        that.z = s * (m[1][2] + m[2][1]);
        that.w = s * (m[2][0] - m[0][2]);
    }
    else
    {
        that.z = 0.5f * std::sqrt(1.0f - m[0][0] - m[1][1] + m[2][2]);
        float s = 0.25f / that.z;
        that.x = s * (m[2][0] + m[0][2]);
        that.y = s * (m[1][2] + m[2][1]);
        that.w = s * (m[0][1] - m[1][0]);
    }
}

template<> quat::Quat(mat3 const &m)
{
    MatrixToQuat(*this, m);
}

template<> quat::Quat(mat4 const &m)
{
    MatrixToQuat(*this, mat3(m));
}

template<> quat quat::rotate(float degrees, vec3 const &v)
{
    float half_angle = radians(degrees) * 0.5f;

    vec3 tmp = normalize(v) * sin(half_angle);

    return quat(cos(half_angle), tmp.x, tmp.y, tmp.z);
}

template<> quat quat::rotate(float degrees, float x, float y, float z)
{
    return quat::rotate(degrees, vec3(x, y, z));
}

template<> quat quat::rotate(vec3 const &src, vec3 const &dst)
{
    /* Algorithm directly taken from Sam Hocevar's article "Quaternion from
     * two vectors: the final version".
     * http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final */
    float magnitude = lol::sqrt(sqlength(src) * sqlength(dst));
    float real_part = magnitude + dot(src, dst);
    vec3 w;

    if (real_part < 1.e-6f * magnitude)
    {
        /* If src and dst are exactly opposite, rotate 180 degrees
         * around an arbitrary orthogonal axis. Axis normalisation
         * can happen later, when we normalise the quaternion. */
        real_part = 0.0f;
        w = abs(src.x) > abs(src.z) ? vec3(-src.y, src.x, 0.f)
                                    : vec3(0.f, -src.z, src.y);
    }
    else
    {
        /* Otherwise, build quaternion the standard way. */
        w = cross(src, dst);
    }

    return normalize(quat(real_part, w.x, w.y, w.z));
}

template<> quat slerp(quat const &qa, quat const &qb, float f)
{
    float const magnitude = lol::sqrt(sqlength(qa) * sqlength(qb));
    float const product = lol::dot(qa, qb) / magnitude;

    /* If quaternions are equal or opposite, there is no need
     * to slerp anything, just return qa. */
    if (std::abs(product) >= 1.0f)
        return qa;

    float const sign = (product < 0.0f) ? -1.0f : 1.0f;
    float const theta = lol::acos(sign * product);
    float const s1 = lol::sin(sign * f * theta);
    float const s0 = lol::sin((1.0f - f) * theta);

    /* This is the same as 1/sin(theta) */
    float const d = 1.0f / lol::sqrt(1.f - product * product);

    return qa * (s0 * d) + qb * (s1 * d);
}

static inline vec3 quat_toeuler_generic(quat const &q, int i, int j, int k)
{
    float n = norm(q);

    if (!n)
        return vec3::zero;

    /* (2 + i - j) % 3 means x-y-z direct order; otherwise indirect */
    float const sign = ((2 + i - j) % 3) ? 1.f : -1.f;

    vec3 ret;

    /* k == i means X-Y-X style Euler angles; otherwise we’re
     * actually handling X-Y-Z style Tait-Bryan angles. */
    if (k == i)
    {
        k = 3 - i - j;

        ret[0] = atan2(q[1 + i] * q[1 + j] + sign * (q.w * q[1 + k]),
                       q.w * q[1 + j] - sign * (q[1 + i] * q[1 + k]));
        ret[1] = acos(2.f * (sq(q.w) + sq(q[1 + i])) - 1.f);
        ret[2] = atan2(q[1 + i] * q[1 + j] - sign * (q.w * q[1 + k]),
                       q.w * q[1 + j] + sign * (q[1 + i] * q[1 + k]));
    }
    else
    {
        ret[0] = atan2(2.f * (q.w * q[1 + i] - sign * (q[1 + j] * q[1 + k])),
                       1.f - 2.f * (sq(q[1 + i]) + sq(q[1 + j])));
        ret[1] = asin(2.f * (q.w * q[1 + j] + sign * (q[1 + i] * q[1 + k])));
        ret[2] = atan2(2.f * (q.w * q[1 + k] - sign * (q[1 + j] * q[1 + i])),
                       1.f - 2.f * (sq(q[1 + k]) + sq(q[1 + j])));
    }

    return degrees(ret / n);
}

static inline mat3 mat3_fromeuler_generic(vec3 const &v, int i, int j, int k)
{
    mat3 ret;

    vec3 const w = radians(v);
    float const s0 = sin(w[0]), c0 = cos(w[0]);
    float const s1 = sin(w[1]), c1 = cos(w[1]);
    float const s2 = sin(w[2]), c2 = cos(w[2]);

    /* (2 + i - j) % 3 means x-y-z direct order; otherwise indirect */
    float const sign = ((2 + i - j) % 3) ? 1.f : -1.f;

    /* k == i means X-Y-X style Euler angles; otherwise we’re
     * actually handling X-Y-Z style Tait-Bryan angles. */
    if (k == i)
    {
        k = 3 - i - j;

        ret[i][i] =   c1;
        ret[i][j] =   s0 * s1;
        ret[i][k] = - sign * (c0 * s1);

        ret[j][i] =   s1 * s2;
        ret[j][j] =   c0 * c2 - s0 * c1 * s2;
        ret[j][k] =   sign * (s0 * c2 + c0 * c1 * s2);

        ret[k][i] =   sign * (s1 * c2);
        ret[k][j] = - sign * (c0 * s2 + s0 * c1 * c2);
        ret[k][k] = - s0 * s2 + c0 * c1 * c2;
    }
    else
    {
        ret[i][i] =   c1 * c2;
        ret[i][j] =   sign * (c0 * s2) + s0 * s1 * c2;
        ret[i][k] =   s0 * s2 - sign * (c0 * s1 * c2);

        ret[j][i] = - sign * (c1 * s2);
        ret[j][j] =   c0 * c2 - sign * (s0 * s1 * s2);
        ret[j][k] =   sign * (s0 * c2) + c0 * s1 * s2;

        ret[k][i] =   sign * s1;
        ret[k][j] = - sign * (s0 * c1);
        ret[k][k] =   c0 * c1;
    }

    return ret;
}

static inline quat quat_fromeuler_generic(vec3 const &v, int i, int j, int k)
{
    vec3 const half_angles = radians(v * 0.5f);
    float const s0 = sin(half_angles[0]), c0 = cos(half_angles[0]);
    float const s1 = sin(half_angles[1]), c1 = cos(half_angles[1]);
    float const s2 = sin(half_angles[2]), c2 = cos(half_angles[2]);

    quat ret;

    /* (2 + i - j) % 3 means x-y-z direct order; otherwise indirect */
    float const sign = ((2 + i - j) % 3) ? 1.f : -1.f;

    /* k == i means X-Y-X style Euler angles; otherwise we’re
     * actually handling X-Y-Z style Tait-Bryan angles. */
    if (k == i)
    {
        k = 3 - i - j;

        ret[0] =     c1 * (c0 * c2 - s0 * s2);
        ret[1 + i] = c1 * (c0 * s2 + s0 * c2);
        ret[1 + j] = s1 * (c0 * c2 + s0 * s2);
        ret[1 + k] = sign * (s1 * (s0 * c2 - c0 * s2));
    }
    else
    {
        ret[0] =     c0 * c1 * c2 - sign * (s0 * s1 * s2);
        ret[1 + i] = s0 * c1 * c2 + sign * (c0 * s1 * s2);
        ret[1 + j] = c0 * s1 * c2 - sign * (s0 * c1 * s2);
        ret[1 + k] = c0 * c1 * s2 + sign * (s0 * s1 * c2);
    }

    return ret;
}

#define DEFINE_GENERIC_EULER_CONVERSIONS(a1, a2, a3) \
    DEFINE_GENERIC_EULER_CONVERSIONS_INNER(a1, a2, a3, a1##a2##a3) \

#define DEFINE_GENERIC_EULER_CONVERSIONS_INNER(a1, a2, a3, name) \
    /* Create quaternions from Euler angles */ \
    template<> quat quat::fromeuler_##name(vec3 const &v) \
    { \
        int x = 0, y = 1, z = 2; UNUSED(x, y, z); \
        return quat_fromeuler_generic(v, a1, a2, a3); \
    } \
    \
    template<> quat quat::fromeuler_##name(float phi, float theta, float psi) \
    { \
        return quat::fromeuler_##name(vec3(phi, theta, psi)); \
    } \
    \
    /* Create 3×3 matrices from Euler angles */ \
    template<> mat3 mat3::fromeuler_##name(vec3 const &v) \
    { \
        int x = 0, y = 1, z = 2; UNUSED(x, y, z); \
        return mat3_fromeuler_generic(v, a1, a2, a3); \
    } \
    \
    template<> mat3 mat3::fromeuler_##name(float phi, float theta, float psi) \
    { \
        return mat3::fromeuler_##name(vec3(phi, theta, psi)); \
    } \
    \
    /* Create 4×4 matrices from Euler angles */ \
    template<> mat4 mat4::fromeuler_##name(vec3 const &v) \
    { \
        int x = 0, y = 1, z = 2; UNUSED(x, y, z); \
        return mat4(mat3_fromeuler_generic(v, a1, a2, a3), 1.f); \
    } \
    \
    template<> mat4 mat4::fromeuler_##name(float phi, float theta, float psi) \
    { \
        return mat4::fromeuler_##name(vec3(phi, theta, psi)); \
    } \
    \
    /* Retrieve Euler angles from a quaternion */ \
    template<> vec3 vec3::toeuler_##name(quat const &q) \
    { \
        int x = 0, y = 1, z = 2; UNUSED(x, y, z); \
        return quat_toeuler_generic(q, a1, a2, a3); \
    }

DEFINE_GENERIC_EULER_CONVERSIONS(x, y, x)
DEFINE_GENERIC_EULER_CONVERSIONS(x, z, x)
DEFINE_GENERIC_EULER_CONVERSIONS(y, x, y)
DEFINE_GENERIC_EULER_CONVERSIONS(y, z, y)
DEFINE_GENERIC_EULER_CONVERSIONS(z, x, z)
DEFINE_GENERIC_EULER_CONVERSIONS(z, y, z)

DEFINE_GENERIC_EULER_CONVERSIONS(x, y, z)
DEFINE_GENERIC_EULER_CONVERSIONS(x, z, y)
DEFINE_GENERIC_EULER_CONVERSIONS(y, x, z)
DEFINE_GENERIC_EULER_CONVERSIONS(y, z, x)
DEFINE_GENERIC_EULER_CONVERSIONS(z, x, y)
DEFINE_GENERIC_EULER_CONVERSIONS(z, y, x)

#undef DEFINE_GENERIC_EULER_CONVERSIONS
#undef DEFINE_GENERIC_EULER_CONVERSIONS_INNER

template<> mat4 mat4::lookat(vec3 eye, vec3 center, vec3 up)
{
    vec3 v3 = normalize(eye - center);
    vec3 v1 = normalize(cross(up, v3));
    vec3 v2 = cross(v3, v1);

    return mat4(vec4(v1.x, v2.x, v3.x, 0.f),
                vec4(v1.y, v2.y, v3.y, 0.f),
                vec4(v1.z, v2.z, v3.z, 0.f),
                vec4(-dot(eye, v1), -dot(eye, v2), -dot(eye, v3), 1.f));
}

template<> mat4 mat4::ortho(float left, float right, float bottom,
                            float top, float near, float far)
{
    float invrl = (right != left) ? 1.0f / (right - left) : 0.0f;
    float invtb = (top != bottom) ? 1.0f / (top - bottom) : 0.0f;
    float invfn = (far != near) ? 1.0f / (far - near) : 0.0f;

    mat4 ret(0.0f);
    ret[0][0] = 2.0f * invrl;
    ret[1][1] = 2.0f * invtb;
    ret[2][2] = -2.0f * invfn;
    ret[3][0] = - (right + left) * invrl;
    ret[3][1] = - (top + bottom) * invtb;
    ret[3][2] = - (far + near) * invfn;
    ret[3][3] = 1.0f;
    return ret;
}

template<> mat4 mat4::ortho(float width, float height,
                            float near, float far)
{
    return mat4::ortho(-0.5f * width, 0.5f * width,
                       -0.5f * height, 0.5f * height, near, far);
}

template<> mat4 mat4::frustum(float left, float right, float bottom,
                              float top, float near, float far)
{
    float invrl = (right != left) ? 1.0f / (right - left) : 0.0f;
    float invtb = (top != bottom) ? 1.0f / (top - bottom) : 0.0f;
    float invfn = (far != near) ? 1.0f / (far - near) : 0.0f;

    mat4 ret(0.0f);
    ret[0][0] = 2.0f * near * invrl;
    ret[1][1] = 2.0f * near * invtb;
    ret[2][0] = (right + left) * invrl;
    ret[2][1] = (top + bottom) * invtb;
    ret[2][2] = - (far + near) * invfn;
    ret[2][3] = -1.0f;
    ret[3][2] = -2.0f * far * near * invfn;
    return ret;
}

//Returns a standard perspective matrix
template<> mat4 mat4::perspective(float fov_y, float width,
                                  float height, float near, float far)
{
    float t2 = lol::tan(radians(fov_y) * 0.5f);
    float t1 = t2 * width / height;

    return frustum(-near * t1, near * t1, -near * t2, near * t2, near, far);
}

//Returns a perspective matrix with the camera location shifted to be on the near plane
template<> mat4 mat4::shifted_perspective(float fov_y, float screen_size,
                                          float screen_ratio_yx, float near, float far)
{
    float tan_y = tanf(radians(fov_y) * .5f);
    ASSERT(tan_y > 0.000001f);
    float dist_scr = (screen_size * screen_ratio_yx * .5f) / tan_y;

    return mat4::perspective(fov_y, screen_size, screen_size * screen_ratio_yx,
                             max(.001f, dist_scr + near),
                             max(.001f, dist_scr + far)) *
           mat4::translate(.0f, .0f, -dist_scr);
}

} /* namespace lol */

