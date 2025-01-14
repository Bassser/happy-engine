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
//Author: Bastian Damman
//Created: 04/08/2011

#ifndef _HE_OBJ_LOADER_H_
#define _HE_OBJ_LOADER_H_
#pragma once

#include "VertexLayout.h"
#include "IModelLoader.h"

namespace he {
namespace ct {
namespace models {

class ObjLoader : public IModelLoader
{
public:
    struct InternalVertex
    {
        vec3 pos;
        vec2 tex;
        vec3 norm;

        InternalVertex(){}
        InternalVertex(const vec3& p, const vec2& t, const vec3& n):
                            pos(p), tex(t), norm(n) {}       
        ~InternalVertex(){}
    };
    ObjLoader();
    virtual ~ObjLoader();

    virtual bool load(const he::String& path);

    virtual size_t getNumMeshes() const;
    virtual const he::String& getMeshName(uint32 mesh) const;

    virtual const void* getVertices(uint32 mesh) const;
    virtual size_t getNumVertices(uint32 mesh) const;

    virtual const he::ObjectList<gfx::Bone>& getBones(uint32 /*mesh*/) const { return m_NoBones; } //No bones in a OBJ

    virtual const void* getIndices(uint32 mesh) const;
    virtual gfx::IndexStride getIndexStride(uint32 mesh) const;
    virtual size_t getNumIndices(uint32 mesh) const;

    const gfx::VertexLayout& getVertexLayout() const { return m_VertexLayout; }

private:
    struct Range
    {
        uint32 begin;
        uint32 end;
    };

    bool read(const he::String& path);
    void flushCreateGroup(uint32 group);
    void create();
    void addIndex(uint32 index, uint32 group);
    void fill(void* pdata) const;

    he::PrimitiveList<vec3> m_PositionData;
    he::PrimitiveList<vec2> m_TextureData;
    he::PrimitiveList<vec3> m_NormalData;
    struct FaceData
    {
        uint32 data[3][3];
    };
    he::PrimitiveList<FaceData> m_FaceData;
    he::PrimitiveList<Range> m_FaceDataMeshRange;
    he::ObjectList<he::String> m_GroupData;

    he::ObjectList<InternalVertex> m_VertexData;
    he::PrimitiveList<Range> m_VertexMeshRange;
    he::Map<he::String, uint32> m_IndexMap;

    he::PrimitiveList<Range> m_IndexMeshRange;
    he::PrimitiveList<uint16> m_IndicesUShort;
    he::PrimitiveList<uint32> m_IndicesUInt;

    void* m_Vertices;
    gfx::VertexLayout m_VertexLayout;

    size_t m_NumVertices;
    he::PrimitiveList<uint32> m_NumIndices;
    he::PrimitiveList<gfx::IndexStride> m_IndexStride;

    he::ObjectList<gfx::Bone> m_NoBones;

    //Disable default copy constructor and default assignment operator
    ObjLoader(const ObjLoader&);
    ObjLoader& operator=(const ObjLoader&);
};

} } } //end namespace

#endif
