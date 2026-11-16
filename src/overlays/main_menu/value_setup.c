#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_system.h"
#include "../../game/display_object_layout.h"
#include "../../game/main_services.h"
#include "../../game/sound.h"
#include "ordering_tables.h"
#include "value_setup.h"
#include "../../game/display_asset_banks.h"

/* The value-setup screen: the three lifecycle entry points
   main_mode_runners.c
   calls - start, update per tick, finish - and the four helpers that only
   they and each other reach.

   value_setup.h, now folded in below, listed three symbols as deliberately
   absent because their
   declarers disagreed. One translation unit forces the choice, and these are
   the results, each verified byte-identical rather than argued:

     gTwoPlayerDuel_apSetupCursors  was u8 *[], void *[] and ValueWidgetView *[]. The typed
                 spelling won because the drawing code dereferences it
                 twenty-odd times through named fields.
     gTwoPlayerDuel_pBackground  was void * and u8 *, and u8 * won on the byte arithmetic
                 the starter did on it.
     gTwoPlayerDuel_pSetupPanel
                 same pair, same reason.

   All of them, including the tween's gTwoPlayerDuel_apSetupCursors[2], are display objects out of DisplayObject_AcquireSlot,
   and every offset any of them was reached at is a DisplayObject member:
   flags at 0x08, update at 0x24, the position pair at 0x30, the saved pair
   at 0x36/0x38, the timer at 0x60 and the two widget bytes at 0x6B/0x6C.
   ValueWidgetView named four of those, so it is retired and the byte
   arithmetic with it; all five handles and the tween's own pointer are
   DisplayObject *, which builds byte-identically.

   The choice array and scalar toggle-choice view retain their separate
   native bindings, including the overlap at the array's third byte. */

/* Value-setup screen state. PR #3069 gave these a shared header when four
   sources drove them and each carried its own copy; they remain centralized
   in value_setup.h after those sources were coalesced. Nothing it found is
   dropped.

     gTwoPlayerDuel_abSetupChoices  [0/1] are the two players' cursor choices:
                 left/right shared-toggle positions or their value row.
                 [2] is the shared toggle choice, not a third player's row.
     gTwoPlayerDuel_pSetupToggle  Points at the toggle byte the caller handed in; the finish
                 step writes through it.
     gTwoPlayerDuel_aSetupEntries  The two values being edited, each a ValueSetupEntry: the
                 value, the shown value tweened toward it, and the pointer
                 the finish step writes it back through. It was a u16 array
                 indexed [0]/[1] and [6]/[7], with the two pointers reached
                 by byte offset; the entry spelling builds byte-identically.

   gTwoPlayerDuel_abSetupChoices[2] (0x801845BE) is the shared toggle choice.
   Input writes it; finish reads it and preserves its != 1 rule.
   The resident caller finishes on both accept and cancel, committing both. */
void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle)
{
    DisplayObject *object;
#ifdef VERSION_EUROPE
    DisplayObject *quad;
#endif

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTwoPlayerDuel_pBackground = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        gTwoPlayerDuel_pBackground->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                             DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
#ifdef VERSION_EUROPE
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_pBackground, -3);
#else
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_pBackground, -2);
#endif
    }

#ifdef VERSION_EUROPE
    quad = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -2);
    quad->position.word = 0xE00000;
    quad->xy.word = 0xE00140;
    quad->field_38.word = 0x1000000;
    quad->field_40.word = 0x1000140;
    quad->field_3C.word = 0x808080;
    quad->field_44.word = 0x808080;

    quad = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -2);
    quad->xy.word = 0x140;
    quad->position.word = 0;
    quad->field_38.word = 0x100000;
    quad->field_40.word = 0x100140;
    quad->field_2C.word = 0x808080;
    quad->field_34.word = 0x808080;
