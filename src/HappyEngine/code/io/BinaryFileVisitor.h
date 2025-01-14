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
//Created: 26/01/2013

#ifndef _HE_BinaryFileVisitor_H_
#define _HE_BinaryFileVisitor_H_
#pragma once

#include "BinaryVisitor.h"

namespace he {
namespace io {

class HAPPY_ENTRY BinaryFileVisitor : public BinaryVisitor
{
public:
    BinaryFileVisitor();
    virtual ~BinaryFileVisitor();

    bool openWrite(const Path& file);
    bool openRead(const Path& file, const bool doBigToLittleEndianSwap = false);
    virtual void close();

    virtual void skipBytes(const size_t bytes);
    virtual size_t getProcessedBytes();

protected:
    virtual	size_t readBuffer(void* buffer, const size_t byteCount);
    virtual size_t writeBuffer(const void* buffer, const size_t byteCount);
    
private:
    bool open(const Path& file, const char* type);
    FILE* m_File;

    bool m_DoEndianSwap;

    //Disable default copy constructor and default assignment operator
    BinaryFileVisitor(const BinaryFileVisitor&);
    BinaryFileVisitor& operator=(const BinaryFileVisitor&);
};

} } //end namespace

#endif
