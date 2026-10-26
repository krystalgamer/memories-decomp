#define D_8009B0C0_IN_DATA
#define DUEL_TERRAIN_SCALAR_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "graphics_frame.h"
#include "model_scene_states.h"
#include "model_debug_controller.h"
#include "../psyq/libgte.h"
#include "fade.h"
#include "sound.h"
#include "main_modes.h"
#include "../unmatched.h"
#include "model_cleanup.h"
#include "model.h"
#include "model_scene_controllers.h"
#include "duel_terrain_boost.h"
#include "main_mode_state.h"

/* Regional value: the geometry offset y, half the screen height (0x78 ->
 * 0x80). The European build (src/game/european/) defines its own. */
#ifndef ANIMATED_BATTLE_GEOM_OFFSET_Y
#define ANIMATED_BATTLE_GEOM_OFFSET_Y 0x78
#endif

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the first read of it below. c_symbols.ld overrides
   this common symbol, so no storage is allocated here. */
u8 gMain_bActiveModeState;

void Main_RunAnimatedBattle(void)
{
    AnimatedBattleModelProperties *p;
    u8 f;
    s32 v;

    SetGeomOffset(0xA0, ANIMATED_BATTLE_GEOM_OFFSET_Y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    f = gMain_bActiveModeState;
    if ((f & 0x40) == 0) {
        gMain_bActiveModeState = f | 0x40;
        gGraphics_bSyncVBlankTarget = 1;
        Model_ResetSceneView();
        Model_ResetSceneState();
        p = gMain_aAnimatedBattleModels;
        if (p->model_id == MODEL_SPECIAL_BATTLE_ID) {
            gMain_bActiveModeState = gMain_bActiveModeState | 0x20;
            Model_StartExodiaPresentation();
        } else {
            Model_SetSlotProperties(
                0,
                p->model_id - 1,
                p->field_02,
                p->field_04,
                p->guardian_star_selection,
                p->defense_position
            );
            p++;
            Model_SetSlotProperties(
                1,
                p->model_id - 1,
                p->field_02,
                p->field_04,
                p->guardian_star_selection,
                p->defense_position
            );
            Model_SetSlotProperties(2, gDuel_bTerrain);
        }
        Fade_WaitInitIn();
    } else {
        if (f & 0x20) {
            v = Model_IsExodiaPresentationComplete();
        } else {
            v = ModelDebug_UpdateController();
        }
        if (v != 0) {
            SD_KeyOffVoiceSlots();
            SD_BGMFadeOut();
            gMain_bActiveModeState = gMain_bReturnMode;
        }
        Model_UpdateScene();
    }
}
