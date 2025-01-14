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
//Created: 09/10/2011
//Rewritten with c-string to be faster -- Bastian
#include "HappyPCH.h" 

#include "Text.h"
#include "Font.h"
#include "ContentManager.h"
#include "Canvas2D.h"

#define CAPA_INCR_MIN 8

namespace he {
namespace gui {

/* CONSTRUCTOR - DESTRUCTOR */
Text::Text() :	m_OverFlowType(OverFlowType_Clip),
                m_HAlignment(HAlignment_Left),
                m_VAlignment(VAlignment_Top),
                m_Bounds(),
                m_HasBounds(false),
                m_Font(nullptr),
                m_Capacity(0), m_Size(0),
                m_Text(nullptr)
{
    he_memset(m_ScratchBuffer, 0, MAX_SCRATCH);
}

Text::~Text()
{
    if (m_Font != nullptr)
        m_Font->release();
    he_free(m_Text);
}

/* GENERAL */
void Text::addText( const char* text, int len )
{
    HIERARCHICAL_PROFILE(__HE_FUNCTION__);
    const size_t size(len == -1? strlen(text) : len);
    const size_t start(m_Size);
    resize(m_Size + size);
    he_memcpy(m_Text + start, text, size);
}

void Text::addLine(const char* text, int len)
{
    addText(text, len);
    addText("\n", 1);
}

void Text::addTextExt( const char* text, ... )
{
    va_list argList;
    va_start(argList, text);
    addTextExt(text, argList);
    va_end(argList);
}

void Text::addTextExt( const char* text, va_list& argList )
{
    int len(hevsnprintf(m_ScratchBuffer, MAX_SCRATCH - 1, text, argList));
    HE_ASSERT(len >= 0, "addTextExt FAILED!");
    addText(m_ScratchBuffer, len);
}

void Text::clear()
{
    m_Size = 0;
}

he::vec2 Text::measureText()
{
    vec2 result(0, 0);
    size_t start(0);
    size_t scratchEnd(0);
    for (size_t i(0); i < m_Size; ++i)
    {
        if (m_Text[i] == '&' || m_Text[i] == '\n' || i + 1 == m_Size) // Color: &FFF
        {
            const size_t size(i - start + (i + 1 == m_Size? 1 : 0));
            memcpy(m_ScratchBuffer + scratchEnd, m_Text + start, size);
            scratchEnd += size;
            if (m_Text[i] == '&')
            {
                start = i + 4;
                i += 3;
            }
            else if (m_Text[i] == '\n' || i + 1 == m_Size)
            {
                result.x = std::max(result.x, m_Font->getStringWidth(m_ScratchBuffer, scratchEnd));
                result.y += m_Font->getLineSpacing();
                scratchEnd = 0;
                start = i + 1;
            }
        }
    }
    return result;
}

/* SETTERS */
void Text::setBounds(const vec2& bounds)
{
    if (bounds != vec2())
    {
        m_Bounds = bounds;
        m_HasBounds = true;
    }
    else
    {
        m_HasBounds = false;
    }
}

void Text::setFont( gui::Font* font )
{
    if (m_Font != nullptr)
        m_Font->release();
    m_Font = font;
    m_Font->instantiate();
}

void Text::resize( const size_t newSize )
{
    if (m_Capacity < newSize)
    {
        m_Text = static_cast<char*>(he_realloc("Text::m_Text", m_Text, he::max(newSize, m_Capacity + CAPA_INCR_MIN)));
        m_Capacity = newSize;
    }
    m_Size = newSize;
}

/* DEFAULT COPY & ASSIGNMENT */
Text::Text(const Text& text)
{
    he_memset(this->m_ScratchBuffer, 0, MAX_SCRATCH);

    this->m_Text = (char*)he_malloc("Text::m_Text", sizeof(char) * text.m_Size);

    this->m_Size = text.m_Size;
    this->m_Capacity = text.m_Size;

    he_memcpy(this->m_Text, text.m_Text, m_Size);

    if (text.m_Font != nullptr)
    {
        text.m_Font->instantiate();
        this->m_Font = text.m_Font;
    }

    this->m_HAlignment = text.m_HAlignment;
    this->m_VAlignment = text.m_VAlignment;

    this->m_Bounds = text.m_Bounds;
    this->m_HasBounds = text.m_HasBounds;
}
Text& Text::operator=(const Text& text)
{
    he_memset(this->m_ScratchBuffer, 0, MAX_SCRATCH);

    this->m_Text = (char*)he_malloc("Text::m_Text", sizeof(char) * text.m_Size);

    this->m_Size = text.m_Size;
    this->m_Capacity = text.m_Size;

    he_memcpy(this->m_Text, text.m_Text, m_Size);

    if (text.m_Font != nullptr)
    {
        text.m_Font->instantiate();
        this->m_Font = text.m_Font;
    }

    this->m_HAlignment = text.m_HAlignment;
    this->m_VAlignment = text.m_VAlignment;

    this->m_Bounds = text.m_Bounds;
    this->m_HasBounds = text.m_HasBounds;

    return *this;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LayoutableText::LayoutableText()
 : m_Text()
{
    m_Text.setOverFlowType(Text::OverFlowType_Clip);
}

LayoutableText::~LayoutableText()
{

}

void LayoutableText::setTextExt( const char* text, ... )
{
    va_list argList;
    va_start(argList, text);
    setTextExt(text, argList);
    va_end(argList);
}

void LayoutableText::setTextExt( const char* text, va_list& argList )
{
    char buff[1024];
    buff[1023] = '\0';
    int len(hevsnprintf(buff, 1023, text, argList));
    HE_ASSERT(len >= 0, "addTextExt FAILED!");
    setText(buff, len);
}

void LayoutableText::setText( const char* text, int len /*= -1*/ )
{
    m_Text.clear();
    m_Text.addText(text, len);
    const he::vec2 size(m_Text.measureText());
    setLayoutMinSize(size);
}

void LayoutableText::setFont( Font* const font )
{
    m_Text.setFont(font);
    const he::vec2 size(m_Text.measureText());
    setLayoutMinSize(size);
}
void LayoutableText::performLayout()
{
    DefaultLayoutable::performLayout();
    m_Text.setBounds(he::vec2(m_LayoutBound.width, m_LayoutBound.height));
}

} } //end namespace