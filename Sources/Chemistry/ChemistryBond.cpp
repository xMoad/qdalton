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

Chemistry::Bond::Bond(Chemistry::Molecule* chemistryMolecule, uint16_t beginIndex, uint16_t endIndex)
{
  chemistryMolecule_ = chemistryMolecule;
  beginIndex_ = beginIndex;
  endIndex_ = endIndex;
}

Chemistry::Atom* Chemistry::Bond::beginAtomPtr()
{
  return chemistryMolecule_->atomPtr(beginIndex_);
}

Chemistry::Atom* Chemistry::Bond::endAtomPtr()
{
  return chemistryMolecule_->atomPtr(endIndex_);
}

uint16_t Chemistry::Bond::beginIndex() const
{
  return beginIndex_;
}

uint16_t Chemistry::Bond::endIndex() const
{
  return endIndex_;
}
