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

#ifndef RENDER_ATOM_H
#define RENDER_ATOM_H

#include <QString>
#include <Eigen/Array>

namespace Chemistry
{
  class Molecule;

  /**
   * @class Atom Atom.h "Chemistry/ChemistryAtom.h"
   * @brief Represents an atom.
   * @author Anton Simakov
   * @version 0.1
   */
  class Atom
  {
  public:
    Atom(Chemistry::Molecule& molecule,
         quint8 atomicNumber,
         quint8 isotope,
         const Eigen::Vector3f& centre);

    Atom(const Chemistry::Atom& atom);

    Chemistry::Atom& operator=(const Chemistry::Atom& rhs);

    bool operator==(const Chemistry::Atom& atom) const;
    bool operator!=(const Chemistry::Atom& atom) const;

    quint16 index() const;

    float exactMass() const;

    quint8 atomicNumber() const;
    void setAtomicNumber(quint8 atomicNumber);

    quint8 isotope() const;
    void setIsotope(quint8 isotope);

    QString symbol() const;

    double vanDerWaalsRadius() const;

    Eigen::Vector3f centre() const;
    void setCentre(const Eigen::Vector3f& centre);

    bool isSelected() const;
    void setSelected(bool selected);
    void toggleSelection();

  private:
    Chemistry::Molecule& molecule_;
    quint8 atomicNumber_;
    quint8 isotope_;
    Eigen::Vector3f centre_;
    bool selected_;
  };
}

#endif // RENDER_ATOM_H
