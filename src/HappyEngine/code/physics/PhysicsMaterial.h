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
//Created: 20/08/2011

#ifndef _HE_PHYSICS_MATERIAL_H_
#define _HE_PHYSICS_MATERIAL_H_
#pragma once

namespace physx {
    class PxMaterial;
}

namespace he {
namespace px {

class HAPPY_ENTRY PhysicsMaterial
{
public:
    PhysicsMaterial();
    PhysicsMaterial(float staticFriction, float dynamicFriction, float restitution);
    PhysicsMaterial(physx::PxMaterial* pMaterial);
    virtual ~PhysicsMaterial();
    //copy and assignement == OK

    physx::PxMaterial* getInternalMaterial() const;

private:
    physx::PxMaterial* m_InternalMaterial;
};

} } //end namespace

#endif
