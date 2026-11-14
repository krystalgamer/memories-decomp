#ifndef MEMORIES_DECOMP_CANDIDATES_APV2_H
#define MEMORIES_DECOMP_CANDIDATES_APV2_H

#include "../../types.h"

/* Types shared by the APv2 module candidates (WA_MRG module 0x16, loaded at
 * 0x80168000). Offsets come from the retail accesses. */

/* One D_8016941C entry: a CD-ROM controller command, its parameter and
 * result byte counts, and a wait value. */
typedef struct {
    u8 cmd;
    u8 nparams;
    u8 nresults;
    u8 wait;
} Apv2CdCommand;

/* One D_8016947C entry: a warning message line. */
typedef struct {
    u8 count;
    u8 unk1;
    u8 width;
    u8 unk3;
    s16 x;
    s16 y;
    u8 *text;
} Apv2Message;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Apv2Rect;

/* The glyph sprite func_8016903C builds on the stack. */
typedef struct {
    u32 tag;
    u32 code;
    u32 xy;
    u32 wh;
    u16 data[16];
} Apv2GlyphPrim;

/* The frame polygon func_80169284 builds on the stack. */
typedef struct {
    u32 tag;
    u32 code;
    u32 xy[4];
} Apv2FramePoly;

#endif
