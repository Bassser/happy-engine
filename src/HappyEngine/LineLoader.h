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
//Author:	Bastian Damman
//Created:	05/09/2011

#ifndef _LINE_LOADER_H_
#define _LINE_LOADER_H_
#pragma once

#include "AssetContainer.h"
#include "Vector3.h"

#include <string>
#include <vector>

namespace happyengine {
namespace content {

class LineLoader
{
public:
	LineLoader();
    virtual ~LineLoader();
 
	std::vector<math::Vector3> loadLine(const std::string& path);

private:

	AssetContainer<std::vector<math::Vector3>>* m_pAssetContainer;

    //Disable default copy constructor and default assignment operator
    LineLoader(const LineLoader&);
    LineLoader& operator=(const LineLoader&);
};

} } //end namespace

#endif