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
//Created: 10/08/2011
#include "HappyPCH.h"

#include "IniReader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace he {
namespace io {

IniReader::IniReader(): m_IsOpen(false)
{
}


IniReader::~IniReader()
{
}

bool parseKeyValue(const he::String& line, he::String& key, he::String& value)
{
    using namespace std;

    he::String::size_type pos(line.find(L'='));

    if (pos != he::String::npos)
    {
        key = line.substr(0, pos);
        value = line.substr(pos + 1, line.size() - (pos + 1));
        return true;
    }
    else
    {
        return false;
    }
}
inline he::String getSubDivision(const he::String& str)
{
    return str.substr(1, str.size() - 2);
}
void removeSpaces(he::String& str)
{
    he::StringStream stream;

    bool stringHalt = false;
    for_each(str.cbegin(), str.cend(), [&](wchar_t c)
    {
        if (stringHalt)
        {
            if (stringHalt && c == '"')
                stringHalt = false;

            stream << c;
        }
        else
        {
            if (c != ' ')
            {
                stream << c;
                if (c == '"')
                    stringHalt = true;
            }
        }
    });
    str = stream.str();
}
bool IniReader::open(const he::String& path)
{
    using namespace std;

    m_Data.clear();
    m_IsOpen = false;
    std::ifstream file;
    file.open(path.c_str(), ios::in);

    if (file.is_open())
    {
        InitReadSubData subData;
        he::String line;
        he::String sub = "";
        while(file.eof() == false)
        {
            getline(file, line);
            removeSpaces(line);
            if (line.size() != 0)
            {
                if (line[0] == '#')
                {
                    // comment - ignore
                }
                else if (line[0] == '[')
                {
                    if (!sub.empty())
                        m_Data.insert(sub, std::move(subData));
                    sub = getSubDivision(line);
                    subData = InitReadSubData();
                }
                else
                {
                    he::String key, value;
                    if (parseKeyValue(line, key, value))
                        subData.insert(key, std::move(value));
                }
            }
        }

        if (sub.empty() == false)
            m_Data.insert(sub, std::move(subData));
        file.close();
        m_IsOpen = true;
        return true;
    }
    else
    {
        return false;
    }
}
bool IniReader::isOpen() const
{
    return m_IsOpen;
}


//-------------------------------------------------------
// Getters
//-----------------------------------------------
bool IniReader::readBool(const he::String& root, const he::String& node, bool defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        if (strcmp(raw.c_str(), "1") == 0 || stricmp(raw.c_str(), "true") == 0)
            return true;
        else
            return false;
    }
    else
    {
        return defaultReturn;
    }
}
int IniReader::readInt(const he::String& root, const he::String& node, int defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        int ret = INT_MAX;
        if (sscanf(raw.c_str(), "%d", &ret) == EOF || ret == INT_MAX)
            return defaultReturn;
        else
            return ret;
    }
    else
    {
        return defaultReturn;
    }
}
float IniReader::readFloat(const he::String& root, const he::String& node, float defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        float ret = FLT_MAX;
        if (sscanf(raw.c_str(), "%f", &ret) == EOF || ret == FLT_MAX)
            return defaultReturn;
        else
            return ret;
    }
    else
    {
        return defaultReturn;
    }
}

vec2 IniReader::readVector2(const he::String& root, const he::String& node, const vec2& defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        vec2 ret(FLT_MAX, FLT_MAX);
        if (sscanf(raw.c_str(), "%f,%f", &ret.x, &ret.y) == EOF || ret.x == FLT_MAX || ret.y == FLT_MAX)
            return defaultReturn;
        else
            return ret;
    }
    else
    {
        return defaultReturn;
    }
}
vec3 IniReader::readVector3(const he::String& root, const he::String& node, const vec3& defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        vec3 ret(FLT_MAX, FLT_MAX, FLT_MAX);
        if (sscanf(raw.c_str(), "%f,%f,%f", &ret.x, &ret.y, &ret.z) == EOF || ret.x == FLT_MAX || ret.y == FLT_MAX || ret.z == FLT_MAX)
            return defaultReturn;
        else
            return ret;
    }
    else
    {
        return defaultReturn;
    }
}
vec4 IniReader::readVector4(const he::String& root, const he::String& node, const vec4& defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        vec4 ret(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
        if (sscanf(raw.c_str(), "%f,%f,%f,%f", &ret.x, &ret.y, &ret.z, &ret.w) == EOF || ret.x == FLT_MAX || ret.y == FLT_MAX ||
                                                                                           ret.z == FLT_MAX || ret.w == FLT_MAX)
            return defaultReturn;
        else
            return ret;
    }
    else
    {
        return defaultReturn;
    }
}

he::String IniReader::readString(const he::String& root, const he::String& node, const he::String& defaultReturn) const
{
    he::String raw("");
    if (readRaw(root, node, raw))
    {
        if (raw.front() == '"')
        {
            raw = raw.substr(1, raw.size() - 1);
            if (raw.back() == '"')
                raw = raw.substr(0, raw.size() - 1);
        }
        return raw;
    }
    else
    {
        return defaultReturn;
    }
}

bool IniReader::readRaw(const he::String& root, const he::String& node, he::String& returnValue) const
{
    HE_ASSERT(m_IsOpen, "there is no file open, please call open first or check for unhandled open errors");

    InitReadSubData* itRoot = m_Data.find(root);
    if (itRoot)
    {
        he::String* itNode = itRoot->find(node);
        if (itNode)
        {
            returnValue = *itNode;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
const he::Map<he::String, he::String>& IniReader::getNodes(const he::String& root) const
{
    HE_ASSERT(m_IsOpen, "there is no file open, please call open first or check for unhandled open errors");

    InitReadSubData* itRoot = m_Data.find(root);
    return *itRoot;
}
bool IniReader::containsRoot(const he::String& root) const
{
    HE_ASSERT(m_IsOpen, "there is no file open, please call open first or check for unhandled open errors");
    return m_Data.contains(root);
}

} } //end namespace
