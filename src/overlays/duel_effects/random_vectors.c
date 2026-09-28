#include "../../types.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"

void func_8014EF2C(u16 count, SVECTOR *vertices)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vertices[i].vx = (rand() - rand()) % 4096;
        vertices[i].vy = (rand() - rand()) % 4096;
        vertices[i].vz = (rand() - rand()) % 4096;
    }
}
