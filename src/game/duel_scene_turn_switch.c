#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "duel_scene_state.h"
#include "duel_view_tween.h"
#include "duel_scene_state.h"
#include "../unmatched.h"
#include "duel_scene_turn_switch.h"

void DuelScene_UpdateTurnSwitch(void){if(!(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED)){gDuel_wSceneStateFlags|=DUEL_SCENE_FLAG_INITIALIZED;Duel_StartViewTween(0x30,0x258,0x100,D_8009B1D5?0x400:0xC00,0);gDuel_wDisplayTransitionFlags=0x10;}
 if(!gDuel_wDisplayTransitionFlags){D_8009B1D5^=1;gDuel_wSceneStateFlags=2;gDuel_pActiveSideState=&gDuel_aSideStates[D_8009B1D5];D_8009B22C=gDuel_abFieldGridCardRecordIndices+D_8009B1D5*DUEL_FIELD_SIDE_GRID_SLOT_COUNT;}}
