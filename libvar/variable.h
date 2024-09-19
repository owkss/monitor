#ifndef VARIABLE_H
#define VARIABLE_H

#include "../global.h"

#include <string>
#include <cstring>
#include <cstdint>

struct Variable
{
    Variable() {}
    Variable(void *p, const char *n, const char *t, int ts, int i, int o)
        : ptr(p)
        , name(n)
        , type(t)
        , type_size(ts)
        , index(i)
        , offset(o)
    {}

    Variable(const Variable &other)
        : ptr(other.ptr)
        , name(other.name)
        , type(other.type)
        , type_size(other.type_size)
        , index(other.index)
        , offset(other.offset)
    {}

    Variable(Variable &&other)
        : Variable()
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
        std::swap(ptr, other.ptr);
        std::swap(name, other.name);
        std::swap(type, other.type);
        std::swap(type_size, other.type_size);
        std::swap(index, other.index);
        std::swap(offset, other.offset);
    }

    void *ptr = nullptr;  /* Указатель на данные */
    std::string name;   /* Указанное имя */
    std::string type;   /* Строковое обозначение типа */
    int type_size = -1; /* Размер типа в байтах */
    int index = -1;     /* Индекс в массиве переменных */
    int offset = -1;    /* Смещение в байтах */
};

#endif // VARIABLE_H
