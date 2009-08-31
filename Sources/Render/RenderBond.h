// © 2009 Anton Simakov
// The Bond3D class.

#ifndef RENDER_BOND_H
#define RENDER_BOND_H

#include "Chemistry/ChemistryBond.h"
#include "Chemistry/ChemistryMolecule.h"
#include "Render/RenderQuadric.h"
#include "Render/RenderAtom.h"

namespace Render
{
  class Bond
  {
  public:
    Bond(Chemistry::Molecule* molecule, int index1, int index2);
    void draw(Quality quality) const;
    void drawSelection(Quality quality) const;
    bool isSelected() const;
    void setSelected(bool selected);
    static const GLfloat DEFAULT_THIKNESS;
  private:
    Chemistry::Molecule* molecule_;
    int beginAtomId_, endAtomId_;
    bool isSelected_;
  };
}

#endif // RENDER_BOND_H
