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
//Created: // 18/11/2012

#include "HappyPCH.h" 

#include "Canvas2DRendererCairo.h"
#include "Canvas2DBuffer.h"
#include "Texture2D.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include "Sprite.h"
#include "Text.h"
#include "Font.h"

namespace
{
    float normalizeAngle(float a)
    {
        return (a * -1.0f);
    }
}

namespace he {
namespace gfx {

/* CONSTRUCTOR - DESTRUCTOR */
Canvas2DRendererCairo::Canvas2DRendererCairo() : m_HandleDrawCalls(true)
{
    m_DrawThread.startThread(std::bind(&Canvas2DRendererCairo::handleDrawCalls, this), "CairoDrawThread");
}

Canvas2DRendererCairo::~Canvas2DRendererCairo()
{
    // wait for thread to finish
    m_HandleDrawCalls = false;
    m_DrawThread.join();

    while (m_SpriteList.empty() == false)
    {
        SpriteData* data(m_SpriteList.back());
        HEDelete(data);
        m_SpriteList.pop();
    }

    while (m_DynamicSpriteList.empty() == false)
    {
        SpriteData* data(m_DynamicSpriteList.back());
        HEDelete(data);
        m_DynamicSpriteList.removeAt(m_DynamicSpriteList.size() - 1);
    }
}

/* GENERAL */
void Canvas2DRendererCairo::tick(float /*dTime*/)
{
}
void Canvas2DRendererCairo::glThreadInvoke()
{
    blit();
}

void Canvas2DRendererCairo::addNewSprite(he::gui::Sprite* sprite)
{
    finishSprite();

    sprite->setRendered(false);

    uint16 id(sprite->getID());

    vec2 size(sprite->getSize());
    int w(static_cast<int>(size.x));
    int h(static_cast<int>(size.y));
    
    const bool isDynamic((sprite->getFlags() & gui::Sprite::DYNAMIC_DRAW) == gui::Sprite::DYNAMIC_DRAW);
    bool foundInDynamicList(false);
    size_t index(UINT32_MAX);

    if (isDynamic)
    {
        if (m_DynamicSpriteList.find_if([&id](const SpriteData* spd) -> bool
        {
            if (spd->m_Id == id)
                return true;
            else
                return false;
        }, index))
        {
            foundInDynamicList = true;
        }
    }

    if (foundInDynamicList)
    {
        SpriteData* data(m_DynamicSpriteList[index]);

        HE_ASSERT((data->m_ReadyState & SpriteReadyForBlit), "Sprite is stil rendering, can't continue!");

        if (size != data->m_Size)
        {
            he_free(data->m_RenderBuffer);
            cairo_destroy(data->m_CairoPaint);
            cairo_surface_destroy(data->m_CairoSurface);
        
            unsigned char* rBuff(static_cast<unsigned char*>(he_malloc("Canvas2DRendererCairo::addNewSprite()::rBuff", 4 * w * h * sizeof(unsigned char))));
            he_memset(rBuff, 0, 4 * w * h * sizeof(unsigned char));

            cairo_surface_t* surf(
                cairo_image_surface_create_for_data(rBuff, CAIRO_FORMAT_ARGB32, w, h, 4 * w));

            cairo_t* cp(cairo_create(surf));

            data->m_RenderBuffer = rBuff;
            data->m_CairoPaint = cp;
            data->m_CairoSurface = surf;
            data->m_Size = size;
        }

        data->m_ReadyState = 0;
        data->m_ReadyState |= SpriteDynamic;

        m_SpriteListLock.lock(FILE_AND_LINE);
            m_SpriteList.push(data);
        m_SpriteListLock.unlock();

        clear();
    }
    else
    {

        unsigned char* rBuff(static_cast<unsigned char*>(he_malloc("Canvas2DRendererCairo::addNewSprite()::rBuff", 4 * w * h * sizeof(unsigned char))));
        he_memset(rBuff, 0, 4 * w * h * sizeof(unsigned char));

        cairo_surface_t* surf(
            cairo_image_surface_create_for_data(rBuff, CAIRO_FORMAT_ARGB32, w, h, 4 * w));

        cairo_t* cp(cairo_create(surf));

        SpriteData* data(HENew(SpriteData)(
                id, size,
                sprite->getRenderTexture(),
                rBuff, surf, cp));

        if (isDynamic)
        {
            data->m_ReadyState |= SpriteDynamic;
            m_DynamicSpriteList.add(data);
        }

        m_SpriteListLock.lock(FILE_AND_LINE);
            m_SpriteList.push(data);
        m_SpriteListLock.unlock();

        clear();
        transformY();
    }
}
void Canvas2DRendererCairo::finishSprite()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    if (m_SpriteList.empty() == false)
    {
        m_SpriteList.back()->m_ReadyState |= SpriteReadyForRender;
    }

