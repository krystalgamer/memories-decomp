#ifndef MEMORIES_DECOMP_CREDITS_H
#define MEMORIES_DECOMP_CREDITS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

/* One of the two text lines the SU credits package draws at a time. */
typedef struct {
    s32 active;
    s32 field_04;
    s16 x;
    s16 y;
    s16 field_0C;
    u16 width;
    u16 height;
    u8 field_12;
    u8 field_13;
    u8 field_14;
    u8 pad[3];
} CreditsLine;

extern RECT D_80180784;
extern RECT D_8018078C;
extern RECT D_80180794;
extern u8 D_80182208;
extern CreditsLine D_8018220C[2];

s32 func_80180F58(s32 index, s32 text);

void func_801807B0(void);
s32 func_80180A24(void);
void func_80181C4C(s32 text);
u8 func_80181D28(void);

#endif
