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
//Created: 14/12/2011

#ifndef _HE_PHYSICS_FIXED_JOINT_H_
#define _HE_PHYSICS_FIXED_JOINT_H_
#pragma once

#include "IPhysicsJoint.h"

namespace physx {
    class PxFixedJoint;
}

namespace he {
namespace px {

class PhysicsActor;

class PhysicsFixedJoint : public IPhysicsJoint
{
public:
    PhysicsFixedJoint(const PhysicsActor* pActor0, const mat44& localAttach0,
                      const PhysicsActor* pActor1, const mat44& localAttach1);
    virtual ~PhysicsFixedJoint();

    void setBreakForce( float force, float torque );

private:

    physx::PxFixedJoint* m_Joint;

    //Disable default copy constructor and default assignment operator
    PhysicsFixedJoint(const PhysicsFixedJoint&);
    PhysicsFixedJoint& operator=(const PhysicsFixedJoint&);
};

} } //end namespace

#endif
