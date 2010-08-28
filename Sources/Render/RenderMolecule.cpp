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
        atoms_(),
        bonds_(),
        incidenceMatrix_(),
        charge_(0),
        planeNormalVector_(Eigen::Vector3f(0.0f, 0.0f, 1.0f))
{
}

Render::Molecule::Molecule(const Render::Molecule& molecule) :
        QObject(),
        atoms_(),
        bonds_(),
        incidenceMatrix_(),
        charge_(molecule.charge_),
        planeNormalVector_(molecule.planeNormalVector_)
{
    for (quint16 i = 0; i < molecule.atomsCount(); ++i)
    {
        newAtom(molecule.atom(i).atomicNumber(),
                molecule.atom(i).isotope(),
                molecule.atom(i).centre());
    }

    for (quint16 i = 0; i < molecule.bondsCount(); ++i)
    {
        newBond(molecule.bond(i).beginAtom().index(),
                molecule.bond(i).endAtom().index(),
                molecule.bond(i).bondOrder());
    }
}

Render::Molecule::~Molecule()
{
}

Render::Atom& Render::Molecule::atom(quint16 index)
{
    return atoms_[index];
}

const Render::Atom& Render::Molecule::atom(quint16 index) const
{
    return atoms_.at(index);
}

Render::Bond& Render::Molecule::bond(quint16 index)
{
    return bonds_[index];
}

const Render::Bond& Render::Molecule::bond(quint16 index) const
{
    return bonds_.at(index);
}

quint16 Render::Molecule::indexOfAtom(const Render::Atom& atom) const
{
    return atoms_.indexOf(atom);
}

quint16 Render::Molecule::indexOfBond(const Render::Bond& bond) const
{
    return bonds_.indexOf(bond);
}

void Render::Molecule::removeAtom(quint16 index)
{
    quint16 bondedAtomIndex;

    const BondsList& bondsList = incidenceMatrix_[index];
    for (quint16 b = 0; b < bondsList.count(); ++b)
    {
        Render::Bond* bond = bondsList[b];
        if (bond->beginAtom().index() == index)
            bondedAtomIndex = bond->endAtom().index();
        else
            bondedAtomIndex = bond->beginAtom().index();

        incidenceMatrix_[bondedAtomIndex].removeOne(bond);

        bonds_.removeOne(*bond);
    }

    incidenceMatrix_.removeAt(index);

    atoms_.removeAt(index);

    if (this->atomsCount() == 0)
        emit becameEmpty();
}

void Render::Molecule::removeBond(quint16 index)
{
    quint16 beginAtomIndex = bond(index).beginAtom().index();
    quint16 endAtomIndex = bond(index).endAtom().index();

    incidenceMatrix_[beginAtomIndex].removeOne(&bonds_[index]);
    incidenceMatrix_[endAtomIndex].removeOne(&bonds_[index]);

    bonds_.removeAt(index);
}

void Render::Molecule::draw(Render::View view, bool fast)
{
    for (int i = 0; i < atomsCount(); ++i)
        atom(i).draw(view, fast);

    if (view != Render::ViewVdWSpheres)
    {
        for (int i = 0; i < bondsCount(); ++i)
            bond(i).draw(view, planeNormalVector(), fast);
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
            atom(i).draw(view, false);
        }
        glPopName();
        n++;
    }

    if (view != Render::ViewVdWSpheres)
        for (int i = 0; i < bondsCount(); ++i)
        {
        glPushName(n);
        {
            bond(i).draw(Render::ViewSticks, planeNormalVector(),
                         false);
        }
        glPopName();
        n++;
    }
}

bool Render::Molecule::importFromFile(const QString& fileName,
                                      OpenBabel::OBFormat* obFormat)
{
    OpenBabel::OBMol obMol;
    OpenBabel::OBConversion obConversion;

    obConversion.SetInFormat(obFormat);

    if (obConversion.ReadFile(&obMol, fileName.toStdString()))
    {
        obMol.Center();
        fromOBMol(obMol);
        return true;
    }
    else
        return false;
}

