#define D_8009B260_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "input.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "func_80039794.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_update.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "func_8002C6C8.h"
#include "../unmatched.h"
#include "duel_magic_effect_dispatch.h"

extern u8 gDuel_bQuitDialogState;
extern u16 D_8009B16C;
extern s8 gDialog_bChoice[9];
extern DuelEffectChannel D_800EB224;
extern s8 gDuel_bOpponentID[9];
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
    if (func_80026B34() != 0 || DuelEffect_UpdateState() != 0) {
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
        callbacks[index & 0xF]();
        if (!(D_8009B23A & 0x8000)) {
            D_8009B174 = 0;
        }
    }
}

void func_80024388(void)
{
    int value = 0;

    if (gDuel_bOpponentID[0] < 0) {
        value = D_8009B1D5;
        if (D_8009B238 >= 0) {
            value = D_8009B238;
        }
    }
    if (value != 0) {
        Input_BackupPad1AndUsePad2();
        func_80024200();
        Input_RestorePad1FromBackup();
    } else {
        func_80024200();
    }
}
