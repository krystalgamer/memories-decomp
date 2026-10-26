#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/display_object_config.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/text_box_update_channels.h"
#include "../../game/duel_effect_set_entry_range_layout.h"
#include "../../game/gpu_packets.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "../../game/player_name_glyphs.h"
#include "../../game/fade.h"
#include "name_entry_keyboard.h"
#include "name_entry_frame.h"
#include "name_entry_state.h"
#include "dialog_choice_ref.h"
#include "../../game/rand_get_interval.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/dialog_choice_state.h"
#include "../../game/display_object_interpolation.h"
#include "../../game/text_sjis_to_glyph_codes.h"

/* The end of the name-entry screen pipeline: the confirmation dialog and
   completion polling, on the same gNameEntry_bStateFlags state block as the rest. They
   follow NameEntry_UpdateKeyboard, the binding-free matching C function that
   closes name_entry_runtime.c after the earlier stages. The shop display unit after them has no proven
   ownership in this lifecycle. */

#ifndef NAME_ENTRY_DIALOG_CHANNEL_TYPE
#define NAME_ENTRY_DIALOG_CHANNEL_TYPE DuelEffectChannel
#endif
#ifndef NAME_ENTRY_DIALOG_COMPLETION_MASK
#define NAME_ENTRY_DIALOG_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_DIALOG) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_DIALOG)
void NameEntry_UpdateDialog(void)
{
    DuelEffectChannel *box;
    DialogCaret *caret;
    u8 flags;
    u8 fb;
    u8 fc;
    u8 fd;
    u8 fe;
    u8 ff;
    u8 fg;
    s32 id;
    s16 pos;
    u8 *p;
    u8 *next;
    DuelEffectChannel *panel;
    s32 c;
    s32 term;

    if (gNameEntry_wPendingDialogId != 0) {
        flags = gNameEntry_bStateFlags;
        if ((flags & 4) == 0) {
            gNameEntry_bStateFlags = flags | 4;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
            /* The Japanese and European builds make no func_8003B6AC call here. */
            func_8003B6AC(2, 2);
#endif
#ifdef VERSION_EUROPE
            box = TextBox_CreateFlagged(2, gNameEntry_wPendingDialogId & 0xFFF, 16, 248,
                                        288, 64, 2);
#else
            box = TextBox_Create(2, gNameEntry_wPendingDialogId & 0xFFF, 16, 248,
                                 288, 48);
#endif
            box->field_59 = 20;
            id = gNameEntry_wPendingDialogId;
            if ((id & 0x8000) == 0) {
                if ((id & 0x4000) == 0) {
                    func_80039A14((struct DuelEffectChannel *)box);
                    D_8009B34D = 0;
                }
                box->field_30 = Dialog_OpenChoice(box);
            } else {
#ifdef VERSION_EUROPE
                box->flags_34 |= 0x10;
#else
                box->flags_34 |= 8;
#endif
                do {
                    TextBox_UpdateChannels();
                } while (box->field_30 == 0);
            }
            caret = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPosition(caret, 16, 248, 0, 0, 0, 23, 257);
            DisplayObject_SelectOrderingTable1(caret);
            DisplayObject_SetDepthOffset(caret, 19);
            caret->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            box->field_2C = caret;
            DisplayObject_SavePosition(caret);
            caret->slide = -1024;
            gNameEntry_bStateFlags |= 2;
            return;
        }
#ifdef VERSION_EUROPE
        box = &gText_Channel2View;
#else
        box = &D_800EB1C0;
#endif
        caret = (DialogCaret *)box->field_2C;
        if ((flags & 2) != 0) {
            pos = caret->slide;
            if (pos >= 0) {
                caret->slide = pos - 85;
                Widget_SlideSine((DisplayObjectPosition *)caret, 16, 248,
                              (s16)(pos - 85));
                if (caret->slide < 0) {
                    caret->x = 16;
                    fb = gNameEntry_bStateFlags;
                    caret->y = 248;
                    gNameEntry_bStateFlags = fb & 0xF9;
                    TextBox_Destroy(box);
                    gNameEntry_wPendingDialogId = 0;
                    return;
                }
            } else {
                caret->slide = pos + 85;
                Widget_SlideSine((DisplayObjectPosition *)caret, 16, 176,
                              (s16)(pos + 85));
                if (caret->slide >= 0) {
                    caret->x = 16;
                    fc = gNameEntry_bStateFlags;
                    caret->y = 176;
                    gNameEntry_bStateFlags = fc & 0xFD;
                }
            }
            TextBox_SetPos((struct DuelEffectChannel *)box, caret->x, caret->y);
            return;
        }
        if ((gNameEntry_wPendingDialogId & 0x8000) == 0) {
            if ((gInput_wPad1Pressed & (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                SD_SEPlayFull(11);
                return;
            }
        } else {
            TextBox_UpdateChannels();
            if ((*(u32 *)&box->flags_34 & NAME_ENTRY_DIALOG_COMPLETION_MASK) !=
                TEXT_BOX_FLAG_DONE) {
                return;
            }
        }
        if (D_8009B34D != 0) {
            fd = gNameEntry_bStateFlags;
            gNameEntry_bStateFlags = fd & 0xDF;
        }
        if ((gNameEntry_bStateFlags & 0x20) != 0) {
            gNameEntry_wPendingDialogId = 0;
        }
        DisplayObject_SavePosition(caret);
        fe = gNameEntry_bStateFlags;
        caret->slide = 1024;
        gNameEntry_bStateFlags = fe | 2;
        return;
    }
    flags = gNameEntry_bStateFlags;
    if ((flags & 0x20) != 0) {
        SD_SEPlayFull(45);
        SD_BGMFadeOut();
        Fade_WaitOut();
        gNameEntry_bStateFlags = gNameEntry_bStateFlags | 0x10;
        return;
    }
    if ((flags & 0x80) != 0) {
        caret = (DialogCaret *)DisplayObject_FindAllocatedByTag(6);
        if (caret == 0) {
            return;
        }
        if ((caret->status & 0x40) == 0) {
            return;
        }
        DisplayObject_ReleaseIfPresent(caret);
        ff = gNameEntry_bStateFlags;
        gNameEntry_bStateFlags = ff & 0x7F;
        Text_SjisToGlyphCodes(gText_aCurrentPlayerNameGlyphs, gNameEntry_pNameBuffer, 6);
#ifdef VERSION_EUROPE
        TextBox_CreateFlagged(3, 254, 112, 204, 96, 16, 3);
#else
        TextBox_Create(3, 254, 112, 204, 96, 16);
#endif
#ifdef VERSION_EUROPE
        func_80039A60((struct DuelEffectChannel *)&gText_Channel3View);
#elif defined(VERSION_JAPAN)
        /* No glyph_advance_x/line_advance_y stores, and 0x60-byte channel records. */
        func_80039A60((struct DuelEffectChannel *)&(
            (NAME_ENTRY_DIALOG_CHANNEL_TYPE *)D_800EB0F8)[3]);
#else
        panel = D_800EB0F8;
        panel[3].glyph_advance_x = 16;
        panel[3].line_advance_y = 16;
        func_80039A60((struct DuelEffectChannel *)&panel[3]);
#endif
        NameEntry_AdjustLength(1, 6);
        return;
    }
    if (DisplayObject_FindAllocatedByTag(1) != 0) {
        return;
    }
    if (DisplayObject_FindAllocatedByTag(2) != 0) {
        return;
    }
    if ((gNameEntry_bStateFlags & 0x40) == 0) {
        NameEntry_UpdateKeyboard();
        return;
    }
    term = TEXT_STRING_TERMINATOR;
    p = gText_aCurrentPlayerNameGlyphs;
    c = *p;
    next = 0;
    goto ztest;
scan:
    if (*p >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
        p++;
    }
    next = p + 1;
    p = next;
    goto load;
zloop:
    p++;
load:
    c = *p;
ztest:
    if (c == 0) {
        goto zloop;
    }
    if (c != TEXT_STRING_TERMINATOR) {
        goto scan;
    }
    fg = gNameEntry_bStateFlags;
    gNameEntry_bStateFlags = fg & 0xBF;
    if (next != 0) {
        *next = term;
        gNameEntry_wPendingDialogId = 0x80F5;
        D_8016D41C = 0;
        gNameEntry_bStateFlags |= 0x20;
        SD_SEPlayFull(48);
        return;
    }
    SD_SEPlayFull(9);
}
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_PASSWORD_POLL_COMPLETION)
s32 NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return gNameEntry_bStateFlags & 0x10;
}
#endif
