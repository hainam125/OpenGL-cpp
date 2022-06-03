/*
        Copyright 2010 Etay Meiri
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MATH_3D_H
#define MATH_3D_H

#ifdef _WIN64
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#else
#include <math.h>
#endif
#include <stdio.h>


#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

float RandomFloat();

struct Vector2i
{
    int x;
    int y;
};

struct Vector2f
{
    float x;
    float y;

    Vector2f()
    {
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};

struct Vector4f;

struct Vector3f
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f(const float* pFloat)
    {
        x = pFloat[0];
        y = pFloat[1];
        z = pFloat[2];
    }

    Vector3f(float f)
    {
        x = y = z = f;
    }

    Vector3f(const Vector4f& v);

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    bool operator==(const Vector3f& r)
    {
        return ((x == r.x) && (y == r.y) && (z == r.z));
    }

    bool operator!=(const Vector3f& r)
    {
        return !(*this == r);
    }

    operator const float* () const
    {
        return &(x);
    }


    Vector3f Cross(const Vector3f& v) const;

    float Dot(const Vector3f& v) const
    {
        float ret = x * v.x + y * v.y + z * v.z;
        return ret;
    }

    float Distance(const Vector3f& v) const
    {
        float delta_x = x - v.x;
        float delta_y = y - v.y;
        float delta_z = z - v.z;
        float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
        return distance;
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y + z * z);
        return len;
    }

    bool IsZero() const
    {
        return ((x + y + z) == 0.0f);
    }

    Vector3f& Normalize();

    void Rotate(float Angle, const Vector3f& Axis);

    Vector3f Negate() const;

    void Print(bool endl = true) const
    {
        printf("(%f, %f, %f)", x, y, z);

        if (endl) {
            printf("\n");
        }
    }
};


struct Vector4f
{
    float x;
    float y;
    float z;
    float w;

    Vector4f()
    {
    }

    Vector4f(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    Vector4f(const Vector3f& v, float _w)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = _w;
    }

    void Print(bool endl = true) const
    {
        printf("(%f, %f, %f, %f)", x, y, z, w);

        if (endl) {
            printf("\n");
        }
    }

    Vector3f to3f() const
    {
        Vector3f v(x, y, z);
        return v;
    }
};



inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x + r.x,
        l.y + r.y,
        l.z + r.z);

    return Ret;
}

inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x - r.x,
        l.y - r.y,
        l.z - r.z);

    return Ret;
}

inline Vector3f operator*(const Vector3f& l, float f)
{
    Vector3f Ret(l.x * f,
        l.y * f,
        l.z * f);

    return Ret;
}

inline Vector3f::Vector3f(const Vector4f& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

inline Vector4f operator/(const Vector4f& l, float f)
{
    Vector4f Ret(l.x / f,
        l.y / f,
        l.z / f,
        l.w / f);

    return Ret;
}


#endif  /* MATH_3D_H */