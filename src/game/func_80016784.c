#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "duel_card.h"
#include "display_object_projection.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "func_80016784.h"

void func_80042188(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

/* Draws one card's frame on the duel field: position, the hand/field
 * flags, the card-number digits (via Duel_CalcCardStats and
 * Text_EncodeDecimalDigits) and the face/back sprite, through
 * func_80042188 on the 0x1F8003xx scratchpad records.
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
    u8 *k;
    u8 *o;
    s32 n;
    s32 d;
    s32 g1;
    s32 g2;
    s32 t;
    u8 *z;
    u8 *y;

    if ((u32)(arg2 + 0x33) < 0x173) {
        if (arg3 >= -0x3B) {
            if (arg3 < 0xF0) {
                o = (u8 *)0x1F8003E0;
                k = (u8 *)0x1F800320;
                y = (u8 *)0x1F800344;
                z = (u8 *)0x1F800000;
                card = &D_801A7AD8[object->field_6A];
                *(s16 *)(o + 8) = arg2;
                *(s16 *)(o + 0xA) = arg3;
                fl = object->field_14 | 0x10000;
                t = object->attribute;
                *(s32 *)k = t;
                *(s32 *)z = t;
                object->field_69 = 0;
                if (object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) {
                    fl = object->field_14 | 0xF0000;
                    if (func_80041F90((struct ProjectionObj *)object,
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
                        *(s32 *)k = *(s32 *)k | 0x80;
                        *(s32 *)z = *(s32 *)k;
                    }
                }
                *(u16 *)(k + 0xC) = 0x1E;
                g2 = *(s32 *)&object->field_0C;
                *(s32 *)(k + 0x10) = 0xF10100;
                *(s32 *)(k + 0x14) = g2;
                n = object->field_67;
                if (n != 0) {
                    if (n < 0x29) {
                        *(s32 *)(k + 8) = 0x10000C;
                        k[0xF] = 0x70;
                        *(u16 *)(k + 4) = *(u16 *)(o + 8) + 0xF;
                        *(u16 *)(k + 6) = *(u16 *)(o + 0xA) + 0xF;
                        k[0xE] = (n / 10) * 0xC;
                        func_80042188(k, y, arg1, fl, o);
                        k[0xE] = (n % 10) * 0xC;
                        *(u16 *)(k + 4) = *(u16 *)(k + 4) + 0xC;
                        func_80042188(k, y, arg1, fl, o);
                    }
                } else if (object->field_69 == 0) {
                    *(s32 *)(k + 8) = 0x100020;
                    *(u16 *)(k + 0xE) = 0x6000;
                    *(u16 *)(k + 4) = *(u16 *)(o + 8) + 0xA;
                    *(u16 *)(k + 6) = *(u16 *)(o + 0xA) + 0x28;
                    switch (object->field_68) {
                    case CARD_TYPE_EQUIP:
                        k[0xE] = 0x20;
                    case CARD_TYPE_MAGIC:
                        func_80042188(k, y, arg1, fl, o);
                        *(u16 *)(k + 0x12) = *(u16 *)(k + 0x12) + 1;
                        break;
                    case CARD_TYPE_TRAP:
                        k[0xE] = 0x40;
                        func_80042188(k, y, arg1, fl, o);
                        *(u16 *)(k + 0x12) = *(u16 *)(k + 0x12) + 2;
                        break;
                    case CARD_TYPE_RITUAL:
                        k[0xE] = 0x60;
                        func_80042188(k, y, arg1, fl, o);
                        *(u16 *)(k + 0x12) = *(u16 *)(k + 0x12) + 3;
                        break;
                    default:
                        *(s32 *)(k + 8) = 0x100008;
                        *(u16 *)(k + 0xE) = 0x7078;
                        *(u16 *)(k + 4) = *(u16 *)(o + 8) + 5;
                        func_80042188(k, y, arg1, fl, o);
                        k[0xF] = 0x58;
                        *(u16 *)(k + 0xA) = 8;
                        *(u16 *)(k + 4) = *(u16 *)(o + 8) + 0xE;
                        d = Duel_CalcCardStats(card);
                        Text_EncodeDecimalDigits((s16)d, 4, sp18);
                        Text_EncodeDecimalDigits(d >> 0x10, 4, sp20);
                        i = 3;
                        do {
                            k[0xE] = sp18[i] * 8;
                            func_80042188(k, y, arg1, fl, o);
                            *(u16 *)(k + 6) = *(u16 *)(k + 6) + 8;
                            k[0xE] = sp20[i] * 8;
                            func_80042188(k, y, arg1, fl, o);
                            i--;
                            *(u16 *)(k + 4) = *(u16 *)(k + 4) + 8;
                            *(u16 *)(k + 6) = *(u16 *)(k + 6) - 8;
                        } while (i >= 0);
                        break;
                    }
                    *(u16 *)(z + 0xC) = 0xE;
                    *(s32 *)(z + 0x14) = *(s32 *)&object->field_0C;
                    *(s16 *)(z + 4) = *(u16 *)(o + 8) + 6;
                    *(s16 *)(z + 6) = *(u16 *)(o + 0xA) + 6;
                    i = (s8)card->table_index;
                    *(u16 *)(z + 0x10) = 0x380;
                    *(s16 *)(z + 0x12) = i + 0xE0;
                    *(u8 *)(z + 0xE) = (i % 5) * 0x28;
                    *(s32 *)(z + 8) = 0x200028;
                    *(u8 *)(z + 0xF) = (i / 5) << 5;
                    func_80042188(z, y, arg1, fl, o);
                }
                *(s32 *)(k + 8) = 0x3C0034;
                *(u16 *)(k + 0xE) = 0x8000;
                *(s32 *)(k + 4) = *(s32 *)(o + 8);
                if (object->field_67 != 0) {
                    *(u16 *)(k + 0xE) = 0xC000;
                }
                if (object->field_69 != 0) {
                    *(u16 *)(k + 0xE) = 0x8038;
                    *(s32 *)(k + 0x10) = 0xF10100;
                }
                func_80042188(k, y, arg1, fl, o);
            }
        }
    }
}
