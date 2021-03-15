#ifndef VARIABLE_H
#define VARIABLE_H

#include "../global.h"

#include <string>
#include <cstring>
#include <cstdint>

struct Variable
{
    Variable() {}
    Variable(void *p, const char *n, const char *t, const int ts, const int i, const int o) : ptr(p), name(n), type(t), type_size(ts), index(i), offset(o) {}

    Variable(const Variable &other)
    {
        *this = other;
    }

    Variable &operator=(const Variable &other)
    {
        if (this != &other)
        {
            this->ptr = other.ptr;
            this->name = other.name;
            this->type = other.type;
            this->type_size = other.type_size;
            this->index = other.index;
            this->offset = other.offset;
        }

        return *this;
    }

    Variable(Variable &&other)
    {
        this->ptr = other.ptr;
        this->name = std::move(other.name);
        this->type = std::move(other.type);
        this->type_size = other.type_size;
        this->index = other.index;
        this->offset = other.offset;

        other.ptr = nullptr;
        other.type_size = -1;
        other.index = -1;
        other.offset = -1;
    }

    void *ptr = nullptr;  /* Указатель на данные */
    std::string name;   /* Указанное имя */
    std::string type;   /* Строковое обозначение типа */
    int type_size = -1; /* Размер типа в байтах */
    int index = -1;     /* Индекс в массиве переменных */
    int offset = -1;    /* Смещение в байтах */
};

#endif // VARIABLE_H
