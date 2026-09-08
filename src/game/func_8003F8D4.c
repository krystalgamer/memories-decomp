#include "../types.h"

#include "duel_effect.h"
#include "input.h"
#include "mem_card.h"
#include "save_data.h"

/* Memory-card save-slot dialog state machine, driven from func_8003FCD8
   and stepping the low nibble of D_8009B3EA.

   Levers that mattered here:
   - func_8003F2B0 is declared returning s32; its definition returns u8, and
     taking that prototype adds an andi after every call.
   - The two TextBox arguments are read into locals before the -0x400 store,
     because gcc will not schedule a load across a store to an unrelated
     pointer and retail issues those byte loads first.
   - Case 1 duplicates the 0x400 store into both arms and shares only the
     return, which is what leaves retail's D_8009B3D8 load in each arm with
     the sb/sh pair cross-jumped.
   - The channel record is reached through DuelEffectChannel so the 0x34 flag
     stays a load displacement instead of folding into the %lo.
   - The two exits in case 10's 0x40 arm are breaks rather than returns; as
     returns gcc keeps one zero live across the flag update and the delay slot
     goes empty. */

extern u8 D_8009B3C0;
extern u8 *D_8009B3D8;
extern u8 D_8009B3EA;
extern u8 D_8009B3EE;
extern u8 D_8009B3F9;
extern u8 D_801D1200[];
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));

extern void SD_SEPlayFull(s32);
extern s32 SaveData_HasSameDuelistCode(void *, void *);
extern DuelEffectChannel *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern DuelEffectChannel *TextBox_CreateFlagged(s32, s32, s32, s32, s32, s32,
                                               s32);
extern void TextBox_Destroy(DuelEffectChannel *);
extern void func_80039794(void);
extern void func_80039A14(DuelEffectChannel *);
extern void func_8004036C(s32);
extern void func_8003F388(void);
extern s32 func_8003F70C(void);
extern void func_8003F758(void *, s32, void *, s32);
extern s32 func_8003F2B0(u8 *, s32, s32, s32);

s32 func_8003F8D4(void)
{
    DuelEffectChannel *box;
    s32 result;
    s32 index;
    s32 kind;

    switch (D_8009B3EA & 0xF) {
    case 0:
        if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EA |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003F388();
            index = D_8009B3EE;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            box = TextBox_Create(index, 0xC2, 0x20, 0x50, 0x100, 0x30);
            box->field_59 = 0x10;
            func_80039A14(box);
        }
        if (func_8003F2B0(D_8009B3D8, 0x20, 0x50, D_8009B3EE) != 0) {
            return 0;
        }
        D_8009B3EA = 1;
        return 0;
    case 1:
        if ((gInput_wPad1Pressed & PAD_BUTTON_CIRCLE) != 0) {
            SD_SEPlayFull(8);
            D_8009B3EA = 0x82;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        } else {
            if ((gInput_wPad1Pressed & PAD_BUTTON_CROSS) == 0) {
                return 0;
            }
            SD_SEPlayFull(7);
            D_8009B3EA = 2;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        return 0;
    case 2:
        if (func_8003F2B0(D_8009B3D8, 0x20, 0x100, D_8009B3EE) != 0) {
            return 0;
        }
        TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
        func_8004036C((s32)D_8009B3D8);
        D_8009B3D8 = (u8 *)0;
        if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_READY) != 0) {
            return 2;
        }
        D_8009B3EA = 3;
        func_8003F758(D_801D1200, SAVE_DATA_STATE_SIZE,
                      gMemCard_szSaveFileName, 1);
        return 0;
    case 3:
        result = func_8003F70C();
        if (result != 0) {
            if (result == 1) {
            if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) != 0) {
                if (SaveData_HasSameDuelistCode(
                        D_801D1200,
                        D_801D1200 + TWO_PLAYER_SAVE_SLOT_STRIDE) == 0) {
                    return 1;
                }
                D_8009B3EA = 0xA;
                return 0;
            }
            D_8009B3EA |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003F758(D_801D1200 + TWO_PLAYER_SAVE_SLOT_STRIDE,
                          SAVE_DATA_STATE_SIZE, gMemCard_szSaveFileName, 1);
            result = 0;
            D_8009B3F9 = 0x10;
            }
        }
        return result;
    case 10:
        if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EA |= MEM_CARD_DIALOG_FLAG_RESULT_READY
                          | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003F388();
            index = D_8009B3EE;
            kind = D_8009B3C0;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            box = TextBox_CreateFlagged(index, kind, 0x20, 0x50,
                                        0x100, 0x30, 0x1008);
            box->field_59 = 0x10;
            do {
                func_80039794();
            } while ((box->flags_34 & 0x2000) == 0);
        }
        if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) != 0) {
            if (func_8003F2B0(D_8009B3D8, 0x20, 0x50, D_8009B3EE) != 0) {
                break;
            }
            D_8009B3EA &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            break;
        }
        func_80039794();
        if ((D_800EB0F8[D_8009B3EE].flags_34 & 8) != 0) {
            return 0;
        }
        D_8009B3EA = 0xB;
        return 0;
    case 11:
        if ((D_8009B3EA & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EA |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        if (func_8003F2B0(D_8009B3D8, 0x20, 0x100, D_8009B3EE) != 0) {
            return 0;
        }
        TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
        func_8004036C((s32)D_8009B3D8);
        D_8009B3D8 = (u8 *)0;
        return 2;
    }
    return 0;
}
