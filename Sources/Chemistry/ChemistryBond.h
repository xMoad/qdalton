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

#ifndef CHEMISTRY_BOND_H
#define CHEMISTRY_BOND_H

#include <QtGlobal>

namespace Chemistry
{
  class Atom;
  class Molecule;

  class Bond
  {
  public:
    Bond(Chemistry::Molecule* molecule,
         quint16 beginIndex,
         quint16 endIndex);

    Bond(const Chemistry::Bond& bond);

    Chemistry::Atom* beginAtomPointer() const;
    Chemistry::Atom* endAtomPointer() const;

    quint16 beginIndex() const;
    quint16 endIndex() const;
    Chemistry::Molecule* molecule_;
  private:
    quint16 beginIndex_;
    quint16 endIndex_;
  };
}
#endif // CHEMISTRY_BOND_H
