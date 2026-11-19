#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/campaign_flags.h"
#include "../../game/display_object_system.h"
#include "../../game/display_object_layout.h"
#include "../../game/file_transfer.h"
#include "../../game/duel_scene.h"
#include "../../game/text_helpers.h"
#include "../../game/duel_effect_text.h"
#include "../../game/text_constants.h"
#include "../../game/text_staging.h"
#include "shop.h"
#include "../../game/sound.h"
#include "../../game/fade.h"
#include "../../game/card_preview.h"
#include "../../game/mem_card.h"
#include "../../game/main_mode_state.h"

/* The shop state, zero-initialized and common (this unit builds with
 * gcc_2_8_1_g0_split_comm); each layout's name_entry_runtime_commons
 * subsegment places it, interleaved with the name-entry state, at that
 * image's addresses. */
u8 gPassword_abDigits[8];
struct DisplayObject *gPassword_pDigitCursorWidget;
u16 gPassword_wShopState;
s32 gPassword_nDigitIndex;
struct DuelEffectResourceRecord *gPassword_pCardCache;
u32 gPassword_dwPriceRemaining;
struct DisplayObject *gPassword_apDigitCursorDecorations[4];
struct DisplayObject *gPassword_pCardPreview;
u16 gPassword_wCardId;

#ifdef VERSION_JAPAN
#define PASSWORD_SHOP_CANCEL_BUTTON PAD_BUTTON_CROSS
#define PASSWORD_SHOP_CONFIRM_BUTTON PAD_BUTTON_CIRCLE
#define PASSWORD_MESSAGE_BOX_WAIT_FLAG 8
#define PASSWORD_PREVIEW_Y 0x1E
#define PASSWORD_TEXT_BOX_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#define PASSWORD_CARD_CACHE_CLUT_X 512
#define PASSWORD_CURSOR_TEXTURE_PAGE 256
#define PASSWORD_CURSOR_TEXTURE_Y 240
#elif defined(VERSION_EUROPE)
#define PASSWORD_SHOP_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define PASSWORD_SHOP_CONFIRM_BUTTON PAD_BUTTON_CROSS
#define PASSWORD_MESSAGE_BOX_WAIT_FLAG 0x10
#define PASSWORD_PREVIEW_Y 0x14
#define PASSWORD_TEXT_BOX_COMPLETION_MASK 0x2010
#define PASSWORD_CARD_CACHE_CLUT_X 640
#define PASSWORD_CURSOR_TEXTURE_PAGE 640
#define PASSWORD_CURSOR_TEXTURE_Y 224
#else
#define PASSWORD_SHOP_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define PASSWORD_SHOP_CONFIRM_BUTTON PAD_BUTTON_CROSS
#define PASSWORD_MESSAGE_BOX_WAIT_FLAG 8
#define PASSWORD_PREVIEW_Y 0x1E
#define PASSWORD_TEXT_BOX_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#define PASSWORD_CARD_CACHE_CLUT_X 512
#define PASSWORD_CURSOR_TEXTURE_PAGE 256
#define PASSWORD_CURSOR_TEXTURE_Y 240
#endif

/* The password shop screen: its initialiser, the preview helper, the
   password lookup and the per-tick updater, Password_UpdateShopScreen, which
   is the preview helper's other caller and the lookup's only one.

   Both doors are resident: Main_RunPasswordMenu (main_mode_runners.c) calls
   Password_InitShopScreen once and Password_UpdateShopScreen each tick.
   Nothing outside this unit reaches Password_RecreateCardPreview or
   Password_LookupCardID.

   NameEntry_BuildStarterDeck, which follows this run in the image, is not
   part of it: its caller is name_entry_main.c, not the shop. */

void Password_RefreshDigitDisplay(void)
{
    DuelEffectChannel *boxes;
    u8 *out;
    s32 i;
    s32 glyph;

    out = gPassword_szDigits;
    for (i = 0; i < 8; i++) {
        glyph = gText_DecimalDigitGlyphMap[gPassword_abDigits[i]];
        if (glyph >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            /* The bare -0x10 is deliberate. `| TEXT_SINGLE_BYTE_GLYPH_LIMIT`
               agrees only after truncation to u8 and changes old-GCC code. */
            out[0] = (glyph >> 8) | -0x10;
            out[1] = glyph;
            out += 2;
        } else {
            out[0] = glyph;
            out += 1;
        }
    }
    *out = TEXT_STRING_TERMINATOR;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    DuelEffect_SetEntryRangeLayout(2, 1);
#endif
#ifdef VERSION_EUROPE
    TextBox_CreateFlagged(2, 0xFD, 0xA8, 0x68, 0xA0, 0x10, 3);
#else
    TextBox_Create(2, 0xFD, 0xA8, 0x68, 0xA0, 0x10);
#endif
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    TextBox_BuildBlocking(TEXT_CHANNEL(2));
#else
    boxes = gText_aChannels;
    boxes[2].glyph_advance_x = 0x10;
    boxes[2].line_advance_y = 0x10;
    TextBox_BuildBlocking(&boxes[2]);
#endif
}

