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

#include <iostream>

#include <QCoreApplication>

#include <openbabel/builder.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include "Render/RenderMolecule.h"

Render::Molecule::Molecule() :
    QObject(),
    obMol_(new OpenBabel::OBMol()),
    listSelectedAtoms_(),
    listSelectedBonds_(),
    planeNormalVector_(Eigen::Vector3f(0.0f, 0.0f, 1.0f))
{
}

Render::Molecule::Molecule(const Render::Molecule& molecule) :
    QObject(),
    obMol_(new OpenBabel::OBMol(*molecule.obMol_)),
    listSelectedAtoms_(molecule.listSelectedAtoms_),
    listSelectedBonds_(molecule.listSelectedBonds_),
    planeNormalVector_(molecule.planeNormalVector_)
{
}

Render::Molecule::~Molecule()
{
  delete obMol_;
}

Render::Molecule& Render::Molecule::operator=(const Render::Molecule& rhs)
                                             {
  if (this == &rhs)
    return *this;

  if (obMol_ != 0)
    delete obMol_;

  obMol_ = new OpenBabel::OBMol(*rhs.obMol_);

  listSelectedAtoms_ = rhs.listSelectedAtoms_;
  listSelectedBonds_ = rhs.listSelectedBonds_;
  planeNormalVector_ = rhs.planeNormalVector_;
  //  emit becameNonempty();
  return *this;
}

Render::Atom Render::Molecule::atom(quint16 index)
{
  return Render::Atom(obAtom(index));
}

Render::Bond Render::Molecule::bond(quint16 index)
{
  return Render::Bond(obBond(index));
}

void Render::Molecule::removeAtom(quint16 index)
{
  QList<quint16> bondsIndices = bondsIndicesForAtom(index);
  for (int i = 0; i < bondsIndices.count(); ++i)
  {
    std::cout << bondsIndices.at(i) << std::endl;
    removeBond(bondsIndices.at(i));
  }
//  for (OpenBabel::OBAtomBondIter b(obAtom(index)); b; ++b)
//  {
//    std::cout << obAtom(index)->GetValence() << "bond" << std::endl;
//    removeBond(b->GetIdx());
//  }

//  obMol_->DeleteAtom(obAtom(index));

  listSelectedAtoms_.removeOne(index);

  for (quint16 i = index + 1; i < atomsCount(); ++i)
  {
    if (listSelectedAtoms_.contains(i))
    {
      listSelectedAtoms_.removeOne(i);
      listSelectedAtoms_.append(i - 1);
    }
  }

  if (this->atomsCount() == 0)
    emit becameEmpty();
}

void Render::Molecule::removeBond(quint16 index)
{
  obMol_->DeleteBond(obBond(index));

  listSelectedBonds_.removeOne(index);

  for (quint16 i = index + 1; i < bondsCount(); ++i)
  {
    if (listSelectedBonds_.contains(i))
    {
      listSelectedBonds_.removeOne(i);
      listSelectedBonds_.append(i - 1);
    }
  }
}

void Render::Molecule::draw(Render::View view, bool fast)
{
  for (int i = 0; i < atomsCount(); ++i)
  {
    if (listSelectedAtoms_.contains(i))
      Render::Atom(obAtom(i)).draw(view, true, fast);
    else
      Render::Atom(obAtom(i)).draw(view, false, fast);
  }

  if (view != Render::ViewVdWSpheres)
  {
    for (int i = 0; i < bondsCount(); ++i)
    {
      if (listSelectedBonds_.contains(i))
        Render::Bond(obBond(i)).draw(view, planeNormalVector(), true, fast);
      else
        Render::Bond(obBond(i)).draw(view, planeNormalVector(), false, fast);
    }
  }
}

void Render::Molecule::drawWithNames(Render::View view)
{
  int n = 0;

  glInitNames();

  for (int i = 0; i < atomsCount(); ++i)
  {
    glPushName(n);
    {
      Render::Atom(obAtom(i)).draw(view, false, false);
    }
    glPopName();
    n++;
  }

  if (view != Render::ViewVdWSpheres)
    for (int i = 0; i < bondsCount(); ++i)
    {
    glPushName(n);
    {
      Render::Bond(obBond(i)).draw(Render::ViewSticks, planeNormalVector(),
                                   false,
                                   false);
    }
    glPopName();
    n++;
  }
}

bool Render::Molecule::importFromFile(const QString& fileName,
                                      OpenBabel::OBFormat* obFormat)
{
  OpenBabel::OBConversion obConversion;

  obConversion.SetInFormat(obFormat);

  if (obConversion.ReadFile(obMol_, fileName.toStdString()))
  {
    obMol_->Center();
    //    emit becameNonempty();
    return true;
  }
  else
    return false;
}

QString Render::Molecule::formula() const
{
  // Change this code! GetFormula() returns wrong formula!
  return QString::fromStdString(obMol_->GetFormula());
}

Render::Atom Render::Molecule::newAtom()
{
  OpenBabel::OBAtom* obAtom = obMol_->NewAtom();

  if (atomsCount() == 1)
    emit becameNonempty();

  return Render::Atom(obAtom);
}

void Render::Molecule::addBond(quint16 beginAtomIndex,
                               quint16 endAtomIndex,
                               quint8 bondOrder)
{
  OpenBabel::OBBond* obBond;

  obMol_->BeginModify();
  {
    obBond = obMol_->NewBond();
    obBond->SetBegin(atom(beginAtomIndex).obAtom());
    obBond->SetEnd(atom(endAtomIndex).obAtom());
    obBond->SetBondOrder(bondOrder);
    atom(beginAtomIndex).obAtom()->AddBond(obBond);
    atom(endAtomIndex).obAtom()->AddBond(obBond);
  }
  obMol_->EndModify();
}

