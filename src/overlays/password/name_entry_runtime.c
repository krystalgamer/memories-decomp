#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/input.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/func_80039794.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/func_80043178.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "name_entry_keyboard.h"
#include "name_entry_state.h"

/* The dialog's own sliding panel object, reached through the text box at
 * field_2C or by tag from func_80042B40. A different object from the
 * selection frame name_entry_state.h declares, and the fields are spelled
 * by hex offset here so that the two views cannot be read as if they shared
 * a numbering: this unit's +0x30 is the old Caret's `f48`. */
typedef struct {
    u8 pad_00[0x8];
    u16 flags;       /* 0x08 */
    u8 pad_0A[0x26];
    s16 x;           /* 0x30 */
    s16 y;           /* 0x32 */
    u8 pad_34[0x2C];
    s16 slide;       /* 0x60, signed distance still to travel */
    u8 pad_62[0xA];
    u8 status;       /* 0x6C */
} DialogCaret;

extern u16 D_8016D4D4;
extern s8 D_8016AB38[][15];
extern u8 D_8016ABC0[][2];

extern s8 D_8009B34D;
extern DuelEffectChannel D_800EB1C0;
extern u8 D_8016D41C;

extern void *func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void *func_800374F4(DuelEffectChannel *);
extern void func_80043230(DialogCaret *, s32, s32, s32);
extern void func_80035B7C(DuelEffectChannel *);
extern void func_80039934(DuelEffectChannel *, s32, s32);
extern void func_80015B00(void);
extern void func_8003BC40(u8 *, u8 *, s32);

s32 NameEntry_AdjustLength(s32 delta, s32 arg)
{
    u8 *object;

    if (delta < 0) {
        if (D_8016D42C == 0) {
            return 0;
        }
    } else {
        if (D_8016D42C == 5) {
            return 0;
        }
    }
    D_8016D42C += delta;
    object = D_8016D43C;
    *(s16 *)(object + 0x44) = D_8016D42C * 16 + 0x6B;
    *(void **)(object + 0x24) = NameEntry_UpdateCaretTween;
    object[0x6C] = 2;
    *(s16 *)(object + 0x60) = arg;
    *(s16 *)(object + 0x46) = *(u16 *)(object + 0x32);
    SD_SEPlayFull(0xC);
    return 1;
}

void NameEntry_UpdateKeyboard(void)
{
    SelectionFrame *w;
    s32 work;
    register s8 *glyphTable __asm__("$4");
    s32 glyphRow;
    s32 glyphCol;
    register s32 glyphIndex __asm__("$2");
    s32 glyphCode;
    s32 home;
    s32 n;
    register s32 walkCol __asm__("$3");
    s32 col;
    s32 row;
    DuelEffectEntry *node;
    u8 *obj;
    s32 kind;
    s32 gx;
    s32 gy;
    s32 d;
    /* Keep this coordinate pair in a0/a1, leaving the global's high half in a2. */
    register union {
        u64 all;
        struct {
            s32 col;
            s32 stride;
        } parts;
    } coords __asm__("$4");

    w = D_8016D404;
    if ((D_8016D4D4 & 0x4000) != 0) {
        home = w->width - 16;
        work = w->widthBonus - home;
        if (work != 0) {
            w->width = (work >= 0) ? (w->width + 2) : (w->width - 2);
        }
        DisplayObject_StepPositionXY(w);
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
    coords.parts.col = (s8)D_8016D401;
    coords.parts.stride = row * 15;
    col = coords.parts.col;
    work = *(s8 *)((coords.parts.col + coords.parts.stride) + (s32)obj);
    walkCol = col;
    while (work < 0) {
        walkCol = walkCol + work;
        D_8016D401 = walkCol;
        work = *(s8 *)(((s8)walkCol + coords.parts.stride) + (s32)obj);
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
    DisplayObject_ResetVelocity(w);
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
    glyphIndex = glyphRow * 15;
    glyphIndex = glyphCol + glyphIndex;
    glyphCode = glyphTable[glyphIndex] & 0xF;
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
    obj[0x6C] = 1;
    *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        *(u16 *)(obj + 8) &= 0xFFBF;
    }
    if (work != 0) {
        *(s16 *)(obj + 0x48) = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = NameEntry_SpawnGlyphSprite(kind, node);
        obj[0x6C] = 1;
        *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphPulse;
        *(s16 *)(obj + 0x48) = 0;
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
        *(s16 *)(obj + 0x60) = 8;
        *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphTransfer;
        *(s16 *)(obj + 0x46) = 204;
        *(s16 *)(obj + 0x44) = D_8016D42C * 16 + 112;
        obj[0x6C] = 6;
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
            caret->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            box->field_2C = caret;
            func_80043178(caret);
            caret->slide = -1024;
            D_8016D400 |= 2;
            return;
        }
        box = &D_800EB1C0;
        caret = (DialogCaret *)box->field_2C;
        if ((flags & 2) != 0) {
            pos = caret->slide;
            if (pos >= 0) {
                caret->slide = pos - 85;
                func_80043230(caret, 16, 248, (s16)(pos - 85));
                if (caret->slide < 0) {
                    caret->x = 16;
                    fb = D_8016D400;
                    caret->y = 248;
                    D_8016D400 = fb & 0xF9;
                    func_80035B7C(box);
                    D_8016D4D2 = 0;
                    return;
                }
            } else {
                caret->slide = pos + 85;
                func_80043230(caret, 16, 176, (s16)(pos + 85));
                if (caret->slide >= 0) {
                    caret->x = 16;
                    fc = D_8016D400;
                    caret->y = 176;
                    D_8016D400 = fc & 0xFD;
                }
            }
            func_80039934(box, caret->x, caret->y);
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
        caret->slide = 1024;
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
        caret = (DialogCaret *)func_80042B40(6);
        if (caret == 0) {
            return;
        }
        if ((caret->status & 0x40) == 0) {
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

s32 NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return D_8016D400 & 0x10;
}
