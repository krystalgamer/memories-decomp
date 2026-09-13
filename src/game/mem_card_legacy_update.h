#ifndef MEMORIES_DECOMP_MEM_CARD_LEGACY_UPDATE_H
#define MEMORIES_DECOMP_MEM_CARD_LEGACY_UPDATE_H

#include "../types.h"
#include "menu_record.h"

void func_8003DC1C(void);

/* Legacy dialog workspace and the poll's two result words. Its selected
 * transfer record starts at +0x1C and advances by 0x268 per selector. */
extern u8 D_800EF6D0[];
extern s32 D_8009B3C8;
extern s32 D_8009B3E4;
extern u8 D_8009B3D5;

/* Like the explicit unused-argument view in func_80058434.h, this preserves
 * the measured caller ABI. The four resident handler implementations also
 * serve D_80090F68 and consume only the first MenuRecord argument. The legacy
 * table call additionally supplies its selected transfer record. */
#ifdef MEM_CARD_LEGACY_CALL_WITH_SLOT
extern void (*D_80090F88[5])(MenuRecord *, u8 *);
#else
extern void (*D_80090F88[5])(MenuRecord *);
#endif

extern char D_80010378[];
extern char D_80010398[];
extern char D_800103A4[];
extern char D_8009AF6C[];

/* Original raw-sector byte labels, not new payload storage. */
extern u8 D_8021007A __attribute__((section(".data")));
extern u8 D_8021007B __attribute__((section(".data")));
extern u8 D_8021007C __attribute__((section(".data")));
extern u8 D_8021007D __attribute__((section(".data")));
extern u8 D_8021007E __attribute__((section(".data")));
extern u8 D_8021007F __attribute__((section(".data")));

#endif
