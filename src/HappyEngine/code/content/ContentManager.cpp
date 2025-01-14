
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

#include "HappyPCH.h" 

#include "ContentManager.h"

#include "Vertex.h"
#include "ModelMesh.h"

#include "TextureLoader.h"
#include "ModelLoader.h"
#include "PhysicsShapeLoader.h"
#include "MaterialLoader.h"
#include "FontLoader.h"
#include "ShaderLoader.h"
#include "Shader.h"
#include "Renderer2D.h"

namespace he {
namespace ct {

ContentManager::ContentManager(): 
    m_ModelLoader(nullptr), 
    m_TextureLoader(nullptr),
    m_PhysicsShapeLoader(nullptr), 
    m_FontLoader(nullptr),
    m_ShaderLoader(nullptr), 
    m_MaterialLoader(nullptr), 
    m_LoadThread(nullptr),
    m_LoadThreadRunning(true),

    m_TextureFolder("textures/"), 
    m_ModelFolder("models/"), 
    m_PhysicsFolder("physics/"), 
    m_FontFolder("fonts/"),
    m_ShaderFolder("shaders/"), 
    m_MaterialFolder("materials/"),
    m_FxFolder("fx/"),

    m_ContentRootDir(he::String("")),
    m_TexturePath(he::String("")), 
    m_ModelPath(he::String("")), 
    m_PhysicsPath(he::String("")), 
    m_FontPath(he::String("")),
    m_ShaderPath(he::String("")), 
    m_MaterialPath(he::String("")),
    m_FxPath(he::String("")),

