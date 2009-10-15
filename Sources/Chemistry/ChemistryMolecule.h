#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <QObject>
#include <QString>

#include <openbabel/mol.h>

#include "Chemistry/ChemistryForceField.h"
#include "Chemistry/ChemistryFormat.h"

namespace Chemistry
{
  enum Algorithm
  {
    ALGORITHM_STEEPEST_DESCENT,
    ALGORITHM_CONJUGATE_GRADIENTS
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

    OpenBabel::OBAtom* obAtom(quint16 index) const;
    OpenBabel::OBBond* obBond(quint16 index) const;

    void addAtom(quint8 atomicNumber);
    void addObAtom(OpenBabel::OBAtom& obAtom);
    void deleteAtom(OpenBabel::OBAtom* obAtom);

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

    void optimize(Chemistry::ForceField forceField,
                  Chemistry::Algorithm algorithm,
                  double convergenceCriteria,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate = 0);
    void conformationalSearch();
    void setConformer(quint16 index);
    qreal conformerEnergy(quint16 index);

  signals:
    void geometryChanged();

  private:
    OpenBabel::OBMol obMol_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
