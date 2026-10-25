#ifndef MEMORIES_DECOMP_TEXT_BOX_UPDATE_CHANNELS_H
#define MEMORIES_DECOMP_TEXT_BOX_UPDATE_CHANNELS_H

#include "../types.h"
#include "duel_effect.h"

/* The second cursor the body parks on a record's 0x30 pair, so the choice
   object and its flags are reached at displacements 0 and 4. Kept as its own
   view because folding it into the record base costs a register and four
   instructions. */
typedef struct {
    struct DisplayObject *G32 obj;
    u16 flags;
} ChoiceView;

typedef char ChoiceView_object_offset_must_be_0[
    (u32)&(((ChoiceView *)0)->obj) == 0 ? 1 : -1
];
typedef char ChoiceView_flags_offset_must_be_4[
    (u32)&(((ChoiceView *)0)->flags) == 4 ? 1 : -1
];
typedef char ChoiceView_size_must_be_8[
    sizeof(ChoiceView) == 8 ? 1 : -1
];

/* The default cursor extent; the Japanese body keeps its explicit 0x60
 * byte advance rather than using this shared view's sizeof. */
typedef struct {
    ChoiceView choice;
    u8 pad_08[sizeof(DuelEffectChannel) - sizeof(ChoiceView)];
} ChoiceChannelCursor;

/* Kind four reuses this byte; other indexed kinds replace it. Shared across
 * channels/frames, with no additional reset or initialization. */
extern u8 gText_PreviousGlyphKind;

/* Pumps all four active channels, coordinates their choice objects and
 * forwards the emitted glyph kind to menu records before entry processing. */
void TextBox_UpdateChannels(void);

#endif
