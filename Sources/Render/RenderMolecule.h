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

#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <QObject>
#include <QString>

#include <openbabel/mol.h>
#include <openbabel/forcefield.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderConstants.h"

namespace OpenBabel
{
  class OBFormat;
}

namespace Render
{

  enum SearchType
  {
    SearchTypeSystematicRotor,
    SearchTypeRandomRotor,
    SearchTypeWeightedRotor
  };

  class Molecule : public QObject
  {
    Q_OBJECT

  public:
    Molecule();
    Molecule(const Render::Molecule& molecule);
    ~Molecule();

    Render::Molecule& operator=(const Render::Molecule& rhs);

    Render::Atom atom(quint16 index);
    Render::Bond bond(quint16 index);

    void removeAtom(quint16 index);
    void removeBond(quint16 index);

    void draw(Render::View view, bool fast);
    void drawWithNames(Render::View view);

    bool importFromFile(const QString& fileName, OpenBabel::OBFormat* obFormat);

    Render::Atom newAtom();

    void addBond(quint16 beginAtomIndex,
                 quint16 endAtomIndex,
                 quint8 bondOrder);

    qint8 charge() const;
    void setCharge(qint8 charge);

    QString formula() const;

    quint16 atomsCount() const;
    quint16 bondsCount() const;

    qreal radius() const;
    quint16 conformersCount() const;

    void rebond();

    void optimize(const QString& obForceFieldName,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate,
                  std::ostream* logOstream);

    void searchConformers(const QString& obForceFieldName,
                          Render::SearchType searchType,
                          quint16 conformers,
                          quint16 steps,
                          std::ostream* logOstream);

    void setConformer(quint16 index);
    qreal conformerEnergy(quint16 index) const;

    const Eigen::Vector3f& planeNormalVector() const;
    void setPlaneNormalVector(const Eigen::Vector3f& vector);

    void toggleSelectionOfAtom(quint16 index);
    void toggleSelectionOfBond(quint16 index);

    QList<quint16> bondsIndicesForAtom(quint16 index) const;

  public slots:
    void addHydrogensAndBuild();
    void removeHydrogens();

  signals:
    void becameEmpty();
    void becameNonempty();
    void geometryChanged();
    void optimizationFinished();
    void conformationalSearchFinished();

  private:
    OpenBabel::OBAtom* obAtom(quint16 index) const;
    OpenBabel::OBBond* obBond(quint16 index) const;
    OpenBabel::OBMol* obMol_;
    QList<quint16> listSelectedAtoms_;
    QList<quint16> listSelectedBonds_;
    Eigen::Vector3f planeNormalVector_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
