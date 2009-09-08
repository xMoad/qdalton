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

float bohrsToAngstroms(float bohrs)
{
  return 0.529177249 * bohrs;
}

Chemistry::Molecule::Molecule()
{
}

Chemistry::Molecule::Molecule(const Chemistry::Molecule& molecule) :
    atoms_(molecule.atoms_),
    bonds_(molecule.bonds_),
    incidenceMatrix_(molecule.incidenceMatrix_),
    charge_(molecule.charge_),
    unitOfLength_(molecule.unitOfLength_)
{
  QListIterator<Chemistry::Atom*> aPtr(atoms_);
  while (aPtr.hasNext())
  {
    aPtr.next()->setParentMolecule(this);
  }

  QListIterator<Chemistry::Bond*> bPtr(bonds_);
  while (bPtr.hasNext())
  {
    bPtr.next()->molecule_ = this;
  }
}

Chemistry::Molecule& Chemistry::Molecule::operator=(const Chemistry::Molecule& molecule)
{
  if (this != &molecule)
  {
    atoms_ = QList<Chemistry::Atom*>(molecule.atoms_);
    bonds_ = QList<Chemistry::Bond*>(molecule.bonds_);
    incidenceMatrix_ = IncidenceMatrix(molecule.incidenceMatrix_);
    charge_ = molecule.charge_;
    unitOfLength_ = molecule.unitOfLength_;

    QListIterator<Chemistry::Atom*> aPtr(atoms_);
    while (aPtr.hasNext())
    {
      aPtr.next()->setParentMolecule(this);
    }

    QListIterator<Chemistry::Bond*> bPtr(bonds_);
    while (bPtr.hasNext())
    {
      bPtr.next()->molecule_ = this;
    }
  }

  return *this;
}

Chemistry::Molecule::UnitOfLength Chemistry::Molecule::unitOfLength() const
{
  return unitOfLength_;
}

void Chemistry::Molecule::setUnitOfLength(UnitOfLength unit)
{
  unitOfLength_ = unit;
}

Chemistry::Atom* Chemistry::Molecule::atomPointer(quint16 index) const
{
  return atoms_.at(index);
}

Chemistry::Bond* Chemistry::Molecule::bondPointer(quint16 index) const
{
  return bonds_.at(index);
}

quint16 Chemistry::Molecule::atomsCount() const
{
  return atoms_.count();
}

quint16 Chemistry::Molecule::bondsCount() const
{
  return bonds_.count();
}

quint8 Chemistry::Molecule::charge() const
{
  return charge_;
}

void Chemistry::Molecule::setCharge(quint8 charge)
{
  charge_ = charge;
}

void Chemistry::Molecule::addAtom(const Chemistry::Atom& atom)
{
  atoms_ << new Chemistry::Atom(atom);
  atoms_.last()->setParentMolecule(this);
  incidenceMatrix_ << IncidenceList();
}

float Chemistry::Molecule::interatomicDistance(quint16 index1, quint16 index2)
{ 
  return (atoms_[index2]->centre() -
          atoms_[index1]->centre()).norm();
}

bool Chemistry::Molecule::isAtomsBonded(quint16 index1, quint16 index2) const
{
  for (int i = 0; i < incidenceMatrix_.at(index1).count(); ++i)
  {
    if (bondPointer(incidenceMatrix_.at(index1).at(i))->endIndex() == index2)
    {
      return true;
    }
  }
  return false;
}

void Chemistry::Molecule::bondAtoms(quint16 index1, quint16 index2)
{
  bonds_ << new Chemistry::Bond(this, index1, index2);
  incidenceMatrix_[index1] << (bondsCount() - 1);
  incidenceMatrix_[index2] << (bondsCount() - 1);
}

void Chemistry::Molecule::rebond()
{
  for (int i = 0; i < atomsCount() - 1; ++i)
  {
    for (int j = i + 1; j < atomsCount(); ++j)
    {
      switch (unitOfLength_)
      {
      case ANGSTROM:
        if (interatomicDistance(i, j) <
            atoms_[i]->covalentRadius() +
            atoms_[j]->covalentRadius() + tolerance)
        {
          bondAtoms(i, j);
        }
        break;
      case BOHR:
        if (bohrsToAngstroms(interatomicDistance(i, j)) <
            atoms_[i]->covalentRadius() +
            atoms_[j]->covalentRadius() + tolerance)
        {
          bondAtoms(i, j);
        }
        break;
      }
    }
  }
}