    m_SpriteListLock.unlock();
}

/* SETTERS */
void Canvas2DRendererCairo::setLineWidth(float width)
{
    HE_ASSERT(width > 0, "Linewidth can't be smaller or equal to zero!");

    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        ::cairo_set_line_width,
        sData->m_CairoPaint,
        static_cast<double>(width)));

    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::setColor(const Color& col)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    if (col.a() == 1.0f)
    {
        sData->m_DrawCalls.push(
            std::bind(
            &cairo_set_source_rgb,
            sData->m_CairoPaint,
            static_cast<double>(col.r()),
            static_cast<double>(col.g()),
            static_cast<double>(col.b())));
    }
    else
    {
        sData->m_DrawCalls.push(
            std::bind(
            &cairo_set_source_rgba,
            sData->m_CairoPaint,
            static_cast<double>(col.r()),
            static_cast<double>(col.g()),
            static_cast<double>(col.b()),
            static_cast<double>(col.a())));
    }
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::setLineCap(gui::LINE_CAP cap)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_set_line_cap,
        sData->m_CairoPaint,
        static_cast<cairo_line_cap_t>(cap)));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::setLineJoin(gui::LINE_JOIN join)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_set_line_join,
        sData->m_CairoPaint,
        static_cast<cairo_line_join_t>(join)));
    m_SpriteListLock.unlock();
}

/* DRAW */
void Canvas2DRendererCairo::clear()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_save,
        sData->m_CairoPaint));

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_set_operator,
        sData->m_CairoPaint,
        CAIRO_OPERATOR_CLEAR));

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_paint,
        sData->m_CairoPaint));

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_restore,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::moveTo(const vec2& pos)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_move_to,
        sData->m_CairoPaint,
        static_cast<double>(pos.x),
        static_cast<double>(pos.y)));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::lineTo(const vec2& pos)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_line_to,
        sData->m_CairoPaint,
        static_cast<double>(pos.x),
        static_cast<double>(pos.y)));
    m_SpriteListLock.unlock();
}

void Canvas2DRendererCairo::rectangle(const vec2& pos, const vec2& size)
{
    HE_ASSERT(size.x > 0 && size.y > 0, "Size of rectangle can't be negative!");

    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_rectangle,
        sData->m_CairoPaint,
        static_cast<double>(pos.x),
        static_cast<double>(pos.y),
        static_cast<double>(size.x),
        static_cast<double>(size.y)));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::arc(const vec2& pos, float radius, float angleRadStart, float angleRadEnd)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");
    cairo_t* paintPtr(sData->m_CairoPaint);
    sData->m_DrawCalls.push([paintPtr, pos, radius, angleRadStart, angleRadEnd]()
        {
            ::cairo_arc(paintPtr, static_cast<double>(pos.x), static_cast<double>(pos.y),
                static_cast<double>(radius), static_cast<double>(normalizeAngle(angleRadStart)),
                static_cast<double>(normalizeAngle(angleRadEnd)));
        });
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::curveTo(const vec2& controlP1, const vec2& controlP2, const vec2& end)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    cairo_t* paintPtr(sData->m_CairoPaint);
    sData->m_DrawCalls.push([paintPtr, controlP1, controlP2, end]()
    {
        cairo_curve_to(paintPtr,
            static_cast<double>(controlP1.x), static_cast<double>(controlP1.y),
            static_cast<double>(controlP2.x), static_cast<double>(controlP2.y),
            static_cast<double>(end.x), static_cast<double>(end.y));
    });
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::newPath()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_new_path,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::closePath()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_close_path,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}

