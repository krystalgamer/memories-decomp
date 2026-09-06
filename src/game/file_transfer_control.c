#include "../types.h"
#include "file_constants.h"

extern volatile u16 D_8009B124;
extern volatile s32 D_8009B0E8;
extern volatile s32 D_8009B0F4;
extern s32 D_8009B134;
extern u8 D_800E9E60[];

extern void File_ActivateTransfer(void);
extern void func_8001455C(void);

void func_80014A5C(s32 arg0)
{
    if (D_8009B124 != 0) {
        D_8009B124 = 0;
        return;
    }
    D_8009B124 = 1;
    if (D_8009B0E8 != 0) {
        return;
    }
    D_8009B0E8 = 1;
    if (!(D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x20)) {
        File_ActivateTransfer();
    }
    if (D_8009B0F4 & 0x10) {
        if (D_8009B134 != 0 && !(D_8009B134 & 0x40)) {
            D_8009B134 |= 0x40;
            D_800E9E60[0x46] = 5;
            D_800E9E60[0x47] = 0;
        }
        func_8001455C();
    } else {
        D_8009B134 = 0;
    }
    D_8009B0E8 = 0;
}

typedef struct {
    u8 gap0[12];
    s32 value_c;
    s32 value_10;
    s32 value_14;
    s32 value_18;
} Shared14B30;

typedef struct {
    u8 gap0[8];
    s32 value_8;
    s32 value_c;
    u8 gap10[12];
    s32 value_1c;
    u8 gap20[16];
    s32 value_30;
    u8 gap34[12];
    s32 value_40;
    u8 gap44[2];
    u8 mode_46;
} Object14B30;

extern Shared14B30 D_801D4200;
extern s32 D_8009B118;
extern void (*D_8009B128)(void);

void func_80014B30(Object14B30 *object, s32 mode)
{
    Shared14B30 *shared;
    s32 value;
    s32 base;
    s32 position;
    void (*callback)(void);

    shared = &D_801D4200;
    if (mode == 1)
        goto reduced;
    if (mode >= 2)
        goto high;
    if (mode == 0)
        goto full;
    return;
high:
    if (mode == 2)
        goto tail;
    return;
full:
    if (shared->value_14 == 0) {
        object->value_40 = 2;
        goto reduced;
    }
    object->mode_46 = 3;
    base = D_8009B118;
    object->value_8 = base;
    object->value_c = base + FILE_SECTOR_SIZE;
    object->value_30 = shared->value_c;
    value = shared->value_14;
    object->value_1c = value;
    goto fix;
reduced:
    if (shared->value_18 == 0)
        goto tail;
    position = shared->value_10;
    D_8009B0F4 &= 0xFFDCFFFF;
    object->value_c = position;
    object->value_8 = position;
    object->mode_46 = 1;
    value = shared->value_18;
    object->value_1c = value;
fix:
    if (value < 0)
        object->value_1c = -(value << FILE_SECTOR_SHIFT);
    return;
tail:
    callback = D_8009B128;
    if (callback != 0)
        callback();
}
