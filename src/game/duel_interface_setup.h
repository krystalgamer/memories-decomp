#ifndef MEMORIES_DECOMP_DUEL_INTERFACE_SETUP_H
#define MEMORIES_DECOMP_DUEL_INTERFACE_SETUP_H

#include "../types.h"

void func_80030198(void);
void func_80030250(
    void *data,
    s32 field_B4,
    s32 field_B5,
    s32 field_B6,
    s32 field_B8,
    s32 field_C0,
    s32 field_E0
);

/* Stored by func_80030250 (duel_interface_setup.c:66, `D_8009B2DC = 0;`;
 * func_80030250.s:13 `sb $zero`) and loaded by func_800307B8 through an
 * lvalue cast, `*(s8 *)&D_8009B2DC` (func_800307B8.c:61; func_800307B8.s:70
 * `lb`). Both units declared it u8; the `lb` is the cast, not the
 * declaration. Still in assembly: func_80030294.s (lb :57, :228, :313;
 * lbu :259, :289; sb :262, :270, :292, :297). Nothing is named at +1
 * (c_symbols.ld:194 is followed by D_8009B2E0 at :195); D_8009B2DE,
 * declared `extern u16` at func_80031354.c:9, is at +2. Every access is
 * `%gp_rel`, so the plain declaration; no .data arm. */
extern u8 D_8009B2DC;

/* Stored by func_80030250 (duel_interface_setup.c:65, `D_8009B2E9 = 0;`;
 * func_80030250.s:12 `sb $zero`) and loaded by func_800307B8 through an
 * lvalue cast, `*(s8 *)&D_8009B2E9` (func_800307B8.c:64; func_800307B8.s:87
 * `lb`). Both units declared it u8. Still in assembly: func_80030294.s
 * (lb :160, :317; lbu :252, :281; sb :255, :271, :284, :290, :300).
 * D_8009B2EA is the next symbol, at +1 (c_symbols.ld:199). Every access is
 * `%gp_rel`, so the plain declaration; no .data arm. */
extern u8 D_8009B2E9;

#endif