QString Render::Molecule::formula() const
{
    // Change this code!
    return "formula";
}

void Render::Molecule::newAtom(quint8 atomicNumber,
                               quint8 isotope,
                               const Eigen::Vector3f& centre)
{
    atoms_ << Render::Atom(this,
                           atomicNumber,
                           isotope,
                           centre);

    incidenceMatrix_ << BondsList();

    if (atomsCount() == 1)
        emit becameNonempty();
}

void Render::Molecule::newBond(quint16 beginAtomIndex,
                               quint16 endAtomIndex,
                               quint8 bondOrder)
{
    bonds_ << Render::Bond(this, &atom(beginAtomIndex), &atom(endAtomIndex), bondOrder);
    incidenceMatrix_[beginAtomIndex] << &bond(bondsCount() - 1);
    incidenceMatrix_[endAtomIndex] << &bond(bondsCount() - 1);
}

qint8 Render::Molecule::charge() const
{
    return charge_;
}

void Render::Molecule::setCharge(qint8 charge)
{
    charge_ = charge;
}

quint16 Render::Molecule::atomsCount() const
{
    return atoms_.count();
}

quint16 Render::Molecule::bondsCount() const
{
    return bonds_.count();
}

float Render::Molecule::radius() const
{
    if (atomsCount() >= 2)
    {
        qreal r = atom(0).centre().norm();
        for (quint16 i = 1; i < atomsCount(); ++i)
        {
            qreal rr = atom(i).centre().norm();
            if (rr > r)
                r = rr;
        }
        return r;
    }
    else
        return 10.0;
}

//quint16 Render::Molecule::conformersCount() const
//{
//  return 1;
//}

void Render::Molecule::rebond()
{
    OpenBabel::OBMol obMol = toOBMol();
    obMol.ConnectTheDots();
    obMol.PerceiveBondOrders();
    fromOBMol(obMol);
}

void Render::Molecule::addHydrogensAndBuild()
{
    OpenBabel::OBMol obMol(toOBMol());
    OpenBabel::OBBuilder obbuilder;

    obbuilder.Build(obMol);
    obMol.AddHydrogens();
    obbuilder.Build(obMol);
    fromOBMol(obMol);
    setOriginOfAxesToCentreOfMass();

    emit geometryChanged();
}

void Render::Molecule::removeHydrogens()
{
    //  for (quint16 )
    //  obMol_->DeleteHydrogens();
    //  // Clear selections.
    //  listSelectedAtoms_.clear();
    //  listSelectedBonds_.clear();
    //  emit geometryChanged();
}

void Render::Molecule::optimize(const QString& obForceFieldName,
                                quint16 maxSteps,
                                quint8 stepsPerUpdate,
                                std::ostream* logOstream)
{
    OpenBabel::OBMol obMol = toOBMol();
    quint8 steps = 0;

    OpenBabel::OBForceField* obForceField = OpenBabel::OBForceField::FindForceField(
            obForceFieldName.toStdString());

    if (!obForceField->Setup(obMol))
    {
        *logOstream << "Force field setup error." << std::endl;
        emit optimizationFinished();
    }
    else
    {
        obForceField->SetLogFile(logOstream);
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
                fromOBMol(obMol);
                emit geometryChanged();
            }
            QCoreApplication::processEvents();
        }

        obForceField->GetCoordinates(obMol);
        fromOBMol(obMol);
        setOriginOfAxesToCentreOfMass();

        emit optimizationFinished();
    }
}

