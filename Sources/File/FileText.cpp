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
