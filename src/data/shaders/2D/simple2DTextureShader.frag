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
//Author: Sebastiaan Sprengers

#version 150 core

in vec2 passTexCoord;

out vec4 outColor;

uniform sampler2D diffuseMap;
uniform float inAlpha;
uniform vec2 texCoordOffset;
uniform vec2 texCoordScale;

void main()
{
	vec2 tcScale = vec2(abs(texCoordScale.x), abs(texCoordScale.y));
    vec4 color = texture(diffuseMap, (passTexCoord * tcScale) + texCoordOffset);

    float alpha = color.a - (1.0f - inAlpha);

    outColor =  vec4(color.rgb, alpha);
}