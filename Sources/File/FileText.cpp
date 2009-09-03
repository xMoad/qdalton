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

#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include "File/FileText.h"

namespace File
{
  Text::Text(const QString& fileName)
  {
    QFileInfo fileInfo(fileName);
    fileName_ = fileInfo.fileName();
    absolutePath_ = fileInfo.absolutePath();
  }
  
  void Text::read()
  {
    QFile file(absolutePath_ + "/" + fileName_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      strings_.append(line);
    }
  }
  
  const QString& Text::fileName() const
  {
    return fileName_;
  }
  
  const QString& Text::filePath() const
  {
    return absolutePath_;
  }
  
  QString Text::text() const
  {
    return strings_.join("\n");
  }
  
  Text::ParseError& Text::getParseError()
  {
    return parseError_;
  }
}
