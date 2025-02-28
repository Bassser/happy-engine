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
//Created: 30/08/2011
#include "HappyPCH.h" 

#include "FPSGraph.h"
#include "ContentManager.h"
#include "Console.h"
#include "Renderer2D.h"
#include "GraphicsEngine.h"
#include "Renderer2D.h"
#include "Text.h"
#include "Canvas2D.h"
#include "View.h"
#include "Font.h"
#include "Sprite.h"
#include "Gui.h"
#include "SystemStats.h"

#define XMARGE 5
#define YMARGE 5
#define WIDTH 100
#define HEIGHT 82
#define GRAPHLOWERBOUND 45

namespace he {
namespace tools {

/* CONSTRUCTOR - DESTRUCTOR */
FPSGraph::FPSGraph(float visualScale, float interval, uint16 recordTime) :
                        m_GameTime(0.0f),
                        m_TBase(0.0f),
                        m_CurrentDTime(0.0f),
                        m_CurrentFPS(0),
                        m_Interval(interval),
                        m_Font(CONTENT->loadFont("Ubuntu-Medium.ttf", static_cast<uint16>(6 * m_VisualScale), gui::Font::NO_COMPRESSION)),
                        m_FPSGraphState(Type_TextOnly),
                        m_Pos(5.0f, 5.0f),
                        m_FpsHistory(300),
                        m_ActiveSprite(0),
                        m_AcumulatedDTime(0.f),
                        m_Ticks(0),
                        m_RecordTime(recordTime),
                        m_CurrentScale(4.0f),
                        m_CurrentCPU(0.0f),
                        m_ColorWhite(Color(1.0f,1.0f,1.0f)),
                        m_ColorWhiteAlpha(Color(1.0f,1.0f,1.0f,0.6f)),
                        m_ColorYellow(Color((uint8)228,(uint8)211,(uint8)93)),
                        m_ColorYellowAlpha(Color((uint8)228,(uint8)211,(uint8)93,(uint8)100)),
                        m_ColorBlue(Color((uint8)94,(uint8)195,(uint8)247)),
                        m_ColorBlueAlpha(Color((uint8)94,(uint8)195,(uint8)247,(uint8)100)),
                        m_ColorGrey(Color((uint8)50,(uint8)47,(uint8)54)),
                        m_ColorDarkGrey(Color((uint8)30,(uint8)27,(uint8)34)),
                        m_VisualScale(visualScale)
{
    m_Text.setFont(m_Font);
    m_Text.setBounds(vec2(m_VisualScale * 100, m_VisualScale * 20));

    CONSOLE->registerVar(&m_FPSGraphState, "s_fps_graph");

    gui::SpriteCreator* const cr(GUI->getSpriteCreator());
    m_Sprites[0] = cr->createSprite(vec2(visualScale * (WIDTH + XMARGE * 2.0f), visualScale * HEIGHT), gui::Sprite::DYNAMIC_DRAW | gui::Sprite::UNIFORM_SCALE);
    m_Sprites[1] = cr->createSprite(vec2(visualScale * (WIDTH + XMARGE * 2.0f), visualScale * HEIGHT), gui::Sprite::DYNAMIC_DRAW | gui::Sprite::UNIFORM_SCALE);
}

FPSGraph::~FPSGraph()
{
    m_Font->release();
    
    gui::SpriteCreator* const cr(GUI->getSpriteCreator());
    cr->removeSprite(m_Sprites[0]);
    cr->removeSprite(m_Sprites[1]);
}

/* GENERAL */
void FPSGraph::tick(float dTime)
{
    m_GameTime += dTime;

    m_AcumulatedDTime += dTime;
    ++m_Ticks;

    if ((m_GameTime - m_TBase) >= m_Interval)
    {
        m_TBase = m_GameTime;

        m_CurrentDTime = m_AcumulatedDTime / m_Ticks;

        uint16 fps(cap(1 / (m_CurrentDTime)));

        m_AcumulatedDTime = 0.f;
        m_Ticks = 0;

        m_CurrentFPS = fps;
        m_FpsHistory.add(fps);

        if (m_FPSGraphState == Type_Full)
        {
            renderGraph();
        }

        // limit recording time to save memory
        if (m_FpsHistory.size() * m_Interval > m_RecordTime)
        {
            m_FpsHistory.orderedRemoveAt(0);
        }

        m_CurrentCPU = he::tools::SystemStats::getInstance()->getCpuUsage();
    }
}

void FPSGraph::draw2D(gui::Canvas2D* canvas)
{
    if (m_GameTime <= m_Interval)
        return;

    switch (m_FPSGraphState)
    {
        case Type_Hide:
        {
            break;
        }
        case Type_ToConsole:
        {
            drawToConsole(canvas);
            break;
        }
        case Type_TextOnly:
        {
            drawTextOnly(canvas);
            break;
        }
        case Type_Full:
        {
            drawFull(canvas);
            break;
        }
    }
}

inline uint16 FPSGraph::cap(const float& fps) const
{
    return static_cast<uint16>(fps);
}

inline uint16 FPSGraph::cap(const uint32& fps) const
{
    return static_cast<uint16>(fps);
}

void FPSGraph::drawToConsole(gui::Canvas2D* /*canvas*/)
{
    if ((m_GameTime - m_TBase) < FLT_EPSILON)
    {
        HE_INFO("Fps: %d", (int)m_CurrentFPS);
    }
}

void FPSGraph::drawTextOnly(gui::Canvas2D* canvas)
{
    m_Text.clear();
    m_Text.addTextExt("%u (%u) FPS", m_CurrentFPS, getAverageFPS());
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    canvas->setColor(m_ColorYellow);
    canvas->fillText(m_Text, m_Pos);

    m_Text.clear();
    m_Text.addTextExt("%.3f MS", m_CurrentDTime * 1000.0f);
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);
    
