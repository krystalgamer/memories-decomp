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
#include "../../game/text_box_update_channels.h"
#include "../../game/duel_effect_set_entry_range_layout.h"
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

#ifndef NAME_ENTRY_CONFIRM_MASK
#define NAME_ENTRY_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif
#ifndef NAME_ENTRY_CANCEL_MASK
#define NAME_ENTRY_CANCEL_MASK PAD_BUTTON_CANCEL
#endif

static __inline__ s32 NameEntry_GetKeyboardCellCode(s8 *table, s32 row, s32 column)
{
    column += row * 15;
    return table[column] & 0xF;
}

void NameEntry_UpdateKeyboard(void)
{
#ifdef VERSION_EUROPE
    s32 work;
    SelectionFrame *w;
    DisplayObject *object;
#else
    SelectionFrame *w;
    s32 work;
#endif
    s8 *glyphTable;
    s32 glyphRow;
    s32 glyphCol;
    s32 glyphCode;
    s32 home;
#ifndef VERSION_EUROPE
    s32 n;
#endif
    s32 walkCol;
    s32 col;
    s32 row;
    DuelEffectEntry *node;
#ifndef VERSION_EUROPE
    u8 *obj;
#endif
    s32 kind;
#ifdef VERSION_EUROPE
    s32 n;
#endif
    s32 gx;
    s32 gy;
    s32 d;

    w = gNameEntry_pSelectionFrame;
    if ((gNameEntry_wSelectionFrameFlags & 0x4000) != 0) {
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
        w->x = gNameEntry_wCursorTargetX;
        w->y = gNameEntry_wCursorTargetY;
        gNameEntry_wSelectionFrameFlags &= 0xBFFF;
    }
    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            gNameEntry_bKeyboardColumn = gNameEntry_bKeyboardColumn + 1;
            if (gNameEntry_bKeyboardColumn >= 15) {
                gNameEntry_bKeyboardColumn = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            gNameEntry_bKeyboardColumn = gNameEntry_bKeyboardColumn - 1;
            if (gNameEntry_bKeyboardColumn < 0) {
                gNameEntry_bKeyboardColumn = 14;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if (gNameEntry_bKeyboardColumn >= 11) {
#ifdef VERSION_EUROPE
                SD_SEPlayFull(9);
                return;
#else
                n = gInput_wPad1Held & PAD_DIRECTION_DOWN;
                work = (n != 0);
                gNameEntry_bKeyboardColumn = 11;
                gNameEntry_bSavedRow = gNameEntry_bKeyboardRow = gNameEntry_aRowLinks[(s8)gNameEntry_bKeyboardRow][work];
                goto tail47;
#endif
            } else if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
                gNameEntry_bKeyboardRow = gNameEntry_bKeyboardRow - 1;
                if ((s8)gNameEntry_bKeyboardRow < 0) {
                    gNameEntry_bKeyboardRow = 8;
                }
            } else {
                gNameEntry_bKeyboardRow = gNameEntry_bKeyboardRow + 1;
                if ((s8)gNameEntry_bKeyboardRow >= 9) {
                    gNameEntry_bKeyboardRow = 0;
                }
            }
            gNameEntry_bSavedRow = gNameEntry_bKeyboardRow;
        }
    } else {
        goto alt800;
    }
tail47:
    SD_SEPlayFull(47);
#ifdef VERSION_EUROPE
    {
        s32 tableBase;

        tableBase = (s32)&gNameEntry_aKeyboardCells[0][0];
        do {
        } while (0);
        row = (s8)gNameEntry_bKeyboardRow;
        {
            PasswordGlyphCoordinates coords = {
                parts: { (s8)gNameEntry_bKeyboardColumn, row * 15 }
            };
            col = coords.parts.column;
            work = *(s8 *)((coords.parts.column + coords.parts.stride) +
                           tableBase);
            walkCol = col;
            while (work < 0) {
                /* Preserve GCC's separate loop-invariant address lifetimes. */
                do {
                    walkCol = walkCol + work;
                    gNameEntry_bKeyboardColumn = walkCol;
                    work = *(s8 *)(((s8)walkCol + coords.parts.stride) +
                                   tableBase);
                } while (0);
            }
        }
    }
#else
    obj = (u8 *)&gNameEntry_aKeyboardCells[0][0];
    row = (s8)gNameEntry_bKeyboardRow;
    {
        /* Keep the pair fully initialized; separate stores change allocation. */
        PasswordGlyphCoordinates coords = {
            parts: { (s8)gNameEntry_bKeyboardColumn, row * 15 }
        };
        col = coords.parts.column;
        work = *(s8 *)((coords.parts.column + coords.parts.stride) + (s32)obj);
        walkCol = col;
        while (work < 0) {
            walkCol = walkCol + work;
            gNameEntry_bKeyboardColumn = walkCol;
            work = *(s8 *)(((s8)walkCol + coords.parts.stride) + (s32)obj);
        }
    }
#endif
    gNameEntry_bKeyboardRow = gNameEntry_bSavedRow;
    w->widthBonus = 0;
    if (work != 0) {
        if ((work & 0x40) != 0) {
            w->widthBonus = 20;
        }
        gNameEntry_bKeyboardRow = work & 0xF;
    }
    gNameEntry_wCursorTargetX = (s8)gNameEntry_bKeyboardColumn * 20 + 22;
    if ((s8)gNameEntry_bKeyboardColumn >= 11) {
        gNameEntry_wCursorTargetX = (s8)gNameEntry_bKeyboardColumn * 20 + 42;
    }
    gNameEntry_wCursorTargetY = (s8)gNameEntry_bKeyboardRow * 18 + 24;
    DisplayObject_ResetVelocity((DisplayObjectVelocity *)w);
    w->timer = 8;
    w->stepX = ((gNameEntry_wCursorTargetX - w->x) << 8) / 8;
    w->stepY = ((gNameEntry_wCursorTargetY - w->y) << 8) / 8;
    gNameEntry_wSelectionFrameFlags |= 0x4000;
    return;

