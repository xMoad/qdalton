/**********************************************************************
  Copyright (C) 2008, 2009, 2010 Anton Simakov

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

#ifndef FILE_TEXT_H
#define FILE_TEXT_H

#include <QStringList>

namespace File
{
  class Text
  {
  public:
    Text();
    Text(const File::Text& fileText);

    const QString& absoluteFilePath() const;
    void setAbsoluteFilePath(const QString& absoluteFilePath);

    void clear();

    bool read();
    bool save(const QString& absoluteFilePath);

    QString fileName() const;
    QString absolutePath() const;

    QString text() const;
    const QString& string(int index);

    void addString(const QString& string);
    void insertString(int index, const QString& string);

    void setParseError(int stringIndex, const QString& message);
    int parseErrorIndex() const;
    const QString& parseErrorMessage() const;

  private:
    QString absoluteFilePath_;
    QStringList strings_;
    int parseErrorIndex_;
    QString parseErrorMessage_;
  };
}

#endif // FILE_TEXT_H
