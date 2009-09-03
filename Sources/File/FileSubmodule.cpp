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
