/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

  This file is part of QDalton.
  For more information, see <http://code.google.com/p/qdalton/>

  QDalton is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QDalton is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QDalton. If not, see <http://www.gnu.org/licenses/>.

 **********************************************************************/

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
    quint8 atomTypes() const;
    void setAtomTypes(quint8 atomTypes);

    bool isCartesian() const;
    void setCartesian(bool isCartesian);
  private:
    Chemistry::Molecule molecule_;
    BasisType basisType_;
    BasisSet basisSet_;
    QString comment_;
    quint8 atomTypes_;
    bool isCartesian_;
  };
}

#endif // FILE_MOL_H
