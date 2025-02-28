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
//Created: 22/10/2011
#include "HappyPCH.h" 

#include "OpenGL.h"
#include "GLContext.h"
#include "VertexLayout.h"
#include "GraphicsEngine.h"
#include "ExternalError.h"
#include <GL/glew.h>

namespace he {
namespace gfx {

GLContext* GL::s_CurrentContext = nullptr;

//Clear
void GL::heClearColor(const Color& color)
{
   if (color != s_CurrentContext->m_ClearColor)
   {
       glClearColor(color.r(), color.g(), color.b(), color.a());
       s_CurrentContext->m_ClearColor = color;
   }
}

//Depth
void GL::heSetDepthRead(bool read)
{
    if (s_CurrentContext->m_DepthRead != read)
    {
        s_CurrentContext->m_DepthRead = read;
        if (read)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }
}
void GL::heSetDepthWrite(bool write)
{
    if (s_CurrentContext->m_DepthWrite != write)
    {
        s_CurrentContext->m_DepthWrite = write;
        glDepthMask(write);
    }
}
void GL::heSetDepthFunc(DepthFunc func)
{
    if (s_CurrentContext->m_DepthFunc != func)
    {
        s_CurrentContext->m_DepthFunc = func;
        glDepthFunc(func);
    }
}

//Culling
void GL::heSetCullFace(bool cullFrontFace)
{
    if (s_CurrentContext->m_CullFrontFace != cullFrontFace)
    {
        s_CurrentContext->m_CullFrontFace = cullFrontFace;
        glCullFace(cullFrontFace? GL_FRONT : GL_BACK);
    }
}
void GL::heSetWindingFrontFace(bool cw)
{
    if (s_CurrentContext->m_CullCWFrontFace != cw)
    {
        s_CurrentContext->m_CullCWFrontFace = cw;
        glFrontFace(cw? GL_CW : GL_CCW);
    }
}

//Binding
void GL::heBindFbo(uint32 fbo)
{
    if (s_CurrentContext->m_BoundFbo != fbo)
    {
        s_CurrentContext->m_BoundFbo = fbo;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
}
uint32 GL::heGetBoundFbo()
{
    return s_CurrentContext->m_BoundFbo;
}
void GL::heBindVao(uint32 vao)
{
    if (s_CurrentContext->m_BoundVao != vao)
    {
        s_CurrentContext->m_BoundVao = vao;
        glBindVertexArray(vao);
    }
}
void GL::heBindTexture2D(uint32 samplerPos, uint32 tex)
{
    HE_ASSERT(samplerPos < GLContext::MAX_SAMPLERS, "samplerPos must be < MAX_SAMPLERS!");
    if (s_CurrentContext->m_BoundTex2D[samplerPos] != tex)
    {
        if (s_CurrentContext->m_ActiveTex != samplerPos)
        {
            glActiveTexture(GL_TEXTURE0 + samplerPos);
            s_CurrentContext->m_ActiveTex = samplerPos;
        }
        s_CurrentContext->m_BoundTex2D[samplerPos] = tex;
        glBindTexture(GL_TEXTURE_2D, tex);
    }
}
void GL::heBindTexture2D(uint32 tex)
{
    s_CurrentContext->m_BoundTex2D[0] = tex;
    glBindTexture(GL_TEXTURE_2D, tex);
}
void GL::heBindTextureCube( uint32 samplerPos, uint32 tex )
{
    HE_ASSERT(samplerPos < GLContext::MAX_SAMPLERS, "samplerPos must be < MAX_SAMPLERS!");
    if (s_CurrentContext->m_BoundTexCube[samplerPos] != tex)
    {
        if (s_CurrentContext->m_ActiveTex != samplerPos)
        {
            glActiveTexture(GL_TEXTURE0 + samplerPos);
            s_CurrentContext->m_ActiveTex = samplerPos;
        }
        s_CurrentContext->m_BoundTex2D[samplerPos] = tex;
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    }
}

void GL::heBindUniformBuffer( uint32 uboId, uint32 bufferId )
{
    if (s_CurrentContext->m_BoundUbo[uboId] != bufferId)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, uboId, bufferId);
        s_CurrentContext->m_BoundUbo[uboId] = bufferId;
    }
}


//Blending
void GL::heBlendEnabled(bool enabled)
{
    if (s_CurrentContext->m_BlendEnabled != enabled)
    {
        s_CurrentContext->m_BlendEnabled = enabled;
        if (enabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
}
void GL::heBlendFunc(BlendFunc srcFactor, BlendFunc destFactor)
{
    if (srcFactor != s_CurrentContext->m_BlendSrc || destFactor != s_CurrentContext->m_BlendDest)
    {
        s_CurrentContext->m_ColorBlendSrc = BlendFunc_Unassigned;

        s_CurrentContext->m_BlendSrc = srcFactor;
        s_CurrentContext->m_BlendDest = destFactor;
        glBlendFunc(srcFactor, destFactor);
    }
}
void GL::heBlendFuncSeperate(BlendFunc colorSrcFactor,
                             BlendFunc colorDestFactor,
                             BlendFunc alphaSrcFactor,
                             BlendFunc alphaDestFactor)
{
    if (colorSrcFactor  != s_CurrentContext->m_ColorBlendSrc  ||
        colorDestFactor != s_CurrentContext->m_ColorBlendDest ||
        alphaSrcFactor  != s_CurrentContext->m_AlphaBlendSrc  ||
        alphaDestFactor != s_CurrentContext->m_AlphaBlendDest)
    {
        s_CurrentContext->m_BlendSrc = BlendFunc_Unassigned;

        s_CurrentContext->m_ColorBlendSrc  = colorSrcFactor;
        s_CurrentContext->m_ColorBlendDest = colorDestFactor;
        s_CurrentContext->m_AlphaBlendSrc  = alphaSrcFactor;
        s_CurrentContext->m_AlphaBlendDest = alphaDestFactor;
        glBlendFuncSeparate(colorSrcFactor, colorDestFactor, alphaSrcFactor, alphaDestFactor);
    }
}
void GL::heBlendEquation( BlendEquation eq )
{
    if (s_CurrentContext->m_BlendEquation != eq)
    {
        s_CurrentContext->m_BlendEquation = eq;
        glBlendEquation(eq);
    }
}

void GL::heBlendColor(const Color& color)
{
    if (color != s_CurrentContext->m_BlendColor)
    {
        s_CurrentContext->m_BlendColor = color;
        glBlendColor(color.r(), color.g(), color.b(), color.a());
    }
}

//Scissor
void GL::heScissorEnabled(bool enabled)
{
    if (enabled != s_CurrentContext->m_ScissorEnabled)
    {
        s_CurrentContext->m_ScissorEnabled = enabled;
        if (enabled)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }
}
void GL::heScissorRect(const RectI& rect)
{
    if (rect != s_CurrentContext->m_ScissorRect)
    {
        s_CurrentContext->m_ScissorRect = rect;
        glScissor(rect.x, rect.y, rect.width, rect.height);
    }
}
//line smoothing
void GL::heLineSmoothEnabled(bool enabled)
{
    if (s_CurrentContext->m_LineSmoothEnabled != enabled)
    {
        s_CurrentContext->m_LineSmoothEnabled = enabled;

        if (enabled)
            glEnable(GL_LINE_SMOOTH);
        else
            glDisable(GL_LINE_SMOOTH);
    }
}
 
#ifndef APIENTRY
#define APIENTRY
#endif
    
void APIENTRY glDebugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity, GLsizei length,
    const GLchar* message, void* /*userParam*/)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_ARB: 
        LOG(LogType_ProgrammerAssert, "GL: %.*s", length, message); break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB: 
        HE_ERROR("GL: %.*s", length, message); break;
    case GL_DEBUG_SEVERITY_LOW_ARB: 
        //HE_WARNING("GL: %.*s", length, message); break;
    default: 
        //HE_INFO("GL: %.*s", length, message);
        break;
    }
}

