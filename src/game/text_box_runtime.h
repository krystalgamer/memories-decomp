#ifndef MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H
#define MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H

#include "../types.h"

/* Declared through the tag rather than the typedef, and duel_effect.h is
   deliberately not included: src/overlays/free_duel/screen_runtime.c reaches
   this header and carries its own byte-array alias for D_800EB0F8, which the
   shared declaration would collide with. That local view is a matching lever
   there. */
struct DuelEffectChannel;
void TextBox_BuildStep(struct DuelEffectChannel *object);

/* Shared glyph-index publication, not a choice index or per-channel array.
 * BuildStep publishes the script word after control/wrap handling. The frame
 * pump stores zero on active-channel entry and -1 before incomplete-channel
 * pumping and batching resets; only nonnegative values index the glyph table.
 * Keep signed halfword storage and all existing publication/reset order. */
extern s16 gText_PumpedGlyphIndex;
/* Repositions an existing type-four outline around an existing primary object.
 * TextBox_SetPos takes a separate path for other second-object types. */
void TextBox_UpdateOutline(struct DuelEffectChannel *record);
void TextBox_SetPos(struct DuelEffectChannel *record, s32 x, s32 y);

/* Refreshes the display objects a record draws through, and is reached only
 * from TextBox_BuildStep above. It creates the object at +0x28 on first use, then
 * on every call copies the record's four geometry halfwords at 0x3C, 0x40,
 * 0x3E and 0x42 into it and derives two half-extents from the latter pair.
 *
 * When the regional outline bit (NA/Japan 0x20, Europe 0x40) is set it also
 * builds the second object at +0x2C, releasing any previous one, and calls
 * TextBox_UpdateOutline on the same record -- which is why it is declared beside it
 * rather than with the display-object headers whose fields it fills. The
 * Primary allocation installs TextBox_DrawEntries; reusing it does not
 * reinstall the callback. Both allocation paths retain their unchecked
 * success assumption. The record is what this operates on. */
void TextBox_RefreshDisplayObjects(struct DuelEffectChannel *record);

/* Requests a build and pumps the text box until it reports done. The two
   entry points differ only in the flag word they set first. */
void func_80039A14(struct DuelEffectChannel *object);
void func_80039A60(struct DuelEffectChannel *object);

#endif
