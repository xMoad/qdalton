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

#ifndef RENDER_BOND_H
#define RENDER_BOND_H

#include <Eigen/Array>

#include "Render/RenderConstants.h"
#include "Render/RenderGL.h"

namespace Render
{
  class Atom;
  class Molecule;

  class Bond
  {
  public:
    Bond(Render::Molecule* molecule,
         Render::Atom* beginAtom,
         Render::Atom* endAtom,
         quint8 bondOrder);
    Bond(const Render::Bond& bond);
    Render::Bond& operator=(const Render::Bond& rhs);

    bool operator==(const Render::Bond& bond) const;
    bool operator!=(const Render::Bond& bond) const;

    quint16 index() const;

    const Render::Atom& beginAtom() const;
    const Render::Atom& endAtom() const;

    void draw(Render::View view,
              const Eigen::Vector3f& planeNormalVector,
              bool fast) const;

    quint8 bondOrder() const;
    void setBondOrder(quint8 bondOrder);
    void cycleBondOrder();

    float length() const;
    Eigen::Vector3f centre() const;

    bool isSelected() const;
    void toggleSelection();

  private:
    Render::Molecule* molecule_;
    Render::Atom* beginAtom_;
    Render::Atom* endAtom_;
    quint8 bondOrder_;
    bool selected_;
  };
}

#endif // RENDER_BOND_H
