#include "../types.h"
#include "duel_effect_state_latch.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "duel_effect_dialog_state.h"
#include "dialog_choice_state.h"
#include "func_80039794.h"
#include "input_is_pad1_confirm_pressed.h"

extern unsigned short D_8009B244;extern unsigned char gDuel_bEffectHandlerFlags;
void DuelEffect_UpdateDialogState(void){DuelEffectChannel*o;if(DuelEffect_MarkStateInitialized()==0){TextBox_Create(0,D_8009B244,0x10,0xB0,0x120,0x30);return;}func_80039794();o=D_800EB0F8;if((gDuel_bEffectHandlerFlags&0x40)==0){unsigned short f=o->flags_34;if((f&TEXT_BOX_FLAG_DONE)==0)return;if((f&0x10)==0){o->field_30=Dialog_OpenChoice(o);gDuel_bEffectHandlerFlags|=0x40;return;}}else{if(Input_IsPad1ConfirmPressed()==0)return;SD_SEPlayFull(11);}TextBox_Destroy(o);gDuel_bEffectState|=DUEL_EFFECT_STATE_FLAG_COMPLETE;}
