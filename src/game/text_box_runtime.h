#ifndef MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H
#define MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H

#include "../types.h"

/* Declared through the tag rather than the typedef, and duel_effect.h is
   deliberately not included: src/overlays/free_duel/cursor_layout.c reaches
   this header and carries its own `extern u8 D_800EB0F8[]`, which the shared
   declaration would collide with. That local one is a matching lever there. */
struct DuelEffectChannel;
void TextBox_BuildStep(struct DuelEffectChannel *object);
void func_80039140(struct DuelEffectChannel *record);
void TextBox_SetPos(u8 *record, s32 x, s32 y);

/* Requests a build and pumps the text box until it reports done. The two
   entry points differ only in the flag word they set first. */
void func_80039A14(u8 *object);
void func_80039A60(u8 *object);

#endif