#endif

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTwoPlayerDuel_pSetupPanel = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0xA, 6, 0, 0, 0xE, 5, gDisplay_abResourceBank1);
        gTwoPlayerDuel_pSetupPanel->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                             DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_pSetupPanel, -1);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTwoPlayerDuel_apSetupCursors[0] = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(gTwoPlayerDuel_apSetupCursors[0], 0, 0, 3, 4, 0, 0xB, 0x20C);
        gTwoPlayerDuel_apSetupCursors[0]->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_apSetupCursors[0], 1);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTwoPlayerDuel_apSetupCursors[1] = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(gTwoPlayerDuel_apSetupCursors[1], 0, 0, 3, 4, 0, 0xB, 0x20C);
        gTwoPlayerDuel_apSetupCursors[1]->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_apSetupCursors[1], 1);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTwoPlayerDuel_apSetupCursors[2] = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(gTwoPlayerDuel_apSetupCursors[2], 0, 0, 3, 4, 0, 0xB, 0x20C);
        gTwoPlayerDuel_apSetupCursors[2]->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(gTwoPlayerDuel_apSetupCursors[2], 1);
    }

    gTwoPlayerDuel_pSetupToggle = toggle;
    gTwoPlayerDuel_abSetupChoices[2] = (*toggle == 0);
    gTwoPlayerDuel_abSetupChoices[0] = gTwoPlayerDuel_abSetupChoices[1] = MAIN_MENU_VALUE_SETUP_CHOICE_VALUE;
    gTwoPlayerDuel_aSetupEntries[0].out = first;
    gTwoPlayerDuel_aSetupEntries[1].out = second;
    gTwoPlayerDuel_aSetupEntries[0].value = gTwoPlayerDuel_aSetupEntries[0].shown = *first;
    gTwoPlayerDuel_aSetupEntries[1].value = gTwoPlayerDuel_aSetupEntries[1].shown = *second;
    gMain_apFrameServiceCallbacks[0] = MainMenu_DrawValueSetup;
}