void Password_RefreshStarchipDisplay(void)
{
    DuelEffectChannel *boxes;

    gText_StagedValues[0].starchips = gLibrary_dwStarchips;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    DuelEffect_SetEntryRangeLayout(3, 1);
#endif
    TextBox_Create(3, 0xE1, 0x98, 0x28, 0xA0, 0x20);
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    TextBox_BuildBlocking(TEXT_CHANNEL(3));
#else
    boxes = gText_aChannels;
    boxes[3].glyph_advance_x = 0x10;
    boxes[3].line_advance_y = 0x10;
    TextBox_BuildBlocking(&boxes[3]);
#endif
}

DuelEffectChannel *Password_CreateMessageBox(int message_id, int flags)
{
    DuelEffectChannel *object;

    TextBox_Destroy(gText_aChannels);
    object = TextBox_Create(0, message_id, 0x98, 0x98, 0xA0, 0x40);
    object->glyph_delay = 1;
    if (flags & 0xF) {
        TextBox_BuildBlocking(object);
    }
    if (flags & 0x80) {
        object->flags_34 |= PASSWORD_MESSAGE_BOX_WAIT_FLAG;
    }
    return object;
}

void Password_UpdateDigitCursor(DisplayObject *object)
{
    s16 remaining;

    object->field_20.b.rotation_z += 1;
    if ((object->update_state & 0x40) != 0) {
        if (DisplayObject_MarkInitialized(object) == 0) {
            DisplayObject_ResetVelocity(object);
            object->field_34.motion.velocity_x =
                (((s16)object->field_18.target.x - object->xy.s.x) << 8) / object->field_60;
            object->field_38.motion.velocity_y =
                (((s16)object->field_18.target.y - object->xy.s.y) << 8) / object->field_60;
        }
        DisplayObject_StepPositionXY(object);
        remaining = (u16)object->field_60 - 1;
        object->field_60 = remaining;
        if (remaining <= 0) {
            object->xy.word = object->field_18.word;
            object->update_state &= 0x3F;
        }
    }
}

void Password_UpdateDigitCursorDecoration(DisplayObject *obj)
{
    switch (obj->resource_variant) {
    case 0:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex >= 7) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        obj->xy.s.x = 0x129;
        obj->xy.s.y = 0x68;
        break;
    case 1:
        obj->xy.s.x = gPassword_pDigitCursorWidget->xy.s.x + 5;
        obj->xy.s.y = 0x78;
        break;
    case 2:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex <= 0) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        obj->xy.s.x = 0xA9;
        obj->xy.s.y = 0x68;
        break;
    case 3:
        obj->xy.s.x = gPassword_pDigitCursorWidget->xy.s.x + 5;
        obj->xy.s.y = 0x58;
        break;
    }
}

void Password_SetDigitCursorTarget(DisplayObject *obj)
{
    s32 value = gPassword_nDigitIndex;

    obj->field_18.target.y = 0x63;
    obj->field_18.target.x = value * 16 + 0xA3;
}

void Password_RecreateCardPreview(s32 ignored)
{
    DisplayObject *obj;

    DuelEffect_ReleaseResourceObjects(0);
    obj = CardPreview_CreateResourceObjects(0, -1, -1);
    obj->xy.h.y = PASSWORD_PREVIEW_Y;
    obj->field_20.b.rotation_y = 0x80;
    obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    gPassword_pCardPreview = obj;
}

