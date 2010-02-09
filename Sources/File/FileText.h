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

    bool read();

    QString fileName() const;
    QString absolutePath() const;

    QString text() const;

    void addString(const QString& string);

  protected:
    QString absoluteFilePath_;
    QStringList strings_;
  };
}

#endif // FILE_TEXT_H
