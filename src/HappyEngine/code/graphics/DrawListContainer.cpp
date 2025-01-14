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
//Created: 17/12/2011
#include "HappyPCH.h"

#include "DrawListContainer.h"
#include "Drawable.h"
#include "MaterialInstance.h"
#ifdef HE_USE_OCTREE
#include "CullOctree.h"
#endif
#include "ICamera.h"

namespace he {
namespace gfx {

DrawListContainer::DrawListContainer()
{
#ifdef HE_USE_OCTREE
    for (uint32 blend(0); blend < BlendFilter_MAX; ++blend)
        m_DrawList[blend] = HENew(CullOctree)();
#endif
}


DrawListContainer::~DrawListContainer()
{
#ifdef HE_USE_OCTREE
    for (uint32 blend(0); blend < BlendFilter_MAX; ++blend)
        HEDelete(m_DrawList[blend]);
#else
    for (uint32 blend(0); blend < BlendFilter_MAX; ++blend)
        HE_ASSERT(m_DrawList[blend].empty() == true, "Drawlist not empty @shutdown");
#endif
}


void DrawListContainer::getContainerIndex(const Drawable* drawable, BlendFilter& blend)
{
    const gfx::MaterialInstance* material(drawable->getMaterial());
    HE_IF_ASSERT(material != nullptr, "Material is nullptr!")
    {
        if (material->isBlended() || material->noPost())
            blend = BlendFilter_Blend;
        else
            blend = BlendFilter_Opac;
    }
}
void DrawListContainer::insert( Drawable* drawable )
{
    BlendFilter blend;
    getContainerIndex(drawable, blend);
#ifdef HE_USE_OCTREE
    if (drawable->calculateBound())
    {
        m_DrawList[blend]->insert(drawable);
    }
#else
    HE_IF_ASSERT(m_DrawList[blend].contains(drawable) == false, "Drawable already attached")
        m_DrawList[blend].add(drawable);
#endif
}

void DrawListContainer::remove( Drawable* drawable )
{
    BlendFilter blend;
    getContainerIndex(drawable, blend);  
#ifdef HE_USE_OCTREE
    m_DrawList[blend]->remove(drawable);
#else
    m_DrawList[blend].remove(drawable);
#endif
    m_Dynamics.remove(drawable);
}

void DrawListContainer::draw( BlendFilter blend, const ICamera* camera, const std::function<void(Drawable*)>& drawFunc ) const
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
#ifdef HE_USE_OCTREE
    m_DrawList[blend]->draw(camera, drawFunc);
#else
    m_DrawList[blend].forEach([camera, drawFunc](Drawable* drawable)
    {
        if (drawable->canDraw() && camera->intersect(drawable->getBound()) != IntersectResult_Outside)
            drawFunc(drawable);
    });
#endif
}
void DrawListContainer::drawAndCreateDebugMesh( BlendFilter blend, const ICamera* camera, const std::function<void(Drawable*)>& drawFunc, he::PrimitiveList<vec3>& vertices, he::PrimitiveList<uint32>& indices ) const
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
#ifdef HE_USE_OCTREE
    m_DrawList[blend]->drawAndCreateDebugMesh(camera, drawFunc, vertices, indices);
#else
    vertices; indices; camera;
    draw(blend, camera, drawFunc);
#endif
}


void DrawListContainer::prepareForRendering()
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
    if (m_Dynamics.empty() == false)
    {
        m_Dynamics.forEach([&](Drawable* drawable)
        {
            if (drawable->calculateBound())
            {
#ifdef HE_USE_OCTREE
                BlendFilter blend;
                getContainerIndex(drawable, blend);
                m_DrawList[blend]->reevaluate(drawable);
#endif
            }
            drawable->nodeReevaluated();
        });
        m_Dynamics.clear();
    }
}

void DrawListContainer::forceReevalute( Drawable* drawable )
{
#ifdef HE_USE_OCTREE
    BlendFilter blend;
    getContainerIndex(drawable, blend);  
    m_DrawList[blend]->reevaluate(drawable);
#else
    drawable;
#endif
}

void DrawListContainer::doReevalute( Drawable* drawable )
{
    m_Dynamics.add(drawable);
}

const PrimitiveList<Drawable*>* DrawListContainer::getList() const
{
    return &m_DrawList[0];
}

} } //end namespace
