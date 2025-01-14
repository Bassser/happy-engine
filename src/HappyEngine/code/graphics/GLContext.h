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
//Created: 16/08/2012

#ifndef _HE_GLContext_H_
#define _HE_GLContext_H_
#pragma once

namespace he {
namespace gfx {
class Window;
class GL;
    
class HAPPY_ENTRY GLContext
{
friend GL;
public:
    GLContext();
    virtual ~GLContext() {}
    
    virtual bool create(Window* const window);
    virtual void destroy();
    virtual void makeCurrent() = 0;

    inline GLEWContext& getGlewContext() { return m_GLEWContext; }
    inline uint32 getID() const { return m_ID; }
    inline void setID(const uint32 id) { m_ID = id; }
    
private:
    GLEWContext m_GLEWContext;
    uint32 m_ID;
    
    // Clear
    Color m_ClearColor;

    // Misc
    RectI m_Viewport;
    int m_MaxMultiSamples;

    // Depth
    bool m_DepthRead, m_DepthWrite;
    DepthFunc m_DepthFunc;

    // Culling
    bool m_CullFrontFace;
    bool m_CullCWFrontFace;

    // Binding
    uint32 m_BoundFbo, m_BoundVao;
    static const int MAX_UBO = 50;
    uint32 m_BoundUbo[MAX_UBO];
    static const int MAX_SAMPLERS = 31;
    uint32 m_BoundTex2D[MAX_SAMPLERS];
    uint32 m_BoundTexCube[MAX_SAMPLERS];
    uint32 m_ActiveTex;

    // Blending
    bool m_BlendEnabled;
    BlendFunc m_BlendSrc, m_BlendDest;
    BlendFunc m_ColorBlendSrc, m_ColorBlendDest,
              m_AlphaBlendSrc, m_AlphaBlendDest;
    BlendEquation m_BlendEquation;
    Color m_BlendColor;

    // Scissor
    bool m_ScissorEnabled;
    RectI m_ScissorRect;

    // Line smoothing
    bool m_LineSmoothEnabled;

    // Texture
    float m_MaxAnisotropicFilteringSupport;
    bool  m_SupportTextureCompression;
    
    uint32 m_DefaultVAO;

    GLContext(const GLContext&);
    GLContext& operator=(const GLContext&);
};

} } //end namespace

#endif
