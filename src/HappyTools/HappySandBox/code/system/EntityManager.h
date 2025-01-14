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
//Created: 05/03/2013

#ifndef _HT_ENTITYMANAGER_H_
#define _HT_ENTITYMANAGER_H_
#pragma once

#include <FixedStringMap.h>
#include <EntityComponentDesc.h>

namespace he {
    namespace ge {
        class Entity;
    }
    namespace gfx {
        class Scene;
    }
}

namespace hs {

class EntityManager
{
public:

    /* CONSTRUCTOR - DESTRUCTOR */
    EntityManager();
    ~EntityManager();

    void init();
    void destroy();

    /* GENERAL */
    he::ge::Entity* createEntity();

    /* GETTERS */
    void getComponentTypes(he::ObjectList<he::FixedString>& outList) const;
    he::ge::EntityComponentDesc* getComponentDescriptor(const he::FixedString& component) const;
    he::ge::EntityComponentDesc* getEntityDescriptor() const { return m_EntityDesc; }

private:
    void onEntityCreated(he::ge::Entity* const entity);
    void onEntityDestroyed(he::ge::Entity* const entity);

    he::PrimitiveList<he::ge::Entity*> m_Entities;
    he::ge::EntityComponentDesc* m_EntityDesc;
    he::FixedStringMap<he::ge::EntityComponentDesc*> m_ComponentDescList;

    he::eventCallback1<void, he::ge::Entity*> m_EntityCreatedCallback;
    he::eventCallback1<void, he::ge::Entity*> m_EntityDestroyedCallback;

    /* DEFAULT COPY & ASSIGNMENT */
    EntityManager(const EntityManager&);
    EntityManager& operator=(const EntityManager&);
};

} //end namespace

#endif
