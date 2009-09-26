#include "File/FileParametre.h"

File::Parametre::Parametre(const QString& name, const QString& value) :
  name_(name), value_(value)
{
}

const QString& File::Parametre::name() const
{
  return name_;
}

const QString& File::Parametre::value() const
{
  return value_;
}
