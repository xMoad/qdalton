#ifndef FILE_PARSE_ERROR_H
#define FILE_PARSE_ERROR_H

#include <QString>

namespace File
{
    class ParseError
    {
    public:
        ParseError(int index, const QString& message);

        int index() const;
        void setIndex(int index);

        const QString& message() const;
        void setMessage(const QString& message);

    private:
        int index_;
        QString message_;
    };
}
#endif // FILE_PARSE_ERROR_H
