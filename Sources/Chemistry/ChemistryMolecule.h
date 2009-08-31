#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <QList>

#include "Chemistry/ChemistryAtom.h"

namespace Chemistry
{
  class Molecule
  {
  public:
    Molecule();

    const Chemistry::Atom& atom(int index) const;

    int atomsCount() const;

    int charge() const;
    void setCharge(int charge);

    void addAtom(const Chemistry::Atom& atom);

    float interatomicDistance(int index1, int index2);

    void rebond();

  private:
    int charge_;
    QList<Chemistry::Atom> atoms_;
  };
} // namespace Chemistry

#endif // CHEMISTRY_MOLECULE_H
