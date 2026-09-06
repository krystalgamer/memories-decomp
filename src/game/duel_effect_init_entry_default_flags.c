#include "../types.h"

extern void DuelEffect_InitEntry(void *, void *, int);

void DuelEffect_InitEntryDefaultFlags(void *arg0, void *arg1)
{
    DuelEffect_InitEntry(arg0, arg1, 0);
}
