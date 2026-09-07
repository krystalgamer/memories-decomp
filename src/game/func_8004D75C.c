#include "../types.h"

typedef struct {
    u8 pad0000[0x18];
    u16 field_0018;
    u8 field_001A;
    u8 pad001B;
} Slot;

typedef struct {
    u16 v[58];
    u16 max;
} Row;

typedef struct {
    u8 pad0000[0x1E0];
    Slot *slots[58];
    u16 keys[10][58];
    Row rows[10];
    u8 pad0BEC[0xDD8 - 0xBEC];
    s32 *list;
    u8 pad0DDC[0xE1B - 0xDDC];
    u8 count;
    u8 pad0E1C[4];
} Channel;

extern Channel D_800F2C40[];

void func_8004D75C(s32 index)
{
    Channel *ch;
    Slot *slot;
    s32 *cmd;
    u32 word;
    s32 row;
    s32 i;
    s32 key;

    ch = &D_800F2C40[index];
    if (ch->list == 0) {
        return;
    }
    i = 0;
    if (i < ch->count) {
        for (; i < ch->count; i++) {
            slot = ch->slots[i];
            if (slot == 0) {
                break;
            }
            row = 1;
            slot->field_001A = row;
            key = ch->slots[i]->field_0018;
            cmd = &ch->list[key];
            ch->keys[row][i] = key;
            while (1) {
                word = *cmd;
                if ((s32)word < 0) {
                    row = word >> 16;
                    row = row & 0x7F;
                    if (row == 0) {
                        break;
                    }
                    if (ch->keys[row][i] != 0xFFFF) {
                        cmd++;
                    } else {
                        ch->keys[row][i] = *(u16 *)cmd;
                        cmd = &ch->list[*(u16 *)cmd];
                    }
                } else {
                    ch->rows[row].v[i] = ch->rows[row].v[i] + *((u8 *)cmd + 2);
                    cmd++;
                }
            }
        }
    }
    for (row = 1; row < 10; row++) {
        ch->rows[row].max = 0;
        for (i = 0; i < ch->count; i++) {
            if (ch->rows[row].max < ch->rows[row].v[i]) {
                ch->rows[row].max = ch->rows[row].v[i];
            }
        }
    }
}
