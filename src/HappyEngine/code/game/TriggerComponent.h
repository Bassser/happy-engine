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
//Created: 22/01/2012

#ifndef _HE_TRIGGER_COMPONENT_H_
#define _HE_TRIGGER_COMPONENT_H_
#pragma once

#include "EntityComponent.h"
#include "ITickable.h"

namespace he {
namespace px {
    class PhysicsTrigger;
    class IPhysicsShape;
}
namespace ge {
    
struct EntityComponentDesc;
class HAPPY_ENTRY TriggerComponent : public EntityComponent
{
public:
    /* CONSTRUCTOR - DESTRUCTOR */
    TriggerComponent();
    ~TriggerComponent();

    /* EntityComponent */
    void visit(he::io::BinaryVisitor* const /*visitor*/) {} // override, final
    void activate(); // override, final
    void deactivate(); // override, final

    const he::FixedString& getComponentID() const { return HEFS::strTriggerComponent; } // override, final

    //// Editor //////////////////////////////////////////////////////////////
    static void fillEntityComponentDesc(EntityComponentDesc& desc);
    bool setProperty(const Property* const inProperty); // override, final
    bool getProperty(Property* const inOutProperty); // override, final
    //////////////////////////////////////////////////////////////////////////

    
    /* GENERAL */
    void addShape(const px::IPhysicsShape* shape, uint32 collisionGroup, uint32 collisionGroupAgainst, 
        const mat44& localPose = mat44::Identity);

    /* EVENTS */
    event1<void, Entity*> OnTriggerEnter;
    event1<void, Entity*> OnTriggerLeave;

protected:
    void init(Entity* parent); // override, final
    void calculateWorldMatrix(); // override, final

private:
    /* DATAMEMBERS */
    Entity* m_Parent;

    px::PhysicsTrigger* m_Trigger;

    /* DEFAULT COPY & ASSIGNENT */
    TriggerComponent(const TriggerComponent&);
    TriggerComponent& operator=(const TriggerComponent&);
};

} } //end namespace

#endif
