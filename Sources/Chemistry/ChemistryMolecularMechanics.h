#ifndef CHEMISTRY_MOLECULAR_MECHANICS_H
#define CHEMISTRY_MOLECULAR_MECHANICS_H

#include <QString>

#include "Chemistry/ChemistryMolecule.h"

namespace Chemistry {

  class MolecularMechanics : public QObject
  {
    Q_OBJECT

  public:
    MolecularMechanics();
    void optimize(Chemistry::Molecule& molecule,
                  const QString& obForceFieldName,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate,
                  std::ostream& logOstream);
    void addHydrogensAndBuild(Chemistry::Molecule& molecule);

  signals:
    void optimizationFinished();
    void geometryChanged();
  };

}

#endif // CHEMISTRY_MOLECULAR_MECHANICS_H
