#include "../../types.h"
#include "../../game/input.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "name_entry_keyboard.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/func_80039794.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/func_80043178.h"
#include "name_entry_state.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[44];
    s16 f96;
    u8 pad98[10];
    u8 f108;
} Caret;

extern s8 D_8009B34D;
extern u8 D_801B125A[];
extern DuelEffectChannel D_800EB1C0;
extern u8 D_8016D41C;

extern DuelEffectChannel *func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void *func_800374F4(DuelEffectChannel *);
extern void func_80043230(Caret *, s32, s32, s32);
extern void func_80035B7C(DuelEffectChannel *);
extern void func_80039934(DuelEffectChannel *, s32, s32);
extern void func_80015B00(void);
extern void func_8003BC40(u8 *, u8 *, s32);

void NameEntry_UpdateDialog(void)
{
    DuelEffectChannel *box;
    Caret *caret;
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

    if (D_8016D4D2 != 0) {
        flags = D_8016D400;
        if ((flags & 4) == 0) {
            D_8016D400 = flags | 4;
            func_8003B6AC(2, 2);
            box = func_80035BE4(2, D_8016D4D2 & 0xFFF, 16, 248,
                                 288, 48);
            box->field_59 = 20;
            id = D_8016D4D2;
            if ((id & 0x8000) == 0) {
                if ((id & 0x4000) == 0) {
                    func_80039A14((u8 *)box);
                    D_8009B34D = 0;
                }
                box->field_30 = func_800374F4(box);
            } else {
                box->flags_34 |= 8;
                do {
                    func_80039794();
                } while (box->field_30 == 0);
            }
            caret = func_800400AC(func_8004002C(), 2);
            func_800404CC(caret, 16, 248, 0, 0, 0, 23, 257);
            func_80042918(caret);
            func_800428EC(caret, 19);
            caret->f8 |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            box->field_2C = caret;
            func_80043178(caret);
            caret->f96 = -1024;
            D_8016D400 |= 2;
            return;
        }
        box = &D_800EB1C0;
        caret = (Caret *)box->field_2C;
        if ((flags & 2) != 0) {
            pos = caret->f96;
            if (pos >= 0) {
                caret->f96 = pos - 85;
                func_80043230(caret, 16, 248, (s16)(pos - 85));
                if (caret->f96 < 0) {
                    caret->f48 = 16;
                    fb = D_8016D400;
                    caret->f50 = 248;
                    D_8016D400 = fb & 0xF9;
                    func_80035B7C(box);
                    D_8016D4D2 = 0;
                    return;
                }
            } else {
                caret->f96 = pos + 85;
                func_80043230(caret, 16, 176, (s16)(pos + 85));
                if (caret->f96 >= 0) {
                    caret->f48 = 16;
                    fc = D_8016D400;
                    caret->f50 = 176;
                    D_8016D400 = fc & 0xFD;
                }
            }
            func_80039934(box, caret->f48, caret->f50);
            return;
        }
        if ((D_8016D4D2 & 0x8000) == 0) {
            if ((gInput_wPad1Pressed & (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                SD_SEPlayFull(11);
                return;
            }
        } else {
            func_80039794();
            if ((*(u32 *)&box->flags_34 & 0x2008) != 0x2000) {
                return;
            }
        }
        if (D_8009B34D != 0) {
            fd = D_8016D400;
            D_8016D400 = fd & 0xDF;
        }
        if ((D_8016D400 & 0x20) != 0) {
            D_8016D4D2 = 0;
        }
        func_80043178(caret);
        fe = D_8016D400;
        caret->f96 = 1024;
        D_8016D400 = fe | 2;
        return;
    }
    flags = D_8016D400;
    if ((flags & 0x20) != 0) {
        SD_SEPlayFull(45);
        SD_BGMFadeOut();
        func_80015B00();
        D_8016D400 = D_8016D400 | 0x10;
        return;
    }
    if ((flags & 0x80) != 0) {
        caret = (Caret *)func_80042B40(6);
        if (caret == 0) {
            return;
        }
        if ((caret->f108 & 0x40) == 0) {
            return;
        }
        func_8004036C(caret);
        ff = D_8016D400;
        D_8016D400 = ff & 0x7F;
        func_8003BC40(D_801B125A, D_8016D418, 6);
        func_80035BE4(3, 254, 112, 204, 96, 16);
        panel = D_800EB0F8;
        panel[3].field_5A = 16;
        panel[3].field_5B = 16;
        func_80039A60((u8 *)&panel[3]);
        NameEntry_AdjustLength(1, 6);
        return;
    }
    if (func_80042B40(1) != 0) {
        return;
    }
    if (func_80042B40(2) != 0) {
        return;
    }
    if ((D_8016D400 & 0x40) == 0) {
        NameEntry_UpdateKeyboard();
        return;
    }
    term = TEXT_STRING_TERMINATOR;
    p = D_801B125A;
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
    fg = D_8016D400;
    D_8016D400 = fg & 0xBF;
    if (next != 0) {
        *next = term;
        D_8016D4D2 = 0x80F5;
        D_8016D41C = 0;
        D_8016D400 |= 0x20;
        SD_SEPlayFull(48);
        return;
    }
    SD_SEPlayFull(9);
}
