#include "../types.h"
#include "func_8002BFCC.h"
#include "display_object_api.h"
#include "card_constants.h"
#include "card_grid.h"
#include "campaign_flags.h"
#include "display_object_layout.h"
#include "file_transfer.h"
#include "func_8003B6AC.h"
#include "graphics_frame.h"
#include "text_box_lifecycle.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "../unmatched.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "func_80029574.h"
#include "func_80029590.h"
#include "func_8002BD0C.h"
#include "library_mark_owned_cards.h"
#include "sound.h"
#include "text_render_state.h"

extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data
extern void (*D_800E9DBC[])(void);
extern u8 D_800EA1E8[];
extern s32 D_801D5608[];

void func_8002BFCC(void) {
    s16 *q;
    DuelEffectResourceRecord *b;
    u8 *r;
    u8 *o;
    u8 *e;
    u8 *m;
    s32 n;
    s32 x;
    s32 y;
    s32 v;
    s32 c;
    s32 d;
    s32 k;
    s32 off;
    s32 rb;

    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    func_80035668(0);
    func_80029574(0);
    n = CARD_COUNT - 1;
    q = (s16 *)0x801805A2;
    b = &D_800EA0E8[0];
    b->src_x = 0x100;
    b->src_y = 0x100;
    b->field_2C = 0x200;
    b->field_2E = 0xF0;
    do {
        *q = n + 1;
        n--;
        q--;
    } while (n >= 0);
    File_RequestAsyncTransfer(0, (u8 *)0, 0x1DCD, 0x8A,
                              (FileTransferCallback)func_8002BD0C, 0, 0);
    File_WaitForTransfers();
    Library_MarkOwnedCards();
    D_800E9DBC[0] = func_80029EC4;
    func_80029590();
    r = D_800EA1E8;
    D_800EA1E8[0] = 0;
    c = gCardGrid_bCursorColumn;
    d = c;
    if (c >= 0xA) {
        x = (c % 10) * 0xE + 0xAE;
    } else {
        x = (d % 10) * 0xE + 0xE;
    }
    *(s16 *)(r + 0x12) = x;
    *(s16 *)(r + 8) = x;
    k = gCardGrid_bCursorRow;
    y = (k / 10) * 0xB2 + (k % 10) * 0x10 + 0xE;
    *(s16 *)(r + 0x14) = y;
    *(s16 *)(r + 0xA) = y;
    func_8002A660(r);
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 0x10, 0xD8, 0, 2, 1, 0x1B, 0x127);
    o[0x5F] = 0x80;
    func_80042918(o);
    n = CARD_ID_FIRST;
    *(u16 *)(o + 8) =
        *(u16 *)(o + 8) | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    *(u8 **)(r + 0x48) = o;
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, *(s16 *)(r + 8), *(s16 *)(r + 0xA), 0, 2, 2, 0x1B, 0x147);
    rb = (s32)r;
    o[0x5F] = 0x80;
    *(u8 **)(r + 0x44) = o;
    r[0x56] = 0;
    *(s16 *)(r + 0x54) = 0;
    do {
        *(u8 *)(rb + n * 4 + 0x56) = 0;
        *(s16 *)(r + 0x54) = 0;
        off = n * 4 - 4;
        v = (*(s32 *)((u8 *)gDuel_adwCardStats + off) >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
        switch (v) {
        case CARD_TYPE_MAGIC:
        case CARD_TYPE_EQUIP:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_MAGIC_EQUIP;
            break;
        case CARD_TYPE_TRAP:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_TRAP;
            break;
        case CARD_TYPE_RITUAL:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_RITUAL;
            break;
        default:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_DEFAULT;
            break;
        }
        n++;
    } while (n < CARD_ID_END);
    D_801D5608[0] = 0;
    n = CARD_ID_FIRST;
    do {
        if (Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0) {
            D_801D5608[0] += 1;
            *(u8 *)(r + n * 4 + 0x56) = 0x80;
            if (func_8002C518(n) < 0) {
                *(u8 *)(r + n * 4 + 0x56) |= 1;
            }
        }
        n++;
    } while (n < CARD_ID_END);
    func_8003B6AC(3, 1);
    m = TextBox_Create(3, 0xF8, 0x58, -0x18, 0x90, 0x10);
    m[0x5A] = 0x10;
    m[0x5B] = 0x10;
    func_80039A14(m);
    func_8004293C(*(DisplayObject **)(m + 0x28));
    *(u16 *)(*(u8 **)(m + 0x28) + 8) &=
        ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8002A2F4(r);
    SD_BGMPlay(0x72D0);
}