void Password_InitShopScreen(void)
{
    s32 i;
    DisplayObject *o;
    DuelEffectResourceRecord *cardCache;
    DisplayObject **slot;
    DisplayObjectCallback hook;
    u8 *p;

    i = 7;
    p = gPassword_abDigits + i;
    gPassword_nDigitIndex = 0;
    gPassword_wShopState = 0;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    Password_RefreshDigitDisplay();
    Password_RefreshStarchipDisplay();
    Password_CreateMessageBox(226, 1);
    gPassword_pCardCache = gCardPreview_Resources;
    DuelEffect_ClearResourceObjectPointers(0);
    cardCache = gPassword_pCardCache;
    cardCache->src_x = 320;
    cardCache->src_y = 256;
    cardCache->clut_x = PASSWORD_CARD_CACHE_CLUT_X;
    cardCache->clut_y = 240;
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
#ifdef VERSION_JAPAN
    DisplayObject_ConfigureSpriteAtPosition(o, 152, 40, 0, 2, 0, 31, 257);
#else
    DisplayObject_ConfigureSpriteAtPosition(o, 152, 40, 0, 2, 3, 31, 257);
#endif
    DisplayObject_SetDepthOffset(o, -8);
    o->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    Password_RecreateCardPreview(1);
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    DisplayObject_ConfigureScreenSprite(
        o, 256, 120, 32, 32, 16, 128, 30,
        PASSWORD_CURSOR_TEXTURE_PAGE, PASSWORD_CURSOR_TEXTURE_Y
    );
    o->attribute &= ~GsROTOFF;
    o->field_48.pivot.x = 13;
    o->field_48.pivot.y = 13;
    o->attribute |= (GsALON | GsAONE);
    DisplayObject_SelectOrderingTable1(o);
    DisplayObject_SetDepthOffset(o, 10);
    o->update = Password_UpdateDigitCursor;
    Password_SetDigitCursorTarget(o);
    hook = Password_UpdateDigitCursorDecoration;
    slot = gPassword_apDigitCursorDecorations;
    /* A non-struct store, through the union's address: as o->xy.word
       GCC lets the gPassword_pDigitCursorWidget store pass it, and the
       target copies the position first. */
    *(u32 *)&o->xy = o->field_18.word;
    gPassword_pDigitCursorWidget = o;
    i = 0;
    do {
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteResource(o, 3, 1, i, 11, 524);
        o->attribute |= GsALON;
        DisplayObject_SelectOrderingTable1(o);
        DisplayObject_SetDepthOffset(o, 8);
        o->update = hook;
        o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        *slot = o;
        i++;
        slot++;
    } while (i < 4);
    SD_BGMPlay(29520);
    Fade_WaitIn();
}

s32 Password_LookupCardID(void)
{
    s32 packed = 0;
    s32 *entry = gPassword_aTable;
    s32 index;
    s32 i;

    for (i = 0; i < 8; i++) {
        packed <<= 4;
        packed |= gPassword_abDigits[i];
    }

    index = 1;
    for (;;) {
        if (entry[0] == -1) {
            return 0;
        }
        if (packed == entry[1]) {
            return index;
        }
        entry += 2;
        index++;
    }
}

