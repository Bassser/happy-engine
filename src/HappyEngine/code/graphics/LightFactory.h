//HappyEngine Copyright (C) 2011 - 2012  Evil Interactive
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
//Author:  
//Created: //

#ifndef _HE_LightFactory_H_
#define _HE_LightFactory_H_
#pragma once

#include "ILight.h"
#include "Singleton.h"

namespace he {
namespace gfx {

class PointLight;
class SpotLight;

class LightFactory : protected ObjectFactory<ILight>, public Singleton<LightFactory>
{
public:
    LightFactory();
    virtual ~LightFactory();

    void destroyLight(const ObjectHandle& handle);

    virtual ILight* get(const ObjectHandle& handle) const;
    ObjectHandle createPointLight();
    ObjectHandle createSpotLight();

    PointLight* getPointLight(const ObjectHandle& handle) const;
    SpotLight* getSpotLight(const ObjectHandle& handle) const;
    
private:
    //Disable default copy constructor and default assignment operator
    LightFactory(const LightFactory&);
    LightFactory& operator=(const LightFactory&);
};

} } //end namespace

#endif