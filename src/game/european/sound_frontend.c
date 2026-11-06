#include "../../types.h"
#define SOUND_FRONTEND_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C360-0x8009C374 (#2602); its
 * layout differs from the North American block the shared source defines. */
u32 gSD_dwSavedBgmCommand;
s8 gSD_bOutputType;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
u32 gSD_dwCurrentBgmCommand;
static PADDING_VARIABLE(4);
static PADDING_VARIABLE(4);

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../sound_frontend.c"
