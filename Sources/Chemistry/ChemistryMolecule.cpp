#include "Chemistry/ChemistryMolecule.h"

#include <iostream>
#include <openbabel/mol.h>
#include <openbabel/forcefield.h>
#include <openbabel/builder.h>
#include <openbabel/obconversion.h>

Chemistry::Molecule::Molecule() :
    QObject()
{
}

Chemistry::Molecule::Molecule(const Chemistry::Molecule& molecule) :
    QObject(),
    obMol_(molecule.obMol_)
{
}

Chemistry::Molecule& Chemistry::Molecule::operator=(const Chemistry::Molecule& molecule)
{
  obMol_ = molecule.obMol_;
  emit formulaChanged();
  return *this;
}

bool Chemistry::Molecule::importFromFile(Chemistry::Format format,
                                         const QString& fileName)
{
  OpenBabel::OBConversion conv;

  switch (format)
  {
  case (Chemistry::FormatGaussianOutput):
    conv.SetInFormat("g03");
    break;
  default:
    break;
  }

  if (conv.ReadFile(&obMol_, fileName.toStdString()))
  {
    emit formulaChanged();
    return true;
  }
  else
  {
    return false;
  }
}

bool Chemistry::Molecule::importFromString(Chemistry::Format format,
                                           const QString& string)
{
  std::string stdString;
  std::stringstream ss;
  OpenBabel::OBBuilder obbuilder;
  OpenBabel::OBConversion conv;

  switch (format)
  {
  case (Chemistry::FormatSmiles):
    conv.SetInFormat("smi");
    break;
  case (Chemistry::FormatInchi):
    conv.SetInFormat("inchi");
    break;
  default:
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
    emit formulaChanged();
    return true;
  }
  else
  {
    return false;
  }
}

QString Chemistry::Molecule::toString(Chemistry::Format format)
{
  std::stringstream ss;
  OpenBabel::OBConversion conv;

  switch (format)
  {
  case (Chemistry::FormatXyz):
    conv.SetOutFormat("xyz");
    break;
  default:
    break;
  }

  if (conv.Write(&this->obMol_, &ss))
  {
    return QString::fromStdString(ss.str());
  }
  else
  {
    return "Error";
  }
}

QString Chemistry::Molecule::formula()
{
  return QString::fromStdString(obMol_.GetFormula());
}

OpenBabel::OBAtom* Chemistry::Molecule::obAtom(quint16 index) const
{
  return obMol_.GetAtom(index + 1);
}

OpenBabel::OBBond* Chemistry::Molecule::obBond(quint16 index) const
{
  return obMol_.GetBond(index);
}

void Chemistry::Molecule::addAtom(quint8 atomicNumber)
{
  OpenBabel::OBAtom* obAtom;

  obMol_.BeginModify();
  {
    obAtom = obMol_.NewAtom();
    obAtom->SetAtomicNum(atomicNumber);
  }
  obMol_.EndModify();

  emit formulaChanged();
}

void Chemistry::Molecule::addObAtom(OpenBabel::OBAtom& obAtom)
{
#ifdef Q_CC_MSVC
#undef AddAtom
#endif
  obMol_.BeginModify();
  {
    obMol_.AddAtom(obAtom);
  }
  obMol_.EndModify();

  emit formulaChanged();
}

void Chemistry::Molecule::deleteAtom(OpenBabel::OBAtom* obAtom)
{
  obMol_.BeginModify();
  {
    obMol_.DeleteAtom(obAtom);
  }
  obMol_.EndModify();

  emit formulaChanged();
}

void Chemistry::Molecule::deleteBond(OpenBabel::OBBond* obBond)
{
  obMol_.BeginModify();
  {
    obMol_.DeleteBond(obBond);
  }
  obMol_.EndModify();
}

void Chemistry::Molecule::addBond(OpenBabel::OBAtom* beginObAtom,
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

void Chemistry::Molecule::setCharge(quint8 charge)
{
  obMol_.SetTotalCharge(charge);
}

quint16 Chemistry::Molecule::atomsCount() const
{
  return obMol_.NumAtoms();
}

quint16 Chemistry::Molecule::bondsCount() const
{
  return obMol_.NumBonds();
}

quint16 Chemistry::Molecule::conformersCount()
{
  return obMol_.NumConformers();
}

void Chemistry::Molecule::rebond()
{
  obMol_.ConnectTheDots();
  obMol_.PerceiveBondOrders();
}

void Chemistry::Molecule::build()
{
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(obMol_);
  optimize(Chemistry::ForceFieldMmff94,
           Chemistry::AlgorithmSteepestDescent,
           1.0e-7,
           50,
           0,
           &std::cout);
  obMol_.Center();
}

void Chemistry::Molecule::addHydrogensAndBuild()
{
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(obMol_);
  obMol_.AddHydrogens();
  obbuilder.Build(obMol_);
  obMol_.Center();

  emit formulaChanged();
}

void Chemistry::Molecule::removeHydrogens()
{
  obMol_.DeleteHydrogens();

  emit formulaChanged();
}

void Chemistry::Molecule::optimize(Chemistry::ForceField forceField,
                                   Chemistry::Algorithm algorithm,
                                   double convergenceCriteria,
                                   quint16 maxSteps,
                                   quint8 stepsPerUpdate,
                                   std::ostream* logOstream)
{
  OpenBabel::OBForceField* ff;

  switch (forceField)
  {
  case (Chemistry::ForceFieldGhemical):
    ff = OpenBabel::OBForceField::FindForceField("Ghemical");
    break;
  case (Chemistry::ForceFieldMmff94):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94");
    break;
  case (Chemistry::ForceFieldMmff94s):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94s");
    break;
  case (Chemistry::ForceFieldUff):
    ff = OpenBabel::OBForceField::FindForceField("UFF");
    break;
  default:
    break;
  }

  if (!ff)
  {
  }

  if (!ff->Setup(obMol_))
  {
    std::cout << "Setup error!";
  }

  ff->SetLogFile(logOstream);
  ff->SetLogLevel(OBFF_LOGLVL_LOW);

  /*
  ff->SystematicRotorSearch();
  ff->GetConformers(obMol_);
  */
  switch (algorithm)
  {
  case (Chemistry::AlgorithmSteepestDescent):
    if (stepsPerUpdate != 0)
    {
      ff->SteepestDescentInitialize(maxSteps, convergenceCriteria);
      while (ff->SteepestDescentTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obMol_);
        emit geometryChanged();
      }
    }
    else
    {
      ff->SteepestDescent(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obMol_);
    }
    break;
  case (Chemistry::AlgorithmConjugateGradients):
    if (stepsPerUpdate != 0)
    {
      ff->ConjugateGradientsInitialize(maxSteps, convergenceCriteria);
      while (ff->ConjugateGradientsTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obMol_);
        emit geometryChanged();
      }
    }
    else
    {
      ff->ConjugateGradients(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obMol_);
    }
    break;
  }
  emit geometryChanged();
}

void Chemistry::Molecule::conformationalSearch()
{
  OpenBabel::OBForceField* ff;

  ff = OpenBabel::OBForceField::FindForceField("MMFF94");

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

//  ff->SystematicRotorSearch();
  ff->RandomRotorSearch(10);
  ff->GetConformers(obMol_);
}

void Chemistry::Molecule::setConformer(quint16 index)
{
  obMol_.SetConformer(index);
}

qreal Chemistry::Molecule::conformerEnergy(quint16 index)
{
  return obMol_.GetEnergy(index);
}