void GL::init()
{
    glewExperimental = true;
    glewInit();

    glClearDepth(1.0f);

    GL::heSetDepthRead(true);
    GL::heSetDepthWrite(true);
    GL::heSetDepthFunc(DepthFunc_LessOrEqual);
    GL::heSetWindingFrontFace(false);
    GL::heSetCullFace(false);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDisable(GL_SCISSOR_TEST);

#ifdef HE_DEBUG
    // no worky on AMD/ATI
    if (glewGetContext()->__GLEW_ARB_debug_output)
    {
        glDebugMessageCallbackARB(glDebugCallback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
    }
#endif

    if (glewGetContext()->__GLEW_EXT_texture_filter_anisotropic == GL_TRUE)
    {
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &s_CurrentContext->m_MaxAnisotropicFilteringSupport);
    }
    else
    {
        s_CurrentContext->m_MaxAnisotropicFilteringSupport = 0.0f;
    }

    s_CurrentContext->m_SupportTextureCompression = glewGetContext()->__GLEW_EXT_texture_compression_s3tc == GL_TRUE;
}
void GL::reset()
{
    s_CurrentContext->m_BoundFbo = UINT32_MAX;
    s_CurrentContext->m_BoundVao = UINT32_MAX;

    he_memset(s_CurrentContext->m_BoundTex2D, 0xff, GLContext::MAX_SAMPLERS * sizeof(uint32));
    he_memset(s_CurrentContext->m_BoundTexCube, 0xff, GLContext::MAX_SAMPLERS * sizeof(uint32));
    he_memset(s_CurrentContext->m_BoundUbo, 0xff, GLContext::MAX_UBO * sizeof(uint32));

    s_CurrentContext->m_Viewport.x = -1;
    s_CurrentContext->m_Viewport.y = -1;
    s_CurrentContext->m_Viewport.width = -1;
    s_CurrentContext->m_Viewport.height = -1;


    s_CurrentContext->m_ActiveTex = UINT32_MAX;
}

void GL::heSetViewport( const RectI& viewport )
{
    if (viewport != s_CurrentContext->m_Viewport)
    {
        s_CurrentContext->m_Viewport = viewport;
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }
}

const RectI& GL::heGetViewport()
{
    return s_CurrentContext->m_Viewport;
}

void GL::getGLTypesFromVertexElement(const VertexElement& element, GLint& components, GLenum& type)
{
    type = element.getType();
    components = element.getComponents();
}

float GL::getMaxAnisotropicFilteringSupport()
{
    return s_CurrentContext->m_MaxAnisotropicFilteringSupport;
}

bool GL::getSupportTextureCompression()
{
    return s_CurrentContext->m_SupportTextureCompression;
}

int GL::getMaxMultiSamples()
{
    if (s_CurrentContext->m_MaxMultiSamples == -1)
    {
        int samples;
        glGetIntegerv(GL_MAX_SAMPLES, &samples);

        HE_ASSERT(samples >= 0, "Could not get max multisamples");

        s_CurrentContext->m_MaxMultiSamples = samples;
    }

    return s_CurrentContext->m_MaxMultiSamples;
}


} } //end namespace
