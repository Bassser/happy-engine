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
//Author:  Sebastiaan Sprengers
//Created: 31/08/2011
//Renamed: 23/07/2012 - Bastian Damman: Camera -> CameraPerspective

#ifndef _HE_CameraPerspective_H_
#define _HE_CameraPerspective_H_
#pragma once

#include "ICamera.h"
#include "Sphere.h"
#include "Cone.h"
#include "Frustum.h"

namespace he {
class Bound;
namespace gfx {

class HAPPY_ENTRY CameraPerspective : public ICamera
{
public:
    class CameraBound
    {
    public:
        inline const Sphere& getSphere() const { return m_Sphere; }
        inline const Cone& getCone() const { return m_Cone; }
        inline const Frustum& getFrustum() const { return m_Frustum; }

        void calculate(const CameraPerspective* cam);

    private:
        Sphere m_Sphere;
        Cone m_Cone;
        Frustum m_Frustum;
    };

    // CONSTRUCTOR - DESTRUCTOR
    CameraPerspective();
    virtual ~CameraPerspective();

    // ICamera
    virtual const mat44& getView() const { return m_View; }
    virtual const mat44& getProjection() const { return m_Projection; }
    virtual const mat44& getViewProjection() const { return m_ViewProjection; }
    virtual float getNearClip() const { return m_NearZ; }
    virtual float getFarClip() const { return m_FarZ; }
    virtual const vec3& getPosition() const { return m_PosWorld; }
    virtual const vec3& getLook() const { return m_LookWorld; }
    virtual ECameraType getCameraType() { return eCameraType_Perspective; }

    virtual void prepareForRendering();
    virtual IntersectResult intersect(const Bound& bound) const;
    virtual IntersectResult intersect(const Sphere& bound) const;

    // GENERAL
    virtual void tick(const float /*dTime*/) {}
    virtual void lookAt(const vec3& pos, const vec3& target, const vec3& up);

    // SETTERS
    virtual void setPosition(const vec3& pos);
    virtual void setLens(float aspectRatio = (16.0f/9.0f), float fov = piOverFour, float nearZ = 10.0f, float farZ = 1000.0f);
    virtual void setAspectRatio(float aspectRatio);
    void setFov( const float fov );
    void setNearFarPlane(float nearZ = 1.0f, float farZ = 1000.0f);
    void setEyeShift(const float lookShift, const float projShift);

    // GETTERS
    virtual const vec3& getRight() const { return m_RightWorld; }
    virtual const vec3& getUp() const {return m_UpWorld; }

    virtual float getFov() const { return m_FOV; }
    virtual float getAspectRatio() const { return m_AspectRatio; }

    virtual const CameraBound& getBound() const { return m_Bound; }

protected:
    // DATAMEMBERS
    mat44 m_View;
    mat44 m_Projection;
    mat44 m_ViewProjection;

    vec3 m_PosWorld;
    vec3 m_RightWorld;
    vec3 m_UpWorld;
    vec3 m_LookWorld;
    float m_LookShift;
    float m_ProjShift;

    CameraBound m_Bound;

    float m_FOV;
    float m_AspectRatio;
    float m_NearZ, m_FarZ;

    bool m_RegenViewMatrix, m_RegenProjMatrix;
    
private:

    //Disable default copy constructor and default assignment operator
    CameraPerspective(const CameraPerspective&);
    CameraPerspective& operator=(const CameraPerspective&);
};

} } //end namespace

#endif
