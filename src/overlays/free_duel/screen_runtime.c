#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../../types.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_layout.h"
#include "../../game/input.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/duel_effect.h"
#include "../../game/duel_side_state.h"
#include "../../game/graphics_frame.h"
#include "../../psyq/rand.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "../../game/display_object_helpers.h"
#define FUNC_8004036C_AMBIENT_OBJECT
#include "../../game/display_object_api.h"
#include "../../game/func_80039794.h"
#define FUNC_80041D60_AMBIENT_ARGS
#include "../../game/func_80041D60.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "free_duel.h"

/* The free-duel screen's per-frame runtime: the entry tick, the screen
   update it calls, the cursor tween that update drives, and the sparkle pool
   upkeep the tick finishes on.

   The README used to say the sparkle pair "stays in sparkle_runtime.c; this
   unit does not absorb it", without a reason. There is not one: both sparkle
   functions have exactly one caller each, FreeDuel_UpdateScreen for the
   allocator and FreeDuel_Entry for the updater, and both are here.

   One unit settles FreeDuel_GetSparkleSlot's return type. It was declared
   u8 ** by the caller and defined void **; the definition wins and the one
   call site assigns through a void ** local.

   func_8004036C uses display_object_api.h's guarded `void (void)` arm. The
   sparkle updater's call passes no argument, so taking the normal
   `void func_8004036C(void *)` declaration would make the compiler set up an
   argument retail does not. */

typedef struct {
    u8 unk0[0x4];
    u32 flags;
    u8 unk8[0x4];
    u8 r;
    u8 g;
    u8 b;
    u8 unkF;
    u8 unk10[0x50];
    s16 timer;
    u8 unk62[0xA];
    u8 state;
} FreeDuelSparkle;

extern u8 *gFreeDuel_pCursorWidget;
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern u8 gFreeDuel_bReturnFlags;
extern u8 *FreeDuel_SpawnSparkle(void);
extern void func_80024DC8(s32, s32, s32, s32);
extern void func_80033C90(void);
extern void func_80035C38(s32, s32, s32, s32, s32, s32, s32);
extern void FreeDuel_PlaceCursor(void *, s32);
extern void FreeDuel_UpdateScrollbar(void);

void **FreeDuel_GetSparkleSlot(void)
{
    s32 i;

    for (i = 15; i >= 0; i--) {
        if (gFreeDuel_apSparklePool[i] == 0) {
            return &gFreeDuel_apSparklePool[i];
        }
    }
    return 0;
}

void FreeDuel_UpdateSparkle(void)
{
    FreeDuelSparkle *obj;
    s32 level;
    s16 timer;
    s32 i;

    for (i = 15; i >= 0; i--) {
        obj = (FreeDuelSparkle *)gFreeDuel_apSparklePool[i];
        if (obj != 0 && (obj->state & 0xF) == 1) {
            if (!(obj->state & 0x80)) {
                obj->state |= 0x80;
                obj->timer = 16;
                *(u32 *)&obj->r = 0x404040;
                obj->flags |= (GsALON | GsAONE);
            }
            level = obj->r - 4;
            obj->b = level;
            obj->g = level;
            obj->r = level;
            timer = obj->timer - 1;
            obj->timer = timer;
            if (timer == 0) {
                func_8004036C();
                gFreeDuel_apSparklePool[i] = 0;
            }
        }
    }
}

void FreeDuel_UpdateCursorTween(void)
{
    u8 *widget = gFreeDuel_pCursorWidget;
    void **slot;
    u8 *sparkle;
    s32 tx;
    s32 ty;
    s32 sx;
    s32 d;
    s16 left;

    if ((gFreeDuel_bScreenFlags & 0x40) == 0) {
        if (gFreeDuel_bCursorColumn == gFreeDuel_bTargetColumn && gFreeDuel_bCursorRow == gFreeDuel_bTargetRow) {
            return;
        }
        gFreeDuel_bScreenFlags |= 0x40;
        *(u16 *)(widget + 0x60) = 8;
        DisplayObject_ResetVelocity(widget);

        d = gFreeDuel_bTargetColumn;
        tx = d * 56 + 20;
        d = *(s16 *)(widget + 0x30);
        d = tx - d;
        sx = (d << 8) / 8;
        d = gFreeDuel_bTargetRow;
        ty = d * 52 + 40;
        *(s16 *)(widget + 0x36) = sx;
        d = *(s16 *)(widget + 0x32);
        d = ty - d;
        *(s16 *)(widget + 0x38) = (d << 8) / 8;
    }

    DisplayObject_StepPositionXY(widget);
    left = *(u16 *)(widget + 0x60) - 1;
    *(u16 *)(widget + 0x60) = left;
    if (left == 0) {
        gFreeDuel_bCursorColumn = gFreeDuel_bTargetColumn;
        gFreeDuel_bCursorRow = gFreeDuel_bTargetRow;
        FreeDuel_PlaceCursor(widget, 1);
        gFreeDuel_bScreenFlags &= ~0x40;
        SD_SEPlayFull(47);
    } else {
        slot = FreeDuel_GetSparkleSlot();
        sparkle = FreeDuel_SpawnSparkle();
        if (sparkle != 0 && slot != 0) {
            *(u32 *)(sparkle + 0x30) = *(u32 *)(widget + 0x30);
            func_800428EC(sparkle, (s8)(widget[0x16] - 1));
            func_80041D60((DisplayObject *)sparkle);
            *(u32 *)(sparkle + 0x4C) = *(u32 *)(widget + 0x4C);
            sparkle[0x6C] = 1;
            *(u16 *)(sparkle + 8) |= 1;
            *slot = sparkle;
        }
    }
}

