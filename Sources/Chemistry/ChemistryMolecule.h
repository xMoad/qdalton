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

#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <QList>

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryBond.h"

namespace Chemistry
{
  class Molecule
  {
  public:
    enum UnitOfLength
    {
      ANGSTROM,
      BOHR
    };

    Molecule();
    Molecule(const Chemistry::Molecule& molecule);
    Chemistry::Molecule& operator=(const Chemistry::Molecule& molecule);

    UnitOfLength unitOfLength() const;
    void setUnitOfLength(UnitOfLength unit);

    Chemistry::Atom* atomPointer(quint16 index) const;
    Chemistry::Bond* bondPointer(quint16 index) const;

    quint16 atomsCount() const;
    quint16 bondsCount() const;

    quint8 charge() const;
    void setCharge(quint8 charge);

    void addAtom(const Chemistry::Atom& atom);

    float interatomicDistance(quint16 index1, quint16 index2);

    bool isAtomsBonded(quint16 index1, quint16 index2) const;
    void bondAtoms(quint16 index1, quint16 index2);

    void rebond();

  private:
    typedef QList<int> IncidenceList;
    typedef QList<IncidenceList> IncidenceMatrix;

    QList<Chemistry::Atom*> atoms_;
    QList<Chemistry::Bond*> bonds_;
    IncidenceMatrix incidenceMatrix_;

    quint8 charge_;
    UnitOfLength unitOfLength_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
