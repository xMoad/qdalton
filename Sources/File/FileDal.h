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
    quint8 moduleIndex(const QString& moduleName);
    QString parametreValue(const QString& moduleName,
                           const QString& submoduleName,
                           const QString& parametreName);
    bool hasParametre(const QString& moduleName, const QString& submoduleName,
                      const QString& parametreName);
  };
}

#endif // FILE_DAL_H
