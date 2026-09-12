#ifndef MEMORIES_DECOMP_FUNC_80013C28_H
#define MEMORIES_DECOMP_FUNC_80013C28_H

#include "../types.h"

/* DsStartReadySystem supplies three callback arguments here, while the
 * unmatched definition currently consumes only the event word. Keep both
 * measured views owned beside that definition. */
#ifdef FUNC_80013C28_CALLBACK_VIEW
void func_80013C28(u8 event, u8 *sector, u32 *header);
#else
void func_80013C28(s32 event);
#endif

#endif
