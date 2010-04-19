/**********************************************************************
  Copyright (C) 2008, 2009, 2010 Anton Simakov

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

#ifndef RENDER_CONSTANTS_H
#define RENDER_CONSTANTS_H

#include "Render/RenderGL.h"

namespace Render
{
  enum View
  {
    ViewBallsAndBonds,
    ViewBallsAndSticks,
    ViewSticks,
    ViewVdWSpheres
  };

  enum Style
  {
    StyleFill,
    StyleLine,
    StyleSilhouette,
    StylePoint
  };

  enum LabelsOnAtoms
  {
    LabelsOnAtomsNone,
    LabelsOnAtomsSymbol
  };

  const GLfloat bondThikness = 0.075f;
  const GLfloat stickThikness = 0.15f;
  const GLfloat selectionRadius = 0.10f;

  const GLint slicesForFastDrawing = 8;
  const GLint slicesForDrawing = 24;
}

#endif // RENDER_CONSTANTS_H
