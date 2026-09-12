/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers and 3 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_scene_update.c.
 */
#define D_8009B260_IN_DATA
#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/input.h"
#include "../game/duel_check_quit_input.h"
#include "../game/text_box_lifecycle.h"
#include "../game/sound.h"
#include "../game/func_80039794.h"
#include "../game/duel_scene_callbacks.h"
#include "../game/duel_scene_state.h"
#include "../game/duel_scene_update.h"
#include "../game/duel_effect.h"
#include "../game/duel_effect_request.h"
#include "../game/func_8002C6C8.h"
#include "../unmatched.h"
#include "../game/duel_magic_effect_dispatch.h"

extern s8 gDialog_bChoice[9];
void func_80024200(void)
{
    u8 value;
    DuelEffectChannel *window;

    if (D_8009B162 != 0) {
        func_800235C0();
    }
    func_8002C6C8();
    value = D_8009B260;
    if (value & 0x80) {
        if (value & 1) {
            return;
        }
        D_8009B260 = value & 0x7F;
    }
    if (DuelEffect_UpdateCardEffect() != 0 || DuelEffect_UpdateState() != 0) {
        return;
    }
    value = gDuel_bQuitDialogState;
    if (value != 0) {
        if (!(value & 0x80)) {
            gDuel_bQuitDialogState = value | 0x80;
            SD_SEPlayFull(0x30);
            window =
                TextBox_CreateFlagged(3, 0x22, 0x78, 0x58, 0x50, 0x24, 0x20);
            do {
                func_80039794();
            } while (window->field_30 == 0);
        } else {
            register DuelEffectChannel *cleanup __asm__("$4");

            func_80039794();
            __asm__(
                "lui $2,%%hi(D_800EB224)\n\t"
                "addiu %0,$2,%%lo(D_800EB224)"
                : "=r"(cleanup)
            );
            if (cleanup->flags_34 & 0x2000) {
                TextBox_Destroy(cleanup);
                gDuel_bQuitDialogState = 0;
                if (gDialog_bChoice[0] != 0) {
                    D_8009B16C |= 0x2000;
                }
            }
        }
    } else {
        register void (**callbacks)(void) __asm__("$3");
        register u16 index __asm__("$2");

        __asm__("lui %0,%%hi(D_80090998)" : "=r"(callbacks));
        index = D_8009B23A;
        __asm__("addiu %0,%0,%%lo(D_80090998)" : "+r"(callbacks));
        callbacks[index & DUEL_SCENE_PHASE_MASK]();
        if (!(D_8009B23A & DUEL_SCENE_FLAG_INITIALIZED)) {
            D_8009B174 = 0;
        }
    }
}
