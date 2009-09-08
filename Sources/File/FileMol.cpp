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

#include <QTextStream>
#include <QFile>

#include "Chemistry/ChemistryAtom.h"
#include "File/FileMol.h"
#include "File/FileText.h"

File::Mol::Mol(const Mol& molFile):
    Text(molFile)
{
  parse(true);
}

File::Mol::Mol(QString fileName):
    Text(fileName)
{
}

bool File::Mol::parse(bool doAutoRebond)
{
  // General string contains most important information
  int generalStringIndex;

  bool ok;
  int n = 0;
  // Parse general string using regular expression (for search to be uniform).
  QRegExp regExp;

  // Read basis type.
  if (strings_[0] == "ATOMBASIS")
  {
    basisType_ = ATOMBASIS;
  }
  else
  {
    if (strings_[0] == "BASIS")
    {
      basisType_ = BASIS;
    }
    else
    {
      if (strings_[0] == "INTGRL")
      {
        basisType_ = INTGRL;
      }
      else
      {
        parseError_.index = 0;
        parseError_.message = "Unknown basis type: " + strings_[0];
        return false;
      }
    }
  }

  // Depending on basis type do some actions and set the number of general string.
  switch (basisType_)
  {
  case BASIS:
    comment_ = strings_[2] + "\n" + strings_[3];
    generalStringIndex = 4;
    break;
  case ATOMBASIS:
  case INTGRL:
    comment_ = strings_[1] + "\n" + strings_[2];
    generalStringIndex = 3;
    break;
  }

  // Parse general string
  // First search for obligatory keywords...
  regExp.setPattern("Atomtypes=(\\d+)");
  n = regExp.indexIn(strings_[generalStringIndex]);
  if (n != -1)
  {
    setAtomTypes(regExp.cap(1).toInt(&ok, 10));
  }
  else
  {
    parseError_.index = generalStringIndex;
    parseError_.message = "Couldn't find Atomtypes keyword.\n\n"
                          "Hint: QDalton recognize only full names of Dalton keywords!";
    return false;
  }

  //...then for
  regExp.setPattern("Angstrom");
  n = regExp.indexIn(strings_[generalStringIndex]);
  if (n != -1)
  {
    molecule_.setUnitOfLength(Chemistry::Molecule::ANGSTROM);
  }
  else
  {
    molecule_.setUnitOfLength(Chemistry::Molecule::BOHR);
  }

  regExp.setPattern("Cartesian");
  n = regExp.indexIn(strings_[generalStringIndex]);
  if (n != -1)
  {
    setCartesian(true);
  }
  else
  {
    setCartesian(false);
  }

  regExp.setPattern("Charge=(\\d+)");
  n = regExp.indexIn(strings_[generalStringIndex]);
  if (n != -1)
  {
    molecule_.setCharge(regExp.cap(1).toInt(&ok, 10));
  }
  else
  {
    molecule_.setCharge(0);
  }

  // Extract info about atoms
  regExp.setPattern("Charge=(\\d+\\.?\\d?) Atoms=(\\d+)");
  int i = generalStringIndex;
  for (int a = 0; a < atomTypes(); a++)
  {
    i++;
    if (regExp.indexIn(this->strings_[i]) != -1)
    {
      quint8 protons = (quint8) regExp.cap(1).toFloat(&ok);
      quint8 count = regExp.cap(2).toInt(&ok, 10);
      QRegExp regExpAtom("(\\w+)\\s+(-?\\d?\\.\\d+)\\s+(-?\\d?\\.\\d+)\\s+(-?\\d?\\.\\d+)");
      for (int j = 0; j < count; j++)
      {
        i++;
        if (regExpAtom.indexIn(this->strings_[i]) != -1)
        {
          Chemistry::Atom atom(protons);
          //          obatom.SetIsotope(0); !!!
          // TODO Fix label!
          QString label = regExpAtom.cap(1);
          float x = regExpAtom.cap(2).toFloat(&ok);
          float y = regExpAtom.cap(3).toFloat(&ok);
          float z = regExpAtom.cap(4).toFloat(&ok);
          Eigen::Vector3f p(x, y, z);
          atom.setCentre(p);
          molecule_.addAtom(atom);
        }
      }
    }
  }
  if (doAutoRebond)
    molecule_.rebond();
  return true;
}

const Chemistry::Molecule& File::Mol::molecule() const
{
  return molecule_;
}

File::Mol::BasisType File::Mol::basisType() const
{
  return basisType_;
}

const QString& File::Mol::comment() const
{
  return comment_;
}

quint8 File::Mol::atomTypes() const
{
  return atomTypes_;
}

void File::Mol::setAtomTypes(quint8 atomTypes)
{
  atomTypes_ = atomTypes;
}

bool File::Mol::isCartesian() const
{
  return isCartesian_;
}

void File::Mol::setCartesian(bool isCartesian)
{
  isCartesian_ = isCartesian;
}