    m_ParticleQuad(nullptr),
    m_FullscreenQuad(nullptr)
{
}

void ContentManager::initialize()
{
    m_ModelLoader = HENew(ModelLoader)();
    m_TextureLoader = HENew(TextureLoader)();
    m_PhysicsShapeLoader = HENew(PhysicsShapeLoader)();
    m_FontLoader = HENew(FontLoader)();
    m_ShaderLoader = HENew(ShaderLoader)();
    m_MaterialLoader = HENew(MaterialLoader)();
    m_LoadThread = HENew(Thread)();

    m_ContentRootDir = he::Path::getDataPath();
    setFontFolder(getFontFolder());
    setFxFolder(getFxFolder());
    setMaterialFolder(getMaterialFolder());
    setModelFolder(getModelFolder());
    setPhysicsFolder(getPhysicsFolder());
    setShaderFolder(getShaderFolder());
    setTextureFolder(getTextureFolder());

    m_LoadThread->startThread(std::bind(&ContentManager::loadTick, this), "ContentLoadThread");
}

ContentManager::~ContentManager()
{
}

void ContentManager::destroy()
{
    m_LoadThreadRunning = false;
    if (m_LoadThread)
    {
        m_LoadThread->join();
        HEDelete(m_LoadThread);
        m_LoadThread = nullptr;
    }

    if (m_ParticleQuad != nullptr)
    {
        m_ParticleQuad->release();
        m_ParticleQuad = nullptr;
    }
    if (m_FullscreenQuad != nullptr)
    {
        m_FullscreenQuad->release();
        m_FullscreenQuad = nullptr;
    }

    // All content should be gone when getting here
    // loaders perform last garbage collect
    HEDelete(m_MaterialLoader);
    m_MaterialLoader = nullptr;
    HEDelete(m_ShaderLoader);
    m_ShaderLoader = nullptr;
    HEDelete(m_PhysicsShapeLoader);
    m_PhysicsShapeLoader = nullptr;
    HEDelete(m_FontLoader);
    m_FontLoader = nullptr;
    HEDelete(m_ModelLoader);
    m_ModelLoader = nullptr;
    HEDelete(m_TextureLoader);
    m_TextureLoader = nullptr;
}

void ContentManager::tick(float dTime)
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
    m_ModelLoader->tick(dTime);
    m_TextureLoader->tick(dTime);
}

void ContentManager::loadTick()
{
    CLAIM_THREAD(he::eThreadTicket_Content);
    while (m_LoadThreadRunning)
    {
        bool sleep(true);
        sleep &= !m_ModelLoader->loadTick();
        sleep &= !m_TextureLoader->loadTick();
        if (sleep)
        {
            Thread::sleep(100); // TOOD(BAS) convert to ThreadEvent
        }
    }
}

void ContentManager::glThreadInvoke()  //needed for all of the gl operations
{
    m_ModelLoader->glThreadInvoke();
    m_TextureLoader->glThreadInvoke();
}

//////////////////////////////////////////////////////////////////////////
gfx::Model* ContentManager::asyncLoadModel(const he::String& asset)
{
    return m_ModelLoader->asyncLoadModel(m_ModelPath.str() + asset, true);
}
gfx::ModelMesh* ContentManager::asyncLoadModelMesh( const he::String& asset, const he::String& meshName )
{
    return m_ModelLoader->asyncLoadModelMesh(m_ModelPath.str() + asset, meshName, true);
}
gfx::Model* ContentManager::loadModel(const he::String& asset)
{
    return m_ModelLoader->loadModel(m_ModelPath.str() + asset, true);
}
gfx::ModelMesh* ContentManager::loadModelMesh(const he::String& asset, const he::String& meshName)
{
    return m_ModelLoader->loadModelMesh(m_ModelPath.str() + asset, meshName, true);
}

//////////////////////////////////////////////////////////////////////////
const gfx::Texture2D* ContentManager::asyncLoadTexture2D(const he::String& asset)
{
    return m_TextureLoader->asyncLoadTexture2D(m_TexturePath.str() + asset);
}
const gfx::TextureCube* ContentManager::asyncLoadTextureCube( const he::String& asset )
{
    return m_TextureLoader->asyncLoadTextureCube(m_TexturePath.str() + asset);
}
const gfx::Texture2D* ContentManager::asyncMakeTexture2D(const Color& color)
{
    return m_TextureLoader->asyncMakeTexture2D(color);
}
const gfx::TextureCube* ContentManager::asyncMakeTextureCube( const Color& /*color*/ )
{
    HE_NOT_IMPLEMENTED;
    return nullptr;
}
const gfx::Texture2D* ContentManager::loadTexture2D(const he::String& path)
{
    return m_TextureLoader->loadTexture2D(m_TexturePath.str()  + path);
}
const gfx::TextureCube* ContentManager::loadTextureCube( const he::String& path )
{
    return m_TextureLoader->loadTextureCube(m_TexturePath.str()  + path);
}
const gfx::Texture2D* ContentManager::makeTexture2D(const Color& color)
{
    return m_TextureLoader->makeTexture2D(color);
}
const gfx::TextureCube* ContentManager::makeTextureCube( const Color& /*color*/ )
{
    HE_NOT_IMPLEMENTED;
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
ObjectHandle ContentManager::loadPhysicsConvex(const he::String& asset)
{
    return m_PhysicsShapeLoader->loadConvex(m_PhysicsPath.str()  + asset);
}
ObjectHandle ContentManager::loadPhysicsConcave(const he::String& asset)
{
    return m_PhysicsShapeLoader->loadConcave(m_PhysicsPath.str() + asset);
}

//////////////////////////////////////////////////////////////////////////
gui::Font* ContentManager::loadFont(const he::String& asset, uint16 size, uint8 options)
{
    return m_FontLoader->load(m_FontPath.str() + asset, size, options);
}

gui::Font* ContentManager::getDefaultFont(uint16 size)
{
    return loadFont(he::String("Ubuntu-Bold.ttf"), size);
}

//////////////////////////////////////////////////////////////////////////
he::gfx::Shader* ContentManager::loadShader(const he::String& vsAsset, const he::String& geomAsset, const he::String& fsAsset, const he::ObjectList<he::String>* const defines /*= nullptr*/, const he::ObjectList<he::String>* const outputLayout /*= nullptr*/)
{
    return m_ShaderLoader->load(m_ShaderPath.str() + vsAsset, geomAsset.empty()? geomAsset : m_ShaderPath.str() + geomAsset, m_ShaderPath.str() + fsAsset, defines, outputLayout);
}

//////////////////////////////////////////////////////////////////////////
he::gfx::Material* ContentManager::loadMaterial(const he::String& asset)
{
    return m_MaterialLoader->load(asset);
}

//////////////////////////////////////////////////////////////////////////
void ContentManager::setTextureFolder(const he::String& folder)
{
    m_TextureFolder = folder;
    m_TexturePath = m_ContentRootDir.append(folder);
}
void ContentManager::setModelFolder(const he::String& folder)
{
    m_ModelFolder = folder;
    m_ModelPath = m_ContentRootDir.append(folder);
}
void ContentManager::setPhysicsFolder(const he::String& folder)
{
    m_PhysicsFolder = folder;
    m_PhysicsPath = m_ContentRootDir.append(folder);
}
void ContentManager::setFontFolder(const he::String& folder)
{
    m_FontFolder = folder;
    m_FontPath = m_ContentRootDir.append(folder);
}
void ContentManager::setShaderFolder(const he::String& folder)
{
    m_ShaderFolder = folder;
    m_ShaderPath = m_ContentRootDir.append(folder);
}
void ContentManager::setMaterialFolder(const he::String& folder)
{
    m_MaterialFolder = folder;
    m_MaterialPath = m_ContentRootDir.append(folder);
}
void ContentManager::setFxFolder( const he::String& folder )
{
    m_FxFolder = folder;
    m_FxPath = m_ContentRootDir.append(folder);
}
const Path& ContentManager::getContentDir() const
{
    return m_ContentRootDir;
}
const he::String& ContentManager::getTextureFolder() const
{
    return m_TextureFolder;
}
const he::String& ContentManager::getModelFolder() const
{
    return m_ModelFolder;
}
const he::String& ContentManager::getPhysicsFolder() const
{
    return m_PhysicsFolder;
}
const he::String& ContentManager::getFontFolder() const
{
    return m_FontFolder;
}
const he::String& ContentManager::getShaderFolder() const
{
    return m_ShaderFolder;
}
const he::String& ContentManager::getMaterialFolder() const
{
    return m_MaterialFolder;
}
const he::String& ContentManager::getFxFolder() const
{
    return m_FxFolder;
}
const Path& ContentManager::getTextureFolderPath() const
{
    return m_TexturePath;
}
const Path& ContentManager::getModelFolderPath() const
{
    return m_ModelPath;
}
const Path& ContentManager::getPhysicsFolderPath() const
{
    return m_PhysicsPath;
}
const Path& ContentManager::getFontFolderPath() const
{
    return m_FontPath;
}
const Path& ContentManager::getShaderFolderPath() const
{
    return m_ShaderPath;
}
const Path& ContentManager::getMaterialFolderPath() const
{
    return m_MaterialPath;
}
const Path& ContentManager::getFxFolderPath() const
{
    return m_FxPath;
}

//////////////////////////////////////////////////////////////////////////
gfx::ModelMesh* ContentManager::getFullscreenQuad()
{
    using namespace gfx;
    if (m_FullscreenQuad == nullptr)
    {
        VertexLayout layout;
        layout.addElement(VertexElement(eShaderAttribute_Position, eShaderAttributeType_Float, eShaderAttributeTypeComponents_3, 0));

        ObjectHandle handle(ResourceFactory<ModelMesh>::getInstance()->create());
        m_FullscreenQuad = ResourceFactory<ModelMesh>::getInstance()->get(handle);
        m_FullscreenQuad->setName(he::String("Full screen quad"));

        he::ObjectList<VertexPos> vertices(4);
        vertices.add(VertexPos(vec3(-1, 1, 1.0f)));
        vertices.add(VertexPos(vec3(1, 1, 1.0f)));
        vertices.add(VertexPos(vec3(-1, -1, 1.0f)));
        vertices.add(VertexPos(vec3(1, -1, 1.0f)));

        he::PrimitiveList<uint16> indices(6);
        indices.add(1); indices.add(0); indices.add(2);
        indices.add(1); indices.add(2); indices.add(3);

        m_FullscreenQuad->init(layout, gfx::MeshDrawMode_Triangles);
        m_FullscreenQuad->setVertices(&vertices[0], 4, gfx::MeshUsage_Static, false);
        m_FullscreenQuad->setIndices(&indices[0], 6, gfx::IndexStride_UShort, gfx::MeshUsage_Static);
        m_FullscreenQuad->setLoaded(eLoadResult_Success);
    }

    ResourceFactory<ModelMesh>::getInstance()->instantiate(m_FullscreenQuad->getHandle());
    return m_FullscreenQuad;
}

bool ContentManager::isLoading() const
{
    return m_ModelLoader->isLoading() || m_TextureLoader->isLoading();
}

//////////////////////////////////////////////////////////////////////////
///  Particle Quad
gfx::ModelMesh* ContentManager::getParticleQuad()
{
    if (m_ParticleQuad == nullptr)
    {    
        using namespace gfx;
        VertexLayout layout;
        layout.addElement(VertexElement(eShaderAttribute_Position, eShaderAttributeType_Float, eShaderAttributeTypeComponents_3, 0));

        he::ObjectList<VertexPos> vertices(4);
        vertices.add(VertexPos(vec3(-1, 1, 0.0f)));
        vertices.add(VertexPos(vec3(1, 1, 0.0f)));
        vertices.add(VertexPos(vec3(-1, -1, 0.0f)));
        vertices.add(VertexPos(vec3(1, -1, 0.0f)));

        he::PrimitiveList<uint16> indices(6);
        indices.add(1); indices.add(0); indices.add(2);
        indices.add(1); indices.add(2); indices.add(3);

        ObjectHandle handle(ResourceFactory<ModelMesh>::getInstance()->create());
        m_ParticleQuad = ResourceFactory<ModelMesh>::getInstance()->get(handle);
        m_ParticleQuad->setName(he::String("Particle quad"));

        m_ParticleQuad->init(layout, gfx::MeshDrawMode_Triangles);
        m_ParticleQuad->setVertices(&vertices[0], 4, gfx::MeshUsage_Static, false);
        m_ParticleQuad->setIndices(&indices[0], 6, gfx::IndexStride_UShort, gfx::MeshUsage_Static);
        m_ParticleQuad->setLoaded(eLoadResult_Success);
    }

    m_ParticleQuad->instantiate();
    return m_ParticleQuad;
}

const gfx::Texture2D* ContentManager::getFallbackTexture()
{
    return m_TextureLoader? m_TextureLoader->getFallbackTexture() : nullptr;
}

//////////////////////////////////////////////////////////////////////////


} } //end namespace
