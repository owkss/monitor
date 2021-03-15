#include "var.h"
#include "var_p.h"

#include <cstring>

static VarManager vm;

int get_type_size(const char *var_str)
{
    if (strcasecmp(var_str, "c") == 0)
        return 1;
    else if (strcasecmp(var_str, "uc") == 0)
        return 1;
    else if (strcasecmp(var_str, "s") == 0)
        return 2;
    else if (strcasecmp(var_str, "us") == 0)
        return 2;
    else if (strcasecmp(var_str, "i") == 0)
        return 4;
    else if (strcasecmp(var_str, "ui") == 0)
        return 4;
    else if (strcasecmp(var_str, "l") == 0)
        return 8;
    else if (strcasecmp(var_str, "ul") == 0)
        return 8;
    else if (strcasecmp(var_str, "f") == 0)
        return 4;
    else if (strcasecmp(var_str, "d") == 0)
        return 8;
    else
        return -1;
}

int add_variable(void *var_ptr, const char *var_name, const char *var_type)
{
    if (!var_ptr)
        return 1;

    if (std::strlen(var_name) < 1)
        return 2;

    static const char * const valid_types[] = { "c", "uc", "s", "us", "i", "ui", "l", "ul", "f", "d", nullptr };
    for (int i = 0; valid_types[i] != nullptr; ++i)
    {
        if (strcasecmp(var_type, valid_types[i]) == 0)
        {
            const int type_size = get_type_size(var_type);
            if (type_size < 1)
                return 3;
            else
                return vm.add_variable(var_ptr, var_name, var_type, type_size);
        }
    }

    return 4;
}

int init()
{
    return vm.init();
}
