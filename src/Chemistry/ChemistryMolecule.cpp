/**********************************************************************
 **********************************************************************/

#include <iostream>

#include <QCoreApplication>

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include "Chemistry/ChemistryMolecule.h"

Chemistry::Molecule::Molecule() :
    QObject(),
    atoms_(),
    bonds_(),
    incidenceMatrix_(),
    charge_(0),
    planeNormalVector_(Eigen::Vector3f(0.0f, 0.0f, 1.0f))
{
}

Chemistry::Molecule::Molecule(const Chemistry::Molecule& molecule) :
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

Chemistry::Molecule::~Molecule()
{
}

Chemistry::Atom& Chemistry::Molecule::atom(quint16 index)
{
  return atoms_[index];
}

const Chemistry::Atom& Chemistry::Molecule::atom(quint16 index) const
{
  return atoms_.at(index);
}

Chemistry::Bond& Chemistry::Molecule::bond(quint16 index)
{
  return bonds_[index];
}

const Chemistry::Bond& Chemistry::Molecule::bond(quint16 index) const
{
  return bonds_.at(index);
}

quint16 Chemistry::Molecule::indexOfAtom(const Chemistry::Atom& atom) const
{
  return atoms_.indexOf(atom);
}

quint16 Chemistry::Molecule::indexOfBond(const Chemistry::Bond& bond) const
{
  return bonds_.indexOf(bond);
}

void Chemistry::Molecule::removeAtom(quint16 index)
{
  quint16 bondedAtomIndex;

  const BondsList& bondsList = incidenceMatrix_[index];
  for (quint16 b = 0; b < bondsList.count(); ++b)
  {
    Chemistry::Bond* bond = bondsList[b];
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

void Chemistry::Molecule::removeBond(quint16 index)
{
  quint16 beginAtomIndex = bond(index).beginAtom().index();
  quint16 endAtomIndex = bond(index).endAtom().index();

  incidenceMatrix_[beginAtomIndex].removeOne(&bonds_[index]);
  incidenceMatrix_[endAtomIndex].removeOne(&bonds_[index]);

  bonds_.removeAt(index);
}

bool Chemistry::Molecule::importFromFile(const QString& fileName,
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

QString Chemistry::Molecule::formula() const
{
  // Change this code!
  return "formula";
}

void Chemistry::Molecule::newAtom(quint8 atomicNumber,
                                  quint8 isotope,
                                  const Eigen::Vector3f& centre)
{
  atoms_ << Chemistry::Atom(*this, atomicNumber, isotope, centre);

  incidenceMatrix_ << BondsList();

  if (atomsCount() == 1)
    emit becameNonempty();
}

void Chemistry::Molecule::newBond(quint16 beginAtomIndex,
                                  quint16 endAtomIndex,
                                  quint8 bondOrder)
{
  bonds_ << Chemistry::Bond(*this, &atom(beginAtomIndex), &atom(endAtomIndex), bondOrder);
  incidenceMatrix_[beginAtomIndex] << &bond(bondsCount() - 1);
  incidenceMatrix_[endAtomIndex] << &bond(bondsCount() - 1);
}

qint8 Chemistry::Molecule::charge() const
{
  return charge_;
}

void Chemistry::Molecule::setCharge(qint8 charge)
{
  charge_ = charge;
}

quint16 Chemistry::Molecule::atomsCount() const
{
  return atoms_.count();
}

quint16 Chemistry::Molecule::bondsCount() const
{
  return bonds_.count();
}

float Chemistry::Molecule::radius() const
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

//quint16 Chemistry::Molecule::conformersCount() const
//{
//  return 1;
//}

void Chemistry::Molecule::rebond()
{
  OpenBabel::OBMol obMol = toOBMol();
  obMol.ConnectTheDots();
  obMol.PerceiveBondOrders();
  fromOBMol(obMol);
}

void Chemistry::Molecule::removeHydrogens()
{
  OpenBabel::OBMol obMol(this->toOBMol());
  obMol.DeleteHydrogens();
  this->fromOBMol(obMol);
  emit repaintingIsNecessary();
}

//void Chemistry::Molecule::searchConformers(const QString& obForceFieldName,
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

//void Chemistry::Molecule::setConformer(quint16 index)
//{
//  obMol_->SetConformer(index);
//  emit geometryChanged();
//}
//
//qreal Chemistry::Molecule::conformerEnergy(quint16 index) const
//{
//  return 1.0;
//}

const Eigen::Vector3f& Chemistry::Molecule::planeNormalVector() const
{
  return planeNormalVector_;
}

void Chemistry::Molecule::setPlaneNormalVector(const Eigen::Vector3f& vector)
{
  planeNormalVector_ = vector;
}

void Chemistry::Molecule::clear()
{
  atoms_.clear();
  bonds_.clear();
  incidenceMatrix_.clear();
  charge_ = 0;
}

Eigen::Vector3f Chemistry::Molecule::centreOfMass() const
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

void Chemistry::Molecule::setOriginOfAxesToCentreOfMass()
{
  Eigen::Vector3f centreOfMass = this->centreOfMass();
  for (quint16 i = 0; i < atomsCount(); ++i)
  {
    atom(i).setCentre(atom(i).centre() - centreOfMass);
  }
}

OpenBabel::OBMol Chemistry::Molecule::toOBMol() const
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

void Chemistry::Molecule::fromOBMol(OpenBabel::OBMol& obMol)
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
