#ifndef FILE_JOB_H
#define FILE_JOB_H

#include "File/FileText.h"

namespace File
{
  class Job : public File::Text
  {
  public:
    Job(const QString& fileName);
  };
}

#endif // FILE_JOB_H
