#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#define D_8009B09C_IN_DATA
#include "../types.h"
#include "func_80032B38.h"
#include "duel_effect.h"
#include "fade.h"
#include "card_constants.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "func_80039794.h"
#include "display_object_helpers.h"
#include "build_deck_transition_state.h"
#include "graphics_frame.h"
#include "func_800339D0.h"
#include "duel_transition_step_table.h"
#include "../psyq/rand.h"
#include "duel_reward_setup.h"
#include "func_80033998.h"

extern u8 D_8009B2F8;
/* Retail addresses these three with %hi/%lo under -G8, so they live outside
   small data. */
extern s8 gDialog_bChoice __attribute__((section(".data")));

/* Handles leaving the deck editor. When the editor's own check passes, the
 * confirm sound plays and, if the deck is complete (func_80033998), bit 14 of
 * the state word at +0x633E is set, a confirmation box is created (the wide
 * one in the 640-wide mode selected by bit 7 of D_8009B2F8, the narrow one
 * otherwise, which is then waited on until its +0x30 pointer is filled),
 * Fade_SetTargetLevel(0xA0, 2) runs and D_8009B140 is set from D_8009AF74[1] - 8.
 * With bit 14 set the effect channel at D_800EB0F8 is polled: once its flags
 * read 0x2000 under the 0x2008 mask the box is destroyed and either bit 14
 * is cleared (narrow mode with a choice made) or the state word is reloaded
 * from +0x6340 and Fade_SetTargetLevel(0xFF, 2) runs. Without bit 14 the CARD_COUNT
 * trunk bytes at +0x5D98 are copied after the DECK_SIZE halfwords of the
 * object at +0. Those halfwords are rebuilt from the 0x10-byte entries at
 * +0x2D54 whose byte 9 is set, func_80032370 runs and the state word is
 * cleared. */
void func_800339D0(u8 *state)
{
    BuildDeckTransitionState *workspace = (BuildDeckTransitionState *)state;
    u8 *box;
    u8 *src;
    u8 *dst;
    u16 *slot;
    CardEntry *entry;
    s32 mode;
    s32 i;

    if (func_80032B38((BuildDeckTransitionState *)state) == 0) {
        SD_SEPlayFull(8);
        if (func_80033998() != 0) {
            /* The mode byte is read before the flag store, as retail
               schedules it. */
            mode = D_8009B2F8 & 0x80;
            ((BuildDeckTransitionState *)state)->state |= 0x4000;
            if (mode) {
                ((u8 *)TextBox_CreateFlagged(
                    0, 8, 0x28, 0x78, 0xF0, 0x10, 0x1028
                ))[0x59] = 0xA;
            } else {
                box = TextBox_CreateFlagged(0, 9, 0x30, 0x60, 0xE0, 0x30, 0x20);
                box[0x59] = 0xA;
                do {
                    func_80039794();
                } while (*(s32 *)(box + 0x30) == 0);
            }
            Fade_SetTargetLevel(0xA0, 2);
            D_8009B140 = *(u8 *)&D_8009AF74[1] - 8;
        }
    }
    /* Computed before the branch: it is only used on the copy path, so it
       crosses no call, and reorg lifts it into the branch delay slot. */
    src = &workspace->chest_card_quantities[CARD_ID_FIRST];
    if (workspace->state & 0x4000) {
        func_80039794();
        /* The same variable as the confirmation box, which keeps the
           channel in $s0 across the destroy call. */
        box = (u8 *)D_800EB0F8;
        if ((*(u32 *)(box + 0x34) & 0x2008) == 0x2000) {
            TextBox_Destroy(box);
            if (!(D_8009B2F8 & 0x80) && gDialog_bChoice != 0) {
                workspace->state &= 0xBFFF;
            } else {
                workspace->state = workspace->next_state;
                Fade_SetTargetLevel(0xFF, 2);
            }
        }
    } else {
        /* One counter for both loops, initialised before the pointers each
           time; that is what ranks the counter and the pointers into
           retail's argument registers. */
        i = 0;
        dst = (u8 *)(workspace->deck_cards + DECK_SIZE);
        for (; i < CARD_COUNT; i++) {
            *dst++ = *src++;
        }
        slot = workspace->deck_cards;
        i = 0;
        entry = workspace->lists[1].entries;
        for (; i < DECK_SIZE; i++) {
            *slot = 0;
            if (entry->flags != 0) {
                *slot = entry->id;
            }
            slot++;
            entry++;
        }
        func_80032370();
        workspace->state = 0;
    }
}

/* Per-frame Build Deck transition driver. It pulses both pane colours, then
 * dispatches the current step through D_80090DF8; func_800339D0 above is the
 * table's exit step. */
s32 func_80033BE8(void)
{
    s32 intensity;
    s32 color;
    BuildDeckTransitionState *base;
    u8 *first;
    u8 *second;

    rand();

    intensity = D_8009B09C & 0x3F;
    if (intensity >= 0x20) {
        intensity = 0x3F - intensity;
    }

    base = D_8009B2FC;
    color = intensity * 2 + 0x40;
    first = *(u8 **)((u8 *)base + 0x2D38);
    second = *(u8 **)((u8 *)base + 0x5A84);

    second[0xE] = color;
    second[0xD] = color;
    second[0xC] = color;
    first[0xE] = color;
    first[0xD] = color;
    first[0xC] = color;

    if (DuelEffect_UpdateState() == 0) {
        D_80090DF8[D_8009B2FC->state & 0x3F]((u8 *)D_8009B2FC);
    }

    return D_8009B2FC->state;
}
