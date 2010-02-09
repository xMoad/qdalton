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

#include <QTextStream>
#include <QFile>

#include "File/FileMol.h"
#include "File/FileText.h"

File::Mol::Mol() :
    File::Text(),
    molecule_(),
    generalStringIndex_(0)
{
  addString("BASIS");
  addString("6-31G*");
  addString("Comment");
  addString("Comment");
}

File::Mol::Mol(const Mol& fileMol) :
    File::Text(fileMol),
    molecule_(fileMol.molecule_),
    generalStringIndex_(fileMol.generalStringIndex_)
{
}

Chemistry::Molecule* File::Mol::molecule()
{
  return &molecule_;
}

void File::Mol::setMolecule(const Chemistry::Molecule& molecule)
{
  molecule_ = molecule;
}

bool File::Mol::parse(bool doAutoRebond)
{
  if (parseBasis())
    if (parseGeneralString())
      if (parseAtoms())
      {
        if (doAutoRebond)
        {
          molecule_.rebond();
        }
        return true;
      }

  return false;
}

const QString& File::Mol::comment() const
{
  return comment_;
}

int File::Mol::atomTypes() const
{
  return atomTypes_;
}

void File::Mol::setAtomTypes(int atomTypes)
{
  atomTypes_ = atomTypes;
}

bool File::Mol::isAngstrom() const
{
  return isAngstrom_;
}

void File::Mol::setAngstrom(bool isAngstrom)
{
  isAngstrom_ = isAngstrom;
}

bool File::Mol::isCartesian() const
{
  return isCartesian_;
}

void File::Mol::setCartesian(bool isCartesian)
{
  isCartesian_ = isCartesian;
}

bool File::Mol::parseBasis()
{
  QStringList basisTypesStringList;
  basisTypesStringList << "ATOMBASIS" << "BASIS" << "INTGRL";


  if (basisTypesStringList.contains(strings_[0]))
  {
    basisType_ = strings_[0];
    // Depending on basis type do some actions and set the number of general string.
    if (basisType_ == "BASIS")
    {
      comment_ = strings_[2] + "\n" + strings_[3];
      generalStringIndex_ = 4;
    }
    else
    {
      comment_ = strings_[1] + "\n" + strings_[2];
      generalStringIndex_ = 3;
    }
    return true;
  }
  else
  {
    //        parseError_.index = 0;
    //        parseError_.message = "Unknown basis type: " + strings_[0];
    return false;
  }
}

bool File::Mol::parseGeneralString()
{
  int n = 0;
  bool ok;
  // Parse general string using regular expression (for search to be uniform).
  QRegExp regExp;
  // First search for obligatory keywords...
  regExp.setPattern("Atomtypes=(\\d+)");
  n = regExp.indexIn(strings_[generalStringIndex_]);
  if (n != -1)
  {
    setAtomTypes(regExp.cap(1).toInt(&ok, 10));
  }
  else
  {
    //    parseError_.index = generalStringIndex;
    //    parseError_.message = "Couldn't find Atomtypes keyword.\n\n"
    //      "Hint: QDalton recognize only full names of Dalton keywords!";
    return false;
  }

  //...then for
  regExp.setPattern("Angstrom");
  n = regExp.indexIn(strings_[generalStringIndex_]);
  if (n != -1)
  {
    setAngstrom(true);
  }
  else
  {
    setAngstrom(false);
  }

  regExp.setPattern("Cartesian");
  n = regExp.indexIn(strings_[generalStringIndex_]);
  if (n != -1)
  {
    setCartesian(true);
  }
  else
  {
    setCartesian(false);
  }

  regExp.setPattern("Charge=(\\d+)");
  n = regExp.indexIn(strings_[generalStringIndex_]);
  if (n != -1)
  {
    molecule_.setCharge(regExp.cap(1).toInt(&ok, 10));
  }
  else
  {
    molecule_.setCharge(0);
  }

  return true;
}

bool File::Mol::parseAtoms()
{
  bool ok;
  QRegExp regExp;

  regExp.setPattern("Charge=(\\d+) Atoms=(\\d+)");
  int i = generalStringIndex_;
  for (int a = 0; a < atomTypes(); a++)
  {
    i++;
    if (regExp.indexIn(this->strings_[i]) != -1)
    {
      int protons = regExp.cap(1).toInt(&ok, 10);
      int count = regExp.cap(2).toInt(&ok, 10);
      QRegExp regExpAtom("(\\w+)\\s+(-?\\d?\\.\\d+)\\s+(-?\\d?\\.\\d+)\\s+(-?\\d?\\.\\d+)");
      for (int j = 0; j < count; j++)
      {
        i++;
        if (regExpAtom.indexIn(this->strings_[i]) != -1)
        {
          OpenBabel::OBAtom obatom;
          obatom.SetAtomicNum(protons);
          obatom.SetIsotope(0);
          // TODO Fix label!
          QString label = regExpAtom.cap(1);
          obatom.SetVector(regExpAtom.cap(2).toFloat(&ok),
                           regExpAtom.cap(3).toFloat(&ok),
                           regExpAtom.cap(4).toFloat(&ok));
          molecule_.addObAtom(obatom);
        }
      }
    }
  }
  return true;
}
