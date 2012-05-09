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
//Created: 30/03/2012
#include "HappyTestsPCH.h" 

#include "Obstacle.h"

#include "ControlsManager.h"
#include "ContentManager.h"

#include "ModelComponent.h"
#include "ModelMesh.h"

#include "Material.h"
#include "Game.h"

namespace ht {

Obstacle::Obstacle(): 
    m_Position(0, 0, 0), 
    m_Radius(8.0f)
{
    he::game::ModelComponent* model(NEW he::game::ModelComponent());
    model->setMaterial(CONTENT->loadMaterial("pong/obstacle.material"));
    he::gfx::ModelMesh* mesh(CONTENT->asyncLoadModelMesh("pong/obstacles.binobj", "M_Torus", model->getMaterial().getCompatibleVertexLayout()));
    model->setModelMesh(mesh->getHandle());
    mesh->release();
    model->setLocalTransform(he::mat44::createScale(100));
    addComponent(model);

    setWorldMatrix(he::mat44::createTranslation(m_Position));
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