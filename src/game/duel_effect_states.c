#include "../types.h"
#include "duel_effect_noop_handlers.h"

/* gGraphics_bSyncVBlankTarget is volatile here. reorg will not look past a volatile store
 * for a delay-slot candidate, so the DuelEffect_ClearResourceObjectPointers call cannot take its own
 * `li a0,3`. That goes to the entry branch instead, and the call is filled
 * forward with the slide_in constant, which otherwise sits with its first
 * use three calls further down. The opening and closing slides are separate
 * variables, which keeps slide_in local to the setup block and in $s0. */
#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define D_8009B0C0_IN_DATA_VOLATILE
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "text_box_blocking_build.h"
#include "duel_effect_state_latch.h"
#include "graphics_frame.h"
#include "input.h"
#include "fade.h"
#include "fade_overlay_controls.h"

#include "display_object_layout.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#define DUEL_CARD_VIEWER_ADDRESS_ALIASES
#include "duel_card_viewer.h"
#include "duel_effect_tables.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "display_object_core.h"
#include "display_object.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "card_preview_objects.h"
#include "duel_effect_resource_setup.h"
#include "display_object_interpolation.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "text_box_runtime.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "card_constants.h"
#include "duel_effect_card_viewer_state.h"
#define D_8009B26C_AS_SCALAR_DATA
#include "../unmatched.h"
#include "main_mode_state.h"
#ifdef VERSION_EUROPE
#include "text_staging.h"
#include "text_message_staging.h"
#endif

#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

#ifndef DUEL_CARD_VIEWER_CHANNEL_TYPE
#define DUEL_CARD_VIEWER_CHANNEL_TYPE DuelEffectChannel
#endif
#ifndef DUEL_CARD_VIEWER_SHOWN_X
#define DUEL_CARD_VIEWER_SHOWN_X 0x94
#endif

/* The European viewer's panel starts at (0x98, 0x108) and slides up to y 6
   and back down, where the US one slides in from x 0x148 at the card's
   y offset. Its card image comes from x 0x280 with row 0xDF, its channels
   are 100 bytes apart and its text box takes styles 0x60 and 0x61. */

void DuelEffect_UpdateCardViewerState(void)
{
    s32 slide_in;
    s32 slide_out;
    DisplayObject *obj;
    DisplayObject *bg;
    DuelEffectResourceRecord *p;
    DisplayObject *next_obj;
    s32 *stats;
    DuelEffectChannel *box;
    DUEL_CARD_VIEWER_CHANNEL_TYPE *chan;
    DuelEffectChannel *pos_box;
    DuelEffectChannel *dead_box;
    s32 adj;
#ifdef VERSION_EUROPE
    s32 card;
    s32 i2;
#endif
    s32 i;
    s32 kind;
    s32 masked;
    s32 speed;
    s32 test;
    u16 flags;
    u16 id;
    u16 objflags;
    u8 closing;
    u8 state;

    if (DuelEffect_MarkStateInitialized() == 0) {
        slide_in = -0x400;
        gGraphics_bSyncVBlankTarget = 1;
        DuelEffect_ClearResourceObjectPointers(3);
        p = &gCardPreview_Resources[0];
        p[3].src_y = 0x100;
        p[3].src_x = 0;
#ifdef VERSION_EUROPE
        p[3].field_2C = 0x280;
        p[3].field_2E = 0xDF;
#else
        p[3].field_2C = 0;
        p[3].field_2E = 0xFF;
#endif
        CardPreview_RequestResourceTransfer(3, (s16)gDuel_wViewerCardID);
        obj = (DisplayObject *)CardPreview_CreateResourceObjects(3, -1, -1);
#ifndef VERSION_EUROPE
        adj = gDuel_bCardViewerYOffset;
#endif
        *(s16 *)&obj->field_30.h.field_30 = -0x8C;
        obj->field_20.b.rotation_y = 0x80;
#ifndef VERSION_EUROPE
        obj->field_30.h.field_32 += adj;
#endif
        obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(obj));
        obj->field_60 = slide_in;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0x14);
        D_8009B24C = obj;
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
#ifdef VERSION_EUROPE
        DisplayObject_ConfigureSpriteAtPosition((u8 *)obj, 0x98, 0x108, 0, 2, 4, 0xD, 0x107);
