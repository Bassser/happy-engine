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

#include "vec2.h"
#include "foundation/PxVec2.h"

namespace he {

const vec2 vec2::zero(0, 0);
const vec2 vec2::one(1, 1);
const vec2 vec2::unitX(1, 0);
const vec2 vec2::unitY(0, 1);

vec2::vec2(): x(0), y(0)
{
}
vec2::vec2(float x_, float y_): x(x_), y(y_)
{
}
vec2::vec2(const float v): x(v), y(v)
{
}
vec2::vec2(const physx::PxVec2& vec) : x(vec.x), y(vec.y)
{
}
vec2::~vec2()
{
}

vec2::vec2(const vec2& other)
{
    x = other.x;
    y = other.y;
}
vec2& vec2::operator=(const vec2& other)
{
    x = other.x;
    y = other.y;
    return *this;
}

//>---------Operators--------------------->
vec2 vec2::operator-() const
{
    return vec2(-x, -y);
}

vec2 vec2::operator*(float a) const
{
    return vec2(x*a, y*a);
}
vec2 vec2::operator/(float a) const
{
    return vec2(x/a, y/a);
}

vec2 vec2::operator+(const vec2& v) const
{
    return vec2(x + v.x, y + v.y);
}
vec2 vec2::operator-(const vec2& v) const
{
    return vec2(x - v.x, y - v.y);
}
vec2 vec2::operator*(const vec2& v) const
{
    return vec2(x * v.x, y * v.y);
}
vec2 vec2::operator/(const vec2& v) const
{
    return vec2(x / v.x, y / v.y);
}

vec2& vec2::operator+=(const vec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}
vec2& vec2::operator-=(const vec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}
vec2& vec2::operator*=(float a)
{
    x *= a;
    y *= a;
    return *this;
}
vec2& vec2::operator/=(float a)
{
    x /= a;
    y /= a;
    return *this;
}

vec2& vec2::operator*=(const vec2& v)
{
    x *= v.x;
    y *= v.y;
    return *this;
}

vec2& vec2::operator/=(const vec2& v)
{
    x /= v.x;
    y /= v.y;
    return *this;
}

bool vec2::operator==(const vec2& v) const
{
    return x == v.x && y == v.y;
}
bool vec2::operator!=(const vec2& v) const
{
    return x != v.x || y != v.y;
}


//<----------------------------------------<

} //end namespace