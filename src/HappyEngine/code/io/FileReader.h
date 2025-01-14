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

#ifndef _HE_FILEREADER_H_
#define _HE_FILEREADER_H_
#pragma once

#include <fstream>

namespace he {
namespace io {

class HAPPY_ENTRY FileReader
{
public:
    enum OpenType
    {
        OpenType_Unicode,
        OpenType_ASCII
    };

    FileReader();
    virtual ~FileReader();

    bool open(const he::Path& path, OpenType type);
    void close();

    he::String readLine();
    he::String readToEnd();
    void readToEndSplit(he::ObjectList<he::String>& outList);

    std::wstring readLineW();
    std::wstring readToEndW();
    void readToEndSplitW(he::ObjectList<std::wstring>& outList);

    bool eof() const;

private:
    std::wfstream m_Wfstream;
    std::fstream m_fstream;

    //Disable default copy constructor and default assignment operator
    FileReader(const FileReader&);
    FileReader& operator=(const FileReader&);
};

} } //end namespace

#endif
