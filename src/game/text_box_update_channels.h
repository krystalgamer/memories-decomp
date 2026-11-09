#ifndef MEMORIES_DECOMP_TEXT_BOX_UPDATE_CHANNELS_H
#define MEMORIES_DECOMP_TEXT_BOX_UPDATE_CHANNELS_H

#include "../types.h"
#include "duel_effect.h"

/* Kind four reuses this byte; other indexed kinds replace it. Shared across
 * channels/frames, with no additional reset or initialization. */
extern u8 gText_PreviousGlyphKind;

/* Pumps all four active channels, coordinates their choice objects and
 * forwards the emitted glyph kind to menu records before entry processing. */
void TextBox_UpdateChannels(void);

#endif
