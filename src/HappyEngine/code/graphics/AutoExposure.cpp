//HappyEngine Copyright (C) 2011  Bastian Damman, Sebastiaan Sprengers
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
//Created: 26/10/2011

#include "StdAfx.h" 

#include "AutoExposure.h"
#include "HappyNew.h"
#include "ContentManager.h"
#include "OpenGL.h"
#include "ExternalError.h"
#include "GraphicsEngine.h"

namespace he {
    namespace gfx {

AutoExposure::AutoExposure(): m_pLumShader(NEW Shader()), m_FirstBuffer(true)
{
    m_pLumTexture[0] = Texture2D::pointer(NEW Texture2D());
    m_pLumTexture[1] = Texture2D::pointer(NEW Texture2D());
}


AutoExposure::~AutoExposure()
{
    glDeleteFramebuffers(1, &m_FboID);
}

void AutoExposure::init()
{
    //////////////////////////////////////////////////////////////////////////
    ///                          LOAD RENDER TARGETS                       ///
    //////////////////////////////////////////////////////////////////////////
    uint lumTexId[2];
    glGenTextures(2, lumTexId);

    for (int i = 0; i < 2; ++i)
    {
        GL::heBindTexture2D(0, lumTexId[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 
            1, 1,
            0, GL_RED, GL_FLOAT, 0);
        m_pLumTexture[i]->init(lumTexId[i], 1, 1, GL_R16F);
    }

    //////////////////////////////////////////////////////////////////////////
    ///                            LOAD FBO's                              ///
    //////////////////////////////////////////////////////////////////////////
    glGenFramebuffers(1, &m_FboID);
    GL::heBindFbo(m_FboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pLumTexture[0]->getID(), 0);
    err::checkFboStatus("auto exposure");

    //////////////////////////////////////////////////////////////////////////
    ///                          LOAD SHADERS                              ///
    //////////////////////////////////////////////////////////////////////////
    ShaderLayout shaderLayout;
    shaderLayout.addElement(ShaderLayoutElement(0, "inPosition"));

    std::string folder(CONTENT->getRootDir() + CONTENT->getShaderFolder());
    m_pLumShader->init(folder + "deferred/post/deferredPostShaderQuad.vert", folder + "deferred/post/lum.frag", shaderLayout);
    m_HDRmapPos = m_pLumShader->getShaderSamplerId("hdrMap");
    m_PrevLumMapPos = m_pLumShader->getShaderSamplerId("prevLumMap");

    //////////////////////////////////////////////////////////////////////////
    ///                         LOAD RENDER QUAD                           ///
    //////////////////////////////////////////////////////////////////////////
    m_pQuad = CONTENT->getFullscreenQuad();
}

void AutoExposure::calculate( const Texture2D::pointer& hdrMap )
{
    PROFILER_BEGIN("AutoExposure::calculate");
    m_FirstBuffer = !m_FirstBuffer;

    GL::heBlendEnabled(false);
    GL::heBindFbo(m_FboID);
    m_pLumShader->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pLumTexture[m_FirstBuffer? 0 : 1]->getID(), 0);
    m_pLumShader->setShaderVar(m_HDRmapPos, hdrMap);
    m_pLumShader->setShaderVar(m_PrevLumMapPos, m_pLumTexture[m_FirstBuffer? 1 : 0]);
    GRAPHICS->draw(m_pQuad);
    PROFILER_END("AutoExposure::calculate");
}

const Texture2D::pointer& AutoExposure::getLuminanceMap() const
{
    return m_pLumTexture[m_FirstBuffer? 0 : 1];
}

} } //end namespace