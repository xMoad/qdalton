#include "File/FileSubmodule.h"

File::Submodule::Submodule()
{
}

const QString& File::Submodule::name() const
{
  return name_;
}

void File::Submodule::setName(const QString& name)
{
  name_ = name;
}

void File::Submodule::addParametre(const Parametre& parametre)
{
  parametreList_.append(parametre);
}

char File::Submodule::parametreIndex(const QString& parametreName)
{
  for (int i = 0; i < parametreList_.size(); ++i)
  {
    if (parametreList_[i].name() == parametreName + "\n")
      return i;
  }
  return -1;
}

File::Parametre& File::Submodule::operator[](char index)
{
  return parametreList_[index];
}
