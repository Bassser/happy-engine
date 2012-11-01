//HappyEngine Copyright (C) 2011 - 2012  Evil Interactive
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
//Created: 18/07/2012

#ifndef _HE_CullOctree_H_
#define _HE_CullOctree_H_
#pragma once

#include "Bound.h"

namespace he {
namespace gfx {
class IDrawable;
class CullOctree;
class ICamera;

class CullOctreeNode
{
public:
    void create(uint32 poolIndex); // pool create
    void reset(); // pool reset
    inline uint32 getPoolIndex() const { return m_PoolIndex; }

    inline const Bound& getStrictBound() const { return m_StrictBound; }
    inline const Bound& getLooseBound() const { return m_LooseBound; }

    CullOctreeNode* rootInsert(IDrawable* drawable); // returns current root
    void remove(IDrawable* obj);
    void reevaluate(IDrawable* obj);
    void draw(const ICamera* camera, boost::function1<void, IDrawable*> drawFunction, bool checkChilderen) const;
    void drawAndCreateDebugMesh(const ICamera* camera, boost::function1<void, IDrawable*> drawFunction, bool checkChilderen, 
        std::vector<vec3>& vertices, std::vector<uint>& indices) const;
    CullOctreeNode* getRoot();

    bool checkRemove() const;
    bool canRemoveChilderen() const;
    void doRemoveChilderen(bool checkParent);
    const std::vector<IDrawable*>& getObjectChilds() const { return m_ObjectChilds; }

private:
    void init(CullOctreeNode* parent, byte xIndex, byte yIndex, byte zIndex);
    void init(const vec3& pos, float strictSize, CullOctreeNode* child, byte xIndex, byte yIndex, byte zIndex);

    void createChilds();
    void createChilds(CullOctreeNode* child, byte xIndex, byte yIndex, byte zIndex);
    void rinsert(IDrawable* obj);
    void createBounds(const vec3& pos, float strictSize);
    void insert(IDrawable* drawable);

    CullOctreeNode* m_ChildNodes[8];
    CullOctreeNode* m_Parent;
    std::vector<IDrawable*> m_ObjectChilds;
    Bound m_StrictBound, m_LooseBound;
    bool m_IsLeafe;
    uint32 m_PoolIndex;
};

class CullOctree
{
public:

    explicit CullOctree(); 
    ~CullOctree();

    void insert(IDrawable* obj);
    void reevaluate(IDrawable* obj);
    void remove(IDrawable* obj);

    void draw(const ICamera* camera, boost::function1<void, IDrawable*> drawFunction) const;
    void drawAndCreateDebugMesh(const ICamera* camera, boost::function1<void, IDrawable*> drawFunction, 
        std::vector<vec3>& vertices, std::vector<uint>& indices) const;

    const static float s_MinLeaveSize;

private:
    CullOctreeNode* m_Root;

    //Disable default copy constructor and default assignment operator
    CullOctree(const CullOctree&);
    CullOctree& operator=(const CullOctree&);
};

} } //end namespace

#endif