void Canvas2DRendererCairo::text(const gui::Text& text, const vec2& pos)
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &Canvas2DRendererCairo::_text,
        this,
        text,
        pos,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}
    
void Canvas2DRendererCairo::stroke()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_stroke_preserve,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::fill()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_fill_preserve,
        sData->m_CairoPaint));
    m_SpriteListLock.unlock();
}
void Canvas2DRendererCairo::clip()
{
}

/* INTERNAL */
void Canvas2DRendererCairo::blit()
{
    uint32 spriteToBlit(0);

    while (m_SpriteListBlit.size() > 0)
    {
        m_SpriteListBlitLock.lock(FILE_AND_LINE);
            // get data from queue
            SpriteData* data(m_SpriteListBlit.front());
            m_SpriteListBlit.pop();
        // make sure to stop blocking render thread
        m_SpriteListBlitLock.unlock();

        // blit
        data->m_Texture2D->setData(
                static_cast<uint32>(data->m_Size.x),
                static_cast<uint32>(data->m_Size.y),
                data->m_RenderBuffer,
                TextureBufferLayout_BGRA,
                TextureBufferType_Byte);

        if ((data->m_ReadyState & SpriteDynamic) == false)
        {
            // cleanup
            HEDelete(data);
        }

        --spriteToBlit;
    }
}

void Canvas2DRendererCairo::handleDrawCalls()
{
    const size_t waitTime(10);

    while (m_HandleDrawCalls)
    {
        if (m_SpriteList.size() > 0)
        {
            while (m_SpriteList.size() > 0 && m_HandleDrawCalls)
            {
                m_SpriteListLock.lock(FILE_AND_LINE);

                // check if needs to be rendered
                SpriteData* data(m_SpriteList.front());          
                bool renderSprite = (data->m_ReadyState & SpriteReadyForRender);
                if (renderSprite)
                {
                    m_SpriteList.pop();
                    m_SpriteListLock.unlock();

                    // execute all the drawcalls for the sprite
                    while (data->m_DrawCalls.empty() == false)
                    {
                        data->m_DrawCalls.front()();
                        data->m_DrawCalls.pop();
                    }

                    // set sprite ready for blitting
                    data->m_ReadyState |= SpriteReadyForBlit;

                    m_SpriteListBlitLock.lock(FILE_AND_LINE);
                        // push spritedata to blitting queue
                        m_SpriteListBlit.push(data);
                    m_SpriteListBlitLock.unlock();
            
                    // pop it off the regular queue
                }
                else
                {
                    m_SpriteListLock.unlock();
                }

                renderSprite = false;
            }
        }
        else
        {
            he::Thread::sleep(waitTime);
        }
    }
}

void Canvas2DRendererCairo::transformY()
{
    m_SpriteListLock.lock(FILE_AND_LINE);

    SpriteData* sData = m_SpriteList.back();

    HE_ASSERT((sData->m_ReadyState & SpriteReadyForRender) == false,
        "Sprite is already marked for rendering, can't draw!");

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_scale,
        sData->m_CairoPaint,
        1.0,
        -1.0));

    sData->m_DrawCalls.push(
        std::bind(
        &cairo_translate,
        sData->m_CairoPaint,
        0.0,
        static_cast<double>(-sData->m_Size.y)));

    m_SpriteListLock.unlock();
}

