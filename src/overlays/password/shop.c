#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/campaign_flags.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_config.h"
#include "../../game/display_object_helpers.h"
#include "../../game/file_transfer.h"
#include "../../game/duel_rewards.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/func_80039794.h"
#include "../../game/text_constants.h"
#include "../../game/text_box_runtime.h"
#include "shop.h"
#include "../../game/sound.h"
#include "../../game/fade.h"
#include "dialog_choice_ref.h"
#include "../../game/func_800291E0.h"
#include "../../game/func_80029574.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/save_data.h"

/* The password shop screen: its two resident entry points, the preview
   helper both of them call, and the password lookup the updater is the only
   caller of.

   Both doors are resident: main_run_frontend_menus.c calls
   Password_InitShopScreen once and Password_UpdateShopScreen each tick.
   Nothing outside this unit reaches Password_RecreateCardPreview or
   Password_LookupCardID.

   NameEntry_BuildStarterDeck, which follows this run in the image, is not
   part of it: its caller is name_entry_main.c, not the shop. */

extern s32 D_801A8008[];
extern u8 D_801B1245[];
extern u8 *D_8016D430;
extern u8 *D_8016D440[];
extern u8 D_800EA0E8[];

extern u16 D_8016D4DC;
extern u32 D_8016D438;
extern u32 D_801A8000[];
extern Pair D_801D5608;
extern s32 D_801D5608_starchips asm("D_801D5608");
extern volatile u16 D_8009B3A4;
extern u8 D_8009B269;
extern u8 D_8009B26C;

void Password_RefreshDigitDisplay(void)
{
    DuelEffectChannel *boxes;
    u8 *out;
    s32 i;
    s32 glyph;

    out = D_801B1245;
    for (i = 0; i < 8; i++) {
        glyph = D_800EAFF8[gPassword_abDigits[i]];
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
    func_8003B6AC(2, 1);
    TextBox_Create(2, 0xFD, 0xA8, 0x68, 0xA0, 0x10);
    boxes = D_800EB0F8;
    boxes[2].field_5A = 0x10;
    boxes[2].field_5B = 0x10;
    func_80039A14((u8 *)&boxes[2]);
}

void Password_RefreshStarchipDisplay(void)
{
    DuelEffectChannel *boxes;

    D_801D5608_starchips = gLibrary_dwStarchips;
    func_8003B6AC(3, 1);
    TextBox_Create(3, 0xE1, 0x98, 0x28, 0xA0, 0x20);
    boxes = D_800EB0F8;
    boxes[3].field_5A = 0x10;
    boxes[3].field_5B = 0x10;
    func_80039A14((u8 *)&boxes[3]);
}

DuelEffectChannel *Password_CreateMessageBox(int message_id, int flags)
{
    DuelEffectChannel *object;

    TextBox_Destroy(D_800EB0F8);
    object = TextBox_Create(0, message_id, 0x98, 0x98, 0xA0, 0x40);
    object->field_53 = 1;
    if (flags & 0xF) {
        func_80039A14((u8 *)object);
    }
    if (flags & 0x80) {
        object->flags_34 |= 8;
    }
    return object;
}

void Password_UpdateDigitCursor(u8 *object)
{
    s16 remaining;

    object[0x22] = object[0x22] + 1;
    if ((object[0x6C] & 0x40) != 0) {
        if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
            DisplayObject_ResetVelocity(object);
            *(s16 *)(object + 0x36) =
                ((*(s16 *)(object + 0x18) - *(s16 *)(object + 0x30)) << 8) /
                *(s16 *)(object + 0x60);
            *(s16 *)(object + 0x38) =
                ((*(s16 *)(object + 0x1A) - *(s16 *)(object + 0x32)) << 8) /
                *(s16 *)(object + 0x60);
        }
        DisplayObject_StepPositionXY(object);
        remaining = *(u16 *)(object + 0x60) - 1;
        *(s16 *)(object + 0x60) = remaining;
        if (remaining <= 0) {
            *(s32 *)(object + 0x30) = *(s32 *)(object + 0x18);
            object[0x6C] = object[0x6C] & 0x3F;
        }
    }
}

