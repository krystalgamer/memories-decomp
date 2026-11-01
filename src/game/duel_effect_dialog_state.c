#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B244-0x8009B258, defined in
 * address order ahead of the headers that declare it (#2602). */
u16 gDuel_wEffectDialogTextID;
u16 gDuel_wViewerCardID;
u8 gDuel_bEffectHandlerFlags;
static u8 D_8009B249;
u8 gDuel_bActiveEffectState;
u8 gDuel_bCardViewerYOffset;
struct DisplayObject *G32 gDuel_pCardViewerCard;
struct DuelEffectChannel *G32 gDuel_pCardViewerTextBox;
u8 gDuel_bEffectState;
static u8 D_8009B255;
static u16 D_8009B256;

#include "duel_effect_state_latch.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "duel_effect_dialog_state.h"
#include "dialog_choice_state.h"
#include "text_box_update_channels.h"
#include "input_is_pad1_confirm_pressed.h"

#ifndef DUEL_EFFECT_DIALOG_DONE_FLAG
#define DUEL_EFFECT_DIALOG_DONE_FLAG TEXT_BOX_FLAG_DONE
#endif

#ifndef DUEL_EFFECT_DIALOG_CHOICE_FLAG
#define DUEL_EFFECT_DIALOG_CHOICE_FLAG 0x10
#endif

int DuelEffect_MarkStateInitialized(void)
{
    unsigned char value = gDuel_bEffectHandlerFlags;

    if (!(value & DUEL_EFFECT_DIALOG_FLAG_CREATED)) {
        gDuel_bEffectHandlerFlags = value | DUEL_EFFECT_DIALOG_FLAG_CREATED;
        return 0;
    }
    return 1;
}

void DuelEffect_UpdateDialogState(void){DuelEffectChannel*o;if(DuelEffect_MarkStateInitialized()==0){TextBox_Create(0,gDuel_wEffectDialogTextID,0x10,0xB0,0x120,0x30);return;}TextBox_UpdateChannels();o=gText_aChannels;if((gDuel_bEffectHandlerFlags&DUEL_EFFECT_DIALOG_FLAG_CHOICE_OPEN)==0){unsigned short f=o->flags_34;if((f&DUEL_EFFECT_DIALOG_DONE_FLAG)==0)return;if((f&DUEL_EFFECT_DIALOG_CHOICE_FLAG)==0){o->prompt_object=Dialog_OpenChoice(o);gDuel_bEffectHandlerFlags|=DUEL_EFFECT_DIALOG_FLAG_CHOICE_OPEN;return;}}else{if(Input_IsPad1ConfirmPressed()==0)return;SD_SEPlayFull(11);}TextBox_Destroy(o);gDuel_bEffectState|=DUEL_EFFECT_STATE_FLAG_COMPLETE;}
