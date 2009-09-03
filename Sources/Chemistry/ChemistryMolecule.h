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
    Molecule();

    const Chemistry::Atom& atom(uint16_t index) const;

    uint16_t atomsCount() const;

    uint8_t charge() const;
    void setCharge(uint8_t charge);

    void addAtom(const Chemistry::Atom& atom);

    float interatomicDistance(uint16_t index1, uint16_t index2);

    bool isConnected(uint16_t index1, uint16_t index2) const;
    void connect(uint16_t index1, uint16_t index2);

    void rebond();

  private:
    typedef QList<Chemistry::Bond*> IncidenceList;
    typedef QList<IncidenceList> IncidenceMatrix;

    IncidenceMatrix incidenceMatrix_;

    uint8_t charge_;
    QList<Chemistry::Atom> atoms_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