#else
        DisplayObject_ConfigureSpriteAtPosition((u8 *)obj, 0x148, gDuel_bCardViewerYOffset + 0xE, 0, 2, 0, 0xD, 0x107);
#endif
        obj->field_60 = slide_in;
        obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0x14);
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(obj));
        obj->field_60 = slide_in;
        D_8009B240 = obj;
        D_8009B250 = 0;
        i = 0;
        stats = gDuel_adwCardStats;
#ifdef VERSION_EUROPE
        for (; i < 3; i++) {
            chan = (DUEL_CARD_VIEWER_CHANNEL_TYPE *)((u8 *)gText_aChannels + i * 100);
#else
        chan = (DUEL_CARD_VIEWER_CHANNEL_TYPE *)gText_aChannels;
        for (; i < 3; i++, chan++) {
#endif
            if ((chan->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE) == 0) {
#ifdef VERSION_EUROPE
                card = (s16)gDuel_wViewerCardID;
                gText_StagedValues[0].card.card_id = gDuel_wSelectedCardID = card;
                kind = 0x60;
                i2 = card - 1;
                if (((gDuel_adwCardStats[i2] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                    kind = 0x61;
                }
                box = TextBox_Create(i, kind, 0x98, 0x108, 0xA0, 0x100);
#else
                id = gDuel_wViewerCardID;
                gDuel_wSelectedCardID = id;
                kind = 3;
                if (((stats[(s16)id - 1] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                    kind = 4;
                }
#ifdef VERSION_JAPAN
                box = TextBox_Create(i, kind, 0x148, 0xE, 0xA0, 0xC0);
                box->color_index_54 = 4;
                box->glyph_delay = 1;
#else
                box = TextBox_Create(i, kind, 0x148, 0xE, 0xA8, 0xC0);
                box->glyph_delay = 1;
                box->color_index_54 = 0;
#endif
#endif
                box->depth_offset = 0x15;
                D_8009B250 = box;
                TextBox_BuildBlocking((struct DuelEffectChannel *)box);
                break;
            }
        }
        Fade_SetTargetLevel(0x40, 2);
        gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 0x13;
        SD_SEPlayFull(0x34);
        gDuel_bEffectHandlerFlags |= 0x40;
        return;
    }

    state = gDuel_bEffectHandlerFlags;
    if (state & 0x40) {
        obj = D_8009B240;
        speed = obj->field_60;
        if (speed != 0) {
            if (state & 0x10) {
#ifdef VERSION_EUROPE
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 *(s16 *)&obj->field_30.h.field_30,
                                 0x108,
                                 speed);
#else
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 0x148,
                                 *(s16 *)&obj->field_30.h.field_32,
                                 speed);
#endif
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
#ifdef VERSION_EUROPE
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 *(s16 *)&obj->field_30.h.field_30,
                                 6,
                                 speed);
#else
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 DUEL_CARD_VIEWER_SHOWN_X,
                                 *(s16 *)&obj->field_30.h.field_32,
                                 speed);
#endif
                flags = *(u16 *)&obj->field_60 + 0x55;
                obj->field_60 = flags;
                if ((s16)flags >= 0) {
#ifdef VERSION_EUROPE
                    *(s16 *)&obj->field_30.h.field_32 = 6;
#else
                    *(s16 *)&obj->field_30.h.field_30 = DUEL_CARD_VIEWER_SHOWN_X;
#endif
                    obj->field_60 = 0;
                }
            }
            pos_box = D_8009B250;
            if (pos_box != 0) {
                TextBox_SetPos(pos_box, *(s16 *)&obj->field_30.h.field_30,
                               *(s16 *)&obj->field_30.h.field_32);
            }
        }
        obj = D_8009B24C;
        speed = obj->field_60;
        if (speed != 0) {
            if (gDuel_bEffectHandlerFlags & 0x10) {
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 -0x8C,
                                 *(s16 *)&obj->field_30.h.field_32,
                                 speed);
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
                Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(obj),
                                 2,
                                 *(s16 *)&obj->field_30.h.field_32,
                                 speed);
                flags = *(u16 *)&obj->field_60 + 0x55;
                obj->field_60 = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 2;
                    obj->field_60 = 0;
                }
            }
        }
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) != 0) {
            return;
        }
        bg = D_8009B240;
        if (bg->field_60 != 0) {
            return;
        }
        if (D_8009B24C->field_60 != 0) {
            return;
        }
        closing = gDuel_bEffectHandlerFlags;
        masked = closing & 0xBF;
        test = closing & 0x10;
        gDuel_bEffectHandlerFlags = masked;
        if (test == 0) {
            return;
        }
        DisplayObject_ReleaseIfPresent(bg);
        DuelEffect_ReleaseResourceObjects(3);
        dead_box = D_8009B250;
        if (dead_box != 0) {
            TextBox_Destroy(dead_box);
        }
        gGraphics_bSyncVBlankTarget = 0;
        gDuel_bEffectState |= 0x40;
        return;
    }

    if ((state & 0x20) == 0) {
        obj = D_8009B24C;
        obj->field_20.b.rotation_y += 0xC;
        if (obj->field_20.b.rotation_y < 0x40) {
            objflags = obj->flags;
            obj->field_20.b.rotation_y = 0;
            state = gDuel_bEffectHandlerFlags | 0x20;
            obj->flags = objflags & 0xFFFB;
            gDuel_bEffectHandlerFlags = state;
        }
        return;
    }

