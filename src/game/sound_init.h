#ifndef MEMORIES_DECOMP_SOUND_INIT_H
#define MEMORIES_DECOMP_SOUND_INIT_H

#include "../types.h"
#include "../psyq/libspu_internal.h"

extern u8 D_80010784[];
extern u8 D_80010790[];
extern u8 D_8001079C[];

void func_80049200(s32 value);
void func_80049230(s32 value, s32 data);
/* func_800473CC passes the second argument as s16, which the caller-side
 * sign extension depends on. That consumer defines SOUND_INIT_S16_VIEW; every
 * other unit sees only the word prototype above and no assembler label. */
#ifdef SOUND_INIT_S16_VIEW
extern void func_80049230_s16(s32 value, s16 data) asm("func_80049230");
#endif
void SD_Init(void);
void func_80049308(void);
void func_80049394(void *entry);
void func_800493F8(void);
void func_80049434(void);
void func_800494F4(s32 *data);
void func_80049544(void);
void func_80049594(s32 value);
void func_800495A4(void);
void func_800495DC(void);
void func_800495EC(void);

#endif
