#include "File/FileDal.h"

#include <QStringList>

#include "File/FileParametre.h"
#include "File/FileSubmodule.h"

enum Divider
{
  MODULES_DIVIDER,
  SUBMODULES_DIVIDER,
  PARAMETRES_DIVIDER
};

QHash<QString, QString> divide(const QString& text, Divider divider)
{
  QHash<QString, QString> hash;
  QRegExp regExp;
  QString s;

  switch (divider)
  {
  case MODULES_DIVIDER:
    regExp.setPattern("(\\*{2}(\\w|\\s)+)$?");
    break;
  case SUBMODULES_DIVIDER:
    regExp.setPattern("(\\*{1}(\\w|\\s)+)$?");
    break;
  case PARAMETRES_DIVIDER:
    regExp.setPattern("(\\.{1}(\\w|\\s)+)$?");
  }

  QStringList list = text.split(regExp);
  QStringListIterator iterator(list);
  s = iterator.next();
  if (divider == SUBMODULES_DIVIDER)
  {
    hash.insert("*DEFAULT\n", s);
  }
  int pos = 0;
  while (regExp.indexIn(text, pos) != -1)
  {
    s = iterator.next();
    hash.insert(regExp.cap(1), s);
    pos = pos + regExp.matchedLength() + s.length();
  }
  return hash;
}

File::Dal::Dal(const File::Dal& dalFile) :
  File::Text(dalFile)
{
  parse();
}

File::Dal::Dal(const QString& fileName) :
  File::Text(fileName)
{
}

bool File::Dal::parse()
{
  QHash<QString, QString> modulesHashTable = divide(text(), MODULES_DIVIDER);
  QHash<QString, QString>::iterator m;
  for (m = modulesHashTable.begin(); m != modulesHashTable.end(); ++m)
  {
    File::Module module;
    module.setName(m.key());
    QHash<QString, QString> submodulesHashTable = divide(m.value(),
                                                         SUBMODULES_DIVIDER);
    QHash<QString, QString>::iterator s;
    for (s = submodulesHashTable.begin(); s != submodulesHashTable.end(); ++s)
    {
      File::Submodule submodule;
      submodule.setName(s.key());
      QHash<QString, QString> parametresHashTable = divide(s.value(),
                                                           PARAMETRES_DIVIDER);
      QHash<QString, QString>::iterator p;
      for (p = parametresHashTable.begin(); p != parametresHashTable.end(); ++p)
      {
        // key consists of name & value divided by \n! parse it!
        QStringList param = p.key().split("\\n");
        QString name = param[0];
        QString value = "";
        for (int i = 1; i < param.size(); ++i)
        {
          value = value + param[i] + "\\n";
        }
        File::Parametre parametre(name, value);
        submodule.addParametre(parametre);
      }
      module.addSubmodule(submodule);
    }
    modules_.append(module);
  }
  if (hasParametre("**DALTON INPUT", "*DEFAULT", ".OPTIMIZE"))
    jobType_ = OPTIMIZE;
  else if (hasParametre("**DALTON INPUT", "*DEFAULT", ".WALK"))
    jobType_ = WALK;
  else if (hasParametre("**DALTON INPUT", "*DEFAULT", ".RUN"))
    jobType_ = RUN;
  return true;
}

char File::Dal::moduleIndex(const QString& moduleName)
{
  for (int i = 0; i < modules_.size(); ++i)
  {
    if (modules_[i].name() == moduleName + "\n")
      return i;
  }
  return -1;
}

QString File::Dal::parametreValue(const QString& moduleName,
                                       const QString& submoduleName,
                                       const QString& parametreName)
{
  if (hasParametre(moduleName, submoduleName, parametreName))
  {
    char m = moduleIndex(moduleName);
    char s = modules_[m].submoduleIndex(submoduleName);
    char p = modules_[m][s].parametreIndex(parametreName);
    return modules_[m][s][p].value();
  }
  else
    return "";
}

bool File::Dal::hasParametre(const QString& moduleName,
                                  const QString& submoduleName,
                                  const QString& parametreName)
{
  char m = moduleIndex(moduleName);
  if (m != -1)
  {
    char s = modules_[m].submoduleIndex(submoduleName);
    if (s != -1)
    {
      char p = modules_[m][s].parametreIndex(parametreName);
      if (p != -1)
        return true;
      else
        return false;
    }
    else
      return false;
  }
  return false;
}

File::Dal::JobType File::Dal::jobType()
{
  return jobType_;
}
