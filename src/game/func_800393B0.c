#include "../types.h"

#include "duel_effect.h"
#include "input.h"
#include "text_constants.h"

/* Steps one dialog channel's text stream.

   On the first call for a channel it resolves the string id at +0x36 through
   the three text banks, drops the two display objects, rebuilds them through
   func_800391E4 and points +0x20/+0x24 at the channel's effect entry. Every
   later call runs the pending state handler, then consumes stream bytes: a
   byte below 0xF0 is a glyph and goes to func_80036C14, anything above is a
   command dispatched through D_80090F18.

   Levers that mattered here:
   - D_8009B33A and D_8009B350 are volatile. Retail re-reads both at every use
     - twice inside the loop and twice more in the emit block - and without
     volatile GCC keeps the first load and the halfword reads narrow to lh
     instead of retail's lhu plus sll/sra pair.
   - The stream loop is a backward goto, not for(;;). As a real loop GCC
     rotates it and emits the head twice, which is seven instructions long.
   - The dispatch table is read into a local before the loop so the %hi/%lo
     pair lands in the preheader and stays in $s1 across the handler call; the
     goto loop gives loop.c nothing to hoist on its own.
   - The sign-extended stream byte is bound to a local before the pointer
     advance. Written after it, the scheduler has the advance available to fill
     the load-delay slot after the lbu, and retail leaves that slot as a nop
     with the advance in the branch delay slot instead.
   - The bank decode block is the one func_800383DC already matches with. */

typedef void (*VoidFn)(void);
typedef void (*ChannelFn)(u8 *);

extern u16 D_801B0000[];
extern u16 D_801C0000[];
extern u16 D_801D5800[];
extern u32 D_801D9000[];
extern VoidFn D_80090C50[];
extern ChannelFn D_80090E64[];
extern ChannelFn D_80090F18[];
extern u8 D_8009B357;
extern s32 D_8009B340;
/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   store below a single %gp_rel instruction whose load-delay slot needs the
   retail nop. The address comes from c_symbols.ld, which overrides this common
   symbol, so no storage is allocated here. */
volatile u16 D_8009B33A;
extern volatile s32 D_8009B350;
extern s16 D_8009B35A;
extern u16 D_8009B27C __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));

extern void func_80039E9C(void);
extern void func_8004036C(void *);
extern void func_800391E4(u8 *);
extern void func_80035CA8(s32);
extern void DuelEffect_ClearMatchingMarker(s32);
extern void func_800373C8(u8 *, s32, s32);
extern s32 func_80037C74(u8 *);
extern void func_80036C14(u8 *, s32);

void func_800393B0(u8 *p)
{
    DuelEffectEntry *entry;
    u8 **slot;
    ChannelFn *table;
    u8 *cur;
    u32 text;
    s32 code;
    s32 id;
    u16 flags;

    flags = *(u16 *)(p + 0x34);
    if ((flags & 0x4000) == 0) {
        flags |= 0x4000;
        *(u16 *)(p + 0x34) = flags;
        if ((flags & 2) == 0) {
            func_80039E9C();
        }
        if ((*(u16 *)(p + 0x34) & 0x100) != 0) {
            p[0x5B] = 8;
            p[0x5A] = 8;
        }
        id = *(u16 *)(p + 0x36);
        D_8009B357 = 0;
        D_8009B340 = 0;
        p[0x52] = 1;
        p[0x60] = 0;
        p[0x58] = 0;
        if (id > 0xCFFF) {
            text = ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) +
                   D_801C0000[id - 0xD000];
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = ((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) +
                   D_801D5800[id - TEXT_GLOBAL_STRING_ID_BASE];
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = ((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) + D_801C0000[id];
        }
        *(u32 *)p = text;
        p[0x56] = 0;
        p[0x51] = 0;
        func_8004036C(*(void **)(p + 0x30));
        func_8004036C(*(void **)(p + 0x2C));
        *(s32 *)(p + 0x30) = 0;
        *(s32 *)(p + 0x2C) = 0;
        func_800391E4(p);
        if ((*(u16 *)(p + 0x34) & 0x40) == 0) {
            entry = &D_800EB288[*(u16 *)(p + 0x5C)];
            *(DuelEffectEntry **)(p + 0x24) = entry;
            *(DuelEffectEntry **)(p + 0x20) = entry;
            func_80035CA8(p[0x57]);
            DuelEffect_ClearMatchingMarker(p[0x57]);
        }
        return;
    }

    if (D_8009B357 != 0) {
        D_80090C50[*(u8 *)&D_8009B27C]();
        if (D_8009B27C == 0) {
            D_8009B357 = 0;
        }
    }

    if (p[0x51] != 0) {
        D_80090E64[p[0x51] & 0x1F](p);
        *(u16 *)(p + 0x34) &= ~0x400;
        return;
    }

    if ((*(u16 *)(p + 0x34) & 0x1C00) == 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_SQUARE) != 0 ||
            (gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) != 0) {
            func_800373C8(p, 0, 0);
            p[0x52] = 1;
            *(u16 *)(p + 0x34) |= 0x400;
        }
        p[0x52]--;
        if (p[0x52] != 0) {
            return;
        }
    }

    table = D_80090F18;
    p[0x52] = p[0x53];
loop:
    slot = &((u8 **)p)[*(s8 *)(p + 0x58)];
    cur = *slot;
    D_8009B33A = *cur;
    code = (s16)D_8009B33A;
    *slot = cur + 1;
    if (code >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
        D_8009B350 = 0;
        table[(s16)D_8009B33A - TEXT_SINGLE_BYTE_GLYPH_LIMIT](p);
        if (D_8009B350 >= 0) {
            if (D_8009B350 == 1) {
                return;
            }
            goto loop;
        }
    }

    if (func_80037C74(p) != 0) {
        p[0x51] = 4;
        return;
    }

    D_8009B35A = D_8009B33A;
    func_80036C14(p, D_801D9000[(s16)D_8009B33A] & 0x8FF0FFFF);
    p[0x60]++;
    if (p[0x61] != 0 && p[0x60] >= p[0x61]) {
        *(u16 *)(p + 0x34) |= 0x2000;
    }
    *(u16 *)(p + 0x38) += p[0x5A];
}
