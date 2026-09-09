#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../types.h"
#include "func_80033500.h"
#include "func_80032B38.h"
#include "card_list_sort.h"
#include "card_list_text_boxes.h"
#include "card_constants.h"
#include "input.h"
#include "sound.h"
#include "build_deck_card_counts.h"

extern u8 D_8009B24B;
extern u16 gDuel_wViewerCardID;
extern u8 D_8009B254;

extern s32 func_800330BC(CardList *);
extern void BuildDeck_AddCard(u8 *, s32);
extern void func_80031574(s32, s32, s32, s32, s32);
extern void func_80031E5C(u8 *);
extern void func_80031EE4(u8 *, s32);
void func_8003353C(u8 *p) {
    u8 *e;
    s32 r;

    e = p + (p[0x6342] * 0x2D4C + 4);

    func_80032B38(p);

    if (func_800330BC((CardList *)e) != 0) {
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_TRIANGLE) != 0) {
        r = func_80033500((CardList *)e);
        if (r != 0) {
            D_8009B24B = 0x14;
            gDuel_wViewerCardID = r;
            D_8009B254 = 2;
        }
        return;
    }

    if (gInput_wPad1Held == PAD_DIRECTION_LEFT) {
        *(s16 *)(p + 0x633E) = 1;
        *(s32 *)(p + 0x5AA4) = 0;
        *(s16 *)(p + 0x6340) = 2;
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        *(s16 *)(p + 0x633E) = 4;
        *(s16 *)(p + 0x6340) = 3;
        return;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) != 0) {
        r = func_80033500((CardList *)e);
        if (r != 0) {
            SD_SEPlayFull(7);
            *(e + 0xD - -((*(s16 *)(e + 0x2D3C) +
                             *(s8 *)(e + 0x2D48)) * 0x10)) = 0;
            func_80032C48((CardList *)(p + 0x2D50));
            func_8003201C(p);
            func_80031EE4(p, r);
            func_80031E5C(p);
            func_80031574(r, 0x234, 0x16, 0x162, 0xA);
            return;
        }
        SD_SEPlayFull(9);
    }
}

void func_800336F0(u8 *p)
{
    u8 *e;
    u8 *q;
    s32 r;
    u32 c;

    e = p + (p[0x6342] * 0x2D4C + 4);
    func_80032B38(p);
    if (func_800330BC((CardList *)e) != 0) {
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_TRIANGLE) != 0) {
        r = func_80033500((CardList *)e);
        if (r != 0) {
            D_8009B24B = 0x14;
            gDuel_wViewerCardID = r;
            D_8009B254 = 2;
        }
        return;
    }

    if (gInput_wPad1Held == PAD_DIRECTION_RIGHT) {
        *(s32 *)(p + 0x5AA4) = 0x140;
        *(s16 *)(p + 0x633E) = 1;
        *(s16 *)(p + 0x6340) = 3;
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        *(s16 *)(p + 0x633E) = 4;
        *(s16 *)(p + 0x6340) = 2;
        return;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) == 0) {
        return;
    }

    r = func_80033500((CardList *)e);
    c = 1;
    if ((u32)(r - EXODIA_FIRST_CARD_ID) < EXODIA_PIECE_COUNT) {
        c = (p + r)[0x5AC4] < c;
    }

    if (r != 0 && c != 0) {
        q = p + r;
        if (*(s32 *)(p + 0x5AA0) < DECK_SIZE &&
            q[0x5D97] != 0 &&
            q[0x5AC4] < DECK_CARD_COPY_LIMIT) {
            SD_SEPlayFull(7);
            BuildDeck_AddCard(p, r);
            func_80031F7C(p, r);
            func_80031E5C(p);
            func_80031574(r, 3, 0x18, 0x11C, 0xC);
            return;
        }
    }

    SD_SEPlayFull(9);
}
