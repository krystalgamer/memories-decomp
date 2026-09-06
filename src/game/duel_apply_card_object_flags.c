#include "../types.h"
#include "duel_card.h"

/* Same D_801A7AD8[] stat table (0x1C-byte stride) as
   obj_apply_table801a7ad8_flags.c / table801a7ad8_row_search.c, but with the
   f21/f22 bit mapping SWAPPED relative to that sibling: here the face-down
   flag marks f22, while the defense-position flag marks f21. */
struct Obj {
    char pad0[0x8];
    u16 f8;
    char pad1[0xC - 0xA];
    u32 fC;
    char pad2[0x21 - 0x10];
    u8 f21;
    u8 f22;
    char pad3[0x67 - 0x23];
    u8 f67;
    char pad4[0x6A - 0x68];
    u8 f6A;
};

extern void func_80017DB4(struct Obj *a0);

/* Clears a0->f8's bit 0x4 and a0->f22, then re-derives f22 (0x80) and f21
   (0xC0) from D_801A7AD8[a0->f6A]'s face-down and defense-position flags;
   always sets fC to DUEL_CARD_COLOR_NORMAL, or DUEL_CARD_COLOR_USED if
   DUEL_CARD_FLAG_USED_THIS_TURN is set; runs func_80017DB4(a0), then clears
   a0->f67 unless bit 0x2000 is set. */
void Duel_ApplyCardObjectFlags(struct Obj *a0) {
    u16 flags8 = a0->f8;
    s32 type = a0->f6A;
    DuelCardRecord *rec;

    a0->f22 = 0;
    a0->f8 = flags8 & 0xFFFB;
    rec = &D_801A7AD8[type];

    if (rec->flags & DUEL_CARD_FLAG_FACE_DOWN) {
        a0->f22 = 0x80;
    }
    a0->f21 = 0;
    if (rec->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
        a0->f21 = 0xC0;
    }
    a0->fC = DUEL_CARD_COLOR_NORMAL;
    if (rec->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        a0->fC = DUEL_CARD_COLOR_USED;
    }
    func_80017DB4(a0);
    if (!(rec->flags & 0x2000)) {
        a0->f67 = 0;
    }
}
