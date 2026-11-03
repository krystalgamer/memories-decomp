#include "../../types.h"
#define DUEL_EFFECT_DIALOG_STATE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1B8-0x8009C1D0 (#2602); its
 * layout differs from the North American block the shared source defines. */
u16 gDuel_wViewerCardID;
u16 gDuel_wEffectDialogTextID;
u8 gDuel_bEffectState;
u8 gDuel_bCardViewerYOffset;
u8 gDuel_bEffectHandlerFlags;
u8 D_8009C1BF_sles_03947;
struct DisplayObject *gDuel_pCardViewerCard;
u32 D_8009C1C4_sles_03947;
struct DuelEffectChannel *gDuel_pCardViewerTextBox;
u32 D_8009C1CC_sles_03947;

#define DUEL_EFFECT_DIALOG_DONE_FLAG 0x2000
#define DUEL_EFFECT_DIALOG_CHOICE_FLAG 0x20
#include "../duel_effect_dialog_state.c"
