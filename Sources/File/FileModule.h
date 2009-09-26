#ifndef FILE_MODULE_H
#define FILE_MODULE_H

#include "File/FileSubmodule.h"

namespace File
{
  class Module
  {
  public:
    Module();
    const QString& name() const;
    void setName(const QString& name);
    void addSubmodule(const File::Submodule& submodule);
    char submoduleIndex(const QString& submoduleName) const;
    File::Submodule& operator[](char index);
  private:
    QString name_;
    QList<File::Submodule> submoduleList_;
  };
}

#endif // FILE_MODULE_H
