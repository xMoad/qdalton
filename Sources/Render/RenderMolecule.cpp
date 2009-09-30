#include "Render/RenderMolecule.h"
#include "Render/RenderViewer.h"

#include <iostream>
#include <openbabel/mol.h>
#include <openbabel/forcefield.h>
#include <openbabel/builder.h>
#include <openbabel/obconversion.h>

Render::Molecule::Molecule() :
    viewer_(0)
{
}

Render::Molecule::Molecule(Render::Viewer* viewer) :
    viewer_(viewer)
{
}

bool Render::Molecule::importFromFile(Render::Format format,
                                      const QString& fileName)
{
  OpenBabel::OBConversion conv;

  switch (format)
  {
  case (Render::FORMAT_GAUSSIAN_OUTPUT):
    conv.SetInFormat("g03");
    break;
  }

  if (conv.ReadFile(&obMol_, fileName.toStdString()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Render::Molecule::importFromString(Render::Format format,
                                        const QString& string)
{
  std::string stdString;
  std::stringstream ss;
  OpenBabel::OBBuilder obbuilder;
  OpenBabel::OBConversion conv;

  switch (format)
  {
  case (Render::FORMAT_SMILES):
    conv.SetInFormat("smi");
    break;
  case (Render::FORMAT_INCHI):
    conv.SetInFormat("inchi");
    break;
  }

  stdString = string.toStdString();
  ss.str(stdString);
  conv.SetInStream(&ss);

  if (conv.Read(&obMol_))
  {
    obMol_.AddHydrogens();
    obbuilder.Build(obMol_);
    obMol_.Center();
    return true;
  }
  else
  {
    return false;
  }
}

QString Render::Molecule::formula()
{
  return QString::fromStdString(obMol_.GetFormula());
}

OpenBabel::OBAtom* Render::Molecule::obAtom(quint16 index) const
{
  return obMol_.GetAtom(index + 1);
}

OpenBabel::OBBond* Render::Molecule::obBond(quint16 index) const
{
  return obMol_.GetBond(index);
}

void Render::Molecule::addAtom(quint8 atomicNumber)
{
  OpenBabel::OBAtom* obAtom;
  obMol_.BeginModify();
  {
    obAtom = obMol_.NewAtom();
    obAtom->SetAtomicNum(atomicNumber);
  }
  obMol_.EndModify();
}

void Render::Molecule::addObAtom(OpenBabel::OBAtom& obAtom)
{
#ifdef Q_CC_MSVC
#undef AddAtom
#endif
  obMol_.BeginModify();
  {
    obMol_.AddAtom(obAtom);
  }
  obMol_.EndModify();
}

void Render::Molecule::deleteAtom(OpenBabel::OBAtom* obAtom)
{
  obMol_.BeginModify();
  {
    obMol_.DeleteAtom(obAtom);
  }
  obMol_.EndModify();
}

void Render::Molecule::addBond(OpenBabel::OBAtom* beginObAtom,
                               OpenBabel::OBAtom* endObAtom,
                               quint8 bondOrder)
{
  OpenBabel::OBBond* obbond;

  obMol_.BeginModify();
  {
    obbond = obMol_.NewBond();
    obbond->SetBegin(beginObAtom);
    obbond->SetEnd(endObAtom);
    obbond->SetBondOrder(bondOrder);
    beginObAtom->AddBond(obbond);
    endObAtom->AddBond(obbond);
  }
  obMol_.EndModify();
}

void Render::Molecule::setCharge(quint8 charge)
{
  obMol_.SetTotalCharge(charge);
}

quint16 Render::Molecule::atomsCount() const
{
  return obMol_.NumAtoms();
}

quint16 Render::Molecule::bondsCount() const
{
  return obMol_.NumBonds();
}

void Render::Molecule::rebond()
{
  obMol_.ConnectTheDots();
}

void Render::Molecule::build()
{
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(obMol_);
  optimize(Render::FF_UFF,
           Render::ALGORITHM_STEEPEST_DESCENT,
           1.0e-7,
           50,
           0);
  obMol_.Center();
}

void Render::Molecule::addHydrogensAndBuild()
{
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(obMol_);
  obMol_.AddHydrogens();
  obbuilder.Build(obMol_);
  obMol_.Center();
}

void Render::Molecule::removeHydrogens()
{
  obMol_.DeleteHydrogens();
}

void Render::Molecule::optimize(ForceField forceField,
                                Algorithm algorithm,
                                double convergenceCriteria,
                                quint16 maxSteps,
                                quint8 stepsPerUpdate)
{
  OpenBabel::OBForceField* ff;

  switch (forceField)
  {
  case (Render::FF_GHEMICAL):
    ff = OpenBabel::OBForceField::FindForceField("Ghemical");
    break;
  case (Render::FF_MMFF94):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94");
    break;
  case (Render::FF_MMFF94s):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94s");
    break;
  case (Render::FF_UFF):
    ff = OpenBabel::OBForceField::FindForceField("UFF");
    break;
  default:
    break;
  }

  if (!ff)
  {
    std::cout << "No FF!";
  }

  if (!ff->Setup(obMol_))
  {
    std::cout << "Setup error!";
  }

  ff->SetLogFile(&std::cout);
  ff->SetLogLevel(OBFF_LOGLVL_LOW);

  switch (algorithm)
  {
  case (ALGORITHM_STEEPEST_DESCENT):
    if (stepsPerUpdate != 0)
    {
      ff->SteepestDescentInitialize(maxSteps, convergenceCriteria);
      while (ff->SteepestDescentTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obMol_);
        viewer_->updateMolecule();
      }
    }
    else
    {
      ff->SteepestDescent(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obMol_);
    }
    break;
  case (ALGORITHM_CONJUGATE_GRADIENTS):
    if (stepsPerUpdate != 0)
    {
      ff->ConjugateGradientsInitialize(maxSteps, convergenceCriteria);
      while (ff->ConjugateGradientsTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obMol_);
        viewer_->updateMolecule();
      }
    }
    else
    {
      ff->ConjugateGradients(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obMol_);
    }
    break;
  }
  viewer_->updateMolecule();
}
