#include "../types.h"
#include "card_constants.h"

/* Allocates a display object (get_or_init_D_800EFE48_slot, type 0),
   positions it at (x, y), wires up its per-frame callback, and — only for
   cards whose type field (bits 26-30 of the packed gDuel_adwCardStats[cardId-1]
   record, same field decoded by idx_table_bitfield_copy.c and
   bitfield_compare_calls_store.c) is one of the non-monster CARD_TYPE_*
   values — tags it with a small icon variant in f42 and switches its state
   (f5C) into "has icon" mode. */
struct Obj {
    char pad4[0x4];
    u32 f4; /* flags; bit 0x1000000 set here */
    char pad10[0x10 - 0x8];
    void (*f10)(void); /* per-frame update callback */
    char pad30[0x30 - 0x14];
    u16 f30; /* x */
    s16 f32;
    u16 f34; /* y */
    char pad42[0x42 - 0x36];
    u16 f42; /* spell/trap icon variant, set only for type 0x14-0x17 */
    char pad5C[0x5C - 0x44];
    u8 f5C; /* state; 0x38 once an icon variant is assigned */
    u8 f5D;
    char pad67[0x67 - 0x5E];
    u8 f67;
    u8 f68; /* card type field */
    u8 f69;
};

extern s32 func_8004002C(void);
extern struct Obj *func_800400AC(s32 a0, s32 a1);
extern s32 gDuel_adwCardStats[];
extern void func_80016778(void);

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

    type = (s32)desc >> 26;
    type &= 0x1F;
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
