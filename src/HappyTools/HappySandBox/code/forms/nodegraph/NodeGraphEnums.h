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

#ifndef _HS_NODEGRAPHENUMS_H_
#define _HS_NODEGRAPHENUMS_H_
#pragma once

namespace he {
namespace gui {
    class Canvas2D;
}
}

namespace hs {

struct NodeGraphDrawContext
{
    he::gui::Canvas2D* canvas;
    he::mat33 transform;
    he::RectF clipRect;
    he::RectF worldRect;
};

}

#endif
