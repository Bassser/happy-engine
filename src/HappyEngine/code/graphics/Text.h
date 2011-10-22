//HappyEngine Copyright (C) 2011  Bastian Damman, Sebastiaan Sprengers
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

#ifndef _HE_TEXT_H_
#define _HE_TEXT_H_
#pragma once

#include "Font.h"
#include <string>
#include <vector>
#include "boost/shared_ptr.hpp"

namespace he {
namespace gui {

class Text
{
public:

	typedef boost::shared_ptr<Text> pointer;

	/* CONSTRUCTOR - DESTRUCTOR */
	Text(uint maxWidth = 0, uint maxHeight = 0);
    virtual ~Text();

	/* GENERAL */
	void addLine(const std::string& string);

	/* SETTERS */
	void setLine(const std::string& string, uint lineNumber);

	/* GETTERS */
	const std::string& getLine(uint lineNumber) const;
	const std::vector<std::string>& getText() const;

	bool isEmpty() const;

	uint getMaxWidth() const;
	uint getMaxHeight() const;

private:

	/* DATAMEMBERS */
	std::vector<std::string> m_Text;

	uint m_MaxWidth;
	uint m_MaxHeight;

    /* DEFAULT COPY & ASSIGNMENT */
    Text(const Text&);
    Text& operator=(const Text&);
};

} } //end namespace

#endif