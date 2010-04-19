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

#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include "File/FileText.h"

File::Text::Text() :
    absoluteFilePath_(),
    strings_(),
    parseErrorIndex_(-1),
    parseErrorMessage_()
{
}

File::Text::Text(const File::Text &fileText):
    absoluteFilePath_(fileText.absoluteFilePath_),
    strings_(fileText.strings_),
    parseErrorIndex_(fileText.parseErrorIndex_),
    parseErrorMessage_(fileText.parseErrorMessage_)
{
}

const QString& File::Text::absoluteFilePath() const
{
  return absoluteFilePath_;
}

void File::Text::setAbsoluteFilePath(const QString& absoluteFilePath)
{
  absoluteFilePath_ = absoluteFilePath;
}

void File::Text::clear()
{
  strings_.clear();
}

bool File::Text::read()
{
  QFile file(absoluteFilePath());

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  strings_.clear();

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QString line = in.readLine();
    strings_.append(line);
  }

  return true;
}

bool File::Text::save(const QString& absoluteFilePath)
{
  QFile file(absoluteFilePath);

  if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    return false;

  QTextStream out(&file);
  out << text();

  setAbsoluteFilePath(absoluteFilePath);

  return true;
}

QString File::Text::fileName() const
{
  return QFileInfo(absoluteFilePath_).fileName();
}

QString File::Text::absolutePath() const
{
  return QFileInfo(absoluteFilePath_).absolutePath();
}

QString File::Text::text() const
{
  return strings_.join("\n");
}

const QString& File::Text::string(int index)
{
  return strings_.at(index);
}

void File::Text::addString(const QString& string)
{
  strings_ << string;
}

void File::Text::insertString(int index, const QString& string)
{
  strings_.insert(index, string);
}

void File::Text::setParseError(int index, const QString& message)
{
  parseErrorIndex_ = index;
  parseErrorMessage_ = message;
}

int File::Text::parseErrorIndex() const
{
  return parseErrorIndex_;
}

const QString& File::Text::parseErrorMessage() const
{
  return parseErrorMessage_;
}
