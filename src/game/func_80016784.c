#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "text_encode_decimal_digits.h"
#include "duel_card.h"
#include "display_object_projection.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"
#include "func_80016784.h"

/* Draws one card's frame on the duel field: position, the hand/field
 * flags, the card-number digits (via Duel_CalcCardStats and
 * Text_EncodeDecimalDigits) and the face/back sprite, through
 * DisplayObject_SubmitPacket on the 0x1F8003xx scratchpad records.
 *
 * The two records it fills are SpritePrim, the GsSPRITE sprite_primitive.h
 * already spells for the two display-object renderers, which build theirs at
 * the same 0x1F800320. This one builds a second at 0x1F800000 for the card
 * art and reuses 0x1F800320 for the number digits, the spell-class frame and
 * the card face, resetting the fields it needs between submissions.
 *
 * The third scratch record, at 0x1F800344, is the POLY_FT4 the clip path
 * primes (len 9, code 0x2C) with the object's colour, and the record at
 * 0x1F8003E0 is CardFrameScratch above.
 *
 * All three reads of the colour word at 0x0C sit between scratchpad stores,
 * and the target keeps every one of them where the source puts it. That used
 * to need the read spelled through a cast, because a member read is a struct
 * reference and GCC 2.8.1 floats one across stores that are not. With the
 * scratchpad records typed, those stores are struct references too, the
 * reads stay put, and every offset this function touches is a member. */
void func_80016784(DisplayObject *object, s32 arg1, s32 arg2, s32 arg3) {
    u8 sp18[8];
    u8 sp20[8];
    DuelCardRecord *card;
    s32 fl;
    s32 i;
    SpritePrim *k;
    CardFrameScratch *o;
    s32 n;
    s32 d;
    s32 g1;
    s32 g2;
    s32 t;
    SpritePrim *z;
    POLY_FT4 *y;

    if ((u32)(arg2 + 0x33) < 0x173) {
        if (arg3 >= -0x3B) {
            if (arg3 < 0xF0) {
                o = (CardFrameScratch *)0x1F8003E0;
                k = (SpritePrim *)0x1F800320;
                y = (POLY_FT4 *)0x1F800344;
                z = (SpritePrim *)0x1F800000;
                card = &D_801A7AD8[object->field_6A];
                o->pos.h.x = arg2;
                o->pos.h.y = arg3;
                fl = object->field_14 | 0x10000;
                t = object->attribute;
                k->attribute = t;
                z->attribute = t;
                object->field_69 = 0;
                if (object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) {
                    fl = object->field_14 | 0xF0000;
                    if (func_80041F90(object,
                                      (s16)o->pos.h.x + 0x1A,
                                      (s16)o->pos.h.y + 0x1E,
                                      (struct ProjectionOut *)0x1F8003E0) < 0) {
                        return;
                    }
                    g1 = object->field_0C;
                    setlen(y, 9);
                    *(s32 *)&y->r0 = g1;
                    y->code = 0x2C;
                    if ((object->field_20.word & 0xFFFFFF) == 0) {
                        k->attribute = k->attribute | 0x80;
                        z->attribute = k->attribute;
                    }
                }
                k->tpage = 0x1E;
                g2 = object->field_0C;
                k->cxcy.word = 0xF10100;
                k->rgb = g2;
                n = object->field_67;
                if (n != 0) {
                    if (n < 0x29) {
                        k->extent.word = 0x10000C;
                        k->uv.b.hi = 0x70;
                        k->xy.h.x = o->pos.h.x + 0xF;
                        k->xy.h.y = o->pos.h.y + 0xF;
                        k->uv.b.lo = (n / 10) * 0xC;
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                        k->uv.b.lo = (n % 10) * 0xC;
                        k->xy.h.x = k->xy.h.x + 0xC;
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                    }
                } else if (object->field_69 == 0) {
                    k->extent.word = 0x100020;
                    k->uv.word = 0x6000;
                    k->xy.h.x = o->pos.h.x + 0xA;
                    k->xy.h.y = o->pos.h.y + 0x28;
                    switch (object->field_68) {
                    case CARD_TYPE_EQUIP:
                        k->uv.b.lo = 0x20;
                    case CARD_TYPE_MAGIC:
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 1;
                        break;
                    case CARD_TYPE_TRAP:
                        k->uv.b.lo = 0x40;
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 2;
                        break;
                    case CARD_TYPE_RITUAL:
                        k->uv.b.lo = 0x60;
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                        k->cxcy.h.cy = k->cxcy.h.cy + 3;
                        break;
                    default:
                        k->extent.word = 0x100008;
                        k->uv.word = 0x7078;
                        k->xy.h.x = o->pos.h.x + 5;
                        DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                        k->uv.b.hi = 0x58;
                        k->extent.wh.h = 8;
                        k->xy.h.x = o->pos.h.x + 0xE;
                        d = Duel_CalcCardStats(card);
                        Text_EncodeDecimalDigits((s16)d, 4, sp18);
                        Text_EncodeDecimalDigits(d >> 0x10, 4, sp20);
                        i = 3;
                        do {
                            k->uv.b.lo = sp18[i] * 8;
                            DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                            k->xy.h.y = k->xy.h.y + 8;
                            k->uv.b.lo = sp20[i] * 8;
                            DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
                            i--;
                            k->xy.h.x = k->xy.h.x + 8;
                            k->xy.h.y = k->xy.h.y - 8;
                        } while (i >= 0);
                        break;
                    }
                    z->tpage = 0xE;
                    z->rgb = object->field_0C;
                    z->xy.h.x = o->pos.h.x + 6;
                    z->xy.h.y = o->pos.h.y + 6;
                    i = (s8)card->table_index;
                    z->cxcy.h.cx = 0x380;
                    z->cxcy.h.cy = i + 0xE0;
                    z->uv.b.lo = (i % 5) * 0x28;
                    z->extent.word = 0x200028;
                    z->uv.b.hi = (i / 5) << 5;
                    DisplayObject_SubmitPacket(z, (u8 *)y, arg1, fl, (u8 *)o);
                }
                k->extent.word = 0x3C0034;
                k->uv.word = 0x8000;
                k->xy.word = o->pos.word;
                if (object->field_67 != 0) {
                    k->uv.word = 0xC000;
                }
                if (object->field_69 != 0) {
                    k->uv.word = 0x8038;
                    k->cxcy.word = 0xF10100;
                }
                DisplayObject_SubmitPacket(k, (u8 *)y, arg1, fl, (u8 *)o);
            }
        }
    }
}
