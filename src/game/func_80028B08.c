#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "display_object_projection.h"
#include "duel_effect_resource_record.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "card_preview_callbacks.h"
#include "sprite_primitive.h"
#define DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST
#include "display_object_packet_submit.h"
#include "card_constants.h"
#include "../ygo_types.h"
/*
 * Duel card-detail panel: builds the scratchpad sprite parameters for the
 * panel frame, the card image, the ATK/DEF digit rows, the repeated level
 * icons and the guardian-star tile, and submits each through
 * DisplayObject_SubmitPacket. Byte-exact under gcc_2_8_1_g8_split with no
 * register pins, aliases or inline assembly.
 *
 * Three constructs exist only to steer the compiler and emit no code:
 *
 *  - `sa`/`sb` with the 0x00090009 extent inside a do/while (0): the loop
 *    note after the x load is a scheduling barrier, which gives retail's
 *    `lhu; lui; ori; addiu` order at the level-icon block.
 *
 *  - the two guarded write-backs (`win->flags = flags;` and the second
 *    `PRM->cxcy.h.cy = white;`). Each stores a value its destination already
 *    holds. cse keeps such a store, so the guard and its condition are real
 *    code for flow, both scheduling passes and both register allocators;
 *    reload's cse then deletes the no-op store, and jump2 deletes the empty
 *    branch together with the whole condition chain. Nothing reaches the
 *    object, but while they exist they shape the conflict graph:
 *      - the first makes the 14 (`k`) and the 0xFEFFFFFF mask (`m`) global
 *        pseudos that conflict with each other and with the two temporaries
 *        in v0/v1, so global-alloc gives `k` t1 and `m` t2 as retail has
 *        (local-alloc alone puts the 14 in v0 and the mask in t1). `k` and
 *        `m` are read before their only assignment on purpose: a second
 *        assignment would stop `li k` being placed next to its store and
 *        lift it above the argument moves. `k` is u16 because an SImode 14
 *        lets reload's cse rewrite `addiu t0,t0,14` as `addu t0,t0,t1`.
 *      - the second keeps the 0xF8 (`white`) live while the clamp counter
 *        `i` holds s0, so `white` takes s4. Its condition repeats the shape
 *        of the clamp test that follows, which keeps local-alloc's order
 *        for the two field loads.
 *    With those registers the second scheduling pass floats `li t1,14` and
 *    `li s4,248` to the top of the second submit block by itself.
 */
void func_80028B08(DisplayObject *obj, s32 arg1) {
    u8 buf1[5];
    u8 buf2[5];
    Func80028B08Extra *EXT;
    SpritePrim *PRM;
    Func80028B08Ctx *CTX;
    DisplayObject *win;
    DuelEffectResourceRecord *rec;
    s32 arg;
    s32 i;
    u16 flags;
    u32 f4;
    u32 tile;
    u32 lo;
    s32 wrap;
    u16 k;
    s32 white;
    s32 sa;
    s32 sb;
    u32 m;

    wrap = 0xFFFF;
    win = (DisplayObject *)obj->field_54;
    if ((obj->attribute & (1<<31) ) != 0) {
        return;
    }
    flags = win->flags;
    if ((obj->field_66 & m) ^ (obj->field_67 & k)) {
        win->flags = flags;
    }
    if ((flags & 0x40) == 0) {
        return;
    }
    EXT = (Func80028B08Extra *)0x1F800398;
    PRM = (SpritePrim *)0x1F800320;
    CTX = (Func80028B08Ctx *)0x1F800344;
    arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0x10000;
    if (flags & 0x4) {
        obj->field_20.word = win->field_20.word;
        f4 = obj->attribute & ~(1<<27) ;
        obj->field_44.word = win->field_44.word;
        obj->attribute = f4;
        f4 = f4 | (win->attribute & 0x08000000);
        obj->attribute = f4;
        if (func_80041F90(
                (struct DisplayObject *)obj, (s16)win->field_30.h.field_30 + (s16)win->field_18,
                (s16)win->field_30.h.field_32 + (s16)win->field_1A, (struct ProjectionOut *)EXT
            ) <= 0) {
            return;
        }
        arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0xF0000;
        *(u32 *)&CTX->field_4 = win->field_0C;
        CTX->field_3 = 9;
        CTX->field_7 = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->field_30.h.field_30 + 0x13;
    PRM->xy.h.y = win->field_30.h.field_32 + 0x32;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->rgb = win->field_0C;
    PRM->cxcy.word = obj->field_40.word;
    PRM->uv.word = obj->field_5C;
    PRM->tpage = obj->field_66;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 |= 2;
    PRM->xy.h.x = win->field_30.h.field_30 + 0xC;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xE;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    m = 0xFEFFFFFF;
    PRM->attribute = (PRM->attribute & m) | 0x60000000;
    PRM->extent.wh.w.word = 0x60;
    k = 0xE;
    PRM->extent.wh.h = k;
    PRM->cxcy.h.cx = 0x1E0;
    white = 0xF8;
    PRM->cxcy.h.cy = white;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    rec = &D_800EA0E8[obj->field_67];
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->field_30.h.field_30 + obj->field_30.h.field_30;
    PRM->xy.h.y = win->field_30.h.field_32 + obj->field_30.h.field_32;
    PRM->extent.word = obj->field_3C.word;
    PRM->uv.word = obj->field_5E;
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = white;
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = white;

        i = rec->field_32 + rec->field_36;
        if (i > 9999) {
            PRM->cxcy.h.cy = white;
        }
        if (i > 9999 ) {
            i = 9999 ;
        }
        Text_EncodeDecimalDigits(i, 4, buf1);
        i = rec->field_34 + rec->field_38;
        if (i > 9999 ) {
            i = 9999 ;
        }
        Text_EncodeDecimalDigits(i, 4, buf2);

        PRM->uv.b.hi = (PRM->uv.b.hi & 0x80) + 0x10;
        PRM->xy.h.x = win->field_30.h.field_30 + 0x61;
        PRM->xy.h.y = win->field_30.h.field_32 + 0x9D;
        *(u32 *)&PRM->extent = 0x000D0006;
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = 0xF9;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf1[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        PRM->xy.h.x = win->field_30.h.field_30 + 0x61;
        PRM->xy.h.y = win->field_30.h.field_32 + 0xAB;
        PRM->cxcy.h.cy = 0xF8;
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf2[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        sa = win->field_30.h.field_30;
        do { sb = 0x00090009; } while (0);
        PRM->xy.h.x = sa + 0x77;
        PRM->xy.h.y = win->field_30.h.field_32 + 0x20;
        *(u32 *)&PRM->extent = sb;
        PRM->uv.b.lo = 0;
        PRM->cxcy.h.cx = 0x1C0;
        PRM->cxcy.h.cy = 0xF8;
        if (rec->field_3A != 0) {
            i = 0;
            do {
                DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
                PRM->xy.h.x = PRM->xy.h.x - 9;
                i++;
            } while (i < (s32)rec->field_3A);
        }
    } else {
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
    }

    PRM->xy.h.x = win->field_30.h.field_30 + 0x6E;
    *(u32 *)&PRM->extent = 0x00100010;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xD;
    lo = rec->field_3B << 4;
    PRM->uv.b.lo = lo;
    PRM->uv.b.hi = PRM->uv.b.hi & 0x80;
    tile = PRM->uv.b.lo;
    PRM->cxcy.h.cx = win->field_40.h.field_40 + tile;
    PRM->cxcy.h.cy = 0xFF;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
}