#ifdef VERSION_JAPAN
#define MAIN_MENU_VALUE_SETUP_CANCEL_BUTTON PAD_BUTTON_CROSS
#else
#define MAIN_MENU_VALUE_SETUP_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif
s32 MainMenu_UpdateValueSetup(void)
{
    s32 busyA;
    s32 busyB;
    s32 step;
    u16 value;

    busyA = (gTwoPlayerDuel_apSetupCursors[0]->update != 0);
    busyB = (gTwoPlayerDuel_apSetupCursors[1]->update != 0);

    if (gTwoPlayerDuel_aSetupEntries[0].value != gTwoPlayerDuel_aSetupEntries[0].shown) {
        step = gTwoPlayerDuel_aSetupEntries[0].value - gTwoPlayerDuel_aSetupEntries[0].shown;
        if (step < 0) {
            step = gTwoPlayerDuel_aSetupEntries[0].shown - gTwoPlayerDuel_aSetupEntries[0].value;
        }
        if (gTwoPlayerDuel_aSetupEntries[0].shown < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (gTwoPlayerDuel_aSetupEntries[0].shown < gTwoPlayerDuel_aSetupEntries[0].value) {
            gTwoPlayerDuel_aSetupEntries[0].shown = gTwoPlayerDuel_aSetupEntries[0].shown + step;
        } else {
            gTwoPlayerDuel_aSetupEntries[0].shown = gTwoPlayerDuel_aSetupEntries[0].shown - step;
        }
        busyA++;
    }

    if (gTwoPlayerDuel_aSetupEntries[1].value != gTwoPlayerDuel_aSetupEntries[1].shown) {
        step = gTwoPlayerDuel_aSetupEntries[1].value - gTwoPlayerDuel_aSetupEntries[1].shown;
        if (step < 0) {
            step = gTwoPlayerDuel_aSetupEntries[1].shown - gTwoPlayerDuel_aSetupEntries[1].value;
        }
        if (gTwoPlayerDuel_aSetupEntries[1].shown < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (gTwoPlayerDuel_aSetupEntries[1].shown < gTwoPlayerDuel_aSetupEntries[1].value) {
            gTwoPlayerDuel_aSetupEntries[1].shown = gTwoPlayerDuel_aSetupEntries[1].shown + step;
        } else {
            gTwoPlayerDuel_aSetupEntries[1].shown = gTwoPlayerDuel_aSetupEntries[1].shown - step;
        }
        busyB++;
    }

    if (busyA == 0 && busyB == 0) {
        if ((gInput_awPadPressed[0] & MAIN_MENU_VALUE_SETUP_CANCEL_BUTTON) ||
            (gInput_awPadPressed[1] & MAIN_MENU_VALUE_SETUP_CANCEL_BUTTON)) {
            SD_SEPlay(8, 0xFF, 0);
            return -1;
        }
        if ((gInput_awPadPressed[0] & PAD_BUTTON_START) || (gInput_awPadPressed[1] & PAD_BUTTON_START)) {
            SD_SEPlay(7, 0xFF, 0);
            return 1;
        }
    }

    if (busyA == 0) {
        if (gTwoPlayerDuel_abSetupChoices[0] < MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
            if (gInput_awPadRepeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_awPadRepeat[0] & PAD_DIRECTION_LEFT) {
                    gTwoPlayerDuel_abSetupChoices[2] = MAIN_MENU_VALUE_SETUP_CHOICE_LEFT;
                } else {
                    gTwoPlayerDuel_abSetupChoices[2] = MAIN_MENU_VALUE_SETUP_CHOICE_RIGHT;
                }
                gTwoPlayerDuel_abSetupChoices[0] = gTwoPlayerDuel_abSetupChoices[2];
                if (gTwoPlayerDuel_abSetupChoices[1] < MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
                    gTwoPlayerDuel_abSetupChoices[1] = gTwoPlayerDuel_abSetupChoices[2];
                }
            } else if (gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(0, MAIN_MENU_VALUE_SETUP_CHOICE_VALUE);
            }
        } else {
            if (gInput_awPadRepeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = gTwoPlayerDuel_aSetupEntries[0].value;
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_awPadRepeat[0] & PAD_DIRECTION_LEFT) {
                    value = (value - DUEL_LIFE_POINT_SELECTION_STEP > 0)
                                ? (value - DUEL_LIFE_POINT_SELECTION_STEP)
                                : 1;
                } else if (value < 2) {
                    value = DUEL_LIFE_POINT_SELECTION_STEP;
                } else {
                    value = (
                        value + DUEL_LIFE_POINT_SELECTION_STEP <
                        DUEL_STARTING_LIFE_POINTS + 1
                    ) ? (value + DUEL_LIFE_POINT_SELECTION_STEP)
                      : DUEL_STARTING_LIFE_POINTS;
                }
                gTwoPlayerDuel_aSetupEntries[0].value = value;
            } else if (gInput_awPadRepeat[0] & PAD_DIRECTION_UP) {
                MainMenu_StartValueWidgetTween(0, gTwoPlayerDuel_abSetupChoices[2]);
            }
        }
    }

    if (busyB == 0) {
        if (gTwoPlayerDuel_abSetupChoices[1] < MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
            if (gInput_awPadRepeat[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_awPadRepeat[1] & PAD_DIRECTION_LEFT) {
                    gTwoPlayerDuel_abSetupChoices[2] = MAIN_MENU_VALUE_SETUP_CHOICE_LEFT;
                } else {
                    gTwoPlayerDuel_abSetupChoices[2] = MAIN_MENU_VALUE_SETUP_CHOICE_RIGHT;
                }
                gTwoPlayerDuel_abSetupChoices[1] = gTwoPlayerDuel_abSetupChoices[2];
                if (gTwoPlayerDuel_abSetupChoices[0] < MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
                    gTwoPlayerDuel_abSetupChoices[0] = gTwoPlayerDuel_abSetupChoices[2];
                }
            } else if (gInput_awPadRepeat[1] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(1, MAIN_MENU_VALUE_SETUP_CHOICE_VALUE);
            }
        } else {
            if (gInput_awPadRepeat[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = gTwoPlayerDuel_aSetupEntries[1].value;
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_awPadRepeat[1] & PAD_DIRECTION_LEFT) {
                    value = (value - DUEL_LIFE_POINT_SELECTION_STEP > 0)
                                ? (value - DUEL_LIFE_POINT_SELECTION_STEP)
                                : 1;
                } else if (value < 2) {
                    value = DUEL_LIFE_POINT_SELECTION_STEP;
                } else {
                    value = (
                        value + DUEL_LIFE_POINT_SELECTION_STEP <
                        DUEL_STARTING_LIFE_POINTS + 1
                    ) ? (value + DUEL_LIFE_POINT_SELECTION_STEP)
                      : DUEL_STARTING_LIFE_POINTS;
                }
                gTwoPlayerDuel_aSetupEntries[1].value = value;
            } else if (gInput_awPadRepeat[1] & PAD_DIRECTION_UP) {
                MainMenu_StartValueWidgetTween(1, gTwoPlayerDuel_abSetupChoices[2]);
            }
        }
    }

    return 0;
}
#undef MAIN_MENU_VALUE_SETUP_CANCEL_BUTTON

