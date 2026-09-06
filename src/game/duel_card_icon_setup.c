#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

struct Obj {
    char pad4[0x4];
    u32 f4; /* flags */
    char pad10[0x10 - 0x8];
    void (*f10)(void); /* per-frame update callback */
    char pad30[0x30 - 0x14];
    u16 f30; /* x */
    s16 f32;
    u16 f34; /* y */
    char pad42[0x42 - 0x36];
    u16 f42; /* spell/trap icon variant */
    char pad5C[0x5C - 0x44];
    u8 f5C; /* icon display state */
    u8 f5D;
    char pad67[0x67 - 0x5E];
    u8 f67;
    u8 f68;
    u8 f69;
    u8 f6A;
};

struct Coords {
    s16 x;
    s16 y;
};

struct Blob {
    char pad_0000[0x36B4];
    DuelCardRecord record;
};

extern s32 func_8004002C(void);
extern struct Obj *func_800400AC(s32 a0, s32 a1);
extern s32 gDuel_adwCardStats[];
extern void func_80016778(void);
extern u8 D_8015C424[];
extern struct Coords D_800908A0[];
extern u8 *func_800249E0(s32, s32);

/* Allocates a display object, positions it, wires up its per-frame callback,
   and selects a small icon variant for non-monster card types. */
struct Obj *func_80024C1C(s32 cardId, s32 x, s32 y) {
    struct Obj *obj;
    u32 desc;
    s32 type;
    s16 val;

    obj = func_800400AC(func_8004002C(), 0);

    obj->f32 = -0x18;
    obj->f30 = x;
    obj->f34 = y;
    obj->f67 = 0;
    obj->f69 = 0;
    obj->f4 = obj->f4 | 0x1000000;

    desc = gDuel_adwCardStats[cardId - 1];
    obj->f10 = func_80016778;
    obj->f5C = 0;
    obj->f5D = 0xC0;

    type = (s32)desc >> CARD_STAT_TYPE_SHIFT;
    type &= CARD_STAT_TYPE_MASK;
    obj->f68 = (u8)type;
    obj->f42 = 0;

    if (type < CARD_TYPE_MAGIC) {
        goto end;
    }
    obj->f5C = 0x38;

    switch (type) {
        case CARD_TYPE_EQUIP:
            obj->f42 = 1;
            goto end;
        case CARD_TYPE_MAGIC:
            val = 1;
            break;
        case CARD_TYPE_TRAP:
            val = 2;
            break;
        case CARD_TYPE_RITUAL:
            val = 3;
            break;
        default:
            goto end;
    }
    obj->f42 = val;

end:
    return obj;
}

void func_80024D34(s32 a, s32 b)
{
    u8 *slot;
    s32 idx;
    u8 *tb;
    struct Blob *blob;
    struct Obj *obj;

    slot = func_800249E0(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + 0xF;
    }
    tb = D_8015C424;
    blob = (struct Blob *)(tb + idx * sizeof(DuelCardRecord) + 0x48000);
    obj = func_80024C1C(*(s16 *)blob->record.data, D_800908A0[idx].x,
                        D_800908A0[idx].y);
    *(struct Obj **)slot = obj;
    obj->f6A = idx;
}
