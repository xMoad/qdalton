#ifndef FILE_PARAMETRE_H
#define FILE_PARAMETRE_H

#include <QString>

namespace File
{
  class Parametre
  {
  public:
    Parametre(const QString& name, const QString& value);
    const QString& name() const;
    const QString& value() const;
  private:
    QString name_;
    QString value_;
  };
}

#endif // FILE_PARAMETRE_H
