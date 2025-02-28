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
//Created: 18/10/2011

#ifndef _HE_ROUNDED_RECTANGLE2D_H_
#define _HE_ROUNDED_RECTANGLE2D_H_
#pragma once

#include "Shape2D.h"
#include "Polygon.h"
#include "vec2.h"

namespace he {
namespace gui {

class RoundedRectangle2D : public Shape2D
{
public:

	/* CONSTRUCTOR - DESTRUCTOR */
	RoundedRectangle2D(	const vec2& centerPos,
						const vec2& size,
						float radius,
						uint interpolSteps = 30);
    virtual ~RoundedRectangle2D();
};

} } //end namespace

#endif