void Password_UpdateDigitCursorDecoration(u8 *object)
{
    PasswordCursorView *obj = (PasswordCursorView *)object;

    switch (obj->kind) {
    case 0:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex >= 7) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        obj->x = 0x129;
        obj->y = 0x68;
        break;
    case 1:
        obj->x = gPassword_pDigitCursorWidget->x + 5;
        obj->y = 0x78;
        break;
    case 2:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex <= 0) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        obj->x = 0xA9;
        obj->y = 0x68;
        break;
    case 3:
        obj->x = gPassword_pDigitCursorWidget->x + 5;
        obj->y = 0x58;
        break;
    }
}

void Password_SetDigitCursorTarget(u8 *a)
{
    PasswordCursorView *obj = (PasswordCursorView *)a;
    s32 value = gPassword_nDigitIndex;

    obj->target_y = 0x63;
    obj->target_x = value * 16 + 0xA3;
}

void Password_RecreateCardPreview(s32 ignored)
{
    PasswordCardPreviewView *obj;

    func_80029528(0);
    obj = (PasswordCardPreviewView *)func_800291E0(0, -1, -1);
    obj->y = 0x1E;
    obj->phase = 0x80;
    obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    D_8016D4D8 = obj;
}

void Password_InitShopScreen(void)
{
    s32 i;
    u8 *o;
    u8 *cardCache;
    u8 **slot;
    PasswordCursorUpdate hook;
    u8 *p;

    i = 7;
    p = gPassword_abDigits + i;
    gPassword_nDigitIndex = 0;
    D_8016D424 = 0;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    Password_RefreshDigitDisplay();
    Password_RefreshStarchipDisplay();
    Password_CreateMessageBox(226, 1);
    D_8016D430 = D_800EA0E8;
    func_80029574(0);
    cardCache = D_8016D430;
    *(s16 *)(cardCache + 40) = 320;
    *(s16 *)(cardCache + 42) = 256;
    *(s16 *)(cardCache + 44) = 512;
    *(s16 *)(cardCache + 46) = 240;
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 152, 40, 0, 2, 3, 31, 257);
    func_800428EC(o, -8);
    *(u16 *)(o + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    Password_RecreateCardPreview(1);
    o = func_800400AC(func_8004002C(), 1);
    func_80040510(o, 256, 120, 32, 32, 16, 128, 30, 256, 240);
    *(u32 *)(o + 4) &= ~GsROTOFF;
    *(s16 *)(o + 72) = 13;
    *(s16 *)(o + 74) = 13;
    *(u32 *)(o + 4) |= (GsALON | GsAONE);
    func_80042918(o);
    func_800428EC(o, 10);
    *(PasswordCursorUpdate *)(o + 36) = Password_UpdateDigitCursor;
    Password_SetDigitCursorTarget(o);
    hook = Password_UpdateDigitCursorDecoration;
    slot = D_8016D440;
    *(u32 *)(o + 48) = *(u32 *)(o + 24);
    gPassword_pDigitCursorWidget = (PasswordCursorView *)o;
    i = 0;
    do {
        o = func_800400AC(func_8004002C(), 2);
        func_80040468(o, 3, 1, i, 11, 524);
        *(u32 *)(o + 4) |= GsALON;
        func_80042918(o);
        func_800428EC(o, 8);
        *(PasswordCursorUpdate *)(o + 36) = hook;
        *(u16 *)(o + 8) |= 0x28;
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
    s32 *entry = D_801A8008;
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
    PasswordCursorView *cursor;
    PasswordCardPreviewView *widget;
    s32 index;
    s32 state;
    register SaveDataWorkspace *pool __asm__("$2");
    u32 count;
    s32 step;
    u16 flags2;
    u16 flags3;
    u16 flags;
    u16 flags4;
    u16 card;

    func_80039794();
    if ((gPassword_pDigitCursorWidget->updateFlags & 0x40) != 0) {
        return;
    }
    if ((D_800EB12C & 0x2008) != 0x2000) {
        return;
    }
    state = D_8016D424 & 0x1F;
    switch (state) {
    case 0:
        if ((D_8009B3A4 & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((D_8009B3A4 & PAD_DIRECTION_RIGHT) != 0) {
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
            Password_SetDigitCursorTarget((u8 *)cursor);
            cursor->timer = 8;
            cursor->updateFlags |= 0x40;
            return;
        }
        if ((gInput_wPad1Repeat & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            step = gPassword_abDigits[gPassword_nDigitIndex];
            if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
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
            SD_SEPlayFull(7);
            gPassword_abDigits[gPassword_nDigitIndex] = step;
            Password_RefreshDigitDisplay();
            return;
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            SD_SEPlayFull(8);
            SD_BGMFadeOut();
            Fade_WaitOut();
            D_8009B26C = D_8009B269;
            return;
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CROSS) != 0) {
            card = Password_LookupCardID();
            D_8016D4DC = card;
            if (card == 0) {
                SD_SEPlayFull(9);
                return;
            }
            D_8016D424 = 1;
            SD_SEPlayFull(48);
        }
        return;
    case 1:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            func_80029164(0, D_8016D4DC);
            return;
        }
        if ((flags & 0x4000) == 0) {
            if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 D_8009B134) != 0) {
                return;
            }
            D_8016D424 = flags | 0x4000;
            Password_RecreateCardPreview(D_8016D4DC);
            return;
        }
        D_8016D4D8->phase = D_8016D4D8->phase + 8;
        if (D_8016D4D8->phase == 0) {
            D_8016D4D8->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            SD_SEPlayFull(12);
            D_8016D424 = 2;
        } else {
            return;
        }
        /* fallthrough */
    case 2:
        flags2 = D_8016D424;
        if ((flags2 & 0x8000) == 0) {
            D_8016D424 = flags2 | 0x8000;
            D_801D5608.lo = D_801A8000[D_8016D4DC * 2];
            D_801D5608.hi = D_8016D4DC;
            if (Campaign_TestStoryFlag(
                    D_8016D4DC + CAMPAIGN_FLAG_PASSWORD_USED_BASE) != 0) {
                Password_CreateMessageBox(229, 128);
                return;
            }
            if (gLibrary_dwStarchips < D_801A8000[D_8016D4DC * 2]) {
                Password_CreateMessageBox(228, 0);
            } else {
                Password_CreateMessageBox(227, 0);
            }
            D_8016D424 |= 0x4000;
            return;
        }
        if ((flags2 & 0x4000) != 0) {
            D_8016D424 = flags2 & 0xBFFF;
            if (D_8009B34D == 0) {
                Library_UpdateCardUsedFlag(
                    D_8016D4DC + CAMPAIGN_FLAG_PASSWORD_USED_BASE);
                Duel_AwardCard(D_8016D4DC);
                D_8016D424 = 3;
                return;
            }
        }
        D_8016D424 = 4;
        return;
    case 3:
        flags3 = D_8016D424;
        if ((flags3 & 0x8000) == 0) {
            D_8016D424 = flags3 | 0x8000;
            card = D_8016D4DC;
            D_8016D438 = D_801A8000[card * 2];
        }
        count = D_8016D438;
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
        pool = (SaveDataWorkspace *)D_801D0000;
        count = count - step;
        D_8016D438 = count;
        pool->state.starchips = pool->state.starchips - step;
        if (count == 0) {
            D_8016D424 = 4;
        }
        Password_RefreshStarchipDisplay();
        return;
    case 4:
        flags4 = D_8016D424;
        if ((flags4 & 0x8000) == 0) {
            D_8016D424 = flags4 | 0x8000;
            D_8016D4D8->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        widget = D_8016D4D8;
        widget->phase = widget->phase + 8;
        if ((s8)D_8016D4D8->phase < 0) {
            Password_CreateMessageBox(226, 0);
            D_8016D424 = 0;
        }
        return;
    }
}
