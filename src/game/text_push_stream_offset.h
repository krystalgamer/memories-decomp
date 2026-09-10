#ifndef MEMORIES_DECOMP_TEXT_PUSH_STREAM_OFFSET_H
#define MEMORIES_DECOMP_TEXT_PUSH_STREAM_OFFSET_H

#include "../types.h"
#include "duel_effect.h"

/* D_80090EAC entry: pushes a stream cursor. It takes a word through
 * func_80036D3C, writes it as the low half of the NEXT cursor slot -- keeping
 * that slot's high half -- and then increments the object's +0x58 cursor index,
 * so the following commands read from the new stream. */
void Text_PushStreamOffset(DuelEffectChannel *object);

#endif
