#include "ChemistryMolecule.h"

float tolerance = 0.4f;

Chemistry::Molecule::Molecule()
{
}

const Chemistry::Atom& Chemistry::Molecule::atom(int index) const
{
  return atoms_.at(index);
}

int Chemistry::Molecule::atomsCount() const
{
  return atoms_.count();
}

int Chemistry::Molecule::charge() const
{
  return charge_;
}

void Chemistry::Molecule::setCharge(int charge)
{
  charge_ = charge;
}

void Chemistry::Molecule::addAtom(const Chemistry::Atom& atom)
{
  atoms_ << atom;
}

float Chemistry::Molecule::interatomicDistance(int index1, int index2)
{ 
  return (atoms_[index2].centre() - atoms_[index1].centre()).norm();
}

void Chemistry::Molecule::rebond()
{
  for (int i = 0; i < atoms_.count() - 1; ++i)
  {
    for (int j = i + 1; j < atoms_.count(); ++j)
    {
      if (interatomicDistance(i, j) <
          atoms_[i].covalentRadius() + atoms_[j].covalentRadius() + tolerance)
      {
        atoms_[i].bondWith(j);
        atoms_[j].bondWith(i);
      }
    }
  }
}
