#ifndef MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H
#define MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H

#include "../types.h"
#include "duel_effect.h"

void TextBox_BuildStep(DuelEffectChannel *object);
void func_80039140(u8 *record);
void TextBox_SetPos(u8 *record, s32 x, s32 y);

/* Requests a build and pumps the text box until it reports done. The two
   entry points differ only in the flag word they set first. */
void func_80039A14(u8 *object);
void func_80039A60(u8 *object);

#endif
