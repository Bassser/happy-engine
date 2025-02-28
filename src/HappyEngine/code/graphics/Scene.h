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
//Created: 14/07/2012

#ifndef _HE_Scene_H_
#define _HE_Scene_H_
#pragma once

#include "DrawListContainer.h"
#include "Bound.h"
#include "Singleton.h"

namespace he {
namespace ge {
class CameraManager;
}
namespace gfx {
class LightManager;
class InstancingManager;
class Picker;
class Drawable;
class Scene;
class ShadowCaster;
class SkyBox;

class SceneFactory: public ObjectFactory<Scene>, public Singleton<SceneFactory>
{
    friend Singleton;
    SceneFactory() { init(1, 2, he::String("SceneFactory")); }
public:
    virtual ~SceneFactory() { }
};

class HAPPY_ENTRY Scene
{
    DECLARE_OBJECT(Scene)
    friend class Drawable;
public:
    Scene();
    virtual ~Scene();

    // Drawlist
    void forceReevalute(Drawable* drawable);
    void doReevalute(Drawable* drawable);
    void prepareForRendering();
    
    // Getters
    LightManager*      getLightManager() const { return m_LightManager; }
    InstancingManager* getInstancingManager() const { return m_InstancingManager; }
    ge::CameraManager* getCameraManager() const { return m_CameraManager; }
    const DrawListContainer& getDrawList() const { return m_DrawList; }
    ShadowCaster*      getShadowRenderer() const { return m_ShadowCaster; }
    
    // Visual Picking
    //void initPicking(); // only init picking when needed, because it requires extra FBO & shader
    //uint pick(const vec2& screenPoint);
    //uint pick(const vec2& screenPoint, const he::PrimitiveList<Drawable*>& drawList);
    
    // Active
    void setActive(bool active) { m_Active =  active; }
    bool getActive() const { return m_Active; }

    // Skybox
    void loadSkybox(const he::String& asset);

private:  
    // Drawable friends
    void attachToScene(Drawable* drawable);
    void detachFromScene(Drawable* drawable);

    // Managers
    LightManager* m_LightManager;
    InstancingManager* m_InstancingManager;
    ge::CameraManager* m_CameraManager;
    DrawListContainer m_DrawList;
    
    Picker* m_Picker;

    ShadowCaster* m_ShadowCaster;
    SkyBox* m_SkyBox;

    bool m_Active;

    //Disable default copy constructor and default assignment operator
    Scene(const Scene&);
    Scene& operator=(const Scene&);
};

} } //end namespace

#endif
