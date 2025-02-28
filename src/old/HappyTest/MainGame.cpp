//HappyTest Copyright (C) 2011 - 2012  Bastian Damman, Sebastiaan Sprengers 
//
//This file is part of HappyTest.
//
//    HappyTest is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    HappyTest is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with HappyTest.  If not, see <http://www.gnu.org/licenses/>.
#include "HappyPCH.h"
#include "MainGame.h"

#include <vector>
#include <set>
#include <list>
#include <sstream>
#include <iostream>

#include "mat44.h"
#include "vec3.h"
#include "MathConstants.h"
#include "HappyEngine.h"
#include "MathFunctions.h"
#include "NetworkManager.h"
#include "GraphicsEngine.h"
#include "ContentManager.h"
#include "Console.h"
#include "ControlsManager.h"
#include "Happy2DRenderer.h"
#include "PhysicsEngine.h"
#include "PhysicsBoxShape.h"
#include "SoundEngine.h"

#include "IniReader.h"
#include "Circle.h"

#include "HappyNew.h"

#include "boost/timer.hpp"

#include "Random.h"
#include "ExtraForward3DRenderer.h"

#include "BinaryStream.h"

#include "ModelComponent.h"
#include "InstancedModelComponent.h"
#include "StaticPhysicsComponent.h"
#include "DynamicPhysicsComponent.h"
#include "PhysicsConvexShape.h"
#include "PhysicsConcaveShape.h"
#include "LightComponent.h"

#include "PhysicsCarManager.h"
#include "LightManager.h"
#include "InstancingManager.h"
#include "InstancingController.h"

#include "Profiler.h"

#include "PhysicsData.h"

#include "Deferred3DRenderer.h"
#include "CameraManager.h"

#include "SlotPContainer.h"

#include "SkyBox.h"

#include "fx/FxManager.h"
#include "fx/FxTimeLine.h"
#include "fx/FxTimeLineTrack.h"
#include "fx/FxParticleSystem.h"
#include "fx/FxRandom.h"
#include "fx/FxConstant.h"
#include "fx/FxCurve.h"

#include "fx/FxParticleSpeed.h"
#include "fx/FxParticleForce.h"
#include "fx/FxParticleScale.h"
#include "fx/FxParticleColor.h"
#include "fx/FxParticleRotation.h"
#include "fx/FxParticleRotationRate.h"
#include "fx/FxParticleLocation.h"

