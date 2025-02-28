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
//Author:  Bastian Damman
//Created: 2011/12/18
//major change: 2013/04/28

#ifndef _HE_DRAWABLE_H_
#define _HE_DRAWABLE_H_
#pragma once

#include "Object3D.h"
#include "Bound.h"

namespace he {
namespace gfx {
class ModelMesh;
class Material;
class MaterialInstance;
class MaterialLayout;
class Scene;
class CullOctreeNode;

class HAPPY_ENTRY Drawable: public he::Object3D
{
    ENUM(EDrawableFlags, he::uint8)
    {
        eDrawableFlags_None = 0,
        eDrawableFlags_CastShadow = 1 << 0,
        eDrawableFlags_NeedsSceneReevaluate = 1 << 1,
        eDrawableFlags_NeedsBoundUpdate = 1 << 2,
        eDrawableFlags_IsLoaded = 1 << 3,
        eDrawableFlags_IsVisible = 1 << 4
    };
public:
    Drawable();
    virtual ~Drawable();

    void setModelMesh(ModelMesh* const mesh);
    void setMaterial(const Material* const material);

    HE_FORCEINLINE ModelMesh* const getModelMesh() const { return m_ModelMesh; }
    HE_FORCEINLINE MaterialInstance* const getMaterial() const { return m_Material; }

    bool canDraw() const { return checkFlag(eDrawableFlags_IsLoaded | eDrawableFlags_IsVisible); }

    bool getCastsShadow() const { return checkFlag(eDrawableFlags_CastShadow); }
    void setCastsShadow(const bool castShadow) { castShadow? raiseFlag(eDrawableFlags_CastShadow) : clearFlag(eDrawableFlags_CastShadow); }

    void setBoundDirty() { raiseFlag(eDrawableFlags_NeedsBoundUpdate); }

    virtual bool calculateBound();
    HE_FORCEINLINE const Bound& getBound() const { return m_Bound; }
    
    void detachFromScene();
    void attachToScene(Scene* const scene);
    void setScene(Scene* const scene) { m_Scene = scene; }
    HE_FORCEINLINE Scene* getScene() const { return m_Scene; }
    HE_FORCEINLINE bool isAttachedToScene() const { return m_Scene != nullptr; }

    void nodeReevaluated() { clearFlag(eDrawableFlags_NeedsSceneReevaluate); }

#ifdef HE_USE_OCTREE
    CullOctreeNode* getNode() const { return m_Node; };
    void setNode(CullOctreeNode* const node) { m_Node = node; };
#endif

protected:
    void setWorldMatrixDirty(const uint8 cause);

private:
    HE_FORCEINLINE bool checkFlag(const int flag) const { return (m_Flags & flag) == flag; }
    HE_FORCEINLINE void raiseFlag(const int flag) { m_Flags |= flag; }
    HE_FORCEINLINE void clearFlag(const int flag) { m_Flags &= ~flag; }

    void updateMaterialLayout(ModelMesh* const mesh, MaterialInstance* const material);

    void cancelLoad();
    void reevaluate();

    Bound m_Bound;
#ifdef HE_USE_OCTREE
    CullOctreeNode* m_Node;
#endif
    
    ModelMesh* m_ModelMesh;
    MaterialInstance* m_Material;

    Scene* m_Scene;
    uint8 m_Flags;

    //Disable default copy constructor and default assignment operator
    Drawable(const Drawable&);
    Drawable& operator=(const Drawable&);
};

} } //end namespace

#endif
