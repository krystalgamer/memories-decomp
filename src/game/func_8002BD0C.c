#define D_8009B118_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_80058DD8.h"
#include "camera_view.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "library_runtime.h"
#include "model_copy_slot_u16_values.h"
#include "model_update_view_metrics.h"
#include "sound_voice_data.h"
#include "view_state.h"
#include "../psyq/rand.h"
#include "model_slot_queries.h"
#include "../unmatched.h"
#include "func_80057AF4.h"
#include "func_8002A788.h"
#include "save_data.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_api.h"
#include "card_grid.h"
#include "display_object_layout.h"
#include "func_8003B6AC.h"
#include "main_services.h"
#include "text_box_lifecycle.h"
#include "func_8002A2F4.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "func_80029574.h"
#include "func_80029590.h"
#include "sound.h"
#include "text_render_state.h"
#include "text_staging.h"

/* The Library screen, in address order: the package-transfer phase
   callback, the pass that marks every owned card in the screen's state, and
   func_8002BFCC, the entry that sets the screen up, installs that callback
   and hands control to the dispatcher. The per-frame dispatcher just before
   this run, func_8002BAB4, is now a candidate in
   src/candidates/func_8002BAB4.c; the two trivial state handlers it calls
   for states 0 and 3 are in library_runtime.c.

   The owned-card pass was recorded at gcc_2_8_1_g0_split. It compiles to an
   identical object at gcc_2_8_1_g8_split, which the entry needs, so the
   unit builds there. */

#define gStageRect (D_800E9D70[0])

void func_8002BD0C(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x20000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->mode = 0x2000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x18000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 4:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->done = 3;
        object->field_30.word = 0x26810;
        object->mode = 0xA000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 3:
    case 5:
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 6:
        func_80048D08(1, (u32 *)D_8009B118);
        break;
    }
}

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + (CAMPAIGN_FLAG_LIBRARY_CARD_BASE + CARD_ID_FIRST));
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + CAMPAIGN_FLAG_LIBRARY_CARD_BASE);
        i++;
        q++;
    } while (i < DECK_SIZE);
}

extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data

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
    D_800E9DB0[3] = func_80029EC4;
    func_80029590();
    r = D_800EA1E8;
    D_800EA1E8[0] = 0;
    c = gCardGrid_bCursorColumn;
    d = c;
    if (c >= CARD_GRID_SECTION_SIDE_LENGTH) {
        x = (c % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xAE;
    } else {
        x = (d % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xE;
    }
    *(s16 *)(r + 0x12) = x;
    *(s16 *)(r + 8) = x;
    k = gCardGrid_bCursorRow;
    y = (k / CARD_GRID_SECTION_SIDE_LENGTH) * 0xB2
        + (k % CARD_GRID_SECTION_SIDE_LENGTH) * 0x10 + 0xE;
    *(s16 *)(r + 0x14) = y;
    *(s16 *)(r + 0xA) = y;
    func_8002A660(r);
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 0x10, 0xD8, 0, 2, 1, 0x1B, 0x127);
    o[0x5F] = 0x80;
    func_80042918((DisplayObject *)o);
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
    D_801D5608[0].library_count = 0;
    n = CARD_ID_FIRST;
    do {
        if (Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0) {
            D_801D5608[0].library_count += 1;
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
