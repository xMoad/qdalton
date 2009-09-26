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

char File::Module::submoduleIndex(const QString& submoduleName) const
{
  for (int i = 0; i < submoduleList_.size(); ++i)
  {
    if (submoduleList_[i].name() == submoduleName + "\n")
      return i;
  }
  return -1;
}

File::Submodule& File::Module::operator[](char index)
{
  return submoduleList_[index];
}
