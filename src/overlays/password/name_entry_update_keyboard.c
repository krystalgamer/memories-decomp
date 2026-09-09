#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/input.h"
#include "name_entry_keyboard.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "name_entry_state.h"

typedef struct {
    u8 pad0[48];
    s16 f30;
    s16 f32;
    u8 pad34[2];
    s16 f36;
    s16 f38;
    u8 pad3A[2];
    u16 f3C;
    u8 pad3E[32];
    u8 f5E;
    u8 pad5F;
    s16 f60;
} W;

extern u16 D_8016D4D4;
extern W *D_8016D404;
extern s8 D_8016D401;
extern s8 D_8016D42C;
extern u16 *D_8016D418;
extern s8 D_8016AB38[][15];
extern u8 D_8016ABC0[][2];

void NameEntry_UpdateKeyboard(void)
{
    W *w;
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
        home = w->f3C - 16;
        work = w->f5E - home;
        if (work != 0) {
            w->f3C = (work >= 0) ? (w->f3C + 2) : (w->f3C - 2);
        }
        DisplayObject_StepPositionXY(w);
        w->f60 = w->f60 - 1;
        if (w->f60 != 0) {
            return;
        }
        w->f3C = w->f5E + 16;
        w->f30 = D_8016D434;
        w->f32 = D_8016D436;
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
    w->f5E = 0;
    if (work != 0) {
        if ((work & 0x40) != 0) {
            w->f5E = 20;
        }
        D_8016D402 = work & 0xF;
    }
    D_8016D434 = (s8)D_8016D401 * 20 + 22;
    if ((s8)D_8016D401 >= 11) {
        D_8016D434 = (s8)D_8016D401 * 20 + 42;
    }
    D_8016D436 = (s8)D_8016D402 * 18 + 24;
    DisplayObject_ResetVelocity(w);
    w->f60 = 8;
    w->f36 = ((D_8016D434 - w->f30) << 8) / 8;
    w->f38 = ((D_8016D436 - w->f32) << 8) / 8;
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
