#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../../types.h"
#include "../../ygo_types.h"
#include "../../unmatched.h"
#include "../../game/card_constants.h"
#include "../../game/duel_effect_mode_7.h"
#include "../../game/campaign_flags.h"
#include "../../game/display_object.h"
#include "../../game/input.h"
#include "../../game/color_constants.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/text_box_runtime.h"
#include "../../game/text_staging.h"
#include "../../game/duel_effect.h"
#include "../../game/duel_side_state.h"
#include "../../game/graphics_frame.h"
#include "../../game/text_constants.h"
#include "../../psyq/rand.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "../../game/display_object_helpers.h"
#define FUNC_8004036C_AMBIENT_OBJECT
#include "../../game/display_object_api.h"
#include "../../game/func_80039794.h"
#define FUNC_80041D60_AMBIENT_ARGS
#include "../../game/func_80041D60.h"
#include "../../game/func_80024DC8.h"
#include "../../game/display_object_config.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "free_duel.h"

/* The Free Duel opponent-select screen in executable order: cursor layout,
   display-object and portrait initialization, sparkle upkeep, input and
   cursor movement, and the overlay entry tick.

   The nine functions are the module's complete text section. They share one
   gcc_2_8_1_g0_split profile and the cursor widgets, target/committed grid
   positions, screen flags and sparkle pool; there is no data or rodata
   boundary between them.

   One unit settles the sparkle path's types from allocation through release:
   FreeDuel_SpawnSparkle returns DisplayObject *, the pool stores those
   pointers, and FreeDuel_GetSparkleSlot returns DisplayObject **. The cursor
   and thumb use the same record; signed coordinate reads are explicit.

   func_8004036C uses display_object_api.h's guarded `void (void)` arm. The
   sparkle updater's call passes no argument, so taking the normal
   `void func_8004036C(void *)` declaration would make the compiler set up an
   argument retail does not. */

extern u8 D_8009B269;
extern u8 D_8009B26C;

void FreeDuel_UpdateScrollbar(void)
{
    DisplayObject *cursor = gFreeDuel_pCursorWidget;
    s32 relative = (s16)cursor->field_30.h.field_32 - gGraphics_sViewportY;

    if (relative < 0x28) {
        gGraphics_sViewportY = (s16)cursor->field_30.h.field_32 - 0x28;
    }
    if (relative >= 0x91) {
        gGraphics_sViewportY = (s16)cursor->field_30.h.field_32 - 0x90;
    }
    gFreeDuel_pThumbWidget->field_30.h.field_32 =
        ((s16)cursor->field_30.h.field_32 - 0x28) * 72 / 364 + 7;
}

void FreeDuel_PlaceCursor(DisplayObject *w, s32 arm)
{
    s32 col;
    s32 index;
    s32 param;
    s16 trunc;
    u8 *panel;
    SaveDataWorkspace *base;
    u16 *slot;

    col = gFreeDuel_bCursorColumn;
    panel = (u8 *)D_800EB0F8;
    w->field_30.h.field_30 = col * 56 + 20;
    w->field_30.h.field_32 = gFreeDuel_bCursorRow * 52 + 40;
    TextBox_Destroy(panel);
    if (arm == 0) {
        return;
    }
    index = gFreeDuel_bTargetColumn +
            gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT;
    if (gFreeDuel_abGridAvailable[index] == 0) {
        return;
    }
    slot = &D_8009B32E;
    trunc = index - 31960;
    *slot = trunc;
    param = trunc;
    if (index != 0) {
        param = 12;
        base = (SaveDataWorkspace *)D_801D0000;
        D_801D5608[0].pair.lo =
            (s16)base->state.duelist_records[index].result.wins;
        D_801D5608[0].pair.hi =
            (s16)base->state.duelist_records[index].result.losses;
    }
    TextBox_Create(0, param, 16, 204, 288, 16);
    func_80039A60((struct DuelEffectChannel *)panel);
}

DisplayObject *FreeDuel_SpawnSparkle(void)
{
    DisplayObject *x;

    x = func_800400AC(func_8004002C(), 2);
    func_800428A8(x, 0, 0, 0, 0, 3, 0x11, 3, D_801AF000);
    ((u8 *)&x->field_5E)[1] = 0x80;
    x->field_48.word = 0x180018;
    func_800428EC((u8 *)x, 5);
    x->flags |= 0x20;
    return x;
}

