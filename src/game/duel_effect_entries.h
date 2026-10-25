#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRIES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRIES_H

#include "../types.h"
#include "duel_effect.h"

extern s16 D_801DA000[];

/* Returns the selected opaque payload byte address, or zero at the key
 * terminator. Key and payload cursors retain their distinct 2/30-byte steps. */
u8 *Text_FindRecordById(s32 id);

/* Conditionally appends an entry at the channel's 0x20 tail. Header stores
 * may already have happened on a rejected value; no append status is implied.
 *
 * NA/Japan use 0x1C/0x18-byte entries and return void. Europe uses compact
 * 0x16-byte entries and returns a signed spacing adjustment, not an append
 * Boolean or the complete glyph width. Keep each regional flag domain. */
#ifdef VERSION_EUROPE
s32 DuelEffect_AppendEntry(DuelEffectChannel *channel, s32 tagged_value);
#else
void DuelEffect_AppendEntry(DuelEffectChannel *channel, s32 tagged_value);
#endif

#ifdef VERSION_JAPAN
void DuelEffect_RasterizeGlyph(DuelEffectChannel *channel, s32 tagged_value, s32 slot, s16 *bitmap);
/* The sixteen 4bpp pixel patterns for one nibble of glyph bits, and the
 * colour masks a channel's color_index_54 selects among. */
extern u16 D_80090E28[16];
extern u16 D_80090E48[];
#define gText_awGlyphNibblePatterns D_80090E28
#define gText_awGlyphColorMasks D_80090E48
#endif

/* The lookup's original symbol remains for non-C callers. */
u8 *func_80036BCC(s32 id);

#endif
