/**********************************************************************
 **********************************************************************/

#include <QDebug>
#include <cmath>

//#include <openbabel/mol.h>

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryBond.h"
#include "Chemistry/ChemistryMolecule.h"

Chemistry::Atom::Atom(Chemistry::Molecule& molecule,
                      quint8 atomicNumber,
                      quint8 isotope,
                      const Eigen::Vector3f& centre) :
    molecule_(molecule),
    atomicNumber_(atomicNumber),
    isotope_(isotope),
    centre_(centre),
    selected_(false)
{
}

Chemistry::Atom::Atom(const Chemistry::Atom& atom) :
    molecule_(atom.molecule_),
    atomicNumber_(atom.atomicNumber_),
    isotope_(atom.isotope_),
    centre_(atom.centre_),
    selected_(atom.selected_)
{
}

//Chemistry::Atom& Chemistry::Atom::operator=(const Chemistry::Atom& rhs)
//                                           {
//  if (this == &rhs)
//    return *this;

//  molecule_ = rhs.molecule_;
//  atomicNumber_ = rhs.atomicNumber_;
//  isotope_ = rhs.isotope_;
//  centre_ = rhs.centre_;
//  selected_ = rhs.selected_;

//  return *this;
//}

bool Chemistry::Atom::operator==(const Chemistry::Atom& atom) const
{
  return this == &atom;
}

bool Chemistry::Atom::operator!=(const Chemistry::Atom& atom) const
{
  return !(*this == atom);
}

quint16 Chemistry::Atom::index() const
{
  return molecule_.indexOfAtom(*this);
}

float Chemistry::Atom::exactMass() const
{
  return OpenBabel::isotab.GetExactMass(atomicNumber_, isotope_);
}

quint8 Chemistry::Atom::atomicNumber() const
{
  return atomicNumber_;
}

void Chemistry::Atom::setAtomicNumber(quint8 atomicNumber)
{
  atomicNumber_ = atomicNumber;
}

quint8 Chemistry::Atom::isotope() const
{
  return isotope_;
}

void Chemistry::Atom::setIsotope(quint8 isotope)
{
  isotope_ = isotope;
}

QString Chemistry::Atom::symbol() const
{
  return QString(OpenBabel::etab.GetSymbol(atomicNumber()));
}

double Chemistry::Atom::vanDerWaalsRadius() const
{
  return OpenBabel::etab.GetVdwRad(atomicNumber());
}

Eigen::Vector3f Chemistry::Atom::centre() const
{
  return centre_;
}

void Chemistry::Atom::setCentre(const Eigen::Vector3f& centre)
{
  centre_ = centre;
}

bool Chemistry::Atom::isSelected() const
{
  return selected_;
}

void Chemistry::Atom::setSelected(bool selected)
{
  selected_ = selected;
}

void Chemistry::Atom::toggleSelection()
{
  selected_ = !selected_;
}
