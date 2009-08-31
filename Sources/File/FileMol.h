#ifndef FILE_MOL_H
#define FILE_MOL_H

#include "Chemistry/ChemistryMolecule.h"
#include "File/FileText.h"

namespace File
{
  class Mol: public File::Text
  {
  public:
    enum BasisType
    {
      ATOMBASIS, BASIS, INTGRL
    };
    enum BasisSet
    {
      STO_2G, STO_3G, STO_6G
    };
    Mol(const File::Mol& molFile);
    Mol(QString fileName);
    bool parse(bool doAutoRebond);
    const Chemistry::Molecule& molecule() const;
    BasisType basisType() const;
    BasisSet basisSet();
    const QString& comment() const;
    int atomTypes() const;
    void setAtomTypes(int atomTypes);
    bool isAngstrom() const;
    void setAngstrom(bool isAngstrom);
    bool isCartesian() const;
    void setCartesian(bool isCartesian);
  private:
    Chemistry::Molecule molecule_;
    BasisType basisType_;
    BasisSet basisSet_;
    QString comment_;
    int atomTypes_;
    bool isAngstrom_;
    bool isCartesian_;
  };
}

#endif // FILE_MOL_H
