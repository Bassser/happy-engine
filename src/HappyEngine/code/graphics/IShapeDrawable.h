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
//Created: 11/08/2012

#ifndef _HE_IShapeDrawable_H_
#define _HE_IShapeDrawable_H_
#pragma once

namespace he {
namespace gfx {
class ShapeRenderer;
class IShapeDrawable
{
public:
    virtual ~IShapeDrawable() {};

    virtual void drawShapes(he::gfx::ShapeRenderer* const renderer) = 0;
}; 

} } //end namespace

#endif
