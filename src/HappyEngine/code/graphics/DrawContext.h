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
//Created: 2014/03/09

#ifndef _HE_DrawContext_H_
#define _HE_DrawContext_H_
#pragma once

namespace he {
namespace gfx {

class ICamera;
class ModelMesh;
class MaterialInstance;

struct DrawContext
{
    DrawContext()
        : m_Camera(nullptr)
        , m_VBO(0)
        , m_IBO(0)
    {}

    const ICamera* m_Camera;
    uint32 m_VBO;
    uint32 m_IBO;
    mat44 m_WorldMatrix;
};

} } //end namespace

#endif
