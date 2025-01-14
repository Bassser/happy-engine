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
//Created: 28/10/2011

#ifndef _HE_ICOMPONENT_H_
#define _HE_ICOMPONENT_H_
#pragma once

#include "Object3D.h"
#include "INetworkSerializable.h"

namespace he {
namespace io {
    class BinaryVisitor;
}
namespace ge {
class Entity;
class Property;
struct EntityComponentDesc;
class HAPPY_ENTRY EntityComponent : public Object3D
{
DECLARE_OBJECT(EntityComponent)
friend class Entity;
public:
    virtual ~EntityComponent() {}

    virtual void visit(he::io::BinaryVisitor* const /*visitor*/) {}

    virtual void activate() {}
    virtual void deactivate() {}

    virtual const he::FixedString& getComponentID() const = 0;

    static void fillEntityComponentDesc(EntityComponentDesc& desc);
    virtual bool setProperty(const Property* const inProperty);
    virtual bool getProperty(Property* const inOutProperty);
    
    virtual Entity* getEntityParent();

private:
    virtual void init(Entity* parent) = 0;

};

} } //end namespace

#endif
