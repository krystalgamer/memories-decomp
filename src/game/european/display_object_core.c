#include "../../types.h"
#define DISPLAY_OBJECT_CORE_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800F0DA0-0x800F37F0 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern s16 gDisplayObject_awListHeads[];
extern s16 gDisplayObject_awListKeys[];
extern struct ViewState gViewState_Current;
extern struct DisplayObject gDisplayObject_aPool[];
#define DISPLAY_OBJECT_CORE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C374-0x8009C388 (#2602); its
 * layout differs from the North American block the shared source defines. */
s32 gDisplayObject_fRenderRetry;
u32 D_8009C378_sles_03947;
u32 D_8009C37C_sles_03947;
u32 D_8009C380_sles_03947;
u16 gDisplayObject_wUnlinkCount;
u16 gDisplayObject_wAcquireCount;

#define DISPLAY_OBJECT_SPRITE_TEXTURE_U(texture) (((texture) & 0xF0) + 0x280)
#define DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE(texture) \
    ((((texture) & 0x300) >> 4) + 0xD0)

#include "../display_object_core.c"
