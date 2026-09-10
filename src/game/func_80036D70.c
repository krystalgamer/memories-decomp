#include "../types.h"
#include "func_80036D70.h"

unsigned int func_80036D70(unsigned char *object) {
    unsigned char **stream = &((unsigned char **)object)[*(s8 *)(object + 0x58)];
    unsigned char *current = *stream;
    *stream = current + 4;
    return (current[3] << 24) | (current[2] << 16) | (current[1] << 8) | current[0];
}