void FreeDuel_Init(u8 *src)
{
    s32 i;
    s32 one;
    s32 k;
    s32 row;
    s32 col;
    s32 count;
    u16 *rec;
    DisplayObject **slot;
    u8 *cell;
    DisplayObject *obj;
    RECT *clut;

    if (gFreeDuel_bReturnFlags & 0x80) {
        rec = gFreeDuel_aDuelistRecords[
            gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
            gFreeDuel_bCursorColumn].counts;
        if (D_8009B362 == 1) {
            rec++;
        }
        *rec = *rec + 1;
        if ((s16)*rec >= FREE_DUEL_RECORD_MAX + 1) {
            *rec = FREE_DUEL_RECORD_MAX;
        }
    }
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    gFreeDuel_bScreenFlags = 0;
    if (gFreeDuel_bReturnFlags == 0) {
        gFreeDuel_bTargetRow = 0;
        gFreeDuel_bTargetColumn = 0;
        gFreeDuel_bCursorRow = 0;
        gFreeDuel_bCursorColumn = 0;
        TextBox_CreateFlagged(1, 13, 48, 108, 224, 16, 4136);
        func_80039794();
        gFreeDuel_bScreenFlags |= 0x20;
    }
    i = FREE_DUEL_SPARKLE_POOL_CAPACITY - 1;
    slot = gFreeDuel_apSparklePool + i;
    do {
        *slot = 0;
        i--;
        slot--;
    } while (i >= 0);
    one = 1;
    i = FREE_DUEL_GRID_ENTRY_COUNT - 1;
    cell = gFreeDuel_abGridAvailable + i;
    do {
        *cell = one;
        i--;
        cell--;
    } while (i >= 0);
    for (i = FREE_DUEL_STORY_OPPONENT_FIRST_INDEX;
         i < FREE_DUEL_STORY_OPPONENT_INDEX_END; i++) {
        if (Campaign_TestStoryFlag(FREE_DUEL_UNLOCK_FLAG_BASE + i) == 0) {
            gFreeDuel_abGridAvailable[i] = 0;
        }
    }
    do {
    } while (IsIdleGPU(10) != 0);
    count = 0;
    clut = &D_800E9D70[1];
    clut->x = 128;
    clut->y = 496;
    clut->w = 64;
    clut->h = 1;
    for (row = 0; row < 5; row++) {
        D_800E9D70[0].x = 128;
        D_800E9D70[0].y = row * 48 + 256;
        D_800E9D70[0].w = 24;
        D_800E9D70[0].h = 48;
        for (col = 0; col < FREE_DUEL_GRID_COLUMN_COUNT; col++) {
            LoadImage2(&D_800E9D70[0], (u32 *)src);
            LoadImage2(&D_800E9D70[1],
                       (u32 *)(src + FREE_DUEL_PORTRAIT_IMAGE_SIZE));
            D_800E9D70[0].x += 24;
            D_800E9D70[1].y++;
            if ((s16)D_800E9D70[1].y >= 512) {
                D_800E9D70[1].y = 496;
                D_800E9D70[1].x += 64;
            }
            count++;
            src += FREE_DUEL_PORTRAIT_RECORD_SIZE;
        }
    }
    for (row = 0; row < 5; row++) {
        D_800E9D70[0].x = 256;
        D_800E9D70[0].y = row * 48 + 256;
        D_800E9D70[0].w = 24;
        D_800E9D70[0].h = 48;
        for (col = 0; col < FREE_DUEL_GRID_COLUMN_COUNT; col++) {
            LoadImage2(&D_800E9D70[0], (u32 *)src);
            LoadImage2(&D_800E9D70[1],
                       (u32 *)(src + FREE_DUEL_PORTRAIT_IMAGE_SIZE));
            count++;
            if (count >= FREE_DUEL_GRID_ENTRY_COUNT) {
                goto done;
            }
            D_800E9D70[0].x += 24;
            D_800E9D70[1].y++;
            src += FREE_DUEL_PORTRAIT_RECORD_SIZE;
            if ((s16)D_800E9D70[1].y >= 512) {
                D_800E9D70[1].y = 496;
                D_800E9D70[1].x += 64;
            }
        }
    }
done:
    for (i = 0; i < 25; i++) {
        if (gFreeDuel_abGridAvailable[i] != 0) {
            obj = func_800400AC(func_8004002C(), 1);
            func_80040510((DisplayObjectConfigView *)obj,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 56 + 20,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 52 + 40, 48, 48,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 48,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 48, 18,
                          (i / 16) * 64 + 128, (i & 15) + 496);
            obj->attribute |= 0x1000000;
            obj->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        }
    }
    for (k = 25, i = 0; i < 15; i++, k++) {
        if (gFreeDuel_abGridAvailable[k] != 0) {
            obj = func_800400AC(func_8004002C(), 1);
            func_80040510((DisplayObjectConfigView *)obj,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 56 + 20,
                          (k / FREE_DUEL_GRID_COLUMN_COUNT) * 52 + 40, 48, 48,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 48,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 48, 20,
                          (k / 16) * 64 + 128, (k & 15) + 496);
            obj->attribute |= 0x1000000;
            obj->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        }
    }
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 0, 16, 0, D_801AF000);
    func_800428EC((u8 *)obj, 10);
    obj->attribute |= 0x1000000;
    obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 1, 16, 0, D_801AF000);
    func_800428EC((u8 *)obj, -10);
    obj->attribute |= 0x1000000;
    obj->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                  DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 2, 17, 3, D_801AF000);
    ((u8 *)&obj->field_5E)[1] = 128;
    func_800428EC((u8 *)obj, 15);
    obj->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                  DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    gFreeDuel_pThumbWidget = obj;
    obj = FreeDuel_SpawnSparkle();
    gFreeDuel_pCursorWidget = obj;
    obj->attribute &= ~GsROTOFF;
    if (gFreeDuel_bReturnFlags == 0) {
        obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        FreeDuel_PlaceCursor(obj, 0);
    } else {
        FreeDuel_PlaceCursor(obj, 1);
    }
    FreeDuel_UpdateScrollbar();
    SD_BGMPlay(29376);
}

