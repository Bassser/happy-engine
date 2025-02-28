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
//Created: 07/10/2011

#ifndef _HE_PointLight_H_
#define _HE_PointLight_H_
#pragma once

#include "Light.h"

namespace he {
namespace gfx {

class ModelMesh;
class MaterialInstance;

class PointLight : public Light, public Object3D
{
private:
    float m_Multiplier;
    vec3 m_Color;
    vec2 m_Attenuation;
    mutable vec2 m_ScaledAttenuation;

protected:
    virtual void calculateWorldMatrix(); // override Object3D

public:
    PointLight();
    virtual ~PointLight();

    void setMultiplier(float multiplier);
    void setAttenuation(float begin, float end);
    void setColor(const vec3& color);
    void setColor(const Color& color);

    float getMultiplier() const;
    float getBeginAttenuation() const;
    float getEndAttenuation() const;
    float getScaledBeginAttenuation() const;
    float getScaledEndAttenuation() const;
    const vec3& getColor() const;
    
    const ModelMesh* getLightVolume() const;

    virtual LightType getType() const { return LightType_Point; }

private:
    PointLight(const PointLight& other);
    PointLight& operator=(const PointLight& other);
};

} } //end namespace

#endif