    canvas->setColor(m_ColorBlue);
    canvas->fillText(m_Text, m_Pos);

    m_Text.clear();
    m_Text.addTextExt("MEM");
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    canvas->setColor(m_ColorWhite);
    canvas->fillText(m_Text, m_Pos + vec2(0,11));
    
    he::tools::SystemStats* const stats(he::tools::SystemStats::getInstance());
    m_Text.clear();
    m_Text.addTextExt("%u - %u (%llu) ~%llu",
        (uint32)(stats->getVirtualMemoryUsed() / (1024 * 1024)),
        (uint32)(stats->getMemoryUsed() / (1024 * 1024)),
        (uint64)(stats->getTotalMemory() / (1024 * 1024)),
        he::gMemMan->getTrackedMemory() / (1024 * 1024));
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);

    canvas->fillText(m_Text, m_Pos + vec2(0,11));

    m_Text.clear();
    m_Text.addTextExt("CPU");
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    canvas->fillText(m_Text, m_Pos + vec2(0,22));

    m_Text.clear();
    m_Text.addTextExt("%.2f", m_CurrentCPU);
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);

    canvas->fillText(m_Text, m_Pos + vec2(0,22));
}

void FPSGraph::drawFull(gui::Canvas2D* canvas)
{
    if (m_FpsHistory.size() == 0)
        return;
    canvas->setColor(Color(1, 1, 1, 0.9f));
    canvas->drawSprite(m_Sprites[m_ActiveSprite], m_Pos);
}

void FPSGraph::updateScale(uint16 currentMaxFpsInFrame)
{
    // cap
    currentMaxFpsInFrame -= (currentMaxFpsInFrame % 20);
    currentMaxFpsInFrame += 20;

    m_CurrentScale = currentMaxFpsInFrame / 20.0f;
}