void Password_UpdateShopScreen(void)
{
    DisplayObject *cursor;
    DisplayObject *widget;
    s32 index;
    s32 state;
    SaveDataWorkspace *pool;
    u32 count;
    s32 step;
    u16 flags2;
    u16 flags3;
    u16 flags;
    u16 flags4;
    u16 card;

    TextBox_UpdateChannels();
    if ((gPassword_pDigitCursorWidget->update_state & 0x40) != 0) {
        return;
    }
    /* One word load covers flags_34 and initial_string_id. */
    if ((*(u32 *)&gText_aChannels[0].flags_34 & PASSWORD_TEXT_BOX_COMPLETION_MASK) !=
        TEXT_BOX_FLAG_DONE) {
        return;
    }
    state = gPassword_wShopState & 0x1F;
    switch (state) {
    case 0:
        if ((gInput_awPadHeld[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_awPadHeld[0] & PAD_DIRECTION_RIGHT) != 0) {
                index = gPassword_nDigitIndex + 1;
                gPassword_nDigitIndex = index;
                if (index >= 8) {
                    gPassword_nDigitIndex = 7;
                    return;
                }
            } else {
                index = gPassword_nDigitIndex - 1;
                gPassword_nDigitIndex = index;
                if (index < 0) {
                    gPassword_nDigitIndex = 0;
                    return;
                }
            }
            SD_SEPlayFull(47);
            cursor = gPassword_pDigitCursorWidget;
            Password_SetDigitCursorTarget(cursor);
            cursor->field_60 = 8;
            cursor->update_state |= 0x40;
            return;
        }
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            step = gPassword_abDigits[gPassword_nDigitIndex];
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0) {
                step = step + 1;
                if (step >= 10) {
                    step = 0;
                }
            } else {
                step = step - 1;
                if (step < 0) {
                    step = 9;
                }
            }
            SD_SEPlayFull(SD_SE_DECIDE);
            gPassword_abDigits[gPassword_nDigitIndex] = step;
            Password_RefreshDigitDisplay();
            return;
        }
        if ((gInput_awPadPressed[0] & PASSWORD_SHOP_CANCEL_BUTTON) != 0) {
            SD_SEPlayFull(SD_SE_CANCEL);
            SD_BGMFadeOut();
            Fade_WaitOut();
            gMain_bActiveModeState = gMain_bReturnMode;
            return;
        }
        if ((gInput_awPadPressed[0] & PASSWORD_SHOP_CONFIRM_BUTTON) != 0) {
            card = Password_LookupCardID();
            gPassword_wCardId = card;
            if (card == 0) {
                SD_SEPlayFull(SD_SE_BUZZER);
                return;
            }
            gPassword_wShopState = 1;
            SD_SEPlayFull(48);
        }
        return;
    case 1:
        flags = gPassword_wShopState;
        if ((flags & 0x8000) == 0) {
            gPassword_wShopState = flags | 0x8000;
            CardPreview_RequestResourceTransfer(0, gPassword_wCardId);
            return;
        }
        if ((flags & 0x4000) == 0) {
            if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 gFile_dwCancelRequestState) != 0) {
                return;
            }
            gPassword_wShopState = flags | 0x4000;
            Password_RecreateCardPreview(gPassword_wCardId);
            return;
        }
        gPassword_pCardPreview->field_20.b.rotation_y =
            gPassword_pCardPreview->field_20.b.rotation_y + 8;
        if (gPassword_pCardPreview->field_20.b.rotation_y == 0) {
            gPassword_pCardPreview->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            SD_SEPlayFull(12);
            gPassword_wShopState = 2;
        } else {
            return;
        }
        /* fallthrough */
    case 2:
        flags2 = gPassword_wShopState;
        if ((flags2 & 0x8000) == 0) {
            gPassword_wShopState = flags2 | 0x8000;
            /* The shared staging buffer is raw bytes; this screen loads the
               price table into it. */
            gText_StagedValues[0].pair.lo = ((PasswordShopCard *)gFile_StagingBuffer)[gPassword_wCardId].price;
            gText_StagedValues[0].pair.hi = gPassword_wCardId;
            if (Campaign_TestStoryFlag(
                    gPassword_wCardId + CAMPAIGN_FLAG_PASSWORD_USED_BASE) != 0) {
                Password_CreateMessageBox(229, 128);
                return;
            }
            if (gLibrary_dwStarchips < ((PasswordShopCard *)gFile_StagingBuffer)[gPassword_wCardId].price) {
                Password_CreateMessageBox(228, 0);
            } else {
                Password_CreateMessageBox(227, 0);
            }
            gPassword_wShopState |= 0x4000;
            return;
        }
        if ((flags2 & 0x4000) != 0) {
            gPassword_wShopState = flags2 & 0xBFFF;
            if (gDialog_bChoice == 0) {
                Library_UpdateCardUsedFlag(
                    gPassword_wCardId + CAMPAIGN_FLAG_PASSWORD_USED_BASE);
                Duel_AwardCard(gPassword_wCardId);
                gPassword_wShopState = 3;
                return;
            }
        }
        gPassword_wShopState = 4;
        return;
    case 3:
        flags3 = gPassword_wShopState;
        if ((flags3 & 0x8000) == 0) {
            gPassword_wShopState = flags3 | 0x8000;
            card = gPassword_wCardId;
            gPassword_dwPriceRemaining = ((PasswordShopCard *)gFile_StagingBuffer)[card].price;
        }
        count = gPassword_dwPriceRemaining;
        step = 1;
        if (count >= 10) {
            step = count / 10;
        }
        if (count >= 100) {
            step = count / 20;
        }
        if (count >= 1000) {
            step = count / 30;
        }
        if (count >= 10000) {
            step = count / 40;
        }
        if (step == 0) {
            step = 1;
        }
        pool = &gSaveData_Workspace;
        gPassword_dwPriceRemaining -= step;
        pool->state.starchips -= step;
        if (gPassword_dwPriceRemaining == 0) {
            gPassword_wShopState = 4;
        }
        Password_RefreshStarchipDisplay();
        return;
    case 4:
        flags4 = gPassword_wShopState;
        if ((flags4 & 0x8000) == 0) {
            gPassword_wShopState = flags4 | 0x8000;
            gPassword_pCardPreview->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        widget = gPassword_pCardPreview;
        widget->field_20.b.rotation_y = widget->field_20.b.rotation_y + 8;
        if ((s8)gPassword_pCardPreview->field_20.b.rotation_y < 0) {
            Password_CreateMessageBox(226, 0);
            gPassword_wShopState = 0;
        }
        return;
    }
}
