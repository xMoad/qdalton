#ifndef FILE_SUBMODULE_H
#define FILE_SUBMODULE_H

#include <QList>

#include "File/FileParametre.h"

namespace File
{
  class Submodule
  {
  public:
    Submodule();
    const QString& name() const;
    void setName(const QString& name);
    void addParametre(const Parametre& parametre);
    char parametreIndex(const QString& parametreName);
    File::Parametre& operator[](char index);
  private:
    QString name_;
    QList<File::Parametre> parametreList_;
  };
}

#endif // FILE_SUBMODULE_H
