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
//Author:  Bastian Damman
//Created: 27/12/2011

#ifndef _HE_FX_PARTICLE_SCALE_H_
#define _HE_FX_PARTICLE_SCALE_H_
#pragma once

#include "IFxParticleModifyComponent.h"
#include "IFxParticleInitComponent.h"
#include "IFxVariable.h"

namespace he {
namespace gfx {

class FxParticleScale : public IFxParticleModifyComponent, public IFxParticleInitComponent
{
public:
    FxParticleScale();
    virtual ~FxParticleScale();
    
    virtual void init(FxParticle* pParticle, const mat44& parentWorld);
    virtual void transform(FxParticle* pParticle, float currentTime, float dTime);

    void setValue(const IFxVariable<vec3>::pointer& value);
    const IFxVariable<vec3>::pointer& getValue() const;

private:

    IFxVariable<vec3>::pointer m_Scale;

    //Disable default copy constructor and default assignment operator
    FxParticleScale(const FxParticleScale&);
    FxParticleScale& operator=(const FxParticleScale&);
};

} } //end namespace

#endif