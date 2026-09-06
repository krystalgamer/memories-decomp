#include "../types.h"
#include "card_constants.h"

typedef struct {
    int field_00;
    int field_04;
    u8 pad_08;
    u8 field_09;
    u8 pad_0A[2];
} Entry;

extern Entry D_800EA030[HAND_SIZE];

void func_800176D0(void)
{
    u8 *entry = (u8 *)D_800EA030;
    int i = 0;
    u8 *field_09 = entry + 9;

    do {
        *(int *)(field_09 - 5) = 0;
        *(int *)entry = 0;
        *field_09 = 0;
        field_09 += sizeof(Entry);
        i++;
        entry += sizeof(Entry);
    } while (i < HAND_SIZE);
}
