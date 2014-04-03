/**********************************************************************
 **********************************************************************/

//#include <openbabel/mol.h>

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryBond.h"
#include "Chemistry/ChemistryMolecule.h"

Chemistry::Bond::Bond(Chemistry::Molecule& molecule,
                      Chemistry::Atom* beginAtom,
                      Chemistry::Atom* endAtom,
                      quint8 bondOrder) :
molecule_(molecule),
beginAtom_(beginAtom),
endAtom_(endAtom),
bondOrder_(bondOrder),
selected_(false)
{
}

Chemistry::Bond::Bond(const Chemistry::Bond& bond) :
    molecule_(bond.molecule_),
    beginAtom_(bond.beginAtom_),
    endAtom_(bond.endAtom_),
    bondOrder_(bond.bondOrder_),
    selected_(bond.selected_)
{
}

//Chemistry::Bond& Chemistry::Bond::operator=(const Chemistry::Bond& rhs)
//                                           {
//  if (this == &rhs)
//    return *this;

//  molecule_ = rhs.molecule_;
//  beginAtom_ = rhs.beginAtom_;
//  endAtom_ = rhs.endAtom_;
//  bondOrder_ = rhs.bondOrder_;
//  selected_ = rhs.selected_;

//  return *this;
//}

bool Chemistry::Bond::operator==(const Chemistry::Bond& bond) const
{
  return this == &bond;
}

bool Chemistry::Bond::operator!=(const Chemistry::Bond& bond) const
{
  return !(*this == bond);
}

quint16 Chemistry::Bond::index() const
{
  return molecule_.indexOfBond(*this);
}

const Chemistry::Atom& Chemistry::Bond::beginAtom() const
{
  return *beginAtom_;
}

const Chemistry::Atom& Chemistry::Bond::endAtom() const
{
  return *endAtom_;
}

quint8 Chemistry::Bond::bondOrder() const
{
  return bondOrder_;
}

void Chemistry::Bond::setBondOrder(quint8 bondOrder)
{
  bondOrder_ = bondOrder;
}

void Chemistry::Bond::cycleBondOrder()
{
  quint8 order = bondOrder();

  if (order == 3)
    setBondOrder(1);
  else
    setBondOrder(order + 1);
}

float Chemistry::Bond::length() const
{
  return (endAtom_->centre() - beginAtom_->centre()).norm();
}

Eigen::Vector3f Chemistry::Bond::centre() const
{
  return (endAtom_->centre() + beginAtom_->centre()) / 2;
}

bool Chemistry::Bond::isSelected() const
{
  return selected_;
}

void Chemistry::Bond::toggleSelection()
{
  selected_ = !selected_;
}
