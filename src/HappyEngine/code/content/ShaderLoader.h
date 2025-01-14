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
//Created: 30/09/2011

#ifndef _HE_SHADER_LOADER_H_
#define _HE_SHADER_LOADER_H_
#pragma once

#include "AssetContainer.h"

namespace he {
namespace gfx {
    class ShaderLayout;
    class Shader;
}

namespace ct {

class ShaderLoader
{
public:
    ShaderLoader();
    ~ShaderLoader();
    
    gfx::Shader* load(const he::String& vsPath, const he::String& geomPath, const he::String& fsPath, const he::ObjectList<he::String>* const defines = nullptr, const he::ObjectList<he::String>* const outputLayout = nullptr);
    
private:
    AssetContainer<ObjectHandle, uint32> m_AssetContainer;
    
    //Disable default copy constructor and default assignment operator
    ShaderLoader(const ShaderLoader&);
    ShaderLoader& operator=(const ShaderLoader&);
};

} } //end namespace

#endif