namespace happytest {

MainGame::MainGame() : m_pTestObject(nullptr), m_BackgroundIndex(0),
                       m_DrawTimer(0), m_UpdateTimer(0),       
                       m_pFPSGraph(NEW he::tools::FPSGraph()),
                       m_pTestButton(nullptr), m_pAxis(nullptr),
                       m_pTextBox(nullptr), m_bTest(true), m_bTest2(true), m_Test3("You can edit this string via console"),
                       m_pScene(0), m_pSky(0),
                       m_pTestSound2D(nullptr), m_pTestGrid(nullptr),
                       m_pFrictionTable(nullptr), m_pFxEditorBinding(NEW he::tools::HappyFxEditorBinding()),
                       m_pSkyBox(NEW he::gfx::SkyBox())
{
    using namespace he;
    m_BackgroundColors[0] = Color((byte)10, (byte)130, (byte)131, (byte)255);
    m_BackgroundColors[1] = Color((byte)122, (byte)186, (byte)122, (byte)255);
    m_BackgroundColors[2] = Color((byte)255, (byte)127, (byte)80, (byte)255);
    m_BackgroundColors[3] = Color((byte)255, (byte)165, (byte)0, (byte)255);
    m_BackgroundColors[4] = Color((byte)30, (byte)144, (byte)255, (byte)255);
}


MainGame::~MainGame()
{
    PHYSICS->stopSimulation();

    CAMERAMANAGER->deleteAllCameras();

    delete m_pTestObject;
    std::for_each(m_Bullets.cbegin(), m_Bullets.cend(), [&](he::game::Entity* pBullet)
    {
        delete pBullet;
    });


    delete m_pFPSGraph;

    delete m_pTestButton;
    delete m_pAxis;
    delete m_pScene;
    delete m_pSky;
    delete m_pTextBox;
    delete m_pTestButton2;
    delete m_pTestButton3;
    delete m_pTestGrid;

    delete m_pSkyBox;

    delete m_pFrictionTable;

    delete m_pFxEditorBinding;

    NETWORK->stop();
}

void MainGame::init()
{
    using namespace he;
    io::IniReader reader;
    if (reader.open("../data/settings.ini"))
    {
        vec2 windowDim(reader.readVector2(L"Window", L"dimension", vec2(1280, 720)));
        GRAPHICS->setScreenDimension(static_cast<int>(windowDim.x), static_cast<int>(windowDim.y));
        GRAPHICS->setWindowTitle(reader.readString(L"Window", L"title", "Test"));

        vec4 bColor(reader.readVector4(L"Background", L"color"));
        GRAPHICS->setBackgroundColor(Color(bColor.x, bColor.y, bColor.z, bColor.w));

        GRAPHICS->setVSync(reader.readBool(L"Graphics", L"vsync", true));
        GRAPHICS->toggleFullscreen(reader.readBool(L"Graphics", L"fullscreen"));
    }
    else
    {
        HE_WARNING("unable to open settings.ini");
    }
}
void MainGame::load()
{
    using namespace he;

    //happycooker::HappyCooker* cooker(happycooker::HappyCooker::getInstance());
    //cooker->cookToBinObj("D:/File Server/Programming/happy-engine/art/models/export/car.dae", "D:/File Server/Programming/happy-engine/src/HappyTest/data/models/car.binobj");

    using namespace he;

    m_SplashImage = CONTENT->asyncLoadTexture("happy_splash.png");

    //////////////////////////////////////////////////////////////////////////
    /// PHYSICS
    //////////////////////////////////////////////////////////////////////////
    m_pFrictionTable = NEW he::px::DriveableSurfaceTyreFrictionTable(MAX_DRIVEABLE_MATERIALS, MAX_TYRE_TYPES);

    m_pFrictionTable->setMaterial(DM_Grass,     px::PhysicsMaterial(0.90f, 0.85f, 0.05f));
    m_pFrictionTable->setMaterial(DM_Sand,      px::PhysicsMaterial(0.75f, 0.70f, 0.01f));
    m_pFrictionTable->setMaterial(DM_Wood,      px::PhysicsMaterial(0.95f, 0.90f, 0.09f));
    m_pFrictionTable->setMaterial(DM_Ice,       px::PhysicsMaterial(0.65f, 0.60f, 0.09f));
    m_pFrictionTable->setMaterial(DM_Tarmac,    px::PhysicsMaterial(1.40f, 1.30f, 0.01f));
    m_pFrictionTable->setMaterial(DM_Concrete,  px::PhysicsMaterial(1.50f, 1.40f, 0.01f));
    m_pFrictionTable->setMaterial(DM_Tyre,      px::PhysicsMaterial(1.30f, 1.20f, 0.10f));
    m_pFrictionTable->setMaterial(DM_Metal,     px::PhysicsMaterial(0.85f, 0.80f, 0.08f));

    m_pFrictionTable->setTyreType(TT_Slicks, 1.0f);
    m_pFrictionTable->setTyreType(TT_Wets, 1.2f);

    PHYSICS->getCarManager()->init(m_pFrictionTable);
    PHYSICS->startSimulation();
    
    GRAPHICS->getLightManager()->setAmbientLight(Color(0.8f, 0.4f, 1.0f, 1.0f), 0.2f);
    GRAPHICS->getLightManager()->setDirectionalLight(normalize(vec3(-1.0f, 1.0f, 1.0f)), Color(1.0f, 1.0f, 0.9f, 1.0f), 0.5f);
   
    m_pSkyBox->load("skybox/night/night.png");
    GRAPHICS->addToDrawList(m_pSkyBox);

    //Bullet
    gfx::Material matBullet(CONTENT->loadMaterial("bullet.material"));
    gfx::ModelMesh::pointer meshBullet(CONTENT->asyncLoadModelMesh("cube.binobj", "M_Cube", matBullet.getCompatibleVertexLayout()));
    GRAPHICS->getInstancingManager()->createController("bullet", true, meshBullet, matBullet);

    m_pAxis = NEW he::game::Entity();
    game::ModelComponent* pAxisModelComp(NEW game::ModelComponent());
    pAxisModelComp->setMaterial(CONTENT->loadMaterial("axis.material"));
    pAxisModelComp->setModelMesh(CONTENT->asyncLoadModelMesh("axis.binobj", "M_Axis", pAxisModelComp->getMaterial().getCompatibleVertexLayout()));
    m_pAxis->addComponent(pAxisModelComp);

    m_pScene = NEW he::game::Entity();
    game::ModelComponent* pSceneModelComp(NEW game::ModelComponent());
    pSceneModelComp->setMaterial(CONTENT->loadMaterial("testSceneBas.material"));
    pSceneModelComp->setModelMesh(CONTENT->asyncLoadModelMesh("testSceneBas/testSceneBas.binobj", "M_Ground", pSceneModelComp->getMaterial().getCompatibleVertexLayout()));
    pSceneModelComp->setVisible(true);
    m_pScene->addComponent(pSceneModelComp);
    game::StaticPhysicsComponent* pScenePxComp(NEW game::StaticPhysicsComponent());
    m_pScene->addComponent(pScenePxComp);
    const auto& pSceneCVmeshes(CONTENT->loadPhysicsConvex("testSceneBas/testSceneBas.pxcv"));
    std::for_each(pSceneCVmeshes.cbegin(), pSceneCVmeshes.cend(), [&](const px::PhysicsConvexMesh::pointer& pMesh)
    {
        he::px::PhysicsConvexShape pShape(pMesh);
        pScenePxComp->addShape(&pShape, PHYSICS->getDriveableMaterial(DM_Concrete));
    });
    const auto& pSceneCCmeshes(CONTENT->loadPhysicsConcave("testSceneBas/testSceneBas.pxcc"));
    std::for_each(pSceneCCmeshes.cbegin(), pSceneCCmeshes.cend(), [&](const px::PhysicsConcaveMesh::pointer& pMesh)
    {
        he::px::PhysicsConcaveShape pShape(pMesh);
        pScenePxComp->addShape(&pShape, PHYSICS->getDriveableMaterial(DM_Tarmac));
    });
    #pragma region lights
    game::PointLightComponent* pTempPointLightComp(NEW game::PointLightComponent());
    m_pScene->addComponent(pTempPointLightComp);
    pTempPointLightComp->setOffset(vec3(8.822f, 6.739f, -20.068f));
    pTempPointLightComp->setMultiplier(5.0f);
    pTempPointLightComp->setColor(Color((byte)126, 190, 255, 255));
    pTempPointLightComp->setAttenuation(0, 15);

    pTempPointLightComp = NEW game::PointLightComponent(); //pyramids
    m_pScene->addComponent(pTempPointLightComp);
    pTempPointLightComp->setOffset(vec3(-41.428f, 23.483f, 46.274f));
    pTempPointLightComp->setMultiplier(4.0f);
    pTempPointLightComp->setColor(Color((byte)229, 174, 126, 255));
    pTempPointLightComp->setAttenuation(0, 42.2f);
    pTempPointLightComp->setBroken(true);

    pTempPointLightComp = NEW game::PointLightComponent(); //green
    m_pScene->addComponent(pTempPointLightComp);
    pTempPointLightComp->setOffset(vec3(-37.718f, 23.483f, -17.693f));
    pTempPointLightComp->setMultiplier(6.0f);
    pTempPointLightComp->setColor(Color((byte)209, 243, 214, 255));
    pTempPointLightComp->setAttenuation(0, 39.6f);

    pTempPointLightComp = NEW game::PointLightComponent(); //small red
    m_pScene->addComponent(pTempPointLightComp);
    pTempPointLightComp->setOffset(vec3(-20.946f, 3.02f, -13.92f));
    pTempPointLightComp->setMultiplier(6.0f);
    pTempPointLightComp->setColor(Color((byte)238, 132, 115, 255));
    pTempPointLightComp->setAttenuation(0, 11.9f);

    pTempPointLightComp = NEW game::PointLightComponent(); //whiteblue side
    m_pScene->addComponent(pTempPointLightComp);
    pTempPointLightComp->setOffset(vec3(-18.697f, 5.963f, -63.635f));
    pTempPointLightComp->setMultiplier(4.0f);
    pTempPointLightComp->setColor(Color((byte)210, 223, 252, 255));
    pTempPointLightComp->setAttenuation(0, 25.798f);

    Random rand;
    for (int i(0); i < 5; ++i) //back
    {
        pTempPointLightComp = NEW game::PointLightComponent(); 
        m_pScene->addComponent(pTempPointLightComp);
        pTempPointLightComp->setOffset(vec3(65.492f, 5.963f, -63.11f + 29.139f*i));
        pTempPointLightComp->setMultiplier(4.0f);
        pTempPointLightComp->setColor(Color((byte)246, 240, 194, 255));
        pTempPointLightComp->setAttenuation(0, 18.57f);
        if (rand.nextInt(0, 3) == 0)
            pTempPointLightComp->setBroken(true);
    }
    for (int i(0); i < 2; ++i) //back side
    {
        pTempPointLightComp = NEW game::PointLightComponent(); 
        m_pScene->addComponent(pTempPointLightComp);
        pTempPointLightComp->setOffset(vec3(41.317f - 24.175f * i, 5.963f, 53.443f));
        pTempPointLightComp->setMultiplier(4.0f);
        pTempPointLightComp->setColor(Color((byte)246, 240, 194, 255));
        pTempPointLightComp->setAttenuation(0, 18.57f);
        if (rand.nextInt(0, 3) == 0)
            pTempPointLightComp->setBroken(true);
    }
    #pragma endregion
            
    m_TestImage = CONTENT->asyncLoadTexture("v8_vantage_color.png");

    m_pFont = CONTENT->loadFont("MODES.ttf", 12);

    m_pTestButton = NEW gui::Button(gui::Button::TYPE_NORMAL, vec2(1200, 600), vec2(60,20));
    m_pTestButton->setText("Play 2D", 12);
    m_pTestButton2 = NEW gui::Button(gui::Button::TYPE_NORMAL, vec2(1200,630), vec2(60,20));
    m_pTestButton2->setText("Stop", 12);
    m_pTestButton3 = NEW gui::Button(gui::Button::TYPE_NORMAL, vec2(1200, 570), vec2(60,20));
    m_pTestButton3->setText("Play 3D", 12);

    m_pTextBox = NEW gui::TextBox(RectF(50,650,200,20), "testing", 10);

    m_pTestSound2D = AUDIO->loadSound2D("../data/audio/stuff.wav", true);
    m_pTestSound2D->setLooping(true);
    m_pTestSound2D->setPitch(0.9f);

    m_pTestSound3D = AUDIO->loadSound3D("../data/audio/goodkat_dnb.wav", false);
    m_pTestSound3D->setLooping(true);
    m_pTestSound3D->setMaximumDistance(50.0f);
    m_pTestSound3D->setMinimumDistance(10.0f);
    //m_pTestSound2D->play();

    m_pTestButton->addOnClickListener([&](){m_pTestSound2D->play(true);});
    m_pTestButton2->addOnClickListener([&](){m_pTestSound2D->stop();m_pTestSound3D->stop();});
    m_pTestButton3->addOnClickListener([&](){m_pTestSound3D->play(true);});

    m_pTestObject = NEW TestObject();

    m_pTestGrid = NEW he::tools::Grid(he::vec3(0,0,0), 100, 1.0f);
    m_pTestGrid->setColor(Color(0.6f,0.6f,0.6f), Color(1.0f,1.0f,1.0f));

    GRAPHICS->initPicking();

    //////////////////////////////////////////////////////////////////////////
    ///                            CAMERA'S                                ///
    //////////////////////////////////////////////////////////////////////////
    FlyCamera* pFlyCamera = NEW FlyCamera(GRAPHICS->getScreenWidth(), GRAPHICS->getScreenHeight());
    pFlyCamera->lookAt(vec3(-5, 5, -4), vec3(0, 0, 0), vec3(0, 1, 0));
    pFlyCamera->setLens(16.0f/9.0f,piOverFour,1.0f,400.0f);
    CAMERAMANAGER->addCamera("fly", pFlyCamera);

    FollowCamera* pFollowCamera = NEW FollowCamera();
    pFollowCamera->setLens(16.0f/9.0f,piOverFour,10.0f, 200.0f);
    pFollowCamera->setFollowObject(m_pTestObject);
    pFollowCamera->setLocalLook(normalize(vec3(0, 0.58f, -1.0f)));
    pFollowCamera->setDistance(15);
    CAMERAMANAGER->addCamera("car", pFollowCamera);

    CAMERAMANAGER->setActiveCamera("fly");

    CONSOLE->registerCmd([](){ CAMERAMANAGER->setActiveCamera("fly"); }, "fly");
    CONSOLE->registerCmd([](){ CAMERAMANAGER->setActiveCamera("car"); }, "car");


    CONSOLE->registerCmd(boost::bind(&MainGame::crazyStuff, this), "c_crazy");

    //////////////////////////////////////////////////////////////////////////
    ///   Fx
    uint effectTestTL = FX->createTimeline();
    gfx::FxTimeLine* pTL(FX->getTimeline(effectTestTL));
    //pTL->setParent(m_pTestObject);
    pTL->setEndTime(80.0f);
    gfx::FxTimeLineTrack* pTrack(pTL->getTrack(pTL->addTrack()));
    gfx::FxParticleSystem* pEffect(pTrack->getComponent<gfx::FxParticleSystem>(pTrack->addComponent(gfx::FxType_ParticleSystem)));
    pEffect->setMaxParticles(500);
    pEffect->setStartTime(0.0f);
    pEffect->setEndTime(60.0f);
    pEffect->setMaterial(CONTENT->loadMaterial("particles/particles.material"));

    pEffect->setSpawnRate(gfx::IFxVariable<float>::pointer(NEW gfx::FxConstant<float>(100)));

    /*gfx::FxParticleSpeed* pPartSpeed(pEffect->getInitComponent<gfx::FxParticleSpeed>(pEffect->addInitComponent(gfx::PICT_Speed)));
    gfx::FxRandomVec3::pointer pRandSpeed(NEW gfx::FxRandomVec3());
    pRandSpeed->setMin(vec3(-5,  1, -5));
    pRandSpeed->setMax(vec3(5, 25, 5));
    pPartSpeed->setValue(pRandSpeed);*/

    gfx::FxParticleRotation* pPartRot(pEffect->getInitComponent<gfx::FxParticleRotation>(pEffect->addInitComponent(gfx::PICT_Rotation)));
    gfx::FxRandomFloat::pointer pRot(NEW gfx::FxRandomFloat());
    pRot->setMin(0);
    pRot->setMax(twoPi);
    pPartRot->setValue(pRot);

    gfx::FxParticleRotationRate* pPartRotRate(pEffect->getModifyComponent<gfx::FxParticleRotationRate>(pEffect->addModifyComponent(gfx::PMCT_RotationRate)));
    gfx::FxConstant<float>::pointer pRotRate(NEW gfx::FxConstant<float>(piOverTwo));
    pPartRotRate->setValue(pRotRate);

    
    gfx::FxParticleForce* pPartGrav(pEffect->getModifyComponent<gfx::FxParticleForce>(pEffect->addModifyComponent(gfx::PMCT_Force)));
    gfx::FxConstant<vec3>::pointer pGrav(NEW gfx::FxConstant<vec3>(vec3(0, 9.81f, 0)));
    pPartGrav->setValue(pGrav);
    
    /*gfx::FxParticleScale* pPartScale(pEffect->getModifyComponent<gfx::FxParticleScale>(pEffect->addModifyComponent(gfx::PMCT_Scale)));
    gfx::FxCurve<vec3>::pointer pScale(NEW gfx::FxCurve<vec3>());
    pScale->addPoint(0.0f, vec3(0, 0, 0));
    pScale->addPoint(0.3f, vec3(2, 2, 2));
    pScale->addPoint(0.7f, vec3(2, 2, 2));
    pScale->addPoint(1.0f, vec3(1, 1, 1));
    pPartScale->setValue(pScale);*/

    gfx::FxParticleColor* pPartColor(pEffect->getModifyComponent<gfx::FxParticleColor>(pEffect->addModifyComponent(gfx::PMCT_Color)));
    gfx::FxCurve<vec4>::pointer pColor(NEW gfx::FxCurve<vec4>());
    pColor->addPoint(0.0f, vec4(0.0f, 0.0f, 0.0f, 0.0f));
    pColor->addPoint(0.2f, vec4(10.0f, 5.0f, 1.0f, 1.0f));
    pColor->addPoint(0.4f, vec4(0.1f, 1.0f, 0.1f, 1.0f));
    pColor->addPoint(0.6f, vec4(1.0f, 1.0f, 10.0f, 1.0f));
    pColor->addPoint(0.8f, vec4(0.1f, 1.0f, 0.1f, 1.0f));
    pColor->addPoint(1.0f, vec4(0.0f, 0.0f, 0.0f, 0.0f));
    pPartColor->setValue(pColor);

    //pTL->start();
    m_pFxEditorBinding->init();
}

void MainGame::crazyStuff()
{
    CONSOLE->addMessage("->this shit works, helloooo", he::CMSG_TYPE_ENGINE);
}

void MainGame::tick(float dTime)
{
    using namespace he;

    PROFILER_BEGIN("MainGame::tick");

    if (CONTROLS->getKeyboard()->isKeyPressed(he::io::Key_Escape))
        HAPPYENGINE->quit();

    m_pTestSound3D->setPosition(m_pTestObject->getWorldMatrix().getTranslation());

    AUDIO->setListenerPos(CAMERAMANAGER->getActiveCamera()->getPosition());
    AUDIO->setListenerOrientation(CAMERAMANAGER->getActiveCamera()->getLook(), CAMERAMANAGER->getActiveCamera()->getUp());
        
    CONTROLS->getFocus(this);
    if (CONTROLS->getKeyboard()->isKeyDown(he::io::Key_Space) && CONTROLS->hasFocus(this))
    {
        game::Entity* pBullet(NEW game::Entity());
        
        pBullet->setWorldMatrix(mat44::createTranslation(CAMERAMANAGER->getActiveCamera()->getPosition()));

        game::DynamicPhysicsComponent* pPhysicsComponent(NEW game::DynamicPhysicsComponent());
        pBullet->addComponent(pPhysicsComponent);
        px::PhysicsBoxShape boxShape(vec3(2, 2, 2));
        pPhysicsComponent->addShape(&boxShape, PHYSICS->getDriveableMaterial(DM_Metal), 80);
        pPhysicsComponent->getDynamicActor()->setVelocity(CAMERAMANAGER->getActiveCamera()->getLook() * 20);

        game::InstancedModelComponent* pBulletModelComp(NEW game::InstancedModelComponent());
        pBulletModelComp->setController("bullet");
        pBullet->addComponent(pBulletModelComp);


        m_Bullets.push_back(pBullet);
        std::cout << m_Bullets.size() << "\n";
    }
    CONTROLS->returnFocus(this);

    he::game::Game::tick(dTime); //tick all components

    //m_pSpotLight->setPosition(m_pCamera->getPosition());
    //m_pSpotLight->setDirection(-he::normalize(m_pCamera->getLook()));

    m_pTestButton->tick();
    m_pTestButton2->tick();
    m_pTestButton3->tick();

    //m_pTextBox->tick();

    if (CONTROLS->getMouse()->isButtonPressed(he::io::MouseButton_Left))
    {
        uint id(GRAPHICS->pick(CONTROLS->getMouse()->getPosition()));

        /*if (id != UINT_MAX)
        {
            m_PickPos = GRAPHICS->getDrawList()[id]->getWorldMatrix().getTranslation();
        }
        else
        {*/
            m_PickPos = vec3((float)id, (float)id, (float)id);
        /*}*/
    }

    m_pFPSGraph->tick(dTime, 0.5f);

    PROFILER_END();
}
void MainGame::drawGui()
{
    using namespace he;
    using namespace gfx;

    PROFILER_BEGIN("MainGame::drawGui");

    // 2D test stuff
    m_pTestButton->draw();
    m_pTestButton2->draw();
    m_pTestButton3->draw();
        
    /*m_pTextBox->draw();

    if (m_bTest2)
    {
        GUI->setColor(1.0f,0.5f,0.0f);
        GUI->setFontVerticalAlignment(Font::VAlignment_Center);
        GUI->setFontHorizontalAlignment(Font::HAlignment_Center);

        GUI->drawString(m_Test3, m_pFont, RectF(0,0,(float)GRAPHICS->getScreenWidth(),(float)GRAPHICS->getScreenHeight()));
    }*/

    /*GUI->setColor(1.0f,1.0f,1.0f);

    std::stringstream stream;
    stream << "2D: " << m_pTestSound2D->getPlayTime() << " / " << m_pTestSound2D->getLength();

    GUI->drawText(gui::Text(stream.str()), vec2(1050,570));

    stream.str("");

    stream << "3D: " << m_pTestSound3D->getPlayTime() << " / " << m_pTestSound3D->getLength();

    GUI->drawText(gui::Text(stream.str()), vec2(1050,590));*/

    
    //stream.str("");
    /*std::stringstream stream;
    stream << "pos: " << m_PickPos.x << " " << m_PickPos.y << " " << m_PickPos.z;

    GUI->drawText(gui::Text(stream.str()), vec2(1050,610));*/

    

    //if (m_Poly.getPolygon().getVertexCount() > 2)
    //{
    //    GUI->setAntiAliasing(true);
    //    GUI->setColor(1.0f,0.0f,1.0f);
    //    GUI->fillShape2D(m_Poly);
    //    /*GUI->setColor(1.0f,1.0f,1.0f);
    //    GUI->drawShape2D(m_Poly);*/
    //}

    //GUI->setAntiAliasing(true);
    //GUI->setColor(1.0f,0.0f,1.0f);
    //GUI->fillShape2D(gui::RoundedRectangle2D(vec2(500,500), vec2(250,100), 50));

    m_pFPSGraph->draw();

    /* DRAW 3D & 2D */

    //GL::reset();
 //   GRAPHICS->pick(vec2(0, 0));
    //GRAPHICS->clearAll();
    
    // TODO: implement into drawmanager/GRAPHICS
    //HE3DX->begin(CAMERAMANAGER->getActiveCamera());
    //m_pTestGrid->draw();
    ////HE3DX->drawBillboard(m_TestImage, vec3(0,5.0f,0));
    //HE3DX->end();
    PROFILER_END();
}

} //end namespace