void FPSGraph::renderGraph()
{
    gui::SpriteCreator* cr(GUI->getSpriteCreator());


    const float realWidth(WIDTH * m_VisualScale);
    const float realXMarge(XMARGE);
    const float realYMarge(YMARGE);
    const float graphPointPadding(2 * m_VisualScale);
    const float realLowerBound(GRAPHLOWERBOUND * m_VisualScale);

    cr->setActiveSprite(m_Sprites[m_ActiveSprite]);

    cr->setLineJoin(gui::LINE_JOIN_ROUND);

    cr->newPath();

    cr->rectangle(vec2(realXMarge,realXMarge),vec2(realWidth , realLowerBound - realYMarge));
    cr->setColor(m_ColorGrey);
    cr->fill();

    PrimitiveList<vec2> poly0(60);
    PrimitiveList<vec2> poly1(60);

    uint32 i(0);
                
    if (m_FpsHistory.size() > 50)
    {
        i = m_FpsHistory.size() - 51;
    }

    uint32 j(m_FpsHistory.size());

    uint8 k(0);
    uint16 currentFPS(0);

    cr->newPath();

    uint16 maxFpsInGraph(0);

    for (; i < j ; ++i)
    {
        currentFPS = m_FpsHistory[i];

        if (currentFPS > maxFpsInGraph)
            maxFpsInGraph = currentFPS;

        if (k == 0)
        {
            poly0.add(vec2( (realWidth + realXMarge * 2.0f) - (k * graphPointPadding), realLowerBound - (currentFPS / (m_CurrentScale / 2.0f))));
        }

        poly0.add(vec2((realWidth + realXMarge) - (k * graphPointPadding), realLowerBound - (currentFPS / (m_CurrentScale / 2.0f))));

        ++k;
    }

    if (k < 50)
    {
        poly0.add(vec2((realWidth + realXMarge) - ((k - 1) * graphPointPadding), realLowerBound + realYMarge));
    }
    else
    {
        poly0.add(vec2(0.0f, realLowerBound - (currentFPS / (m_CurrentScale / 2.0f))));
        poly0.add(vec2(0.0f, realLowerBound + realYMarge));
    }

    poly0.add(vec2((realWidth + realXMarge * 2.0f), realLowerBound + realYMarge));

    if (m_FpsHistory.size() > 50)
    {
        i = m_FpsHistory.size() - 51;
    }
    else
    {
        i = 0;
    }

    k = 0;
    uint16 currentDTime(0);

    for (; i < j ; ++i)
    {
        currentDTime = static_cast<uint16>((1.0f / m_FpsHistory[i]) * 1000.0f);

        if (currentDTime > maxFpsInGraph)
            maxFpsInGraph = currentDTime;

        if (k == 0)
        {
            poly1.add(vec2((realWidth + realXMarge * 2.0f) - (k * graphPointPadding), realLowerBound - (currentDTime / (m_CurrentScale / 2.0f))));
        }

        poly1.add(vec2((realWidth + realXMarge) - (k * graphPointPadding), realLowerBound - (currentDTime / (m_CurrentScale / 2.0f))));

        ++k;
    }

    if (k < 50)
    {
        poly1.add(vec2((realWidth + realXMarge) - ((k - 1) * graphPointPadding), realLowerBound + realYMarge));
    }
    else
    {
        poly1.add(vec2(0.0f, realLowerBound - (currentDTime / (m_CurrentScale / 2.0f))));
        poly1.add(vec2(0.0f, realLowerBound + realYMarge));
    }

    poly1.add(vec2((realWidth + realXMarge * 2.0f), realLowerBound + realYMarge));

    updateScale(maxFpsInGraph);

    cr->setLineWidth(1.5f);
    cr->newPath();

    if (m_CurrentFPS >= static_cast<uint16>(m_CurrentDTime * 1000.0f))
    {
        i = 0;

        poly0.forEach([&](vec2 p)
        {
            if (i == 0)
            {
                cr->moveTo(p);

                ++i;
            }
            else
            {
                cr->lineTo(p);
            }
        });

        cr->setColor(m_ColorYellowAlpha);
        cr->fill();

        cr->setColor(m_ColorYellow);//,0.6f));
        cr->stroke();

        cr->newPath();
        i = 0;

        poly1.forEach([&](vec2 p)
        {
            if (i == 0)
            {
                cr->moveTo(p);

                ++i;
            }
            else
            {
                cr->lineTo(p);
            }
        });

        cr->setColor(m_ColorBlueAlpha);
        cr->fill();

        cr->setColor(m_ColorBlue);
        cr->stroke();
    }
    else
    {
        i = 0;

        poly1.forEach([&](vec2 p)
        {
            if (i == 0)
            {
                cr->moveTo(p);

                ++i;
            }
            else
            {
                cr->lineTo(p);
            }
        });

        cr->setColor(m_ColorBlueAlpha);
        cr->fill();

        cr->setColor(m_ColorBlue);
        cr->stroke();

        i = 0;
        cr->newPath();

        poly0.forEach([&](vec2 p)
        {
            if (i == 0)
            {
                cr->moveTo(p);

                ++i;
            }
            else
            {
                cr->lineTo(p);
            }
        });

        cr->setColor(m_ColorYellowAlpha);
        cr->fill();

        cr->setColor(m_ColorYellow);
        cr->stroke(); 
    }

    cr->setLineWidth(1.0f);

    cr->newPath();

    cr->rectangle(vec2(realXMarge,24 * m_VisualScale),vec2(realXMarge,1));
    cr->rectangle(vec2(realXMarge,44 * m_VisualScale),vec2(realXMarge,1));

    cr->setColor(m_ColorWhite);

    cr->fill();

    cr->newPath();

    cr->rectangle(vec2(0,0),vec2((realWidth + realXMarge * 2.0f),realYMarge));
    cr->rectangle(vec2(0,0),vec2(realXMarge,65 * m_VisualScale));
    cr->rectangle(vec2((realWidth + realXMarge),0),vec2(realXMarge,65 * m_VisualScale));
    cr->rectangle(vec2(0,realLowerBound),vec2((realWidth + realXMarge * 2.0f),realLowerBound + realYMarge));

    cr->setColor(m_ColorDarkGrey);
    cr->fill();

    cr->newPath();

    cr->rectangle(vec2(0,0),vec2((realWidth + realXMarge * 2.0f),82 * m_VisualScale));

    cr->setColor(Color((uint8)20,(uint8)20,(uint8)20));

    cr->stroke();

    m_Text.clear();
    m_Text.addTextExt("%u (%u) FPS", m_CurrentFPS, getAverageFPS());
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    cr->newPath();
    cr->setColor(m_ColorYellow);
    cr->text(m_Text, vec2(realXMarge,realLowerBound + realYMarge));
    cr->fill();

    m_Text.clear();
    m_Text.addTextExt("%.3f MS", m_CurrentDTime * 1000.0f);
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);
    
    cr->newPath();
    cr->setColor(m_ColorBlue);
    cr->text(m_Text, vec2(realXMarge,realLowerBound + realYMarge));
    cr->fill();

    cr->setColor(m_ColorWhiteAlpha);

    m_Text.clear();
    m_Text.addTextExt("%.0f",  m_CurrentScale * 20 * 0.75f);
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    cr->newPath();
    cr->text(m_Text, vec2(realXMarge,11 * m_VisualScale));
    cr->fill();

    m_Text.clear();
    m_Text.addTextExt("%.0f",  m_CurrentScale * 20 * 0.25f);

    cr->newPath();
    cr->text(m_Text, vec2(realXMarge,31 * m_VisualScale));
    cr->fill();

    m_Text.clear();
    m_Text.addTextExt("MEM");
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    cr->newPath();
    cr->setColor(m_ColorWhite);
    cr->text(m_Text, vec2(realXMarge,61 * m_VisualScale));
    cr->fill();

    he::tools::SystemStats* const stats(he::tools::SystemStats::getInstance());
    m_Text.clear();
    m_Text.addTextExt("%u - %u (%llu) (%llu)",
        (uint32)(stats->getVirtualMemoryUsed() / (1024 * 1024)),
        (uint32)(stats->getMemoryUsed() / (1024 * 1024)),
        (uint64)(stats->getTotalMemory() / (1024 * 1024)),
        (uint64)(he::gMemMan->getTrackedMemory() / (1024 * 1024)));
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);

    cr->newPath();
    cr->text(m_Text, vec2(realXMarge,61 * m_VisualScale));
    cr->fill();

    m_Text.clear();
    m_Text.addTextExt("CPU");
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Left);

    cr->newPath();
    cr->text(m_Text, vec2(realXMarge,72 * m_VisualScale));
    cr->fill();

    m_Text.clear();
    m_Text.addTextExt("%.2f", m_CurrentCPU);
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Right);

    cr->newPath();
    cr->text(m_Text, vec2(realXMarge,72 * m_VisualScale));
    cr->fill();

    cr->renderSpriteAsync();

    ++m_ActiveSprite;

    if (m_ActiveSprite > 1)
        m_ActiveSprite = 0;
}

/* GETTERS */
uint16 FPSGraph::getMaxFPS() const
{
    uint16 maxFPS(0);
    m_FpsHistory.forEach([&](uint16 FPS)
    {
        if (FPS > maxFPS)
            maxFPS = FPS;
    });

    return maxFPS;
}

uint16 FPSGraph::getMinFPS() const
{
    uint16 minFPS(0xff);
    m_FpsHistory.forEach([&](uint16 FPS)
    {
        if (FPS < minFPS)
            minFPS = FPS;
    });

    return minFPS;
}

uint16 FPSGraph::getAverageFPS() const
{
    if (m_FpsHistory.size() == 0)
        return 0;

    uint32 i(0);
    uint32 j((uint32)m_FpsHistory.size());

    uint32 avFPS(0);

    for (; i < j ; ++i)
    {
        avFPS += m_FpsHistory[i];
    }

    avFPS /= (uint32)m_FpsHistory.size();

    return cap(avFPS);
}

/* SETTERS */
void FPSGraph::setType(Type type)
{
    m_FPSGraphState = type;
}

void FPSGraph::setPos(const vec2& pos)
{
    m_Pos = pos;
}

} } //end namespace