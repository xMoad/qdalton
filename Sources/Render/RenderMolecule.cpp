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

#include "Render/RenderMolecule.h"

float tolerance = 0.4f;

float bohrsToAngstroms(float bohrs)
{
  return 0.529177249 * bohrs;
}

Render::Molecule::Molecule() :
    atoms_(),
    bonds_(),
    incidenceMatrix_(),
    charge_(0),
    unitOfLength_(Render::Molecule::ANGSTROM)
{
}

Render::Molecule::Molecule(const Render::Molecule& molecule) :
    atoms_(molecule.atoms_),
    bonds_(molecule.bonds_),
    incidenceMatrix_(molecule.incidenceMatrix_),
    charge_(molecule.charge_),
    unitOfLength_(molecule.unitOfLength_)
{
  becomeParentForAtoms();
}

Render::Molecule& Render::Molecule::operator=(const Render::Molecule& molecule)
{
  if (this != &molecule)
  {
    atoms_ = QList<Render::Atom>(molecule.atoms_);
    bonds_ = QList<Render::Bond>(molecule.bonds_);
    incidenceMatrix_ = IncidenceMatrix(molecule.incidenceMatrix_);
    charge_ = molecule.charge_;
    unitOfLength_ = molecule.unitOfLength_;
    becomeParentForAtoms();
  }

  return *this;
}

Render::Molecule::UnitOfLength Render::Molecule::unitOfLength() const
{
  return unitOfLength_;
}

void Render::Molecule::setUnitOfLength(UnitOfLength unit)
{
  unitOfLength_ = unit;
}

Render::Atom& Render::Molecule::atom(quint16 index)
{
  return atoms_[index];
}

Render::Bond& Render::Molecule::bond(quint16 index)
{
  return bonds_[index];
}

const Render::Atom& Render::Molecule::atomAt(quint16 index) const
{
  return atoms_.at(index);
}

const Render::Bond& Render::Molecule::bondAt(quint16 index) const
{
  return bonds_.at(index);
}

quint16 Render::Molecule::atomsCount() const
{
  return atoms_.count();
}

quint16 Render::Molecule::bondsCount() const
{
  return bonds_.count();
}

quint8 Render::Molecule::charge() const
{
  return charge_;
}

void Render::Molecule::setCharge(quint8 charge)
{
  charge_ = charge;
}

void Render::Molecule::addAtom(const Render::Atom& atom)
{
  atoms_ << atom;
  atoms_.last().setParentMolecule(this);
  incidenceMatrix_ << IncidenceList();
}

float Render::Molecule::interatomicDistance(quint16 index1, quint16 index2)
{ 
  return (atoms_[index2].centre() -
          atoms_[index1].centre()).norm();
}

bool Render::Molecule::isAtomsBonded(quint16 index1, quint16 index2) const
{
  for (int i = 0; i < incidenceMatrix_.at(index1).count(); ++i)
  {
    if (bondAt(incidenceMatrix_.at(index1).at(i)).endIndex() == index2)
    {
      return true;
    }
  }
  return false;
}

void Render::Molecule::bondAtoms(quint16 index1, quint16 index2)
{
  bonds_ << Render::Bond(this, index1, index2);
  incidenceMatrix_[index1] << (bondsCount() - 1);
  incidenceMatrix_[index2] << (bondsCount() - 1);
}

void Render::Molecule::rebond()
{
  for (int i = 0; i < atomsCount() - 1; ++i)
  {
    for (int j = i + 1; j < atomsCount(); ++j)
    {
      switch (unitOfLength_)
      {
      case ANGSTROM:
        if (interatomicDistance(i, j) <
            atoms_[i].covalentRadius() +
            atoms_[j].covalentRadius() + tolerance)
        {
          bondAtoms(i, j);
        }
        break;
      case BOHR:
        if (bohrsToAngstroms(interatomicDistance(i, j)) <
            atoms_[i].covalentRadius() +
            atoms_[j].covalentRadius() + tolerance)
        {
          bondAtoms(i, j);
        }
        break;
      }
    }
  }
}

void Render::Molecule::becomeParentForAtoms()
{
  QMutableListIterator<Render::Atom> a(atoms_);
  while (a.hasNext())
  {
    a.next().setParentMolecule(this);
  }

  QMutableListIterator<Render::Bond> b(bonds_);
  while (b.hasNext())
  {
    b.next().molecule_ = this;
  }
}
