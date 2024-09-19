#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <cstring>
#include <cstdint>

#include <QString>
#include <QVariant>

class Variable
{
public:
    enum Type
    {
        UNKNOWN = -1,
        CHAR,
        UCHAR,
        SHORT,
        USHORT,
        INT,
        UINT,
        LONG,
        ULONG,
        FLOAT,
        DOUBLE
    };

    Variable() = delete;
    Variable(const char *name_str, const char *type_str, const int ts, const int i, const int o)
    {
        if (strcasecmp(type_str, "c") == 0)
        {
            type = CHAR;
            this->type_str = "int8_t";
        }
        else if (strcasecmp(type_str, "uc") == 0)
        {
            type = UCHAR;
            this->type_str = "uint8_t";
        }
        else if (strcasecmp(type_str, "s")  == 0)
        {
            type = SHORT;
            this->type_str = "int16_t";
        }
        else if (strcasecmp(type_str, "us") == 0)
        {
            type = USHORT;
            this->type_str = "uint16_t";
        }
        else if (strcasecmp(type_str, "i")  == 0)
        {
            type = INT;
            this->type_str = "int32_t";
        }
        else if (strcasecmp(type_str, "ui") == 0)
        {
            type = UINT;
            this->type_str = "uint32_t";
        }
        else if (strcasecmp(type_str, "l")  == 0)
        {
            type = LONG;
            this->type_str = "int64_t";
        }
        else if (strcasecmp(type_str, "ul") == 0)
        {
            type = ULONG;
            this->type_str = "uint64_t";
        }
        else if (strcasecmp(type_str, "f")  == 0)
        {
            type = FLOAT;
            this->type_str = "float";
        }
        else if (strcasecmp(type_str, "d")  == 0)
        {
            type = DOUBLE;
            this->type_str = "double";
        }

        if (valid())
        {
            name = name_str;
            type_size = ts;
            index = i;
            offset = o;
        }
    }

    Variable(const Variable &other)
        : name(other.name)
        , type_str(other.type_str)
        , value(other.value)
        , type(other.type)
        , type_size(other.type_size)
        , index(other.index)
        , offset(other.offset)
    {}

    Variable(Variable &&other) noexcept
    {
        swap(other);
    }

    Variable &operator=(const Variable &other)
    {
        if (this != &other)
        {
            Variable tmp(other);
            swap(tmp);
        }

        return *this;
    }

    Variable &operator=(Variable &&other) noexcept
    {
        if (this != &other)
        {
            Variable tmp(std::move(other));
            swap(tmp);
        }

        return *this;
    }

    void swap(Variable &other) noexcept
    {
        std::swap(name, other.name);
        std::swap(type_str, other.type_str);
        std::swap(value, other.value);
        std::swap(type, other.type);
        std::swap(type_size, other.type_size);
        std::swap(index, other.index);
        std::swap(offset, other.offset);
    }

    inline bool valid() const noexcept { return type != UNKNOWN; }

    QString name;
    QString type_str;
    QVariant value;

    Type type = UNKNOWN;
    int type_size = -1; /* Размер типа в байтах */
    int index = -1;     /* Индекс в массиве переменных */
    int offset = -1;    /* Смещение в байтах */
};

#endif // VARIABLE_H
