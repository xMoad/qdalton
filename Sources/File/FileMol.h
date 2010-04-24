/**********************************************************************
  Copyright (C) 2008, 2009, 2010 Anton Simakov

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

#include <QHash>

#include "Constants.h"
#include "File/FileText.h"
#include "Render/RenderMolecule.h"

namespace File
{
  class Mol: public File::Text
  {
  public:
    Mol();
    Mol(const Render::Molecule& molecule);
    Mol(const File::Mol& fileMol);



    bool parse(bool doAutoRebond);
    bool generate();

    Render::Molecule& molecule();
//    void setMolecule(const Render::Molecule& molecule);

    const QString& comment() const;
    int atomTypes() const;
    void setAtomTypes(int atomTypes);

    UnitOfLength unitOfLength() const;
    void setUnitOfLength(UnitOfLength unitOfLength);

    GaussiansType gaussiansType() const;
    void setGaussiansType(GaussiansType gaussiansType);

  private:
    File::Mol& operator=(const File::Mol& rhs);

    bool parseBasis();
    bool parseGeneralString();
    bool parseAtoms();

    Render::Molecule molecule_;
    // General string contains most important information
    int generalStringIndex_;
    QString basisType_;
    QString basisSet_;
    QString comment_;
    int atomTypes_;
    UnitOfLength unitOfLength_;
    GaussiansType gaussiansType_;
    int generatorsCount_;
    QString generators_;
    QHash<int, QString> hashBasisSets_;
  };
}

#endif // FILE_MOL_H
