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
    setAngstrom(true);
  }
  else
  {
    setAngstrom(false);
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
  regExp.setPattern("Charge=(\\d+) Atoms=(\\d+)");
  int i = generalStringIndex;
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
          Chemistry::Atom atom;
          atom.setProtons(protons);
          //          obatom.SetIsotope(0); !!!
          // TODO Fix label!
          QString label = regExpAtom.cap(1);
          Eigen::Vector3f p(regExpAtom.cap(2).toFloat(&ok),
                            regExpAtom.cap(3).toFloat(&ok),
                            regExpAtom.cap(4).toFloat(&ok));
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