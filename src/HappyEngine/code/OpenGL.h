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

#ifndef _HE_OPENGL_H_
#define _HE_OPENGL_H_
#pragma once

namespace he {
namespace gfx {

#define MAX_OPENGL_CONTEXT 10
#define MAX_VERTEX_ARRAY_OBJECTS MAX_OPENGL_CONTEXT
typedef uint32 VaoID;

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

//http://www.opengl.org/sdk/docs/man3/xhtml/glDepthFunc.xml
ENUM(DepthFunc, uint16)
{
    DepthFunc_Never         =     GL_NEVER,        // Never passes.
    DepthFunc_Less          =     GL_LESS,         // Passes if the incoming depth value is less than the stored depth value.
    DepthFunc_Equal         =     GL_EQUAL,        // Passes if the incoming depth value is equal to the stored depth value.
    DepthFunc_LessOrEqual   =     GL_LEQUAL,       // Passes if the incoming depth value is less than or equal to the stored depth value.
    DepthFunc_Greater       =     GL_GREATER,      // Passes if the incoming depth value is greater than the stored depth value.
    DepthFunc_NotEqual      =     GL_NOTEQUAL,     // Passes if the incoming depth value is not equal to the stored depth value.
    DepthFunc_GeaterOrEqual =     GL_GEQUAL,       // Passes if the incoming depth value is greater than or equal to the stored depth value.
    DepthFunc_Always        =     GL_ALWAYS,       // Always passes.
    DepthFunc_Unassigned    =     0xffff
};

//more info check http://www.opengl.org/sdk/docs/man3/xhtml/glBlendFunc.xml
ENUM(BlendFunc, uint16)
{
    BlendFunc_Zero               =  GL_ZERO,	                        // (0, 0, 0, 0)
    BlendFunc_One                =  GL_ONE,	                            // (1, 1, 1, 1)
    BlendFunc_SrcColor           =  GL_SRC_COLOR,	                    // (Rs0, Gs0, Bs0, As0)
    BlendFunc_OneMinusSrcColor   =  GL_ONE_MINUS_SRC_COLOR,	            // (1, 1, 1, 1) - (Rs0, Gs0, Bs0, As0)
    BlendFunc_DestColor          =  GL_DST_COLOR,	                    // (Rd, Gd, Bd, Ad)
    BlendFunc_OneMinusDestColor  =  GL_ONE_MINUS_DST_COLOR,	            // (1, 1, 1, 1) - (Rd, Gd, Bd, Ad) 
    BlendFunc_SrcAlpha           =  GL_SRC_ALPHA,	                    // (As0, As0, As0, As0) 
    BlendFunc_OneMinusSrcAlpha   =  GL_ONE_MINUS_SRC_ALPHA,	            // (1, 1, 1, 1) - (As0, As0, As0, As0)
    BlendFunc_DestAlpha          =  GL_DST_ALPHA,	                    // (Ad, Ad, Ad, Ad)
    BlendFunc_OneMinusDestAlpha  =  GL_ONE_MINUS_DST_ALPHA,	            // (1, 1, 1, 1) - (Ad, Ad, Ad, Ad) 
    BlendFunc_ConstColor         =  GL_CONSTANT_COLOR,	                // (Rc, Gc, Bc, Ac)
    BlendFunc_OneMinusConstColor =  GL_ONE_MINUS_CONSTANT_COLOR,        // (1, 1, 1, 1) - (Rc, Gc, Bc, Ac)
    BlendFunc_ConstAlpha         =  GL_CONSTANT_ALPHA,	                // (Ac, Ac, Ac, Ac)
    BlendFunc_OneMinusConstAlpha =  GL_ONE_MINUS_CONSTANT_ALPHA,	    // (1, 1, 1, 1) - (Ac, Ac, Ac, Ac)
    BlendFunc_SrcAlphaSaturate   =  GL_SRC_ALPHA_SATURATE,	            // (i, i, i, 1)
    //GL_SRC1_COLOR	                                            // (Rs1, Gs1, Bs1, As1)
    //GL_ONE_MINUS_SRC1_COLOR	                                // (1, 1, 1, 1) - (Rs1, Gs1, Bs1, As1)
    //GL_SRC1_ALPHA	                                            // (As1, As1, As1, As1)
    //GL_ONE_MINUS_SRC1_ALPHA	                                // (1, 1, 1, 1) - (As1, As1, A s1, As1) 
    BlendFunc_Unassigned         =  0xffff
};

//http://www.opengl.org/sdk/docs/man3/xhtml/glBlendEquationSeparate.
ENUM(BlendEquation, uint16)
{
    BlendEquation_Add             =     GL_FUNC_ADD,                // src * srcFunc + dest * destFunc
    BlendEquation_Subtract        =     GL_FUNC_SUBTRACT,           // src * srcFunc - dest * destFunc
    BlendEquation_ReverseSubtract =     GL_FUNC_REVERSE_SUBTRACT,   // dest * destFunc - src * srcFunc
    BlendEquation_Min             =     GL_MIN,                     // min(src, dest)      per component, not using factors
    BlendEquation_Max             =     GL_MAX,                     // max(src, dest)      per component, not using factors
    BlendEquation_Unassigned      =     0xffff
};

class VertexElement;
class GLContext;
class GL
{
public:
    static GLContext* s_CurrentContext;

    // Reset
    static void reset();
    static void init();

    // Misc
    static void heSetViewport(const RectI& viewport);
    static const RectI& heGetViewport();
    static void getGLTypesFromVertexElement(const VertexElement& element, GLint& components, GLenum& type);
    static int getMaxMultiSamples();

    // Clear
    static void heClearColor(const Color& color);

    // Depth
    static void heSetDepthRead(bool read);
    static void heSetDepthFunc(DepthFunc func);
    static void heSetDepthWrite(bool write);

    // Culling
    static void heSetCullFace(bool cullFrontFace);
    static void heSetWindingFrontFace(bool cw);

    // Binding
    static void heBindFbo(uint32 fbo);
    static uint32 heGetBoundFbo();
    static void heBindVao(uint32 vao);
    static void heBindTexture2D(uint32 tex);
    static void heBindTexture2D(uint32 texPos, uint32 tex);
    static void heBindTextureCube(uint32 texPos, uint32 tex);
    static void heBindUniformBuffer(uint32 uboId, uint32 bufferId);

    // Blending
    static void heBlendEnabled(bool enabled);
    static void heBlendFunc(BlendFunc srcFactor, BlendFunc destFactor);
    static void heBlendFuncSeperate(BlendFunc colorSrcFactor,
                                    BlendFunc colorDestFactor,
                                    BlendFunc alphaSrcFactor,
                                    BlendFunc alphaDestFactor);
    static void heBlendEquation(BlendEquation eq);
    static void heBlendColor(const Color& color);

    // Scissor
    static void heScissorEnabled(bool enabled);
    static void heScissorRect(const RectI& rect);

    // Line smoothing
    static void heLineSmoothEnabled(bool enabled);

    // Texture
    static float getMaxAnisotropicFilteringSupport();
    static bool  getSupportTextureCompression();


private:

    GL() {}
};

} } //end namespace

#endif
