#ifndef FILE_DAL_H
#define FILE_DAL_H

#include <QHash>

#include "File/FileText.h"
#include "File/FileModule.h"

namespace File
{
  class Dal: public File::Text
  {
  public:
    enum JobType
    {
      OPTIMIZE,
      WALK,
      RUN
    };
    Dal(const File::Dal& dalFile);
    Dal(const QString& fileName);
    bool parse();
    JobType jobType();
  private:
    QList<File::Module> modules_;
    bool isDirect_;
    bool isDouglasKroll_;
    bool isInptes_;
    bool isParallel_;
    bool isProperties_;
    JobType jobType_;
    char moduleIndex(const QString& moduleName);
    QString parametreValue(const QString& moduleName,
                           const QString& submoduleName,
                           const QString& parametreName);
    bool hasParametre(const QString& moduleName, const QString& submoduleName,
                      const QString& parametreName);
  };
}

#endif // FILE_DAL_H