void Canvas2DRendererCairo::_text(const gui::Text& text, const vec2& pos, cairo_t* cairoPaint)
{
    vec2 linePos(0,0);
    const bool hasBounds(text.hasBounds());
    const gui::Font* font(text.getFont());
    cairo_font_face_t* cairoFont(font->getCairoFontFace());
    const uint32 lineSpacing(font->getLineSpacing());
    const gui::Text::HAlignment hAl(text.getHorizontalAlignment());
    const gui::Text::VAlignment vAl(text.getVerticalAlignment());
    const char* fullText(text.getText());
    const size_t size(text.getTextSize());
    const vec2 bounds(hasBounds ? text.getBounds() : vec2(0,0));
    uint16 lines(0);

    cairo_glyph_t* cairoGlyphs((cairo_glyph_t*)he_malloc("Canvas2DRendererCairo::_text()::cairoGlyphs", size * sizeof(cairo_glyph_t)));
    size_t numGlyphs(0);

    size_t lineCharStart(0);

    for (uint32 i(0); i < size; ++i)
    {
        const char currentChar(fullText[i]);

        if (currentChar == '\n')
        {
            const size_t lineSize(i - lineCharStart);
            if (lineSize > 0)
            {
                vec2 glyphPos;
                float hoffset(0.0f);
                switch (hAl)
                {
                    case gui::Text::HAlignment_Left:
                        break;
                    case gui::Text::HAlignment_Center:
                        hoffset = bounds.x - (font->getStringWidth(fullText + lineCharStart, lineSize) / 2.0f);
                        break;
                    case gui::Text::HAlignment_Right:
                        hoffset = (bounds.x / 2.0f) - font->getStringWidth(fullText + lineCharStart, lineSize);
                        break;
                }

                glyphPos.x += hoffset;

                for (uint32 i2(lineCharStart); i2 < lineCharStart + lineSize; ++i2)
                {
                    const char currentCharIL(fullText[i2]);

                    cairo_glyph_t& c(cairoGlyphs[i2]);

                    c.index = static_cast<unsigned long>(font->getGlyphIndex(currentCharIL));
                    c.x = static_cast<double>(glyphPos.x);
                    c.y = static_cast<double>(glyphPos.y);

                    glyphPos.x += font->getAdvance(currentCharIL);

                    ++numGlyphs;
                }
            }

            lineCharStart = i + 1;

            linePos.y += lineSpacing;
            linePos.x = 0;

            ++lines;
        } 
    }

    if (lineCharStart < size)
    {
        const size_t lineSize(size - lineCharStart);
        if (lineSize > 0)
        {
            vec2 glyphPos;
            float hoffset(0.0f);
            switch (hAl)
            {
                case gui::Text::HAlignment_Left:
                    break;
                case gui::Text::HAlignment_Center:
                    hoffset = (bounds.x / 2.0f) - (font->getStringWidth(fullText + lineCharStart, lineSize) / 2.0f);
                    break;
                case gui::Text::HAlignment_Right:
                    hoffset = bounds.x - font->getStringWidth(fullText + lineCharStart, lineSize);
                    break;
            }

            glyphPos.x += hoffset;

            for (uint32 i2(lineCharStart); i2 < lineCharStart + lineSize; ++i2)
            {
                const char currentCharIL(fullText[i2]);

                cairo_glyph_t& c(cairoGlyphs[i2]);

                c.index = static_cast<unsigned long>(font->getGlyphIndex(currentCharIL));
                c.x = static_cast<double>(glyphPos.x);
                c.y = static_cast<double>(glyphPos.y);

                glyphPos.x += font->getAdvance(currentCharIL);

                ++numGlyphs;
            }

            ++lines;
        }
    }

    vec2 offset(pos.x, pos.y);
    switch (vAl)
    {
        case gui::Text::VAlignment_Top:
            offset.y += font->getPixelHeight();
            break;
        case gui::Text::VAlignment_Center:
            offset.y -= (bounds.y / 2.0f) - ((lineSpacing * (lines - 1)) / 2.0f) - ((lines * font->getPixelHeight()) / 2.0f);
            break;
        case gui::Text::VAlignment_Bottom:
            offset.y -= bounds.y + (lineSpacing * lines);
            break;
    }

    cairo_translate(cairoPaint, static_cast<double>(offset.x), static_cast<double>(offset.y));

    cairo_set_font_face(cairoPaint, cairoFont);

    // TODO: font size in cairo is smaller, find out why!
    // doesn't work well with non integer values
    cairo_set_font_size(cairoPaint, static_cast<double>(ceil(font->getPixelHeight() * 1.3)));

    // render glyphs
    cairo_glyph_path(cairoPaint, cairoGlyphs, numGlyphs);

    cairo_translate(cairoPaint, static_cast<double>(-offset.x), static_cast<double>(-offset.y));

    he_free(cairoGlyphs);

    //cairo_glyph_free(cairoGlyphs);
}

}} //end namespace