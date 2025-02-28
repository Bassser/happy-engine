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
//
//Author:  Bastian Damman
//Created: 19/07/2012
#include "HappyPCH.h" 

#include "Cone.h"
#include "Sphere.h"
#include "Frustum.h"

namespace he {

Cone::Cone(): 
      m_Position(0, 0, 0)
    , m_Axis(0, 1, 0)
    , m_Fov(piOverTwo)
    , m_CosSqr(0.5f)
    , m_SinSqr(0.5f)
    , m_SinRecip(1.41421356f)
    , m_Length(5.0f)
{
}

Cone::Cone( const vec3& position, const vec3& axis, float fov, float axisLength ): 
      m_Position(position)
    , m_Axis(axis)
    , m_Fov(fov)
    , m_Length(axisLength)
    , m_CosSqr(sqr(cos(fov / 2.0f)))
    , m_SinSqr(sqr(sin(fov / 2.0f)))
    , m_SinRecip(1 / sin(fov / 2.0f))
{
}

void Cone::setFov( float fov )
{
    m_Fov = fov;
    m_CosSqr = sqr(cos(fov / 2.0f));
    float sinFov(sin(fov / 2.0f));
    m_SinSqr = sqr(sinFov);
    m_SinRecip = 1 / sinFov;
}

bool Cone::intersectTest( const Sphere& sphere ) const
{
    vec3 temp(sphere.getPosition() - m_Position);
    vec3 temp2(temp + m_Axis * sphere.getRadius() * m_SinRecip);
    float distSq(lengthSqr(temp2));
    float projDist(dot(temp2, m_Axis));
    if ( projDist > 0 && sqr(projDist) >= distSq * m_CosSqr )
    {
        distSq = lengthSqr(temp);
        projDist = -dot(temp2, m_Axis);
        if ( projDist > 0 && sqr(projDist) >= distSq * m_SinSqr )
            return distSq <= sqr(sphere.getRadius());
        else
            return true;
    }
    return false;
}

void Cone::generateConeVertices( uint32 circleVertices, he::PrimitiveList<vec3>& outBuffer ) const
{
    outBuffer.add(m_Position);

    vec3 centerCircle(m_Position + m_Axis * m_Length);
    float radius(tan(m_Fov / 2.0f) * m_Length);
    vec3 up(dot(m_Axis, vec3::up) < 0.99f? vec3::up : vec3::right);
    vec3 right(normalize(cross(up, m_Axis)));
    up = normalize(cross(right, m_Axis));
    for (uint32 i(0); i < circleVertices; ++i)
    {
        float angle(he::twoPi / circleVertices * i);
        outBuffer.add(centerCircle + up * (radius * cos(angle)) + right * (radius * sin(angle)));
    }
}

} //end namespace
