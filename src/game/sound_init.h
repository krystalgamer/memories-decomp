#ifndef MEMORIES_DECOMP_SOUND_INIT_H
#define MEMORIES_DECOMP_SOUND_INIT_H

#include "../types.h"

void func_80049200(s32 value);
void func_80049230(s32 value, s32 data);
extern void func_80049230_s16(s32 value, s16 data) asm("func_80049230");
void func_80074E60(void);
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
