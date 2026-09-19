#include "../types.h"
#include "../game/text_encode_decimal_digits.h"
#include "../game/display_object_projection.h"
#include "../game/duel_effect_resource_record.h"
#include "../psyq/libgs.h"
#include "../game/display_object.h"
#include "../game/card_preview_callbacks.h"
#include "../game/sprite_primitive.h"
#define DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST
#include "../game/display_object_packet_submit.h"
#include "../game/card_constants.h"
#include "../ygo_types.h"
/*
 * Current best under gcc_2_8_1_g8_split: 384/384 instructions, 357 of 384
 * aligned on opcode and registers in 23 divergent blocks, against 340/384 in
 * 30 blocks before. The opcode census over the divergent blocks is nop +1,
 * distance 1; nothing structural remains.
 *
 * SIX levers, all measured one at a time against the state below them, and all
 * of one family: retail batches a group of loads before the group of stores
 * that consumes them, and the source has to say so.
 *   1-2. w14 and f0c name win->field_14 and win->field_0C before the
 *        do { CTX->field_3 = 9; } while (0). Retail loads both (target 59-60)
 *        before the sb at 61; written inline, field_0C was loaded inside its
 *        own store and the sb came first. 340 -> 345.
 *   3.   e3c names obj->field_3C.word in the third block. 345 -> 346.
 *   4.   nab hoists the row coordinate at the +0xAB site: retail (267-275)
 *        loads field_32 before materialising 0xF8. 346 -> 349, 30 -> 28.
 *   5.   The uv store moves to the END of the first block, after tpage:
 *        retail (72-96) writes uv eighth and the source wrote it third. The
 *        position gradient is monotone and was measured at five positions --
 *        second 349, after y 350, after rgb 351, after cxcy 349, last 353.
 *        349 -> 353, 28 -> 25.
 *   6.   hi names 0xF0000 and wrap is borrowed for rec->field_36, assigned
 *        before the submit that precedes its use. 353 -> 357, 25 -> 23, and
 *        the two halves compose (354/23 and 356/25 alone).
 *
 * MEASURED AND CLOSED, so the next attempt does not spend a round on them:
 *   - Shared constants as locals. The listing keeps 0xE in $t1 (105), 0xF8 in
 *     $s4 (107) and 0x60 in $s0 from instruction 74 across blocks, which reads
 *     as long-lived pseudos. It is not: 0xF8 in a local is 348/31, 0x60 is
 *     336/36, all three together 326/44, and 0xE is inert. Register reuse in
 *     the listing is not evidence of name reuse in the source.
 *   - Statement order inside the 104-120 and 302-312 zones. Seven spellings,
 *     five of them tie at exactly 353/25 and two are worse.
 *   - The 9D and 0D row-coordinate sites. Inert alone (346) and inert on top
 *     of the 0xAB site; only 0xAB pays. This refines the older note's "two
 *     sites is the peak", which was measured before levers 1-3 existed.
 *
 * decomp-permuter, rerun from this base, reaches 357/23 by a different route
 * (a pointer to the anonymous union at obj->field_30). Levers 6 reach the same
 * figure without naming an anonymous type, which is why they are what is
 * installed. Residual: register choices only.
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
    s32 wrap;
    s32 ny2;
    u32 e3c;
    s32 nab;
    s32 hi;
    s32 ny;
    s32 nye;
    u32 f0c;
    s32 w14;

    wrap = 0xFFFF;
    win = (DisplayObject *)obj->field_54;
    if ((obj->attribute & GsDOFF) != 0) {
        return;
    }
    flags = win->flags;
    if ((flags & 0x40) == 0) {
        return;
    }
    CTX = (Func80028B08Ctx *)0x1F800344;
    PRM = (SpritePrim *)0x1F800320;
    EXT = (Func80028B08Extra *)0x1F800398;
    arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0x10000;
    if (flags & 0x4) {
        obj->field_20.word = win->field_20.word;
        f4 = obj->attribute & ~GsROTOFF;
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
        w14 = (s16)win->field_14;
        f0c = win->field_0C;
        do {
            CTX->field_3 = 9;
        } while (0);
        hi = 0xF0000;
        arg = ((w14 - 1) & 0xFFFF) | hi;
        *(u32 *)&CTX->field_4 = f0c;
        CTX->field_7 = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->field_30.h.field_30 + 0x13;
    ny = win->field_30.h.field_32;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->xy.h.y = ny + 0x32;
    PRM->rgb = win->field_0C;
    *(u32 *)&PRM->cxcy = obj->field_40.word;
    PRM->tpage = obj->field_66;
    PRM->uv.word = obj->field_5C;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 = CTX->field_7 | 2;
    PRM->xy.h.x = win->field_30.h.field_30 + 0xC;
    nye = win->field_30.h.field_32;
    PRM->extent.wh.w.word = 0x60;
    PRM->xy.h.y = nye + 0xE;
    PRM->extent.wh.h = 0xE;
    PRM->cxcy.h.cy = 0xF8;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    PRM->attribute = (PRM->attribute & 0xFEFFFFFF) | 0x60000000;
    PRM->cxcy.h.cx = 0x1E0;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    rec = &D_800EA0E8[obj->field_67];
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->field_30.h.field_30 + obj->field_30.h.field_30;
    PRM->xy.h.y = win->field_30.h.field_32 + obj->field_30.h.field_32;
    e3c = obj->field_3C.word;
    PRM->uv.word = obj->field_5E;
    *(u32 *)&PRM->extent = e3c;
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        wrap = rec->field_36;
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;

        i = rec->field_32 + wrap;
        if (i > CARD_STAT_MAX) {
            i = CARD_STAT_MAX;
        }
        Text_EncodeDecimalDigits(i, 4, buf1);
        i = rec->field_34 + rec->field_38;
        if (i > CARD_STAT_MAX) {
            i = CARD_STAT_MAX;
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
        nab = win->field_30.h.field_32;
        PRM->cxcy.h.cy = 0xF8;
        PRM->xy.h.y = nab + 0xAB;
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

        PRM->xy.h.x = win->field_30.h.field_30 + 0x77;
        ny2 = win->field_30.h.field_32 + 0x20;
        PRM->cxcy.h.cx = 0x1C0;
        *(u32 *)&PRM->extent = 0x00090009;
        PRM->uv.b.lo = 0;
        PRM->cxcy.h.cy = 0xF8;
        PRM->xy.h.y = ny2;
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
    tile = rec->field_3B << 4;
    PRM->uv.b.lo = tile;
    PRM->uv.b.hi = PRM->uv.b.hi & 0x80;
    PRM->cxcy.h.cy = 0xFF;
    PRM->cxcy.h.cx = win->field_40.h.field_40 + (u8)tile;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
}
