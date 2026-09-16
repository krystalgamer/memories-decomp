#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/display_object_config.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/func_80039794.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/gpu_packets.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "../../game/fade.h"
#include "../../overlays/password/name_entry_keyboard.h"
#include "../../overlays/password/name_entry_frame.h"
#include "../../overlays/password/name_entry_state.h"
#include "../../overlays/password/dialog_choice_ref.h"
#include "../../game/rand_get_interval.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/dialog_choice_state.h"
#include "../../game/display_object_interpolation.h"
#include "../../game/text_sjis_to_glyph_codes.h"
#include "name_entry_tables.h"

static __inline__ s32 NameEntry_GetKeyboardCellCode(s8 *table, s32 row, s32 column)
{
    column += row * 15;
    return table[column] & 0xF;
}

void NameEntry_UpdateKeyboard(void)
{
    SelectionFrame *w;
    s32 work;
    s8 *glyphTable;
    s32 glyphRow;
    s32 glyphCol;
    s32 glyphCode;
    s32 home;
    s32 n;
    s32 walkCol;
    s32 col;
    s32 row;
    DuelEffectEntry *node;
    u8 *obj;
    s32 kind;
    s32 gx;
    s32 gy;
    s32 d;

    w = D_8016D404;
    if ((D_8016D4D4 & 0x4000) != 0) {
        home = w->width - 16;
        work = w->widthBonus - home;
        if (work != 0) {
            w->width = (work >= 0) ? (w->width + 2) : (w->width - 2);
        }
        DisplayObject_StepPositionXY((DisplayObjectVelocity *)w);
        w->timer = w->timer - 1;
        if (w->timer != 0) {
            return;
        }
        w->width = w->widthBonus + 16;
        w->x = D_8016D434;
        w->y = D_8016D436;
        D_8016D4D4 &= 0xBFFF;
    }
    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            D_8016D401 = D_8016D401 + 1;
            if (D_8016D401 >= 15) {
                D_8016D401 = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            D_8016D401 = D_8016D401 - 1;
            if (D_8016D401 < 0) {
                D_8016D401 = 14;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if (D_8016D401 >= 11) {
                n = gInput_wPad1Held & PAD_DIRECTION_DOWN;
                work = (n != 0);
                D_8016D401 = 11;
                D_8016D426 = D_8016D402 = D_8016ABC0[(s8)D_8016D402][work];
                goto tail47;
            } else if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
                D_8016D402 = D_8016D402 - 1;
                if ((s8)D_8016D402 < 0) {
                    D_8016D402 = 8;
                }
            } else {
                D_8016D402 = D_8016D402 + 1;
                if ((s8)D_8016D402 >= 9) {
                    D_8016D402 = 0;
                }
            }
            D_8016D426 = D_8016D402;
        }
    } else {
        goto alt800;
    }
tail47:
    SD_SEPlayFull(47);
    obj = (u8 *)&D_8016AB38[0][0];
    row = (s8)D_8016D402;
    {
        /* Keep the pair fully initialized; separate stores change allocation. */
        PasswordGlyphCoordinates coords = {
            parts: { (s8)D_8016D401, row * 15 }
        };
        col = coords.parts.column;
        work = *(s8 *)((coords.parts.column + coords.parts.stride) + (s32)obj);
        walkCol = col;
        while (work < 0) {
            walkCol = walkCol + work;
            D_8016D401 = walkCol;
            work = *(s8 *)(((s8)walkCol + coords.parts.stride) + (s32)obj);
        }
    }
    D_8016D402 = D_8016D426;
    w->widthBonus = 0;
    if (work != 0) {
        if ((work & 0x40) != 0) {
            w->widthBonus = 20;
        }
        D_8016D402 = work & 0xF;
    }
    D_8016D434 = (s8)D_8016D401 * 20 + 22;
    if ((s8)D_8016D401 >= 11) {
        D_8016D434 = (s8)D_8016D401 * 20 + 42;
    }
    D_8016D436 = (s8)D_8016D402 * 18 + 24;
    DisplayObject_ResetVelocity((DisplayObjectVelocity *)w);
    w->timer = 8;
    w->stepX = ((D_8016D434 - w->x) << 8) / 8;
    w->stepY = ((D_8016D436 - w->y) << 8) / 8;
    D_8016D4D4 |= 0x4000;
    return;

alt800:
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto select;
    }
    D_8016D401 = 14;
    D_8016D402 = 8;
    goto tail47;
select:
    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) == 0) {
        goto sel_ret;
    }
    kind = 0;
    work = kind;
    glyphTable = &D_8016AB38[0][0];
    glyphRow = (s8)D_8016D402;
    glyphCol = col = (s8)D_8016D401;
    glyphCode = NameEntry_GetKeyboardCellCode(glyphTable, glyphRow, glyphCol);
    gx = kind;
    if (glyphCode == 4) {
        goto arm4;
    }
    if (glyphCode == 6) {
        goto arm6;
    }
    goto arme;
arm4:
    d = 1;
    if (glyphCol == 11) {
        d = -1;
    } else {
        gx = 20;
    }
    if (NameEntry_AdjustLength(d, 6) == 0) {
        SD_SEPlayFull(9);
    }
    gy = 36;
    goto join;
arm6:
    work = 2;
    gy = 72;
    D_8016D400 |= 0x40;
    goto join;
arme:
    kind = 1;
    gx = (s8)D_8016D401 * 20;
    gy = ((s8)D_8016D402 * 9) << kind;
    SD_SEPlayFull(41);
join:
    ;
    node = TextBox_GetGlyphAt(kind, gx, gy);
    obj = NameEntry_SpawnGlyphSprite(kind, node);
    ((DisplayObject *)obj)->field_6C = 1;
    ((DisplayObject *)obj)->update =
        (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        ((DisplayObject *)obj)->flags &= 0xFFBF;
    }
    if (work != 0) {
        ((DisplayObject *)obj)->field_48.h.field_48 = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = NameEntry_SpawnGlyphSprite(kind, node);
        ((DisplayObject *)obj)->field_6C = 1;
        ((DisplayObject *)obj)->update =
            (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
        ((DisplayObject *)obj)->field_48.h.field_48 = 0;
    }
    if (kind == 1) {
        u16 *slot;
        D_8016D400 |= 0x80;
        slot = (u16 *)(D_8016D42C * 2 + (s32)D_8016D418);
        *slot = 0;
        if (node != 0) {
            *slot = node->code_00;
        }
        obj = NameEntry_SpawnGlyphSprite(1, node);
        ((DisplayObject *)obj)->field_60 = 8;
        ((DisplayObject *)obj)->update =
            (DisplayObjectCallback)NameEntry_UpdateGlyphTransfer;
        ((DisplayObject *)obj)->field_44.h.field_46 = 204;
        ((DisplayObject *)obj)->field_44.h.field_44 =
            D_8016D42C * 16 + 112;
        ((DisplayObject *)obj)->field_6C = 6;
    }
    return;
sel_ret:
    if ((gInput_wPad1Repeat & PAD_BUTTON_CANCEL) != 0) {
        n = NameEntry_AdjustLength(-1, 6);
        if (n != 0) {
            n = 12;
        } else {
            n = 9;
        }
        SD_SEPlayFull(n);
    }
    return;
}
