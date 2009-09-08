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

#ifndef CHEMISTRY_ATOM_H
#define CHEMISTRY_ATOM_H

#include <QList>

#include <Eigen/Array>

namespace Chemistry
{
  class Molecule;

  /**
   * @class Chemistry::Atom ChemistryAtom.h "Chemistry/ChemistryAtom.h"
   * @brief Represents atom from the chemical point of view.
   * @author Anton Simakov
   * @version 0.1
   */
  class Atom
  {
  public:
    /** Constructor.
     * @param protons is is the number of protons.
     * @param neutrons is the number of neutrons.
     * .
     */
    Atom(quint8 protons,
         quint8 neutrons = 0);

    Atom(const Chemistry::Atom& atom);

    void setParentMolecule(Chemistry::Molecule* molecule);
    
    quint8 protons() const;
    quint8 neutrons() const;
    
    quint16 relativeAtomicMass() const;
    
    const Eigen::Vector3f& centre() const;
    void setCentre(const Eigen::Vector3f& centre);
    
    float covalentRadius() const;

  private:
    Chemistry::Molecule* molecule_;
    quint8 protons_;
    quint8 neutrons_;
    Eigen::Vector3f centre_;

    /** Default table of covalent Radii.
     * Values (milliangstrom)  taken from <a href="http://openbabel.org/">OpenBabel</a>.
     */
    static const quint16 covalentRadii_[];
  };
}

#endif // CHEMISTRY_ATOM_H
