#include <iostream>
#include <QCoreApplication>
#include <openbabel/builder.h>
#include <openbabel/obconversion.h>

#include "ChemistryMolecularMechanics.h"

Chemistry::MolecularMechanics::MolecularMechanics()
{
}

void Chemistry::MolecularMechanics::optimize(Chemistry::Molecule& molecule,
                                             const QString& obForceFieldName,
                                             quint16 maxSteps,
                                             quint8 stepsPerUpdate,
                                             std::ostream& logOstream)
{
  // Do not delete!
  OpenBabel::OBConversion conv;

  OpenBabel::OBMol obMol(molecule.toOBMol());
  quint8 steps = 0;

  OpenBabel::OBForceField* obForceField = OpenBabel::OBForceField::FindForceField(
      obForceFieldName.toStdString());

  if (!obForceField->Setup(obMol))
  {
    logOstream << "Force field setup error." << std::endl;
    emit optimizationFinished();
  }
  else
  {
    obForceField->SetLogFile(&logOstream);
    obForceField->SetLogLevel(OBFF_LOGLVL_LOW);

    //    obForceField->ConjugateGradientsInitialize(maxSteps);
    obForceField->SteepestDescentInitialize(maxSteps);
    //    while (obForceField->ConjugateGradientsTakeNSteps(1))
    while (obForceField->SteepestDescentTakeNSteps(1))
    {
      steps++;
      if (stepsPerUpdate != 0 && steps == stepsPerUpdate)
      {
        steps = 0;
        obForceField->GetCoordinates(obMol);
        molecule.fromOBMol(obMol);
        emit geometryChanged();
      }
      QCoreApplication::processEvents();
    }

    obForceField->GetCoordinates(obMol);
    molecule.fromOBMol(obMol);
    molecule.setOriginOfAxesToCentreOfMass();

    emit optimizationFinished();
  }
}

void Chemistry::MolecularMechanics::addHydrogensAndBuild(Chemistry::Molecule& molecule)
{
  OpenBabel::OBMol obMol(molecule.toOBMol());
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(obMol);
  obMol.AddHydrogens();
  obbuilder.Build(obMol);
  molecule.fromOBMol(obMol);
  molecule.setOriginOfAxesToCentreOfMass();

  emit geometryChanged();
}
