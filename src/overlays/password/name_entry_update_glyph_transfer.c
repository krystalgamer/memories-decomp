#include "../../types.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_lifecycle.h"
#include "name_entry_keyboard.h"

extern s8 D_8016D42C;
extern void func_800429D8(u8 *);
extern void func_80042A78(u8 *);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void NameEntry_UpdateGlyphTransfer(u8 *w)
{
    u8 *o;
    DuelEffectEntry *node;
    s32 n;

    if ((w[0x6C] & 0x80) == 0) {
        w[0x6C] |= 0x80;
        func_800429D8(w);
        *(s16 *)(w + 0x36) =
            ((*(s16 *)(w + 0x44) - *(s16 *)(w + 0x30)) << 8) / *(s16 *)(w + 0x60);
        *(s16 *)(w + 0x38) =
            ((*(s16 *)(w + 0x46) - *(s16 *)(w + 0x32)) << 8) / *(s16 *)(w + 0x60);
    }
    if ((w[0x6C] & 0x40) != 0) {
        n = *(u16 *)(w + 0x60) - 1;
        *(s16 *)(w + 0x60) = n;
        if ((s16)n <= 0) {
            func_8004036C(w);
        }
        return;
    }
    func_80042A78(w);
    n = *(u16 *)(w + 0x60) - 1;
    *(s16 *)(w + 0x60) = n;
    if ((s16)n > 0) {
        o = func_800400AC(func_8004002C(), 1);
        func_80040510(o, *(s16 *)(w + 0x30), *(s16 *)(w + 0x32), 16, 16,
                      w[0x5C], w[0x5D], w[0x66], *(u16 *)(w + 0x40),
                      *(u16 *)(w + 0x42));
        *(u32 *)(o + 0x0C) = 0x606060;
        *(s16 *)(o + 0x60) = 6;
        *(void **)(o + 0x24) = func_80042BC0;
        *(u32 *)(o + 4) |= 0x50000000;
        return;
    }
    w[0x6C] |= 0x40;
    *(u32 *)(w + 0x30) = *(u32 *)(w + 0x44);
    /* The reload of the timer is written inside the last argument, not as the
       statement before the call.  A statement is emitted ahead of every
       argument, which leaves the two instructions for it at the head of the
       block and pushes the zero for a2 to the end; carried with the argument
       they are emitted after a1 is computed, which is the order the target
       has.  See notes/overlays/README.md. */
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, (*(s16 *)(w + 0x60) = 2, 0));
    if (node == 0) {
        return;
    }
    o = NameEntry_SpawnGlyphSprite(3, node);
    *(NameEntryGlyphUpdate *)(o + 0x24) = NameEntry_UpdateGlyphShatter;
    o[0x6C] = 5;
}
