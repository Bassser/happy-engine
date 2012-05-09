//HappyEngine Copyright (C) 2011 - 2012  Bastian Damman, Sebastiaan Sprengers 
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

#include "IComponent.h"
#include "mat44.h"
#include "ITickable.h"
#include "PhysicsTrigger.h"

namespace he {
namespace game {

class TriggerComponent : public IComponent, public ITickable
{
public:

    /* CONSTRUCTOR - DESTRUCTOR */
    TriggerComponent();
    virtual ~TriggerComponent();

    /* ICOMPONENT */
    virtual void init(Entity* pParent);

    virtual void serialize(SerializerStream& stream);
    virtual void deserialize(const SerializerStream& stream);

    /* ITICKABLE */
    virtual void tick(float dTime);

    /* GENERAL */
    void addShape(const px::IPhysicsShape* pShape, const mat44& localPose = mat44::Identity);
    void addOnTriggerEnterCallBack(boost::function<void()> callback);
    void addOnTriggerLeaveCallBack(boost::function<void()> callback);

    /* GETTERS */
    px::PhysicsTrigger* getTrigger();

private:

    /* DATAMEMBERS */
    Entity* m_pParent;

    px::PhysicsTrigger* m_pTrigger;

    /* DEFAULT COPY & ASSIGNENT */
    TriggerComponent(const TriggerComponent&);
    TriggerComponent& operator=(const TriggerComponent&);
};

} } //end namespace

#endif