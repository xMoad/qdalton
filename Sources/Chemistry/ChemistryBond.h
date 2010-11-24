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

namespace Chemistry
{
  class Atom;
  class Molecule;

  class Bond
  {
  public:
    Bond(Chemistry::Molecule& molecule,
         Chemistry::Atom* beginAtom,
         Chemistry::Atom* endAtom,
         quint8 bondOrder);

    Bond(const Chemistry::Bond& bond);

    Chemistry::Bond& operator=(const Chemistry::Bond& rhs);

    bool operator==(const Chemistry::Bond& bond) const;
    bool operator!=(const Chemistry::Bond& bond) const;

    quint16 index() const;

    const Chemistry::Atom& beginAtom() const;
    const Chemistry::Atom& endAtom() const;

    quint8 bondOrder() const;
    void setBondOrder(quint8 bondOrder);
    void cycleBondOrder();

    float length() const;
    Eigen::Vector3f centre() const;

    bool isSelected() const;
    void toggleSelection();

  private:
    Chemistry::Molecule& molecule_;
    Chemistry::Atom* beginAtom_;
    Chemistry::Atom* endAtom_;
    quint8 bondOrder_;
    bool selected_;
  };
}

#endif // RENDER_BOND_H
