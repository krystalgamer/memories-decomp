/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0_split this source
 * rebuilt the target byte for byte, but only by pinning 1 variable to hard
 * registers, so it is kept here as a candidate rather than counted as a
 * decompilation. It was src/overlays/password/shop.c.
 */
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define TEXT_STAGING_STARCHIPS_ALIAS
#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#define GINPUT_PAD1_HELD_IS_VOLATILE
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
#include "../../game/text_staging.h"
#include "../../overlays/password/shop.h"
#include "../../game/sound.h"
#include "../../game/fade.h"
#include "../../overlays/password/dialog_choice_ref.h"
#include "../../game/func_800291E0.h"
#include "../../game/func_80029574.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/save_data.h"
#include "../../unmatched.h"

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
extern u8 D_8009B269;
extern u8 D_8009B26C;

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
        if ((gInput_wPad1Held & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
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
            D_801D5608[0].pair.lo = D_801A8000[D_8016D4DC * 2];
            D_801D5608[0].pair.hi = D_8016D4DC;
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
