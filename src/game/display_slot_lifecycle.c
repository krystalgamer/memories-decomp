#include "../types.h"
#include "display_object_layout.h"

typedef struct {
    s16 field_00;
    s16 field_02;
    u32 field_04;
    u16 flags_08;
    u8 field_0A;
    u8 field_0B;
    u32 field_0C;
    u32 field_10;
    u16 field_14;
    u8 field_16;
    u8 field_17;
    u16 field_18;
    u16 field_1A;
    u16 field_1C;
    s16 field_1E;
    u32 field_20;
    u32 field_24;
    u8 pad_28[0x18];
    u32 field_40;
    u32 field_44;
    u32 field_48;
    u8 pad_4C[8];
    void *field_54;
    u8 pad_58[6];
    u16 field_5E;
    u8 pad_60[5];
    u8 field_65;
    u8 field_66;
    u8 pad_67[5];
    u8 field_6C;
    u8 pad_6D[DISPLAY_OBJECT_RECORD_SIZE - 0x6D];
} DisplaySlot;

extern u16 D_8009AF74[4];
extern u16 D_8009B410;
extern u16 D_8009B412;
extern s16 D_800EFE38[];
extern DisplaySlot D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];
extern DisplaySlot D_800F0548[
    DISPLAY_OBJECT_POOL_CAPACITY - DISPLAY_OBJECT_RESERVED_CAPACITY
];
extern s16 D_800F2878[];
extern u8 tail_data_start[];

s32 func_8004002C(void)
{
    DisplaySlot *entry = D_800F0548;
    s32 i;

    for (i = DISPLAY_OBJECT_RESERVED_CAPACITY;
         i < DISPLAY_OBJECT_POOL_CAPACITY;
         i++, entry++) {
        if ((entry->flags_08 & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_8004006C(void)
{
    DisplaySlot *entry = D_800EFE48;
    s32 i;

    for (i = 0; i < DISPLAY_OBJECT_POOL_CAPACITY; i++, entry++) {
        if ((entry->flags_08 & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

DisplaySlot *func_800400AC(s32 index, s32 key)
{
    DisplaySlot *slot;
    s16 other;
    u16 initialized;

    if (index < 0) {
        return 0;
    }

    slot = &D_800EFE48[index];
    initialized = slot->flags_08 & DISPLAY_OBJECT_FLAG_ALLOCATED;
    D_8009B412++;
    if (!initialized) {
        other = D_800EFE38[key];
        if (other < 0) {
            D_800F2878[key] = index;
            slot->field_02 = -1;
            slot->field_00 = -1;
        } else {
            D_800EFE48[other].field_00 = index;
            slot->field_02 = (u16)D_800EFE38[key];
        }

        slot->field_00 = -1;
        D_800EFE38[key] = index;
        slot->field_04 = 0x08000000;
        slot->flags_08 = DISPLAY_OBJECT_RENDERABLE_MASK;
        slot->field_17 = 2;
        slot->field_54 = tail_data_start;
        slot->field_6C = 0;
        slot->field_24 = 0;
        slot->field_1E = key;
        slot->field_0A = index;
        slot->field_0B = 0;
        slot->field_0C = 0x00808080;
        slot->field_20 = 0;
        slot->field_10 = 0;
        slot->field_1C = 0;
        slot->field_1A = 0;
        slot->field_18 = 0;
        slot->field_66 = 0;
        slot->field_48 = 0;
        slot->field_44 = 0x10001000;
        slot->field_40 = 0;
        slot->field_5E = 0;
        slot->field_65 = 0;
        slot->field_16 = 0;
        slot->field_14 = D_8009AF74[slot->field_17];
    }
    return slot;
}

void func_8004020C(DisplaySlot *slot)
{
    s32 first = slot->field_00;
    s32 second = slot->field_02;

    D_8009B410++;

    if (first < 0) {
        s16 index = slot->field_1E;
        D_800EFE38[index] = second;
        if (second >= 0) {
            D_800EFE48[second].field_00 = -1;
        }
    } else {
        D_800EFE48[first].field_02 = second;
        if (second >= 0) {
            D_800EFE48[second].field_00 = first;
        }
    }

    slot->flags_08 = 0;
}
