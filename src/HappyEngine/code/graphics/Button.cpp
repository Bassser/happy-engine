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
//Created: 01/10/2011
#include "HappyPCH.h" 

#include "Button.h"
#include "ResourceFactory.h"
#include "Texture2D.h"
#include "ContentManager.h"
#include "Rect.h"
#include "Text.h"
#include "ControlsManager.h"
#include "Renderer2D.h"
#include "IMouse.h"
#include "Font.h"
#include "Canvas2D.h"
#include "GraphicsEngine.h"

namespace he {
namespace gui {

/* CONSTRUCTOR - DESTRUCTOR */
Button::Button() :	m_Type(TYPE_NORMAL),
                    m_Pos(0, 0),
                    m_Size(128, 64),
                    m_State(STATE_NORMAL),
                    m_ActivationType(ACTIVATION_MOUSE),
                    m_bClicked(false),
                    m_pSpriteSheet(nullptr),
                    m_Hitregion(vec2(0, 0), vec2(128, 64))
{
    m_Text.setHorizontalAlignment(gui::Text::HAlignment_Center);
    m_Text.setVerticalAlignment(gui::Text::VAlignment_Center);
    m_Text.setBounds(m_Size);
    setColors(
        Color(static_cast<uint8>(220), 220, 220),
        Color(static_cast<uint8>(180), 200, 220),
        Color(static_cast<uint8>(146), 162, 179),
        Color(static_cast<uint8>(150), 150, 150),
        Color(static_cast<uint8>(140), 140, 140));
}

Button::~Button()
{
    if (m_pSpriteSheet != nullptr)
        m_pSpriteSheet->release();
    std::for_each(m_Sprites.cbegin(), m_Sprites.cend(), [](const gfx::Texture2D* tex2D)
    {
        tex2D->release();
    });
}

/* GENERAL */
void Button::tick()
{
    if (m_State != STATE_DISABLED)
    {
        m_bClicked = false;

        if (m_ActivationType == ACTIVATION_MOUSE)
        {
            io::IMouse* mouse(CONTROLS->getMouse(GRAPHICS->getActiveWindow()));
            if (m_Hitregion.hitTest(mouse->getPosition()))
            {
                if (mouse->isButtonDown(io::MouseButton_Left))
                    m_State = STATE_DOWN;
                else
                    m_State = STATE_HOVER;

                if (mouse->isButtonReleased(io::MouseButton_Left))
                {
                    m_bClicked = true;

                    clicked();
                }
            }
            else
            {
                m_State = STATE_NORMAL;
            }
        }
    }
}

void Button::draw2D(gui::Canvas2D* renderer)
{
    switch (m_Type)
    {
        case TYPE_NORMAL:
        {
            drawColor(renderer);

            break;
        }

        case TYPE_SPRITE:
        {
            drawSprites(renderer);

            break;
        }
    }
}

/* SETTERS */
void Button::setSpriteSheet(const ObjectHandle& spriteSheet)
{
    ResourceFactory<gfx::Texture2D>::getInstance()->instantiate(spriteSheet);
    m_pSpriteSheet = ResourceFactory<gfx::Texture2D>::getInstance()->get(spriteSheet);
}

void Button::setSprites(	const ObjectHandle& normalSprite,
                            const ObjectHandle& hoverSprite,
                            const ObjectHandle& downSprite,
                            const ObjectHandle& disabledSprite)
{
    ResourceFactory<gfx::Texture2D>::getInstance()->instantiate(normalSprite);
    ResourceFactory<gfx::Texture2D>::getInstance()->instantiate(hoverSprite);
    ResourceFactory<gfx::Texture2D>::getInstance()->instantiate(downSprite);
    ResourceFactory<gfx::Texture2D>::getInstance()->instantiate(disabledSprite);

    m_Sprites.add(ResourceFactory<gfx::Texture2D>::getInstance()->get(normalSprite));
    m_Sprites.add(ResourceFactory<gfx::Texture2D>::getInstance()->get(hoverSprite));
    m_Sprites.add(ResourceFactory<gfx::Texture2D>::getInstance()->get(downSprite));
    m_Sprites.add(ResourceFactory<gfx::Texture2D>::getInstance()->get(disabledSprite));
}

void Button::setState(STATE state)
{
    m_State = state;
}

void Button::setActivationMode(ACTIVATION activationMode)
{
    m_ActivationType = activationMode;
}

void Button::setPosition(const vec2& centerPos)
{
    m_Pos = centerPos;
    m_Hitregion.setPosition(m_Pos);
}

void Button::setText(const he::String& text, uint16 fontSize)
{
    m_Text.clear();
    m_Text.addText(text.c_str());

    if (m_Text.getFont() == nullptr || m_Text.getFont()->getPixelHeight() != fontSize)
    {
        gui::Font* font(CONTENT->getDefaultFont(fontSize));
        m_Text.setFont(font);
        font->release();
    }
}

void Button::setSize( const vec2& size )
{
    m_Size = size;
    m_Text.setBounds(m_Size);
    m_Hitregion.setSize(m_Size);
}

void Button::setType( const TYPE& type )
{
    m_Type = type;
}

/* GETTERS */
bool Button::isNormal() const
{
    if (m_State == STATE_NORMAL) 
        return true;

    return false;
}

bool Button::isHover() const
{
    if (m_State == STATE_HOVER) 
        return true;

    return false;
}

bool Button::isDown() const
{
    if (m_State == STATE_DOWN) 
        return true;

    return false;
}

bool Button::isDisabled() const
{
    if (m_State == STATE_DISABLED) 
        return true;

    return false;
}

bool Button::isClicked() const
{
    return m_bClicked;
}

/* EXTRA */
void Button::drawColor(gui::Canvas2D* /*renderer*/)
{
    /*
    switch (m_State)
    {
        case STATE_NORMAL:
        {
            renderer->setFillColor(m_Colors[0]);
            break;
        }

        case STATE_HOVER:
        {
            renderer->setFillColor(m_Colors[1]);		
            break;
        }

        case STATE_DOWN:
        {
            renderer->setFillColor(m_Colors[2]);
            break;
        }

        case STATE_DISABLED:
        {
            renderer->setFillColor(m_Colors[3]);
            break;
        }
    }

    renderer->fillRect(m_Pos - m_Size / 2, m_Size);

    renderer->setBlendStyle(gfx::BlendStyle_Add);
    renderer->setFillColor(Color(0.1f, 0.1f, 0.1f));
    renderer->fillRect(m_Pos - m_Size / 2.0f, vec2(m_Size.x, m_Size.y / 2));
    renderer->setBlendStyle(gfx::BlendStyle_Alpha);

    renderer->setStrokeColor(m_Colors[4]);
    renderer->strokeRect(m_Pos - m_Size / 2, m_Size);

    renderer->setFillColor(Color(0.25f, 0.25f, 0.25f));
    renderer->fillText(m_Text, m_Pos - m_Size / 2.0f);
    */
}

void Button::drawSprites(gui::Canvas2D* /*renderer*/)
{
    LOG(LogType_ProgrammerAssert, "Not Implemented!");
}

void Button::drawSpriteSheet()
{
    LOG(LogType_ProgrammerAssert, "Not Implemented!");
}

/* CALLBACK HANDLERS */
void Button::clicked()
{
    OnClick();
}

void Button::setColors( const Color& normal, const Color& hoover, const Color& down, const Color& disabled, const Color& border )
{
    m_Colors[0] = normal;
    m_Colors[1] = hoover;
    m_Colors[2] = down;
    m_Colors[3] = disabled;
    m_Colors[4] = border;
}


} } //end namespace