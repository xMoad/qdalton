/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

  This file is part of QDalton.
  For more information, see <http://code.google.com/p/qdalton/>

  QDalton is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QDalton is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QDalton. If not, see <http://www.gnu.org/licenses/>.

 **********************************************************************/

#ifndef RENDER_QUADRIC_H
#define RENDER_QUADRIC_H

#include "Render/RenderGLU.h"
#include "Render/RenderStyle.h"

namespace Render
{
  class Quadric
  {
  public:
    Quadric();
    ~Quadric();
    void prepare(Render::Style style) const;
    GLUquadricObj* GLUquadric_;
  };
}

#endif // RENDER_QUADRIC_H
