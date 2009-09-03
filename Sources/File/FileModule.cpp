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

#include "File/FileModule.h"

File::Module::Module()
{
}

const QString& File::Module::name() const
{
  return name_;
}

void File::Module::setName(const QString& name)
{
  name_ = name;
}

void File::Module::addSubmodule(const File::Submodule& submodule)
{
  submoduleList_.append(submodule);
}

uint8_t File::Module::submoduleIndex(const QString& submoduleName) const
{
  for (uint8_t i = 0; i < submoduleList_.size(); ++i)
  {
    if (submoduleList_[i].name() == submoduleName + "\n")
      return i;
  }
  return -1;
}

File::Submodule& File::Module::operator[](uint8_t index)
{
  return submoduleList_[index];
}
