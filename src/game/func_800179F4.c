#define D_8009B369_IN_DATA
#include "../types.h"
#include "duel_draw_status_numbers.h"
#include "save_data.h"
#include "duel_grid.h"
#include "duel_card_pick_cursor.h"
#include "duel_selection_layout.h"
#include "duel_hand.h"
#include "main_frame.h"
#include "duel_side_state.h"
#include "duel_shuffle_deck.h"
#include "duel_check_quit_input.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"
#include "display_object_helpers.h"
#include "view_state.h"
#include "main_services.h"
#include "duel_effect_resource_record.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "duel_action_lock.h"
#include "../unmatched.h"
#include "func_800179F4.h"

extern u8 gDuel_bTerrain __attribute__((section(".data")));
extern s8 gDuel_bOpponentID __attribute__((section(".data")));

extern u16 D_8009B204;
extern u16 D_8009B16C;
extern u8 *D_8009B1D8;
extern u8 *D_8009B1DC;

extern void func_8001778C(void);
extern void func_800178BC(void);
extern void func_8002C598(void);
extern void func_80029574(s32);
extern void func_80035668(s32);
extern void func_8001755C(void);
extern void Duel_ShuffleBothDecks(u8 *, u8 *);

void func_800179F4(void)
{
    DisplayObject *obj;
    u8 *p;
    u8 *q;
    s32 value;
    s32 side;
    DuelEffectResourceRecord *pane;
    /* Two allocation pins. The map-entry pointer wants $v1 so the load of
       D_8009B21C fills its own delay slot with the callback address's addiu
       rather than the next %hi; the pool base wants $a0 so the %hi of
       D_801D1200 is completed in place instead of through $v0. Releasing them
       costs five and two differing words. */
    register u8 *pool __asm__("$4");
    register u8 *prev __asm__("$3");
    s8 *pid;

    pid = &gDuel_bOpponentID;
    func_8004763C();
    func_80047AD0(1);
    func_80012D84(4);
    File_WaitForTransfers();
    value = gDuel_bTerrain;
    File_RequestAsyncTransfer(
        0, 0,
        (((value * 15) * 4 - value) * 4 - value) +
            DUEL_TERRAIN_PACKAGE_FIRST_SECTOR,
        DUEL_TERRAIN_PACKAGE_SECTOR_COUNT, Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    D_8009B238 = -1;
    D_8009B23A = 11;
    gDuel_bQuitDialogState = 0;
    D_8009B162 = 0;
    D_8009B1D4 = 0;
    D_8009B204 = 0;
    D_8009B220 = 0;
    D_8009B16C = 0;
    D_8009B174 = 0;
    if (D_8009B369 != 1) {
        if (gDuel_bOpponentID >= 0) {
            File_RequestAsyncTransfer(
                0, 0, gDuel_bOpponentID * 2 + gDuel_bOpponentID + 7475, 3, 0, 0,
                (s32)gDuel_awOpponentDeckPool
            );
        }
        D_8009B1D5 = 0;
        D_8009B23A = 1;
        func_8001778C();
        func_80017708();
        func_800175A0();
        D_8009B16C |= 0x1000;
    }
    D_8009B1C8 = (DuelSideState *)((u8 *)D_800E9FF0 + D_8009B1D5 * sizeof(DuelSideState));
    func_800178BC();
    D_800F2848.angle = D_8009B1D5 * 2048 + 1024;
    func_8001352C();
    Duel_ClearHandSlots();
    pane = &D_800EA0E8[0];
    func_8002C598();
    func_80029574(0);
    pane->src_x = 0;
    pane->src_y = 256;
    pane->field_2C = 0;
    pane->field_2E = 255;
    func_80029574(1);
    pane[1].src_x = 64;
    pane[1].src_y = 256;
    pane[1].field_2C = 0;
    pane[1].field_2E = 254;
    func_80035668(0);
    func_8001755C();
    File_WaitForTransfers();
    D_8009B22C = &D_800907D8[D_8009B1D5 * 20];
    obj = func_800400AC(func_8004002C(), 2);
    func_800404CC(obj, 12, 24, 4, 2, gDuel_bTerrain, 11, 732);
    func_80042918(obj);
    /* Both of this function's flag merges keep u8 * arithmetic instead of
       obj->flags, and the spelling is load-bearing rather than untidy.

       Writing obj->flags lets GCC 2.8.1 schedule the sh into the delay slot
       of the following jal and drop the nops around it, which shortens the
       executable by sixteen bytes. Taking the member's address instead,
       *(u16 *)&obj->flags, fails identically, so the barrier is the pointer
       arithmetic on u8 * and not the cast. Each site was measured on its own:
       leaving only the second as member access still loses twelve bytes, and
       the first accounts for the other four. */
    *(u16 *)((u8 *)obj + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    side = (u32)gDuel_bOpponentID >> 31;
    D_8009B214 = obj;
    obj = func_800400AC(func_8004002C(), 2);
    func_800404CC(
        obj, 280, 32, 4, side, 0, 11, 748
    );
    func_80042918(obj);
    *(u16 *)((u8 *)obj + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if (D_8009B1D5 != 0) {
        *(u16 *)&obj->field_40.h.field_40 += 16;
    }
    D_8009B21C = obj;
    obj = func_800400AC(func_8004002C(), 6);
    func_80042918(obj);
    func_800428EC((u8 *)obj, 1);
    obj->field_4C = (s32)Duel_DrawLifePointsAndDeckCounts;
    prev = (u8 *)D_8009B21C;
    obj->field_50.word = (s32)prev;
    D_800E9DB0[3] = func_800164FC;
    if (D_8009B369 != 1) {
        p = 0;
        D_8009B1DC = 0;
        D_8009B1D8 = 0;
        if (pid[-1] < 0) {
            if (gDuel_bOpponentID < 0) {
                pool = D_801D1200;
                p = pool;
                q = pool + 0x1000;
                D_8009B1D8 = p;
                D_8009B1DC = q;
                goto shuffle;
            }
            p = (u8 *)gDuel_awPlayerDeck;
            D_8009B1D8 = p;
            if (gDuel_bOpponentID < 39) {
                q = 0;
                goto shuffle;
            }
        }
        q = p;
    shuffle:
        Duel_ShuffleBothDecks(p, q);
    }
}
