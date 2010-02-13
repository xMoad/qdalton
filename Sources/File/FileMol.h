#ifndef FILE_MOL_H
#define FILE_MOL_H

#include "File/FileText.h"
#include "Render/RenderMolecule.h"

namespace File
{
  class Mol: public File::Text
  {
  public:
    Mol();
    Mol(const File::Mol& fileMol);

    bool parse(bool doAutoRebond);
    bool generate();

    Render::Molecule& molecule();
    void setMolecule(const Render::Molecule& molecule);

    const QString& comment() const;
    int atomTypes() const;
    void setAtomTypes(int atomTypes);
    bool isAngstrom() const;
    void setAngstrom(bool isAngstrom);
    bool isCartesian() const;
    void setCartesian(bool isCartesian);

  private:
    Render::Molecule molecule_;
    // General string contains most important information
    int generalStringIndex_;
    QString basisType_;
    QString basisSet_;
    QString comment_;
    int atomTypes_;
    bool isAngstrom_;
    bool isCartesian_;
    bool parseBasis();
    bool parseGeneralString();
    bool parseAtoms();
  };
}

#endif // FILE_MOL_H
