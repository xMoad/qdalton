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

#include "Chemistry/ChemistryBond.h"
#include "Chemistry/ChemistryMolecule.h"

Chemistry::Bond::Bond(Chemistry::Molecule* molecule,
                      quint16 beginIndex,
                      quint16 endIndex)
{
  molecule_ = molecule;
  beginIndex_ = beginIndex;
  endIndex_ = endIndex;
}

Chemistry::Bond::Bond(const Chemistry::Bond& bond) :
    molecule_(bond.molecule_),
    beginIndex_(bond.beginIndex_),
    endIndex_(bond.endIndex_)
{
}

Chemistry::Atom* Chemistry::Bond::beginAtomPointer() const
{
  return molecule_->atomPointer(beginIndex_);
}

Chemistry::Atom* Chemistry::Bond::endAtomPointer() const
{
  return molecule_->atomPointer(endIndex_);
}

quint16 Chemistry::Bond::beginIndex() const
{
  return beginIndex_;
}

quint16 Chemistry::Bond::endIndex() const
{
  return endIndex_;
}
