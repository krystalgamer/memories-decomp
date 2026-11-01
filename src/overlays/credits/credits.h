#ifndef MEMORIES_DECOMP_CREDITS_H
#define MEMORIES_DECOMP_CREDITS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

/* One row of the credits table at gCredits_aEntries. `group` is the page the row
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

extern RECT gCredits_ClutRect0;
extern RECT gCredits_ClutRect1;
extern RECT gCredits_ClutRect2;
extern u8 gCredits_bFinished;
extern CreditsEntry gCredits_aEntries[];
extern CreditsLine gCredits_aLines[2];
/* Replacement glyphs for character codes 0xE056 and 0x9B92. */
extern u8 gCredits_abGlyphE056[];
extern u8 gCredits_abGlyph9B92[];

s32 Credits_BuildLineGlyphs(s32 index, s32 group);
void Credits_DrawLine(s32 index, s32 c0, s32 c1, s32 c2, s32 c3);

extern void Credits_InitPresentation(void);
extern s32 Credits_UpdateLines(void);
extern void Credits_StartLineGroup(s32 group);

u8 Credits_IsFinished(void);

#endif