alt800:
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto select;
    }
    gNameEntry_bKeyboardColumn = 14;
    gNameEntry_bKeyboardRow = 8;
    goto tail47;
select:
#ifdef VERSION_EUROPE
    if ((gInput_wPad1Repeat & 0xC) != 0) {
        work = 1;
        if ((gInput_wPad1Repeat & 4) != 0) {
            work = -1;
        }
        if (NameEntry_AdjustLength(work, 6) == 0) {
            SD_SEPlayFull(9);
        }
        return;
    }
#endif
    if ((gInput_wPad1Repeat & NAME_ENTRY_CONFIRM_MASK) == 0) {
        goto sel_ret;
    }
#ifdef VERSION_EUROPE
    kind = 0;
    work = -1;
    gx = 0;
    gy = 0;
    glyphTable = &gNameEntry_aKeyboardCells[0][0];
    glyphRow = (s8)gNameEntry_bKeyboardRow;
    glyphCol = col = (s8)gNameEntry_bKeyboardColumn;
    glyphCode = NameEntry_GetKeyboardCellCode(glyphTable, glyphRow, glyphCol);
    if (glyphCode == 8) {
        work = 2;
        gy = 144;
        gNameEntry_bStateFlags |= 0x40;
    } else {
        n = 41;
        kind = 1;
        gx = glyphCol * 20;
        gy = glyphRow * 18;
        if (glyphRow == 8 && (u8)(gNameEntry_bKeyboardColumn - 9) < 2) {
            work = glyphCol - 9;
            d = kind;
            if (work == 0) {
                d = -1;
            }
            if (NameEntry_AdjustLength(d, 6) == 0) {
                n = 9;
            }
        }
        SD_SEPlayFull(n);
    }
    if (work >= 0) {
        object = gNameEntry_apKeySprites[work];
        object->field_6C = 1;
        object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
        object->field_4C = 0;
        object->field_6B = 1;
        return;
    }
    node = TextBox_GetGlyphAt(kind, gx | 2, gy);
    object = NameEntry_SpawnGlyphSprite(kind, node);
    object->field_6C = 1;
    object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        object->flags &= 0xFFBF;
    }
    if (kind == 1) {
        u16 *slot;
        gNameEntry_bStateFlags |= 0x80;
        slot = (u16 *)(gNameEntry_bCaretPos * 2 + (s32)gNameEntry_pNameBuffer);
        *slot = 0;
        if (node != 0) {
            *slot = node->code_00;
        }
        object = NameEntry_SpawnGlyphSprite(1, node);
        object->field_60 = 8;
        object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphTransfer;
        object->field_44.h.field_46 = 204;
        object->field_44.h.field_44 = gNameEntry_bCaretPos * 16 + 114;
        object->field_6C = 6;
    }
    return;
#else
    kind = 0;
    work = kind;
#ifdef VERSION_JAPAN
    gy = kind;
#endif
    glyphTable = &gNameEntry_aKeyboardCells[0][0];
    glyphRow = (s8)gNameEntry_bKeyboardRow;
    glyphCol = col = (s8)gNameEntry_bKeyboardColumn;
    glyphCode = NameEntry_GetKeyboardCellCode(glyphTable, glyphRow, glyphCol);
    gx = kind;
#ifdef VERSION_JAPAN
    switch (glyphCode) {
    case 2:
        goto arm2;
    case 4:
        goto arm4;
    case 6:
        goto arm6;
    }
    goto arme;
arm2:
    /* The Japanese keyboard's mode key: it cycles the character set
     * gNameEntry_bCharacterSet through 0, 1 and 2 and rebuilds the keyboard for it. */
    gNameEntry_bCharacterSet++;
    if (gNameEntry_bCharacterSet >= 3) {
        gNameEntry_bCharacterSet = 0;
    }
    NameEntry_BuildKeyboardTextBox(gNameEntry_bCharacterSet);
    work = 2;
    SD_SEPlayFull(7);
    goto join;
#else
    if (glyphCode == 4) {
        goto arm4;
    }
    if (glyphCode == 6) {
        goto arm6;
    }
    goto arme;
#endif
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
    gNameEntry_bStateFlags |= 0x40;
    goto join;
arme:
    kind = 1;
    gx = (s8)gNameEntry_bKeyboardColumn * 20;
    gy = ((s8)gNameEntry_bKeyboardRow * 9) << kind;
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
        gNameEntry_bStateFlags |= 0x80;
        slot = (u16 *)(gNameEntry_bCaretPos * 2 + (s32)gNameEntry_pNameBuffer);
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
            gNameEntry_bCaretPos * 16 + 112;
        ((DisplayObject *)obj)->field_6C = 6;
    }
    return;
#endif
sel_ret:
#ifdef VERSION_EUROPE
    if ((gInput_wPad1Repeat & NAME_ENTRY_CANCEL_MASK) != 0) {
        if (NameEntry_AdjustLength(-1, 6) != 0) {
            SD_SEPlayFull(12);
        } else {
            SD_SEPlayFull(9);
        }
    }
#else
    if ((gInput_wPad1Repeat & NAME_ENTRY_CANCEL_MASK) != 0) {
        n = NameEntry_AdjustLength(-1, 6);
        if (n != 0) {
            n = 12;
        } else {
            n = 9;
        }
        SD_SEPlayFull(n);
    }
#endif
    return;
}
