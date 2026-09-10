#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "data_transfer_request.h"
#include "card_constants.h"
#include "display_object_api.h"
#include "duel_effect.h"
#include "func_80039794.h"
#include "input.h"
#include "mem_card_dialog_runtime.h"
#include "save_data_update_trade_load.h"
#include "save_data_update_load_pair.h"
#include "mem_card.h"
#include "save_data.h"
#include "text_box_lifecycle.h"
#include "text_sjis_to_glyph_codes.h"
#include "two_player_save_setup.h"
#include "text_staging.h"
#include "util_memory.h"
#include "../unmatched.h"

extern u8 D_801D2200[];
extern u8 D_801D160C[];
extern u8 D_801B122B[];
extern u8 D_801B1238[];

/* The complete single-player and two-player save-transfer runtime. The first
   three functions request, poll, apply, build, and write the resident save;
   the remaining state machine loads both card slots, then the two wrappers add
   trade or duel setup, and the final request writes the paired trade result
   back. */

void SaveData_RequestLoad(void)
{
    /* The symbolic store changes the target $at scheduling and relocation. */
    *(u8 *)0x8009B0D1 = 0;
    MemCardDialog_Request(
        gSaveData_aTransferBuffer,
        SAVE_DATA_STATE_SIZE,
        gMemCard_szSaveFileName,
        0
    );
}

s32 SaveData_PollLoad(void) {
    s32 r = MemCardDialog_Poll();
    if (r != 0) {
        if (r == 1) {
            u8 *p = (u8 *)gDuel_awPlayerDeck;
            Util_CopyWords(p, gSaveData_aTransferBuffer, SAVE_DATA_STATE_SIZE);
            SaveData_ApplyRuntimeState((SaveDataState *)p);
        }
        return r;
    }
    return 0;
}

void SaveData_RequestWrite(void){Util_CopyWords(gSaveData_aTransferBuffer,(u8 *)gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);SaveData_BuildPayload(gSaveData_aTransferBuffer-SAVE_DATA_HEADER_SIZE);MemCardDialog_Request(gSaveData_aTransferBuffer,SAVE_DATA_REPLICATED_STATE_SIZE,gMemCard_szSaveFileName,2);}

/* The two-player load, validation and write-back runtime. The state machine
   first loads both card slots, then the two wrappers add trade or duel setup,
   and the final request writes the paired trade result back. */

/* MATCH 2026-09-06, first-day function from the m2c draft (257 instructions,
 * the memory-card save state machine: format, load, verify, retry). Jump
 * table on the mode nibble, so only the full build proves it. Levers, in
 * order: the scalar arm for D_8009B3EA; gInput_wPad1Pressed on its .data
 * VOLATILE arm (retail reloads it for the second bit test and reads it bare);
 * the call arguments D_8009B3EE and D_8009B3C0 hoisted into locals BEFORE the
 * +0x60 store that precedes the sprite call (-6 -> -1); ONE name for the
 * sprite pointer in cases 0 and 0xA, which is what puts it in $s0 (its live
 * range crosses the wait loop's calls in 0xA); the record passed to
 * TextBox_Destroy as its ADDRESS, index-first (`&D_800EB0F8[i]`; m2c had read
 * it as a load); case 3's `v = 0` before the D_8009B3F9 store reaching a
 * SHARED `return v` through a goto, so the zero stays a variable; and the
 * flags read through a block-local base `q = D_800EB0F8` so the 0x34 is a
 * load displacement rather than folded into %lo. */

