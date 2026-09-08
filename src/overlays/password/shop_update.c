#include "../../types.h"
#include "../../game/campaign_flags.h"
#include "../../game/display_object_layout.h"
#include "shop.h"
#include "../../game/sound.h"

typedef struct {
    u8 pad0[96];
    s16 timer;
    u8 pad98[10];
    u8 updateFlags;
} Cursor;

typedef struct {
    u32 lo;
    u32 hi;
} Pair;

extern u16 D_8016D424;
extern u32 D_800EB12C;
extern u16 D_8016D4DC;
extern s32 gPassword_nDigitIndex;
extern Cursor *gPassword_pDigitCursorWidget;
extern u8 gPassword_abDigits[];
extern u32 D_8016D438;
extern u32 D_801A8000[];
extern Pair D_801D5608;
extern u32 D_801D07E0;
extern u32 D_801D0000[];
extern volatile u16 D_8009B394;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;
extern u32 D_8009B0F4;
extern u32 D_8009B134;
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern s8 D_8009B34D;

extern void func_80039794(void);
extern void Password_SetDigitCursorTarget(Cursor *);
extern void Password_RefreshDigitDisplay(void);
extern void SD_BGMFadeOut(void);
extern void Fade_WaitOut(void);
extern s32 Password_LookupCardID(void);
extern void func_80029164(s32, s32);
extern s32 Campaign_TestStoryFlag(s32);
extern u8 *Password_CreateMessageBox(s32, s32);
extern void Duel_AwardCard(s32);
extern void Password_RefreshStarchipDisplay(void);

void Password_UpdateShopScreen(void)
{
    Cursor *cursor;
    PasswordCardPreviewView *widget;
    s32 index;
    s32 state;
    register u32 *pool __asm__("$2");
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
        if ((D_8009B3A4 & 0xA000) != 0) {
            if ((D_8009B3A4 & 0x2000) != 0) {
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
            cursor->timer = 8;
            cursor->updateFlags |= 0x40;
            return;
        }
        if ((D_8009B394 & 0x5000) != 0) {
            step = gPassword_abDigits[gPassword_nDigitIndex];
            if ((D_8009B394 & 0x1000) != 0) {
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
        if ((D_8009B398 & 0x20) != 0) {
            SD_SEPlayFull(8);
            SD_BGMFadeOut();
            Fade_WaitOut();
            D_8009B26C = D_8009B269;
            return;
        }
        if ((D_8009B398 & 0x40) != 0) {
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
            if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
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
            if (D_801D07E0 < D_801A8000[D_8016D4DC * 2]) {
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
        pool = D_801D0000;
        count = count - step;
        D_8016D438 = count;
        pool[504] = pool[504] - step;
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
