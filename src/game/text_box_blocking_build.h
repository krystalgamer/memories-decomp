#ifndef MEMORIES_DECOMP_TEXT_BOX_BLOCKING_BUILD_H
#define MEMORIES_DECOMP_TEXT_BOX_BLOCKING_BUILD_H

#include "../types.h"

/* Keep the legacy runtime header and its private regional views unchanged. */
struct DuelEffectChannel;
void TextBox_BuildBlocking(struct DuelEffectChannel *object);
void TextBox_BuildBlockingWithGlyphScratch(struct DuelEffectChannel *object);

/* Unsized original halfword view: RECT prefix and regional glyph scratch. */
extern s16 gText_GlyphUploadScratch[];

#endif
