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

#include "Render/RenderQuadric.h"
#include "Render/RenderAtom.h"

namespace Render
{
  class Bond
  {
  public:
    Bond(Render::Molecule* molecule,
         quint16 beginIndex,
         quint16 endIndex);

    Bond(const Render::Bond& bond);

    const Render::Atom& beginAtom() const;
    const Render::Atom& endAtom() const;

    quint16 beginIndex() const;
    quint16 endIndex() const;

    Render::Molecule* molecule_;

    void draw(Quality quality) const;
    void drawSelection(Quality quality) const;
    bool isSelected() const;
    void setSelected(bool selected);
    static const GLfloat DEFAULT_THIKNESS;
  private:
    quint16 beginIndex_;
    quint16 endIndex_;
    bool isSelected_;
  };
}

#endif // RENDER_BOND_H