#ifdef VERSION_JAPAN
    if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & PAD_BUTTON_CROSS) == 0) {
        return;
    }
#else
    if ((gMain_bActiveModeState & 0x1F) == 0xE) {
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0x20) != 0) {
            goto press;
        }
        return;
    }
    if ((gInput_wPad1Pressed & 0x20) == 0) {
        return;
    }
press:
#endif
    slide_out = 0x400;
    DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(D_8009B240));
    next_obj = D_8009B24C;
    D_8009B240->field_60 = slide_out;
    DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(next_obj));
    D_8009B24C->field_60 = slide_out;
    Fade_SetTargetLevel(0xFF, 2);
    SD_SEPlayFull(0x34);
    gDuel_bEffectHandlerFlags |= 0x50;
}

/* The duel-effect state dispatcher, the next function in the image. It
 * latches gDuel_bEffectState into gDuel_bActiveEffectState on first entry and
 * then runs gDuelEffect_apfnStateHandler[gDuel_bActiveEffectState] until the
 * handler raises DUEL_EFFECT_STATE_FLAG_COMPLETE. DuelEffect_UpdateCardViewerState
 * above is one of those handlers (duel_effect_tables.c). None of the addressing
 * selectors defined at the top of this unit names a symbol it uses. */
s32 DuelEffect_UpdateState(void) {
    u8 v = gDuel_bEffectState;
    if (v == 0) return 0;
    if ((v & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDuel_bActiveEffectState = v;
        gDuel_bEffectState = v | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gDuel_bEffectHandlerFlags = 0;
        return 1;
    }
    if (v & DUEL_EFFECT_STATE_FLAG_COMPLETE) {
        gDuel_bEffectState = 0;
        return 0;
    }
    gDuelEffect_apfnStateHandler[gDuel_bActiveEffectState]();
    return 1;
}

#ifdef VERSION_EUROPE
#include "../psyq/rand.h"
#include "duel_effect.h"
#include "duel_effect_state_latch.h"
#include "data_transfer_request.h"
#include "mem_card.h"
#include "save_data.h"
#endif

void func_800289AC(void)
{
}

#ifdef VERSION_EUROPE
/* European only: this last duel-effect state handler is not empty. Once,
   it stores (rand() << 8) + 1 as the save's duelist code, sets gSaveData_AllowIdentityBypass
   and requests a save write; then, while MemCardDialog_Poll returns
   nonzero, it sets bit 0x40 of gDuel_bEffectState. */
void DuelEffect_UpdateIdentitySave(void)
{
    if (DuelEffect_MarkStateInitialized() == 0) {
        ((SaveDataState *)gDuel_awPlayerDeck)->duelist_code = (rand() << 8) + 1;
        gSaveData_AllowIdentityBypass = 1;
        SaveData_RequestWrite();
    }
    if (MemCardDialog_Poll() != 0) {
        gDuel_bEffectState |= 0x40;
    }
}
#else
void DuelEffect_UpdateIdentitySave(void)
{
}
#endif
