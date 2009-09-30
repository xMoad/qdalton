#ifndef RENDER_MOLECULE_H
#define RENDER_MOLECULE_H

#include <QObject>
#include <QString>
#include <openbabel/mol.h>

namespace Render
{
  enum Format
  {
    FORMAT_GAUSSIAN_OUTPUT,
    FORMAT_SMILES,
    FORMAT_INCHI
  };

  enum ForceField
  {
    FF_GHEMICAL,
    FF_MMFF94,
    FF_MMFF94s,
    FF_UFF
  };

  enum Algorithm
  {
    ALGORITHM_STEEPEST_DESCENT,
    ALGORITHM_CONJUGATE_GRADIENTS
  };

  class Viewer;
  class Molecule
  {
  public:
    Molecule();
    Molecule(Render::Viewer* viewer);

    bool importFromFile(Render::Format format,
                        const QString& fileName);
    bool importFromString(Render::Format format,
                          const QString& string);

    OpenBabel::OBAtom* obAtom(quint16 index) const;
    OpenBabel::OBBond* obBond(quint16 index) const;

    void addAtom(quint8 atomicNumber);
    void addObAtom(OpenBabel::OBAtom& obAtom);
    void deleteAtom(OpenBabel::OBAtom* obAtom);

    void addBond(OpenBabel::OBAtom* beginObAtom,
                 OpenBabel::OBAtom* endObAtom,
                 quint8 bondOrder);

    void setCharge(quint8 charge);

    QString formula();
    quint16 atomsCount() const;
    quint16 bondsCount() const;

    void rebond();
    void build();
    void addHydrogensAndBuild();
    void removeHydrogens();

    void optimize(ForceField forceField,
                  Algorithm algorithm,
                  double convergenceCriteria,
                  quint16 maxSteps,
                  quint8 stepsPerUpdate = 0);

  private:
    OpenBabel::OBMol obMol_;
    Render::Viewer* viewer_;
  };
}

#endif // RENDER_MOLECULE_H
