#ifndef MEMORIES_DECOMP_CREDITS_H
#define MEMORIES_DECOMP_CREDITS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

/* One row of the credits table at D_80181D38. `group` is the page the row
 * belongs to and a negative group ends the table; the low nibble of `flags`
 * picks the colour and the high nibble the paragraph inside the page. */
typedef struct {
    s8 group;
    u8 flags;
    u8 pad[2];
    u8 *G32 text;
} CreditsEntry;

/* One of the two text lines the SU credits package draws at a time. */
typedef struct {
    CreditsEntry *G32 entries;
    CreditsEntry *G32 end;
    s16 x;
    s16 y;
    s16 timer;
    u16 width;
    u16 height;
    u8 state;
    u8 field_13;
    u8 page;
    u8 pad[3];
} CreditsLine;

extern RECT D_80180784;
extern RECT D_8018078C;
extern RECT D_80180794;
extern u8 D_80182208;
extern CreditsEntry D_80181D38[];
extern CreditsLine D_8018220C[2];
/* Replacement glyphs for character codes 0xE056 and 0x9B92. */
extern u8 D_80011820[];
extern u8 D_8001183E[];

s32 func_80180F58(s32 index, s32 group);
void func_8018173C(s32 index, s32 c0, s32 c1, s32 c2, s32 c3);

void func_801807B0(void);
s32 func_80180A24(void);
void func_80181C4C(s32 group);
u8 func_80181D28(void);

#endif
