#include "FileParseError.h"

File::ParseError::ParseError(int index, const QString& message):
        index_(index),
        message_(message)
{
}

int File::ParseError::index() const
{
    return index_;
}

void File::ParseError::setIndex(int index)
{
    index_ = index;
}

const QString& File::ParseError::message() const
{
    return message_;
}

void File::ParseError::setMessage(const QString& message)
{
    message_ = message;
}
