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
//Author: Bastian Damman

#version 150 core

#include "packing/encode.frag"

in vec2 passTexCoord;
in vec3 passNormal;
in vec3 passTangent;

out vec4 outColor;
out vec2 outNormal;
out vec4 outSGI;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specGlossIllMap;

vec3 calcNormal(in vec3 normal, in vec3 tangent, in vec3 rgb)
{
    //NormalMap
    tangent = normalize(tangent);
    normal = normalize(normal);

    vec3 binormal = normalize(cross(tangent, normal));

    mat3 assenstelsel = mat3(binormal, tangent, normal);

    vec3 xyz = vec3(rgb.x * 2 - 1, (1-rgb.y) * 2 - 1, rgb.z * 2 - 1);

    return normalize(assenstelsel * xyz);
}

void main()
{
    vec4 color = texture(diffuseMap, passTexCoord);
    
    if (color.a < 0.5f)
        discard;
    
    vec4 normal = texture(normalMap, passTexCoord);
    vec4 specGlossIll = texture(specGlossIllMap, passTexCoord);
    
    outColor = vec4(color.rgb, specGlossIll.b);

    outNormal = encodeNormal(calcNormal(passNormal, passTangent, normal.rgb));

    outSGI = vec4(specGlossIll.rg, 0.0f, 1.0f);
}