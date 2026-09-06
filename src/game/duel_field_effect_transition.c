#include "../types.h"
#include "duel_card.h"

typedef struct Obj {
    s16 x;
    s16 y;
    u8 pad_04[0x19];
    u8 field_1D;
    u8 pad_1E[4];
    u8 timer;
    u8 pad_23;
    void *callback;
    u8 pad_28[0x3F];
    u8 mark;
    u8 pad_68[2];
    u8 index;
    u8 pad_6B;
    u8 active;
} Obj;

extern u8 D_800907D8[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
extern u8 D_8009B1D5;
extern Obj *D_8009B17C;
extern u16 D_8009B220;
extern u8 D_8009B260 __attribute__((section(".data")));

extern s32 func_80024E24(void);
extern Obj *func_8002C604(s32);
extern void SD_SEPlayFull(s32);
extern void Duel_ApplyCardObjectFlags(Obj *);
extern s32 func_80042B40(s32);

void func_80025B28(Obj *o)
{
    o->timer += 8;
    if (!(o->active & 0x80) && o->timer >= 40) {
        o->active |= 0x80;
        D_801A7AD8[o->index].flags &= ~0x2000;
        o->mark = 0;
    }
    if (o->timer < 64) {
        o->timer = 0;
        o->active = 0;
        o->callback = 0;
        D_801A7AD8[o->index].flags &= ~0x3400;
        Duel_ApplyCardObjectFlags(o);
    }
}

/* Opens the duel-side effect prompt and, once acknowledged, hands every
   occupied slot of the current side over to the func_80025B28 animation. */
void func_80025BEC(void)
{
    Obj *object;
    Obj *target;
    DuelCardRecord *record;
    u16 flags;
    s32 i;

    if (func_80024E24() == 0) {
        object = func_8002C604(0x13);
        object->x = 0xA0;
        D_8009B17C = object;
        object->y = 0x68;
        SD_SEPlayFull(0x13);
        return;
    }
    flags = D_8009B220;
    if ((flags & 0x40) == 0 && D_8009B17C->field_1D != 0) {
        D_8009B220 = flags | 0x40;
        SD_SEPlayFull(0x1D);
        for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++) {
            record = &D_801A7AD8[D_800907D8[D_8009B1D5][i]];
            /* The retail code tests the two halfwords at +0x14 as one
               word; 0x90000000 selects bits 0x9000 of flags at +0x16. */
            if ((*(u32 *)&record->terrain_modifier & 0x90000000) ==
                0x90000000) {
                target = (Obj *)record->object;
                target->callback = (void *)func_80025B28;
                target->active = 1;
            }
        }
    }
    if ((D_8009B260 & 1) == 0 && func_80042B40(1) == 0) {
        D_8009B220 = 0;
    }
}
