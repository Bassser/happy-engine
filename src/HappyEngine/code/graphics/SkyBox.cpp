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
//Created: 14/01/2012
#include "HappyPCH.h" 

#include "SkyBox.h"
#include "ContentManager.h"

#include "ModelMesh.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "TextureCube.h"

#include "Entity.h"
#include "ModelComponent.h"

namespace he {
namespace gfx {

SkyBox::SkyBox()
    : m_CubeMap(nullptr)
    , m_Drawable(nullptr)
{
}


SkyBox::~SkyBox()
{
    unload();
}

void SkyBox::unload()
{
    if (m_CubeMap != nullptr)
    {
        m_CubeMap->release();
        m_CubeMap = nullptr;
    }
    HEDelete(m_Drawable);
    m_Drawable = nullptr;
}

void SkyBox::load( const he::String& asset )
{
    HE_ASSERT(m_Drawable == nullptr, "Skybox is loaded twice!");
    m_Drawable = HENew(Drawable)();
    m_Drawable->setLocalScale(vec3(1000000000)); // bounds must be huge
    //////////////////////////////////////////////////////////////////////////
    /// Load Model
    //////////////////////////////////////////////////////////////////////////
    ModelMesh* const cube(
        ResourceFactory<gfx::ModelMesh>::getInstance()->get(ResourceFactory<gfx::ModelMesh>::getInstance()->create()));
    cube->setName(he::String("skybox-") + asset);
    he::PrimitiveList<vec3> vertices(8);
    vertices.add(vec3(-1,  1, -1));
    vertices.add(vec3( 1,  1, -1));
    vertices.add(vec3(-1, -1, -1));
    vertices.add(vec3( 1, -1, -1));

    vertices.add(vec3(-1,  1,  1));
    vertices.add(vec3( 1,  1,  1));
    vertices.add(vec3(-1, -1,  1));
    vertices.add(vec3( 1, -1,  1));

    he::PrimitiveList<uint16> indices(36);
    indices.add(0); indices.add(1); indices.add(2); //front
    indices.add(1); indices.add(3); indices.add(2);

    indices.add(5); indices.add(4); indices.add(7); //back
    indices.add(4); indices.add(6); indices.add(7);

    indices.add(4); indices.add(0); indices.add(6); //left
    indices.add(0); indices.add(2); indices.add(6);

    indices.add(1); indices.add(5); indices.add(3); //right
    indices.add(5); indices.add(7); indices.add(3);

    indices.add(4); indices.add(5); indices.add(0); //top
    indices.add(5); indices.add(1); indices.add(0);

    indices.add(3); indices.add(7); indices.add(2); //bottom
    indices.add(7); indices.add(6); indices.add(2);

    VertexLayout layout;
    layout.addElement(VertexElement(eShaderAttribute_Position, eShaderAttributeType_Float, eShaderAttributeTypeComponents_3, 0));
    cube->init(layout, MeshDrawMode_Triangles);
    cube->setVertices(&vertices[0], static_cast<uint32>(vertices.size()), MeshUsage_Static, false);
    cube->setIndices(&indices[0], static_cast<uint32>(indices.size()), IndexStride_UShort, MeshUsage_Static);
    cube->setLoaded(eLoadResult_Success);
    m_Drawable->setModelMesh(cube);
    cube->release();

    Material* const material(CONTENT->loadMaterial("engine/sky.material"));
    m_Drawable->setMaterial(material);
    material->release();

    const int8 cubeMap(m_Drawable->getMaterial()->findParameter(HEFS::strcubeMap));
    if (cubeMap >= 0)
    {
        const TextureCube* cube(CONTENT->asyncLoadTextureCube(asset));
        m_Drawable->getMaterial()->getParameter(cubeMap).setTextureCube(cube);
        cube->release();
    }
}

} } //end namespace
