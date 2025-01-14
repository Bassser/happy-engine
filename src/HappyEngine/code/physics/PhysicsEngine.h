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
//Created: 19/08/2011

#ifndef _HE_PHYSICS_ENGINE_H_
#define _HE_PHYSICS_ENGINE_H_
#pragma once

#include "PhysicsMaterial.h"

#include "AssetContainer.h"
#include <foundation/PxAllocatorCallback.h>
#include <PxSimulationEventCallback.h>

namespace physx
{
    class PxPhysics;
    class PxFoundation;
    class PxScene;
    class PxProfileZoneManager;
    class PxControllerManager;

    class PxErrorCallback;
    class PxAllocatorCallback;

    class PxDefaultCpuDispatcher;

    namespace pxtask {
        class CudaContextManager;
    }
#ifdef HE_WINDOWS
    namespace debugger { namespace comm {
            class PvdConnection;
    } }
#endif
}


namespace he {
    class Ray;
    struct RayCastResult;
namespace px {

class HappyPhysicsAllocator : public physx::PxAllocatorCallback
{
    void* allocate(size_t size, const char*, const char* file, int line)
    {
        file; line;
        return gMemMan->allocAligned(size, 16 MEM_DEBUG_PARAM("HappyPhysicsAllocator::allocate") PASS_MEM_DEBUG_FL_PARAMS);
    }

    void deallocate(void* ptr)
    {
        he_aligned_free(ptr);
    }
};

enum PxFilter
{
    COLLISION_FLAG_GROUND			=	1 << 0,
    COLLISION_FLAG_WHEEL			=	1 << 1,
    COLLISION_FLAG_CHASSIS			=	1 << 2,
    COLLISION_FLAG_OBSTACLE			=	1 << 3,
    COLLISION_FLAG_DRIVABLE_OBSTACLE=	1 << 4,

    COLLISION_FLAG_GROUND_AGAINST	=															COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_WHEEL_AGAINST	=									COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
    COLLISION_FLAG_CHASSIS_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_OBSTACLE_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST=	COLLISION_FLAG_GROUND 						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
};

class PhysicsCarManager;
class HAPPY_ENTRY PhysicsEngine : public physx::PxSimulationEventCallback
{
public:
    PhysicsEngine();
    virtual ~PhysicsEngine();

    void tick(float dTime);

    void startSimulation();
    void stopSimulation();
    
    physx::PxPhysics* getSDK() const;
    physx::PxScene* getScene() const;
    //PhysicsCarManager* getCarManager() const;
    physx::PxControllerManager* getControllerManager() const { return m_ControllerManager; }

    RayCastResult raycast(const Ray& ray, uint32 collisionGroup = 0xffffffff) const;

    physx::PxMaterial* createMaterial(float staticFriction, float dynamicFriction, float restitution);
   // const px::PhysicsMaterial& getDriveableMaterial( byte id );

    virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
    virtual void onWake(physx::PxActor** actors, physx::PxU32 count);
    virtual void onSleep(physx::PxActor** actors, physx::PxU32 count);
    virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
    virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);

private:

    physx::PxPhysics* m_PhysXSDK;
    physx::PxFoundation* m_PhysXFoundation;
    physx::PxScene* m_Scene;
    physx::PxProfileZoneManager* m_PxProfileZoneManager;
    physx::PxControllerManager* m_ControllerManager;

    physx::PxErrorCallback* m_ErrorCallback;
    physx::PxAllocatorCallback* m_Allocator;

    physx::PxDefaultCpuDispatcher* m_CpuDispatcher;
    physx::pxtask::CudaContextManager* m_CudaContextManager;
    
#ifdef HE_WINDOWS
    physx::debugger::comm::PvdConnection* m_VisualDebuggerConnection;
#endif

    //PhysicsCarManager* m_pCarManager;

    ct::AssetContainer<physx::PxMaterial*> m_Materials;

    void createScene();
    
    float m_Timer;

    bool m_Simulate;

    //Disable default copy constructor and default assignment operator
    PhysicsEngine(const PhysicsEngine&);
    PhysicsEngine& operator=(const PhysicsEngine&);
};

} } //end namespace

#endif