qint8 Render::Molecule::charge() const
{
  return obMol_->GetTotalCharge();
}

void Render::Molecule::setCharge(qint8 charge)
{
  obMol_->SetTotalCharge(charge);
}

quint16 Render::Molecule::atomsCount() const
{
  return obMol_->NumAtoms();
}

quint16 Render::Molecule::bondsCount() const
{
  return obMol_->NumBonds();
}

qreal Render::Molecule::radius() const
{
  if (atomsCount() >= 2)
  {
    qreal r = (obAtom(0)->GetVector() - obMol_->Center(0)).length();
    for (quint16 i = 2; i <= obMol_->NumAtoms(); ++i)
    {
      qreal rr = (obAtom(0)->GetVector() - obMol_->Center(0)).length();
      if (rr > r)
      {
        r = rr;
      }
    }
    return r;
  }
  else
  {
    return 1.0;
  }
}

quint16 Render::Molecule::conformersCount() const
{
  return obMol_->NumConformers();
}

void Render::Molecule::rebond()
{
  obMol_->ConnectTheDots();
  obMol_->PerceiveBondOrders();
}

void Render::Molecule::addHydrogensAndBuild()
{
  OpenBabel::OBBuilder obbuilder;

  obbuilder.Build(*obMol_);
  obMol_->AddHydrogens();
  obbuilder.Build(*obMol_);
  obMol_->Center();

  emit geometryChanged();
}

void Render::Molecule::removeHydrogens()
{
  obMol_->DeleteHydrogens();
  // Clear selections.
  listSelectedAtoms_.clear();
  listSelectedBonds_.clear();
  emit geometryChanged();
}

void Render::Molecule::optimize(const QString& obForceFieldName,
                                quint16 maxSteps,
                                quint8 stepsPerUpdate,
                                std::ostream* logOstream)
{
  quint8 steps = 0;

  OpenBabel::OBForceField* obForceField = OpenBabel::OBForceField::FindForceField(
      obForceFieldName.toStdString());

  if (!obForceField->Setup(*obMol_))
  {
    *logOstream << "Force field setup error." << std::endl;
  }
  else
  {
    obForceField->SetLogFile(logOstream);
    obForceField->SetLogLevel(OBFF_LOGLVL_LOW);

    obForceField->ConjugateGradientsInitialize(maxSteps);
    while (obForceField->ConjugateGradientsTakeNSteps(1))
    {
      steps++;
      if (stepsPerUpdate != 0 && steps == stepsPerUpdate)
      {
        steps = 0;
        obForceField->GetCoordinates(*obMol_);
        emit geometryChanged();
      }
      QCoreApplication::processEvents();
    }

    obForceField->GetCoordinates(*obMol_);
    emit optimizationFinished();
  }
}

void Render::Molecule::searchConformers(const QString& obForceFieldName,
                                        Render::SearchType searchType,
                                        quint16 conformers,
                                        quint16 steps,
                                        std::ostream* logOstream)
{
  OpenBabel::OBForceField* obForceField = OpenBabel::OBForceField::FindForceField(
      obForceFieldName.toStdString());

  if (!obForceField->Setup(*obMol_))
  {
    *logOstream << "Force field setup error." << std::endl;
  }
  else
  {
    obForceField->SetLogFile(logOstream);
    obForceField->SetLogLevel(OBFF_LOGLVL_LOW);

    switch (searchType)
    {
    case Render::SearchTypeSystematicRotor:
      obForceField->SystematicRotorSearch(steps);
      break;
    case Render::SearchTypeRandomRotor:
      obForceField->RandomRotorSearch(conformers, steps);
      break;
    case Render::SearchTypeWeightedRotor:
      obForceField->WeightedRotorSearch(conformers, steps);
      break;
    }

    obForceField->GetConformers(*obMol_);

    emit conformationalSearchFinished();
  }
}

void Render::Molecule::setConformer(quint16 index)
{
  obMol_->SetConformer(index);
  emit geometryChanged();
}

qreal Render::Molecule::conformerEnergy(quint16 index) const
{
  return obMol_->GetEnergy(index);
}

const Eigen::Vector3f& Render::Molecule::planeNormalVector() const
{
  return planeNormalVector_;
}

void Render::Molecule::setPlaneNormalVector(const Eigen::Vector3f& vector)
{
  planeNormalVector_ = vector;
}

void Render::Molecule::toggleSelectionOfAtom(quint16 index)
{
  if (listSelectedAtoms_.contains(index))
    listSelectedAtoms_.removeOne(index);
  else
    listSelectedAtoms_.append(index);
}

void Render::Molecule::toggleSelectionOfBond(quint16 index)
{
  if (listSelectedBonds_.contains(index))
    listSelectedBonds_.removeOne(index);
  else
    listSelectedBonds_.append(index);
}

QList<quint16> Render::Molecule::bondsIndicesForAtom(quint16 index) const
{
  QList<quint16> result;
  for (OpenBabel::OBAtomBondIter b(obAtom(index)); b; ++b)
  {
    std::cout << b->GetIdx() << std::endl;
    result << b->GetIdx();
  }

  return result;
}

OpenBabel::OBAtom* Render::Molecule::obAtom(quint16 index) const
{
  return obMol_->GetAtom(index + 1);
}

OpenBabel::OBBond* Render::Molecule::obBond(quint16 index) const
{
  return obMol_->GetBond(index);
}
