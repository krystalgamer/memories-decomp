#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "duel_card.h"
#include "display_object_projection.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "sprite_primitive.h"
#include "func_80016784.h"

void func_80042188(SpritePrim *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

/* Draws one card's frame on the duel field: position, the hand/field
 * flags, the card-number digits (via Duel_CalcCardStats and
 * Text_EncodeDecimalDigits) and the face/back sprite, through
 * func_80042188 on the 0x1F8003xx scratchpad records.
 *
 * The two records it fills are SpritePrim, the GsSPRITE sprite_primitive.h
 * already spells for the two display-object renderers, which build theirs at
 * the same 0x1F800320. This one builds a second at 0x1F800000 for the card
 * art and reuses 0x1F800320 for the number digits, the spell-class frame and
 * the card face, resetting the fields it needs between submissions.
 *
 * The colour word at 0x0C is the one field read through a cast rather than
 * as `object->field_0C`, and that is measured, not a leftover: a plain
 * member read is a struct reference, the scratchpad stores around it are
 * not, and GCC 2.8.1 will float such a load across them. All three reads of
 * it sit between scratchpad stores, and the target keeps every one of them
 * where the source puts it, so the cast stays and the name comes from the
 * `&object->field_0C` it is taken through. Every other offset this function
 * touches is an ordinary member read. */
void func_80016784(DisplayObject *object, s32 arg1, s32 arg2, s32 arg3) {
    u8 sp18[8];
    u8 sp20[8];
    DuelCardRecord *card;
    s32 fl;
    s32 i;
    SpritePrim *k;
    u8 *o;
    s32 n;
    s32 d;
    s32 g1;
    s32 g2;
    s32 t;
    SpritePrim *z;
    u8 *y;

    if ((u32)(arg2 + 0x33) < 0x173) {
        if (arg3 >= -0x3B) {
            if (arg3 < 0xF0) {
                o = (u8 *)0x1F8003E0;
                k = (SpritePrim *)0x1F800320;
                y = (u8 *)0x1F800344;
                z = (SpritePrim *)0x1F800000;
                card = &D_801A7AD8[object->field_6A];
                *(s16 *)(o + 8) = arg2;
                *(s16 *)(o + 0xA) = arg3;
                fl = object->field_14 | 0x10000;
                t = object->attribute;
                k->attribute = t;
                z->attribute = t;
                object->field_69 = 0;
                if (object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) {
                    fl = object->field_14 | 0xF0000;
                    if (func_80041F90(object,
                                      (s16)*(u16 *)(o + 8) + 0x1A,
                                      (s16)*(u16 *)(o + 0xA) + 0x1E,
                                      (struct ProjectionOut *)0x1F8003E0) < 0) {
                        return;
                    }
                    g1 = *(s32 *)&object->field_0C;
                    *(u8 *)(y + 3) = 9;
                    *(s32 *)(y + 4) = g1;
                    *(u8 *)(y + 7) = 0x2C;
                    if ((object->field_20.word & 0xFFFFFF) == 0) {
                        k->attribute = k->attribute | 0x80;
                        z->attribute = k->attribute;
                    }
                }
                k->tpage = 0x1E;
                g2 = *(s32 *)&object->field_0C;
                k->cxcy.word = 0xF10100;
                k->rgb = g2;
                n = object->field_67;
                if (n != 0) {
                    if (n < 0x29) {
                        k->extent.word = 0x10000C;
                        k->uv.b.hi = 0x70;
                        k->xy.h.x = *(u16 *)(o + 8) + 0xF;
                        k->xy.h.y = *(u16 *)(o + 0xA) + 0xF;
                        k->uv.b.lo = (n / 10) * 0xC;
                        func_80042188(k, y, arg1, fl, o);
                        k->uv.b.lo = (n % 10) * 0xC;
                        k->xy.h.x = k->xy.h.x + 0xC;
                        func_80042188(k, y, arg1, fl, o);
                    }
                } else if (object->field_69 == 0) {
                    k->extent.word = 0x100020;
                    k->uv.word = 0x6000;
                    k->xy.h.x = *(u16 *)(o + 8) + 0xA;
                    k->xy.h.y = *(u16 *)(o + 0xA) + 0x28;
                    switch (object->field_68) {
                    case CARD_TYPE_EQUIP:
                        k->uv.b.lo = 0x20;
                    case CARD_TYPE_MAGIC:
                        func_80042188(k, y, arg1, fl, o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 1;
                        break;
                    case CARD_TYPE_TRAP:
                        k->uv.b.lo = 0x40;
                        func_80042188(k, y, arg1, fl, o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 2;
                        break;
                    case CARD_TYPE_RITUAL:
                        k->uv.b.lo = 0x60;
                        func_80042188(k, y, arg1, fl, o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 3;
                        break;
                    default:
                        k->extent.word = 0x100008;
                        k->uv.word = 0x7078;
                        k->xy.h.x = *(u16 *)(o + 8) + 5;
                        func_80042188(k, y, arg1, fl, o);
                        k->uv.b.hi = 0x58;
                        k->extent.wh.h = 8;
                        k->xy.h.x = *(u16 *)(o + 8) + 0xE;
                        d = Duel_CalcCardStats(card);
                        Text_EncodeDecimalDigits((s16)d, 4, sp18);
                        Text_EncodeDecimalDigits(d >> 0x10, 4, sp20);
                        i = 3;
                        do {
                            k->uv.b.lo = sp18[i] * 8;
                            func_80042188(k, y, arg1, fl, o);
                            k->xy.h.y = k->xy.h.y + 8;
                            k->uv.b.lo = sp20[i] * 8;
                            func_80042188(k, y, arg1, fl, o);
                            i--;
                            k->xy.h.x = k->xy.h.x + 8;
                            k->xy.h.y = k->xy.h.y - 8;
                        } while (i >= 0);
                        break;
                    }
                    z->tpage = 0xE;
                    z->rgb = *(s32 *)&object->field_0C;
                    z->xy.h.x = *(u16 *)(o + 8) + 6;
                    z->xy.h.y = *(u16 *)(o + 0xA) + 6;
                    i = (s8)card->table_index;
                    z->cxcy.h.cx = 0x380;
                    z->cxcy.h.cy = i + 0xE0;
                    z->uv.b.lo = (i % 5) * 0x28;
                    z->extent.word = 0x200028;
                    z->uv.b.hi = (i / 5) << 5;
                    func_80042188(z, y, arg1, fl, o);
                }
                k->extent.word = 0x3C0034;
                k->uv.word = 0x8000;
                k->xy.word = *(s32 *)(o + 8);
                if (object->field_67 != 0) {
                    k->uv.word = 0xC000;
                }
                if (object->field_69 != 0) {
                    k->uv.word = 0x8038;
                    k->cxcy.word = 0xF10100;
                }
                func_80042188(k, y, arg1, fl, o);
            }
        }
    }
}
