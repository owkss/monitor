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
    Variable() = delete;
    explicit Variable(const char *name_str, const char *type_str, const int ts, const int i, const int o)
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
    {
        *this = other;
    }

    Variable &operator=(const Variable &other)
    {
        if (this != &other)
        {
            this->value = other.value;
            this->name = other.name;
            this->type_str = other.type_str;

            this->type = other.type;
            this->type_size = other.type_size;
            this->index = other.index;
            this->offset = other.offset;
        }

        return *this;
    }

    Variable(Variable &&other)
    {
        this->value = std::move(other.value);
        this->name = std::move(other.name);
        this->type_str = std::move(other.type_str);

        this->type = other.type;
        this->type_size = other.type_size;
        this->index = other.index;
        this->offset = other.offset;

        other.type = UNKNOWN;
        other.type_size = -1;
        other.index = -1;
        other.offset = -1;
    }

    bool valid() const { return type != UNKNOWN; }

    enum Type { UNKNOWN = -1, CHAR, UCHAR, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE };

    QString name;
    QString type_str;
    QVariant value;

    Type type = UNKNOWN;
    int type_size = -1; /* Размер типа в байтах */
    int index = -1;     /* Индекс в массиве переменных */
    int offset = -1;    /* Смещение в байтах */
};

#endif // VARIABLE_H