DisplayObject **FreeDuel_GetSparkleSlot(void)
{
    s32 i;

    for (i = FREE_DUEL_SPARKLE_POOL_CAPACITY - 1; i >= 0; i--) {
        if (gFreeDuel_apSparklePool[i] == 0) {
            return &gFreeDuel_apSparklePool[i];
        }
    }
    return 0;
}

void FreeDuel_UpdateSparkle(void)
{
    DisplayObject *obj;
    s32 level;
    s16 timer;
    s32 i;

    for (i = FREE_DUEL_SPARKLE_POOL_CAPACITY - 1; i >= 0; i--) {
        obj = gFreeDuel_apSparklePool[i];
        if (obj != 0 && (obj->field_6C & 0xF) == 1) {
            if (!(obj->field_6C & 0x80)) {
                obj->field_6C |= 0x80;
                obj->field_60 = 16;
                obj->field_0C = COLOR_RGB24_DIM_GREY;
                obj->attribute |= (GsALON | GsAONE);
            }
            level = ((u8 *)&obj->field_0C)[0] - 4;
            ((u8 *)&obj->field_0C)[2] = level;
            ((u8 *)&obj->field_0C)[1] = level;
            ((u8 *)&obj->field_0C)[0] = level;
            timer = obj->field_60 - 1;
            obj->field_60 = timer;
            if (timer == 0) {
                func_8004036C();
                gFreeDuel_apSparklePool[i] = 0;
            }
        }
    }
}

void FreeDuel_UpdateCursorTween(void)
{
    DisplayObject *widget = gFreeDuel_pCursorWidget;
    DisplayObject **slot;
    DisplayObject *sparkle;
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
        widget->field_60 = 8;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)widget);

        d = gFreeDuel_bTargetColumn;
        tx = d * 56 + 20;
        d = (s16)widget->field_30.h.field_30;
        d = tx - d;
        sx = (d << 8) / 8;
        d = gFreeDuel_bTargetRow;
        ty = d * 52 + 40;
        widget->field_34.h.field_36 = sx;
        d = (s16)widget->field_30.h.field_32;
        d = ty - d;
        widget->field_38.h.field_38 = (d << 8) / 8;
    }

    DisplayObject_StepPositionXY((DisplayObjectVelocity *)widget);
    left = (u16)widget->field_60 - 1;
    widget->field_60 = left;
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
            sparkle->field_30.word = widget->field_30.word;
            func_800428EC((u8 *)sparkle, (s8)((u8)widget->field_16 - 1));
            func_80041D60(sparkle);
            sparkle->field_4C = widget->field_4C;
            sparkle->field_6C = 1;
            sparkle->flags |= 1;
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
            gFreeDuel_pCursorWidget->flags |=
                DISPLAY_OBJECT_FLAG_RENDERABLE;
            FreeDuel_PlaceCursor(
                gFreeDuel_pCursorWidget, 1
            );
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
                TextBox_CreateFlagged(1, 8, 0x30, 0x6C, 0xE0, 0x10, 0x1028);
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
        gFreeDuel_pCursorWidget->field_44.h.field_46 =
            phase * 48 + 0x1000;
        gFreeDuel_pCursorWidget->field_44.h.field_44 =
            phase * 48 + 0x1000;
    }
    FreeDuel_UpdateSparkle();
}