void MainMenu_DrawValueSetup(void)
{
    POLY_GT4 digit;
    POLY_G4 bar;
    DisplayObject *w;
    DisplayObject *mk;
    s32 first;
    s32 second;
    s32 x;
    s32 i;
    s32 count;
    s32 width;
    s32 c106;
    s32 c114;
    s32 c112;
    s32 c120;
    s32 d134;
    s32 d142;
    s32 d112;
    s32 d120;

    first = gTwoPlayerDuel_aSetupEntries[0].shown;
    second = gTwoPlayerDuel_aSetupEntries[1].shown;
    mk = gTwoPlayerDuel_apSetupCursors[2];
    if (gTwoPlayerDuel_abSetupChoices[2] == MAIN_MENU_VALUE_SETUP_CHOICE_LEFT) {
        x = 116;
    } else {
        x = 220;
    }
    mk->xy.h.x = x;
    gTwoPlayerDuel_apSetupCursors[2]->xy.h.y = 74;

    w = gTwoPlayerDuel_apSetupCursors[0];
    if (w->update != 0 || gTwoPlayerDuel_abSetupChoices[0] == MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = gTwoPlayerDuel_apSetupCursors[0];
        if (w->update == 0) {
            w->xy.h.x = first * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            gTwoPlayerDuel_apSetupCursors[0]->xy.h.y = 111;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        gTwoPlayerDuel_apSetupCursors[0]->xy.h.x = gTwoPlayerDuel_apSetupCursors[2]->xy.h.x;
        gTwoPlayerDuel_apSetupCursors[0]->xy.h.y = gTwoPlayerDuel_apSetupCursors[2]->xy.h.y;
    }

    w = gTwoPlayerDuel_apSetupCursors[1];
    if (w->update != 0 || gTwoPlayerDuel_abSetupChoices[1] == MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = gTwoPlayerDuel_apSetupCursors[1];
        if (w->update == 0) {
            w->xy.h.x = second * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            gTwoPlayerDuel_apSetupCursors[1]->xy.h.y = 139;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        gTwoPlayerDuel_apSetupCursors[1]->xy.h.x = gTwoPlayerDuel_apSetupCursors[2]->xy.h.x;
        gTwoPlayerDuel_apSetupCursors[1]->xy.h.y = gTwoPlayerDuel_apSetupCursors[2]->xy.h.y;
    }

    setPolyG4(&bar);
    bar.r0 = 64;
    bar.g0 = 32;
    bar.b0 = 32;
    bar.r1 = 255;
    bar.g1 = 32;
    bar.b1 = 32;
    bar.r2 = 64;
    bar.g2 = 32;
    bar.b2 = 32;
    bar.r3 = 255;
    bar.g3 = 32;
    bar.b3 = 32;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 107;
    bar.y1 = 107;
    bar.y2 = 115;
    bar.y3 = 115;
    bar.x1 = first * 128 / DUEL_STARTING_LIFE_POINTS + 176;
    bar.x3 = bar.x1;
    GsSortPoly(&bar, gGraphics_apOrderingTables[2], 2048);

    bar.r0 = 32;
    bar.g0 = 32;
    bar.b0 = 64;
    bar.r1 = 32;
    bar.g1 = 32;
    bar.b1 = 255;
    bar.r2 = 32;
    bar.g2 = 32;
    bar.b2 = 64;
    bar.r3 = 32;
    bar.g3 = 32;
    bar.b3 = 255;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 135;
    bar.y1 = 135;
    bar.y2 = 143;
    bar.y3 = 143;
    bar.x1 = second * 128 / DUEL_STARTING_LIFE_POINTS + 176;
    bar.x3 = bar.x1;
    GsSortPoly(&bar, gGraphics_apOrderingTables[2], 2048);

    setPolyGT4(&digit);
#ifdef VERSION_EUROPE
    digit.tpage = 10;
    digit.clut = 0x3028;
    digit.r0 = 128;
    digit.g0 = 128;
    digit.b0 = 128;
    digit.r1 = 128;
    digit.g1 = 128;
    digit.b1 = 128;
    digit.r2 = 128;
    digit.g2 = 128;
    digit.b2 = 128;
    digit.r3 = 128;
    digit.g3 = 128;
    digit.b3 = 128;
#else
    digit.tpage = 11;
    digit.clut = 16041;
    digit.r0 = 255;
    digit.g0 = 255;
    digit.b0 = 255;
    digit.r1 = 255;
    digit.g1 = 255;
    digit.b1 = 255;
    digit.r2 = 255;
    digit.g2 = 255;
    digit.b2 = 255;
    digit.r3 = 255;
    digit.g3 = 255;
    digit.b3 = 255;
#endif

    width = MainMenu_CountDecimalDigits(DUEL_STARTING_LIFE_POINTS);
    count = MainMenu_CountDecimalDigits(first);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        c106 = 106;
        c114 = 114;
#ifdef VERSION_EUROPE
        c112 = 64;
        c120 = 72;
#else
        c112 = 112;
        c120 = 120;
#endif
        digit.y0 = c106;
        digit.x1 = digit.x0 + 8;
        digit.y1 = c106;
        digit.x2 = digit.x0;
        digit.y2 = c114;
        digit.x3 = digit.x1;
        digit.y3 = c114;
        digit.v0 = c112;
        digit.v1 = c112;
        digit.v2 = c120;
        digit.v3 = c120;
        digit.u0 = (first % 10) * 8 - 128;
        digit.u1 = (first % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        GsSortPoly(&digit, gGraphics_apOrderingTables[2], 2048);
        first = first / 10;
    }

    count = MainMenu_CountDecimalDigits(second);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        d134 = 134;
        d142 = 142;
#ifdef VERSION_EUROPE
        d112 = 64;
        d120 = 72;
#else
        d112 = 112;
        d120 = 120;
#endif
        digit.y0 = d134;
        digit.x1 = digit.x0 + 8;
        digit.y1 = d134;
        digit.x2 = digit.x0;
        digit.y2 = d142;
        digit.x3 = digit.x1;
        digit.y3 = d142;
        digit.v0 = d112;
        digit.v1 = d112;
        digit.v2 = d120;
        digit.v3 = d120;
        digit.u0 = (second % 10) * 8 - 128;
        digit.u1 = (second % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        GsSortPoly(&digit, gGraphics_apOrderingTables[2], 2048);
        second = second / 10;
    }
}

void MainMenu_UpdateValueWidgetTween(DisplayObject *o)
{
    DisplayObject *widget;
    s32 targetX;
    s32 targetY;
    s32 valueA;
    s32 valueB;
    s16 remaining;

    widget = gTwoPlayerDuel_apSetupCursors[2];
    targetX = (s16)widget->xy.h.x;
    targetY = (s16)widget->xy.h.y;
    valueA = gTwoPlayerDuel_aSetupEntries[0].shown;
    valueB = gTwoPlayerDuel_aSetupEntries[1].shown;
    if (o->update_state == MAIN_MENU_VALUE_SETUP_CHOICE_VALUE) {
        if (o->kind_arg.choice_index == 0) {
            targetX = (valueA * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 111;
        } else {
            targetX = (valueB * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 139;
        }
    }
    remaining = o->field_60 - 1;
    o->field_60 = remaining;
    if (remaining <= 0) {
        gTwoPlayerDuel_abSetupChoices[o->kind_arg.choice_index] = o->update_state;
        o->update = 0;
    }
    o->xy.h.x =
        (o->field_34.h.field_36 * o->field_60) / 10 +
        (targetX * (10 - o->field_60)) / 10;
    o->xy.h.y =
        (o->field_38.h.field_38 * o->field_60) / 10 +
        (targetY * (10 - o->field_60)) / 10;
}

void MainMenu_FinishValueSetup(void)
{
    *gTwoPlayerDuel_aSetupEntries[0].out = gTwoPlayerDuel_aSetupEntries[0].value;
    *gTwoPlayerDuel_aSetupEntries[1].out = gTwoPlayerDuel_aSetupEntries[1].value;
    *gTwoPlayerDuel_pSetupToggle = (gTwoPlayerDuel_abSetupChoices[2] != MAIN_MENU_VALUE_SETUP_CHOICE_RIGHT);
    DisplayObject_ReleaseIfPresent(gTwoPlayerDuel_pBackground);
    gTwoPlayerDuel_pBackground = 0;
    DisplayObject_ReleaseIfPresent(gTwoPlayerDuel_pSetupPanel);
    gTwoPlayerDuel_pSetupPanel = 0;
    DisplayObject_ReleaseIfPresent(gTwoPlayerDuel_apSetupCursors[0]);
    gTwoPlayerDuel_apSetupCursors[0] = 0;
    DisplayObject_ReleaseIfPresent(gTwoPlayerDuel_apSetupCursors[1]);
    gTwoPlayerDuel_apSetupCursors[1] = 0;
    DisplayObject_ReleaseIfPresent(gTwoPlayerDuel_apSetupCursors[2]);
    gTwoPlayerDuel_apSetupCursors[2] = 0;
    gMain_apFrameServiceCallbacks[0] = 0;
}

s32 MainMenu_CountDecimalDigits(s32 value)
{
    s32 digits = 0;
    s32 next;

    do {
        digits++;
        next = value / 10;
        value = next;
    } while (next != 0);

    return digits;
}

void MainMenu_StartValueWidgetTween(s32 index, s32 value)
{
    DisplayObject *object = gTwoPlayerDuel_apSetupCursors[index];

    object->kind_arg.choice_index = index;
    object->field_60 = 0xA;
    object->update_state = value;
    object->update = MainMenu_UpdateValueWidgetTween;
    object->field_34.h.field_36 = object->xy.h.x;
    object->field_38.h.field_38 = object->xy.h.y;
}
