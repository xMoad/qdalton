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

    Render::Atom& atom(quint16 index);
    const Render::Atom& atom(quint16 index) const;

    Render::Bond& bond(quint16 index);
    const Render::Bond& bond(quint16 index) const;

    quint16 indexOfAtom(const Render::Atom& atom) const;
    quint16 indexOfBond(const Render::Bond& bond) const;

    void newAtom(quint8 atomicNumber,
                 quint8 isotope,
                 const Eigen::Vector3f& centre);

    void newBond(quint16 beginAtomIndex,
                 quint16 endAtomIndex,
                 quint8 bondOrder);

    void removeAtom(quint16 index);
    void removeBond(quint16 index);

    void draw(Render::View view, bool fast);
    void drawWithNames(Render::View view);

    bool importFromFile(const QString& fileName, OpenBabel::OBFormat* obFormat);

    qint8 charge() const;
    void setCharge(qint8 charge);

    QString formula() const;

    quint16 atomsCount() const;
    quint16 bondsCount() const;

    float radius() const;
//    quint16 conformersCount() const;

    void rebond();

    void optimize(const QString& obForceFieldName,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate,
                  std::ostream* logOstream);

//    void searchConformers(const QString& obForceFieldName,
//                          Render::SearchType searchType,
//                          quint16 conformers,
//                          quint16 steps,
//                          std::ostream* logOstream);

//    void setConformer(quint16 index);
//    qreal conformerEnergy(quint16 index) const;

    const Eigen::Vector3f& planeNormalVector() const;
    void setPlaneNormalVector(const Eigen::Vector3f& vector);

    void clear();

    Eigen::Vector3f centreOfMass() const;
    void setOriginOfAxesToCentreOfMass();

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
    Render::Molecule& operator=(const Render::Molecule& rhs);

    OpenBabel::OBMol toOBMol() const;
    void fromOBMol(OpenBabel::OBMol& obMol);

    typedef QList<Render::Bond*> BondsList;
    typedef QList<BondsList> IncidenceMatrix;

    QList<Render::Atom> atoms_;
    QList<Render::Bond> bonds_;
    IncidenceMatrix incidenceMatrix_;

    quint8 charge_;
    Eigen::Vector3f planeNormalVector_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