s32 SaveData_UpdateLoadPair(void)
{
    DuelEffectChannel *o;
    s32 v;
    s32 m;
    DuelEffectChannel *q;
    s32 a;
    s32 b;

    m = D_8009B3EA & 0xF;
    switch (m) {
    case 0:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0x80;
            MemCardDialog_CreateObject();
            a = D_8009B3EE;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            o = TextBox_Create(a, 0xC2, 0x20, 0x50, 0x100, 0x30);
            o->field_59 = 0x10;
            func_80039A14(o);
        }
        if (MemCardDialog_StepSlide(
                (DisplayObject *)D_8009B3D8, 0x20, 0x50, D_8009B3EE) == 0) {
            D_8009B3EA = 1;
        }
        return 0;
    case 1:
        if (gInput_wPad1Pressed & PAD_BUTTON_CANCEL) {
            SD_SEPlayFull(8);
            D_8009B3EA = 0x82;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        } else if (gInput_wPad1Pressed & PAD_BUTTON_CROSS) {
            SD_SEPlayFull(7);
            D_8009B3EA = 2;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        return 0;
    case 2:
        if (MemCardDialog_StepSlide(
                (DisplayObject *)D_8009B3D8, 0x20, 0x100, D_8009B3EE) == 0) {
            TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
            func_8004036C(D_8009B3D8);
            D_8009B3D8 = (u8 *)0;
            if ((D_8009B3EA & 0x80) == 0) {
                D_8009B3EA = 3;
                MemCardDialog_Request(
                    D_801D1200,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                return 0;
            }
            return 2;
        }
        return 0;
    case 3:
        v = MemCardDialog_Poll();
        if (v != 0) {
            if (v == 1) {
                if (D_8009B3EA & 0x40) {
                    if (SaveData_HasSameDuelistCode(
                            (SaveDataState *)D_801D1200,
                            (SaveDataState *)(D_801D1200 + 0x1000)) == 0) {
                        return 1;
                    }
                    D_8009B3EA = 0xA;
                    return 0;
                }
                D_8009B3EA |= 0x40;
                MemCardDialog_Request(
                    D_801D2200,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                v = 0;
                D_8009B3F9 = 0x10;
                goto done;
            }
            return v;
        }
    done:
        return v;
    case 0xA:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0xC0;
            MemCardDialog_CreateObject();
            a = D_8009B3EE;
            b = D_8009B3C0;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            o = TextBox_CreateFlagged(a, b, 0x20, 0x50, 0x100, 0x30, 0x1008);
            o->field_59 = 0x10;
            do {
                func_80039794();
            } while ((o->flags_34 & TEXT_BOX_FLAG_DONE) == 0);
        }
        if (D_8009B3EA & 0x40) {
            if (MemCardDialog_StepSlide(
                    (DisplayObject *)D_8009B3D8, 0x20, 0x50, D_8009B3EE) == 0) {
                D_8009B3EA &= 0xBF;
            }
            return 0;
        }
        func_80039794();
        q = D_800EB0F8;
        if ((q[D_8009B3EE].flags_34 & 8) == 0) {
            D_8009B3EA = 0xB;
        }
        return 0;
    case 0xB:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0x80;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        if (MemCardDialog_StepSlide(
                (DisplayObject *)D_8009B3D8, 0x20, 0x100, D_8009B3EE) == 0) {
            TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
            func_8004036C(D_8009B3D8);
            D_8009B3D8 = (u8 *)0;
            return 2;
        }
        return 0;
    }
    return 0;
}

s32 SaveData_UpdateTradeLoad(void)
{
    if ((D_8009B3ED & 0x80) == 0) {
        D_8009B3ED |= 0x80;
        D_8009B3C0 = 0x29;
    }
    return SaveData_UpdateLoadPair();
}

s32 SaveData_UpdateDuelLoad(void)
{
    s32 result;
    s32 i;
    u8 *left;
    u8 *right;
    TextStagingValues *state;

    if ((D_8009B3ED & 128) == 0) {
        D_8009B3ED |= 128;
        D_8009B3C0 = 40;
    }
    result = SaveData_UpdateTradeLoad();
    if (result == 1) {
        left = D_801D1200;
        right = D_801D1200 + TWO_PLAYER_SAVE_SLOT_STRIDE;
        i = 0;
        state = D_801D5608;
        D_8009B3EA = 10;
        D_8009B3C0 = 36;
        for (; i < DECK_SIZE; i++, left += 2, right += 2) {
            if (*(u16 *)left == 0) {
                state->deck_validation.invalid_side = 1;
                return 0;
            }
            if (*(u16 *)right == 0) {
                state->deck_validation.invalid_side = 2;
                return 0;
            }
        }
        Text_SjisToGlyphCodes(
            D_801B122B,
            D_801D160C,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
        Text_SjisToGlyphCodes(
            D_801B1238,
            D_801D160C + TWO_PLAYER_SAVE_SLOT_STRIDE,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
    }
    return result;
}

extern u8 D_801D1880[];
extern void *D_8009B3E0;

void SaveData_RequestTradeWrite(void)
{
    u8 *p = D_801D1880;
    u8 *q = p + TWO_PLAYER_SAVE_SLOT_STRIDE;

    SaveData_WritePrimarySecondaryIntegrity(p);
    SaveData_WritePrimarySecondaryIntegrity(q);
    D_8009B3E0 = q;
    MemCardDialog_Request(
        p,
        TWO_PLAYER_SAVE_TRANSFER_SIZE,
        gMemCard_szSaveFileName,
        4
    );
}
