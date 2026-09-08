#include "../types.h"
#include "func_8004036C.h"

typedef struct { void *first; void *second; unsigned char pad[0x38]; } Entry;
extern Entry D_800EA0E8[];
void func_80029528(int index) {
    Entry *entry = &D_800EA0E8[index];
    func_8004036C(entry->first);
    func_8004036C(entry->second);
    entry->second = 0;
    entry->first = 0;
}
