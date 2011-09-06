//HappyEngine Copyright (C) 2011  Bastian Damman, Sebastiaan Sprengers
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
//Created: 22/08/2011

#ifndef _PHYSICS_DYNAMIC_ACTOR_H_
#define _PHYSICS_DYNAMIC_ACTOR_H_
#pragma once

#include "PxRigidDynamic.h"
#include "Vector3.h"
#include "Matrix.h"
#include "IPhysicsShape.h"
#include "PhysicsMaterial.h"

namespace happyengine {
namespace physics {

class PhysicsDynamicActor
{
public:
	PhysicsDynamicActor(const math::Vector3 position, const shapes::IPhysicsShape& shape, PhysicsMaterial* pMaterial);
    virtual ~PhysicsDynamicActor();

    math::Vector3 getPosition() const;
    math::Matrix getPose() const;

private:

    PxRigidDynamic* m_pActor;

    //Disable default copy constructor and default assignment operator
    PhysicsDynamicActor(const PhysicsDynamicActor&);
    PhysicsDynamicActor& operator=(const PhysicsDynamicActor&);
};

} } //end namespace

#endif