//void Render::Molecule::searchConformers(const QString& obForceFieldName,
//                                        Render::SearchType searchType,
//                                        quint16 conformers,
//                                        quint16 steps,
//                                        std::ostream* logOstream)
//{
//  OpenBabel::OBMol obMol = toOBMol();
//
//  OpenBabel::OBForceField* obForceField = OpenBabel::OBForceField::FindForceField(
//      obForceFieldName.toStdString());
//
//  if (!obForceField->Setup(obMol))
//  {
//    *logOstream << "Force field setup error." << std::endl;
//  }
//  else
//  {
//    obForceField->SetLogFile(logOstream);
//    obForceField->SetLogLevel(OBFF_LOGLVL_LOW);
//
//    switch (searchType)
//    {
//    case Render::SearchTypeSystematicRotor:
//      obForceField->SystematicRotorSearch(steps);
//      break;
//    case Render::SearchTypeRandomRotor:
//      obForceField->RandomRotorSearch(conformers, steps);
//      break;
//    case Render::SearchTypeWeightedRotor:
//      obForceField->WeightedRotorSearch(conformers, steps);
//      break;
//    }
//
//    obForceField->GetConformers(obMol);
//
//    emit conformationalSearchFinished();
//  }
//}

//void Render::Molecule::setConformer(quint16 index)
//{
//  obMol_->SetConformer(index);
//  emit geometryChanged();
//}
//
//qreal Render::Molecule::conformerEnergy(quint16 index) const
//{
//  return 1.0;
//}

const Eigen::Vector3f& Render::Molecule::planeNormalVector() const
{
    return planeNormalVector_;
}

void Render::Molecule::setPlaneNormalVector(const Eigen::Vector3f& vector)
{
    planeNormalVector_ = vector;
}

void Render::Molecule::clear()
{
    atoms_.clear();
    bonds_.clear();
    incidenceMatrix_.clear();
    charge_ = 0;
}

Eigen::Vector3f Render::Molecule::centreOfMass() const
{
    if (atomsCount() > 0)
    {
        Eigen::Vector3f numerator = atom(0).exactMass() * atom(0).centre();
        float denominator = atom(0).exactMass();

        for (quint16 i = 1; i < atomsCount();  ++i)
        {
            numerator += atom(i).exactMass() * atom(i).centre();
            denominator += atom(i).exactMass();
        }

        return numerator / denominator;
    }
    else
        return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
}

void Render::Molecule::setOriginOfAxesToCentreOfMass()
{
    Eigen::Vector3f centreOfMass = this->centreOfMass();
    for (quint16 i = 0; i < atomsCount(); ++i)
    {
        atom(i).setCentre(atom(i).centre() - centreOfMass);
    }
}

OpenBabel::OBMol Render::Molecule::toOBMol() const
{
#ifdef Q_CC_MSVC
#undef AddAtom
#endif
    OpenBabel::OBMol obMol;

    for (quint16 i = 0; i < atomsCount(); ++i)
    {
        OpenBabel::OBAtom obAtom;
        obAtom.SetAtomicNum(atom(i).atomicNumber());
        obAtom.SetIsotope(atom(i).isotope());
        obAtom.SetVector(atom(i).centre().x(),
                         atom(i).centre().y(),
                         atom(i).centre().z());
        obMol.AddAtom(obAtom);
    }

    for (quint16 i = 0; i < bondsCount(); ++i)
    {
        obMol.AddBond(bond(i).beginAtom().index() + 1,
                      bond(i).endAtom().index() + 1,
                      bond(i).bondOrder());
    }

    obMol.SetTotalCharge(charge_);

    return obMol;
}

void Render::Molecule::fromOBMol(OpenBabel::OBMol& obMol)
{
    clear();

    for (quint16 i = 1; i <= obMol.NumAtoms(); ++i)
    {
        OpenBabel::OBAtom* obAtom = obMol.GetAtom(i);
        newAtom(obAtom->GetAtomicNum(),
                obAtom->GetIsotope(),
                Eigen::Vector3f(obAtom->GetX(), obAtom->GetY(), obAtom->GetZ()));
    }

    for (quint16 i = 0; i < obMol.NumBonds(); ++i)
    {
        OpenBabel::OBBond* obBond = obMol.GetBond(i);
        newBond(obBond->GetBeginAtomIdx() - 1,
                obBond->GetEndAtomIdx() - 1,
                obBond->GetBondOrder());
    }

    charge_ = obMol.GetTotalCharge();
}
