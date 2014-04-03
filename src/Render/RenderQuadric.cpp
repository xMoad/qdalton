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

#include "Render/RenderQuadric.h"

Render::Quadric::Quadric()
{
  gluQuadricObj_ = gluNewQuadric();
}

Render::Quadric::~Quadric()
{
  gluDeleteQuadric(gluQuadricObj_);
}

void Render::Quadric::prepare(Render::Style style) const
{
  switch (style)
  {
  case Render::StyleFill:
    gluQuadricDrawStyle(gluQuadricObj_, GLU_FILL);
    break;
  case Render::StyleLine:
    gluQuadricDrawStyle(gluQuadricObj_, GLU_LINE);
    break;
  case Render::StylePoint:
    gluQuadricDrawStyle(gluQuadricObj_, GLU_POINT);
    break;
  case Render::StyleSilhouette:
    gluQuadricDrawStyle(gluQuadricObj_, GLU_SILHOUETTE);
    break;
  }
  gluQuadricNormals(gluQuadricObj_, GLU_SMOOTH);
}
