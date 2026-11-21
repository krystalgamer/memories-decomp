#ifndef MEMORIES_DECOMP_OVERLAYS_SAFECHECK2003_SAFECHK_H
#define MEMORIES_DECOMP_OVERLAYS_SAFECHECK2003_SAFECHK_H

#include "../../types.h"
#include "entrypoints.h"

/* Sony's runtime library safechk 2.0.0.3, the boot module safecheck2003
 * (WA_MRG module 0x16, loaded at 0x80168000). Offsets come from the retail
 * accesses. */

/* One safechk_commands entry: a CD-ROM controller command, its parameter and
 * result byte counts, and the sum of the interrupt numbers that complete it
 * (3 for INT3 alone, 5 for INT3 then INT2). */
typedef struct {
    u8 cmd;
    u8 nparams;
    u8 nresults;
    u8 intsum;
} SafechkCdCommand;

/* One safechk_messages entry: the warning text and its glyph layout. */
typedef struct {
    u8 count;
    u8 unk1;
    u8 width;
    u8 unk3;
    s16 x;
    s16 y;
    u8 *text;
} SafechkMessage;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} SafechkRect;

/* The glyph sprite SafetyDrawGlyph builds on the stack. */
typedef struct {
    u32 tag;
    u32 code;
    u32 xy;
    u32 wh;
    u16 data[16];
} SafechkGlyphPrim;

/* The flat quad SafetyDrawProhibitionSign builds on the stack. */
typedef struct {
    u32 tag;
    u32 code;
    u32 xy[4];
} SafechkSignPoly;

s32 SafetyPollResult(s32 n);
void SafetySendCommand(s32 n);
void SafetyShowWarningAndExit(void);
void SafetyDrawText(u8 *str, SafechkRect *rect, s32 color);
void SafetyDrawGlyph(SafechkRect *rect, u8 *bits, s32 color, u32 range);
u32 SafetyGetGlyphRange(u8 *p);
void SafetyDrawProhibitionSign(void);

#endif
