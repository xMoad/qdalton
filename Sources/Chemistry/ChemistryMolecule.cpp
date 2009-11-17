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

#include "Chemistry/ChemistryMolecule.h"

#include <iostream>
#include <openbabel/mol.h>

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
  optimize(OpenBabel::OBForceField::FindForceField("MMFF94"),
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

void Chemistry::Molecule::optimize(OpenBabel::OBForceField* obForceField,
                                   Chemistry::Algorithm algorithm,
                                   double convergenceCriteria,
                                   quint16 maxSteps,
                                   quint8 stepsPerUpdate,
                                   std::ostream* logOstream)
{
  if (!obForceField->Setup(obMol_))
  {
    *logOstream << "Force field setup error." << std::endl;
  }
  else
  {
    obForceField->SetLogFile(logOstream);
    obForceField->SetLogLevel(OBFF_LOGLVL_LOW);

    switch (algorithm)
    {
    case (Chemistry::AlgorithmSteepestDescent):
      if (stepsPerUpdate != 0)
      {
        obForceField->SteepestDescentInitialize(maxSteps, convergenceCriteria);
        while (obForceField->SteepestDescentTakeNSteps(stepsPerUpdate))
        {
          obForceField->GetCoordinates(obMol_);
          emit geometryChanged();
        }
      }
      else
      {
        obForceField->SteepestDescent(maxSteps, convergenceCriteria);
        obForceField->GetCoordinates(obMol_);
      }
      break;
    case (Chemistry::AlgorithmConjugateGradients):
      if (stepsPerUpdate != 0)
      {
        obForceField->ConjugateGradientsInitialize(maxSteps, convergenceCriteria);
        while (obForceField->ConjugateGradientsTakeNSteps(stepsPerUpdate))
        {
          obForceField->GetCoordinates(obMol_);
          emit geometryChanged();
        }
      }
      else
      {
        obForceField->ConjugateGradients(maxSteps, convergenceCriteria);
        obForceField->GetCoordinates(obMol_);
      }
      break;
    }
    emit geometryChanged();
  }
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
