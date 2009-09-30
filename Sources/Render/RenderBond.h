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

#ifndef RENDER_BOND_H
#define RENDER_BOND_H

#include <Eigen/Array>
#include "Render/RenderQuality.h"

namespace OpenBabel
{
  class OBBond;
}

namespace Render
{
  class Bond
  {
  public:
    enum DrawStyle
    {
      DRAW_STYLE_BOND,
      DRAW_STYLE_STICK
    };

    Bond(const Render::Bond& bond);
    Bond(OpenBabel::OBBond* obBond);

    void draw(Render::Bond::DrawStyle drawStyle,
              Render::Quality quality) const;

    void drawSelection(Quality quality) const;

    bool isSelected() const;
    void setSelected(bool selected);
    void toggleSelected();

    static const GLfloat BOND_THIKNESS;
    static const GLfloat STICK_THIKNESS;
  private:
    OpenBabel::OBBond* obBond_;
    bool isSelected_;
  };
}

#endif // RENDER_BOND_H
