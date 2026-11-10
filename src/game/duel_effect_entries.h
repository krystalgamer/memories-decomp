#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRIES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRIES_H

#include "../types.h"
#include "duel_effect.h"


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
extern u16 gText_awGlyphNibblePatterns[16];
extern u16 gText_awGlyphColorMasks[];
#endif

/* The glyph records Text_FindRecordById walks: a zero-terminated table of
 * big-endian halfword ids and, in lockstep, 0x1E-byte records. The Japanese
 * rasterizer reads a record as fifteen 16-pixel rows of glyph bits; Japan
 * defines its 27 records in japanese/text_glyph_records.c. The North American
 * image has no records: both labels sit just past the 93-entry glyph lookup
 * table, where the linker script binds them. */
extern u8 gText_aGlyphRecordBitmaps[];
extern u8 gText_aGlyphRecordKeys[];

#endif
