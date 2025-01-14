
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
//Created: 06/11/2011

#include "HappyPCH.h" 
#include "Forward3DRenderer.h"

#include "CameraPerspective.h"
#include "DrawContext.h"
#include "Drawable.h"
#include "MaterialInstance.h"
#include "ModelMesh.h"
#include "RenderTarget.h"
#include "Scene.h"
#include "View.h"
#include "GraphicsEngine.h"
#include "ShaderUniformBufferManager.h"

namespace he {
namespace gfx {


/* CONSTRUCTOR - DESCTRUCTOR */
Forward3DRenderer::Forward3DRenderer(const RenderPass pass, bool addSceneRenderer): 
            m_RenderTarget(nullptr),
            m_View(nullptr),
            m_Scene(nullptr),
            m_RenderPass(pass)
{
    if (addSceneRenderer)
    {
        const DrawListContainer::BlendFilter blendFilter(m_RenderPass == RenderPass_Opac? DrawListContainer::BlendFilter_Opac : DrawListContainer::BlendFilter_Blend);
        he::eventCallback1<void, const ICamera*> sceneRender([this, blendFilter](const ICamera* camera)
        {
            HE_IF_ASSERT(m_Scene != nullptr, "Assign a scene to the renderer before rendering!")
            {
                const DrawListContainer& drawList(m_Scene->getDrawList());
                drawList.draw(blendFilter, camera, [&camera](Drawable* drawable)
                {
                    ModelMesh* const mesh(drawable->getModelMesh());
                    DrawContext context;
                    context.m_Camera = camera;
                    context.m_VBO = mesh->getVBO();
                    context.m_IBO = mesh->getIBO();
                    context.m_WorldMatrix = drawable->getWorldMatrix();
                    drawable->getMaterial()->apply(context);
                    mesh->draw();
                }); 
            }
        });
        PostRender += sceneRender;
    }
}

Forward3DRenderer::~Forward3DRenderer()
{
}

void Forward3DRenderer::init( View* view, const RenderTarget* target)
{
    m_RenderTarget = target;
    m_View = view;
}

void Forward3DRenderer::render()
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
    const ICamera* camera(m_View->getCamera());
    if (camera != nullptr)
    {
        GRAPHICS->getShaderUniformBufferManager()->updateSceneBuffer(m_Scene);

        m_RenderTarget->prepareForRendering();
        
        GL::heSetViewport(RectI(0, 0, m_View->getViewport().width, m_View->getViewport().height));

        PreRender(camera);
        PostRender(camera);
    }
}


} } //end namespace
