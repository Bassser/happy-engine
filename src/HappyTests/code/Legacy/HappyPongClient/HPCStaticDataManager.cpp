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
//Created: 27/06/2012
#include "HappyPongClientPCH.h" 

#include "HPCStaticDataManager.h"
#include "NetworkObjectFactory.h"
#include "NetworkObject.h"
#include "Obstacle.h"
#include "Palet.h"
#include "Ball.h"

namespace hpc {

void HPCStaticDataManager::init()
{
    he::net::NetworkObjectFactory<Palet>::sdmInit();
    he::net::NetworkObjectFactory<Palet>::getInstance()->init(5, 5, "Palet factory");
    he::net::NetworkObjectFactory<Ball>::sdmInit();
    he::net::NetworkObjectFactory<Ball>::getInstance()->init(5, 5, "Ball factory");
}

void HPCStaticDataManager::destroy()
{
    he::net::NetworkObjectFactory<Palet>::sdmDestroy();
    he::net::NetworkObjectFactory<Ball>::sdmDestroy();
}

} //end namespace