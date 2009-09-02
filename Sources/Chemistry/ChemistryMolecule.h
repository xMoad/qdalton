#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <QList>

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryBond.h"

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

    bool isConnected(int index1, int index2) const;
    void connect(int index1, int index2);

    void rebond();

  private:
    typedef QList<Chemistry::Bond*> IncidenceList;
    typedef QList<IncidenceList> IncidenceMatrix;

    IncidenceMatrix incidenceMatrix_;
    int charge_;
    QList<Chemistry::Atom> atoms_;
  };
}

#endif // CHEMISTRY_MOLECULE_H
