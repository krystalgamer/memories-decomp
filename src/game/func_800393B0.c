#include "../types.h"
#include "func_8004036C.h"
#include "duel_effect.h"
#include "text_constants.h"

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
   opcode store below a single %gp_rel instruction whose load-delay slot needs
   the retail nop. The address comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here.  volatile is what keeps the
   read-back after the store, which retail issues at every use. */
volatile u16 D_8009B33A;
extern volatile s32 D_8009B350;
extern u16 D_8009B35A;
extern u16 D_8009B27C __attribute__((section(".data")));
extern u16 gInput_wPad1Held __attribute__((section(".data")));
extern u16 gInput_wPad1Pressed __attribute__((section(".data")));

extern void func_80039E9C(void);
extern void func_800391E4(u8 *);
extern void func_80035CA8(s32);
extern void DuelEffect_ClearMatchingMarker(s32);
extern void func_800373C8(u8 *, s32, s32);
extern s32 func_80037C74(u8 *);
extern void func_80036C14(u8 *, s32);

void func_800393B0(u8 *object)
{
    u16 flags;
    s32 id;
    u8 *text;
    u8 **slot;
    u8 *script;
    DuelEffectEntry *entry;
    s32 op;
    ChannelFn *handlers;

    flags = *(u16 *)(object + 0x34);
    if ((flags & 0x4000) == 0) {
        flags |= 0x4000;
        *(u16 *)(object + 0x34) = flags;
        if ((flags & 2) == 0) {
            func_80039E9C();
        }
        if ((*(u16 *)(object + 0x34) & 0x100) != 0) {
            object[0x5B] = 8;
            object[0x5A] = 8;
        }
        id = *(u16 *)(object + 0x36);
        D_8009B357 = 0;
        D_8009B340 = 0;
        object[0x52] = 1;
        object[0x60] = 0;
        object[0x58] = 0;
        if (id > 0xCFFF) {
            text = (u8 *)(((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) +
                D_801C0000[id - 0xD000]);
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = (u8 *)(((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) +
                D_801D5800[id - TEXT_GLOBAL_STRING_ID_BASE]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) +
                D_801C0000[id]);
        }
        *(u8 **)object = text;
        object[0x56] = 0;
        object[0x51] = 0;
        func_8004036C(*(void **)(object + 0x30));
        func_8004036C(*(void **)(object + 0x2C));
        *(s32 *)(object + 0x30) = 0;
        *(s32 *)(object + 0x2C) = 0;
        func_800391E4(object);
        if ((*(u16 *)(object + 0x34) & 0x40) == 0) {
            entry = &D_800EB288[*(u16 *)(object + 0x5C)];
            *(DuelEffectEntry **)(object + 0x24) = entry;
            *(DuelEffectEntry **)(object + 0x20) = entry;
            func_80035CA8(object[0x57]);
            DuelEffect_ClearMatchingMarker(object[0x57]);
        }
        return;
    }

    if (D_8009B357 != 0) {
        D_80090C50[*(u8 *)&D_8009B27C]();
        if (D_8009B27C == 0) {
            D_8009B357 = 0;
        }
    }
    if (object[0x51] != 0) {
        D_80090E64[object[0x51] & 0x1F](object);
        *(u16 *)(object + 0x34) = *(u16 *)(object + 0x34) & 0xFBFF;
        return;
    }
    if ((*(u16 *)(object + 0x34) & 0x1C00) == 0) {
        if ((gInput_wPad1Held & 0x80) || (gInput_wPad1Pressed & 0xC0)) {
            func_800373C8(object, 0, 0);
            object[0x52] = 1;
            *(u16 *)(object + 0x34) = *(u16 *)(object + 0x34) | 0x400;
        }
        object[0x52] = object[0x52] - 1;
        if (object[0x52] != 0) {
            return;
        }
    }
    handlers = D_80090F18;
    object[0x52] = object[0x53];
next_opcode:
    slot = (u8 **)(object + *(s8 *)(object + 0x58) * 4);
    script = *slot;
    D_8009B33A = script[0];
    op = (s16)D_8009B33A;
    *slot = script + 1;
    if (op >= 0xF0) {
        D_8009B350 = 0;
        handlers[(s16)D_8009B33A - 0xF0](object);
        if (D_8009B350 >= 0) {
            if (D_8009B350 == 1) {
                return;
            }
            goto next_opcode;
        }
    }
    if (func_80037C74(object) != 0) {
        object[0x51] = 4;
        return;
    }
    D_8009B35A = D_8009B33A;
    func_80036C14(object, D_801D9000[(s16)D_8009B33A] & 0x8FF0FFFF);
    object[0x60] = object[0x60] + 1;
    if (object[0x61] != 0 && object[0x60] >= object[0x61]) {
        *(u16 *)(object + 0x34) = *(u16 *)(object + 0x34) | 0x2000;
    }
    *(u16 *)(object + 0x38) = *(u16 *)(object + 0x38) + object[0x5A];
}
