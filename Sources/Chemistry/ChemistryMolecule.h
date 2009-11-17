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

#include <QObject>
#include <QString>

#include <openbabel/mol.h>
#include <openbabel/forcefield.h>

#include "Chemistry/ChemistryForceField.h"
#include "Chemistry/ChemistryFormat.h"

namespace Chemistry
{
  enum Algorithm
  {
    AlgorithmSteepestDescent,
    AlgorithmConjugateGradients
  };

  class Molecule : public QObject
  {
    Q_OBJECT

  public:
    Molecule();
    Molecule(const Chemistry::Molecule& molecule);

    Chemistry::Molecule& operator=(const Chemistry::Molecule& molecule);

    bool importFromFile(Chemistry::Format format,
                        const QString& fileName);
    bool importFromString(Chemistry::Format format,
                          const QString& string);
    QString toString(Chemistry::Format format);

    OpenBabel::OBAtom* obAtom(quint16 index) const;
    OpenBabel::OBBond* obBond(quint16 index) const;

    void addAtom(quint8 atomicNumber);
    void addObAtom(OpenBabel::OBAtom& obAtom);
    void deleteAtom(OpenBabel::OBAtom* obAtom);
    void deleteBond(OpenBabel::OBBond* obBond);

    void addBond(OpenBabel::OBAtom* beginObAtom,
                 OpenBabel::OBAtom* endObAtom,
                 quint8 bondOrder);

    void setCharge(quint8 charge);

    QString formula();

    quint16 atomsCount() const;
    quint16 bondsCount() const;
    quint16 conformersCount();

    void rebond();
    void build();
    void addHydrogensAndBuild();
    void removeHydrogens();

    void optimize(OpenBabel::OBForceField* obForceField,
                  Chemistry::Algorithm algorithm,
                  double convergenceCriteria,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate,
                  std::ostream* logOstream);
    void conformationalSearch();
    void setConformer(quint16 index);
    qreal conformerEnergy(quint16 index);

  signals:
    void formulaChanged();
    void geometryChanged();

  private:
    OpenBabel::OBMol obMol_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
