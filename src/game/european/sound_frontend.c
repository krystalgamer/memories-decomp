#include "../../types.h"
#define SOUND_FRONTEND_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C360-0x8009C374 (#2602); its
 * layout differs from the North American block the shared source defines. */
u32 gSD_dwSavedBgmCommand;
s8 gSD_bOutputType;
u8 D_8009C365_sles_03947;
u16 D_8009C366_sles_03947;
u32 gSD_dwCurrentBgmCommand;
u32 D_8009C36C_sles_03947;
u32 D_8009C370_sles_03947;

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../sound_frontend.c"
