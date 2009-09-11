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

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"

namespace Render
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
    Molecule(const Render::Molecule& molecule);
    Render::Molecule& operator=(const Render::Molecule& molecule);

    UnitOfLength unitOfLength() const;
    void setUnitOfLength(UnitOfLength unit);

    Render::Atom& atom(quint16 index);
    Render::Bond& bond(quint16 index);

    const Render::Atom& atomAt(quint16 index) const;
    const Render::Bond& bondAt(quint16 index) const;

    quint16 atomsCount() const;
    quint16 bondsCount() const;

    quint8 charge() const;
    void setCharge(quint8 charge);

    void addAtom(const Render::Atom& atom);

    float interatomicDistance(quint16 index1, quint16 index2);

    bool isAtomsBonded(quint16 index1, quint16 index2) const;
    void bondAtoms(quint16 index1, quint16 index2);

    void rebond();

  private:
    void becomeParentForAtoms();
    typedef QList<int> IncidenceList;
    typedef QList<IncidenceList> IncidenceMatrix;

    QList<Render::Atom> atoms_;
    QList<Render::Bond> bonds_;
    IncidenceMatrix incidenceMatrix_;

    quint8 charge_;
    UnitOfLength unitOfLength_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
