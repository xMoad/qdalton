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

#ifndef RENDER_ATOM_H
#define RENDER_ATOM_H

#include <QString>
#include <Eigen/Array>
#include <openbabel/atom.h>

#include "Render/RenderQuality.h"
#include "Render/RenderGL.h"

namespace Render
{
  class Color;
  /**
   * @class Atom Atom.h "Render/Atom.h"
   * @brief Represents and draw atom.
   * @author Anton Simakov
   * @version 0.1
   */
  class Atom
  {
  public:
    enum DrawStyle
    {
      DRAW_STYLE_ATOM,
      DRAW_STYLE_CONNECTOR,
      DRAW_STYLE_VDW
    };

    Atom(const Atom& atom);
    Atom(OpenBabel::OBAtom* obAtom);

    GLfloat drawRadius() const;
    GLfloat vanderwaalsRadius() const;

    Render::Color color() const;

    Eigen::Vector3f centre() const;
    void setCentre(const Eigen::Vector3f& centre);

    void draw(Render::Atom::DrawStyle style,
              Render::Quality quality) const;

    void drawSelection(Render::Atom::DrawStyle style,
                       Render::Quality quality) const;

    OpenBabel::OBAtom* obAtom() const;

    bool isSelected() const;
    void setSelected(bool selected);
    void toggleSelected();

    static const GLfloat SELECTON_RADIUS;
  private:
    OpenBabel::OBAtom* obAtom_;
    bool isSelected_;
  };
}

#endif // RENDER_ATOM_H
