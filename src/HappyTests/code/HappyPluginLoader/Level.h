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
//Author: Thijs Morlion
//Created: 27/01/2013

#ifndef _HT_LEVEL_H_
#define _HT_Level_H_
#pragma once

namespace ht {

class Level
{
public:
    Level();
    virtual ~Level();

private:

    //Disable default copy constructor and default assignment operator
    Level(const Level&);
    Level& operator=(const Level&);
};

} //end namespace

#endif
