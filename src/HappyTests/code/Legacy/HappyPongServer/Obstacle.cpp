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
//Created: 30/03/2012
#include "HappyPongServerPCH.h" 

#include "Obstacle.h"

#include "Material.h"
#include "Game.h"

namespace hps {

Obstacle::Obstacle(): 
    m_Position(0, 0, 0), 
    m_Radius(8.0f)
{
    setLocalTranslate(m_Position);
}


Obstacle::~Obstacle()
{
}

float Obstacle::getRadius() const
{
    return m_Radius;
}

const he::vec3& Obstacle::getPosition() const
{
    return m_Position;
}

} //end namespace