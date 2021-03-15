#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>

static const uint32_t SET_VALUES_FLAG  = 0xF0000;
static const uint32_t READ_VALUES_FLAG = 0xF0000000;

static const int MAX_VARIABLES = 128;
static const int SERVICE_DATA_OFFSET = 4 + 4 + MAX_VARIABLES;
static const int SHARED_MEM_SIZE = SERVICE_DATA_OFFSET + (MAX_VARIABLES * 8 * 8);

#endif // GLOBAL_H
