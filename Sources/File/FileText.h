#ifndef FILE_TEXT_H
#define FILE_TEXT_H

#include <QStringList>

namespace File
{
  class Text
  {
  public:
    struct ParseError
    {
      QString message;
      int index;
    };
    Text(const QString& fileName);
    void read();
    const QString& fileName() const;
    const QString& filePath() const;
    QString text() const;
    ParseError& getParseError();
  protected:
    QString fileName_;
    QString absolutePath_;
    QStringList strings_;
    ParseError parseError_;
  };
}

#endif // FILE_TEXT_H