void FreeDuel_UpdateScreen(void)
{
    u8 *panel;
    u16 *entry;
    s32 index;

    if ((gFreeDuel_bScreenFlags & 0x20) != 0) {
        func_80039794();
        panel = (u8 *)&D_800EB15C;
        if ((*(u16 *)(panel + 0x34) & 8) == 0) {
            gFreeDuel_bScreenFlags &= 0xDF;
            TextBox_Destroy(panel);
            *(u16 *)(gFreeDuel_pCursorWidget + 8) |=
                DISPLAY_OBJECT_FLAG_RENDERABLE;
            FreeDuel_PlaceCursor(gFreeDuel_pCursorWidget, 1);
        }
        return;
    }

    FreeDuel_UpdateCursorTween();
    FreeDuel_UpdateScrollbar();
    if ((gFreeDuel_bScreenFlags & 0x40) != 0) {
        return;
    }

    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            if (++gFreeDuel_bTargetColumn >= FREE_DUEL_GRID_COLUMN_COUNT) {
                gFreeDuel_bTargetColumn = FREE_DUEL_GRID_COLUMN_COUNT - 1;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            if (--gFreeDuel_bTargetColumn < 0) {
                gFreeDuel_bTargetColumn = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
            if (++gFreeDuel_bTargetRow >= FREE_DUEL_GRID_ROW_COUNT) {
                gFreeDuel_bTargetRow = FREE_DUEL_GRID_ROW_COUNT - 1;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
            if (--gFreeDuel_bTargetRow <= 0) {
                gFreeDuel_bTargetRow = 0;
            }
        }
    } else {
        if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            SD_SEPlayFull(8);
            D_8009B26C = 8;
            return;
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) == 0) {
            return;
        }
        if (gFreeDuel_abGridAvailable[
                gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
                gFreeDuel_bCursorColumn] == 0) {
            return;
        }
        if ((gFreeDuel_bCursorColumn | gFreeDuel_bCursorRow) == 0) {
            func_80033C90();
            D_8009B269 = 6;
            gFreeDuel_bReturnFlags = 0x40;
            SD_SEPlayFull(0x30);
            return;
        }
        entry = gDuel_awPlayerDeck;
        for (index = 0; index < DECK_SIZE; index++) {
            if (*entry == 0) {
                SD_SEPlayFull(9);
                func_80035C38(1, 8, 0x30, 0x6C, 0xE0, 0x10, 0x1028);
                gFreeDuel_bScreenFlags |= 0x20;
                return;
            }
            entry++;
        }
        SD_SEPlayFull(0x30);
        gFreeDuel_bReturnFlags = 0x80;
        func_80024DC8(
            -1,
            gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
                gFreeDuel_bCursorColumn,
            0x6000, 0x6000);
        D_8009B368 = 6;
        D_8009B26C = 3;
    }
}

void FreeDuel_Entry(void)
{
    s32 phase;

    rand();
    FreeDuel_UpdateScreen();
    phase = D_8009B0CC & 0x7F;
    if (phase < 0x10) {
        if (phase >= 8) {
            phase = 0xF - phase;
        }
        *(s16 *)(gFreeDuel_pCursorWidget + 0x46) = phase * 48 + 0x1000;
        *(s16 *)(gFreeDuel_pCursorWidget + 0x44) = phase * 48 + 0x1000;
    }
    FreeDuel_UpdateSparkle();
}
