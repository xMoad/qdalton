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

#include "Chemistry/ChemistryMolecule.h"

float tolerance = 0.4f;

Chemistry::Molecule::Molecule()
{
}

const Chemistry::Atom& Chemistry::Molecule::atom(uint16_t index) const
{
  return chemistryAtoms_.at(index);
}

Chemistry::Atom* Chemistry::Molecule::atomPtr(uint16_t index)
{
  return &chemistryAtoms_[index];
}

Chemistry::Bond* Chemistry::Molecule::bondPtr(uint16_t index)
{
  return &chemistryBonds_[index];
}

uint16_t Chemistry::Molecule::atomsCount() const
{
  return chemistryAtoms_.count();
}

uint16_t Chemistry::Molecule::bondsCount() const
{
  return chemistryBonds_.count();
}

uint8_t Chemistry::Molecule::charge() const
{
  return charge_;
}

void Chemistry::Molecule::setCharge(uint8_t charge)
{
  charge_ = charge;
}

void Chemistry::Molecule::addAtom(const Chemistry::Atom& atom)
{
  chemistryAtoms_ << atom;
  incidenceMatrix_ << IncidenceList();
}

float Chemistry::Molecule::interatomicDistance(uint16_t index1, uint16_t index2)
{ 
  return (chemistryAtoms_[index2].centre() -
          chemistryAtoms_[index1].centre()).norm();
}

bool Chemistry::Molecule::isConnected(uint16_t index1, uint16_t index2) const
{
  for (int i = 0; i < incidenceMatrix_.at(index1).count(); ++i)
  {
    if (incidenceMatrix_.at(index1).at(i)->endIndex() == index2)
    {
      return true;
    }
  }
  return false;
}

void Chemistry::Molecule::connect(uint16_t index1, uint16_t index2)
{
  chemistryBonds_ << Chemistry::Bond(this, index1, index2);
  incidenceMatrix_[index1] << &chemistryBonds_[bondsCount() - 1];
  incidenceMatrix_[index2] << &chemistryBonds_[bondsCount() - 1];
}

void Chemistry::Molecule::rebond()
{
  for (int i = 0; i < atomsCount() - 1; ++i)
  {
    for (int j = i + 1; j < atomsCount(); ++j)
    {
      if (interatomicDistance(i, j) <
          chemistryAtoms_[i].covalentRadius() +
          chemistryAtoms_[j].covalentRadius() + tolerance)
      {
        connect(i, j);
      }
    }
  }
}
