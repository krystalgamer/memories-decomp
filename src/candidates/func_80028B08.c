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
 * Current best under gcc_2_8_1_g8_split: 384/384 instructions with an empty
 * opcode census against the assembled target. Scratchpad records must be
 * locals below the entry guards; the parameter word widths, field_67/field_68
 * roles, live 0xFFFF addend, shared clamp/loop local, and delayed field_0
 * update are structural. CTX->field_3 = 9 sits in its own do/while (0), the
 * sprite's uv word is stored right after its first x coordinate, and the
 * field_32 + 0x20 row coordinate is computed into ny2 ahead of its store.
 *
 * The 0xF8 sprite height is written as a literal at all five of its sites,
 * not held in a local. gcc 2.8 has no global CSE, so a named local becomes
 * one pseudo that survives across blocks and is materialised where retail
 * has nothing; the literal is recomputed per block, as retail does. The five
 * sites are a coupled set -- substituting any one of them alone changes
 * nothing measurable, and only all five together remove both surplus addiu.
 *
 * The uv word is read before the extent word, which is the reverse of the
 * order the fields are declared in. Written the other way round, the extent
 * load fills the delay slot of the uv load, where retail leaves it empty;
 * the two assignments are to distinct fields and neither depends on the
 * other, so the order is free.
 *
 * The record pointer is computed immediately after the EXT->field_4
 * store, above the PRM field writes rather than just before its first
 * use. Nothing between the two positions writes obj or D_800EA0E8, so
 * the position is free; retail materialises the address there and the
 * whole block allocates around it.
 *
 * Residual: register choices only.
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
        do {
            CTX->field_3 = 9;
        } while (0);
        arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0xF0000;
        *(u32 *)&CTX->field_4 = win->field_0C;
        CTX->field_7 = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->field_30.h.field_30 + 0x13;
    PRM->uv.word = obj->field_5C;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->xy.h.y = win->field_30.h.field_32 + 0x32;
    PRM->rgb = win->field_0C;
    *(u32 *)&PRM->cxcy = obj->field_40.word;
    PRM->tpage = obj->field_66;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 = CTX->field_7 | 2;
    PRM->xy.h.x = win->field_30.h.field_30 + 0xC;
    PRM->extent.wh.w.word = 0x60;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xE;
    PRM->extent.wh.h = 0xE;
    PRM->cxcy.h.cy = 0xF8;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    PRM->attribute = (PRM->attribute & 0xFEFFFFFF) | 0x60000000;
    PRM->cxcy.h.cx = 0x1E0;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    rec = &D_800EA0E8[obj->field_67];
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->field_30.h.field_30 + obj->field_30.h.field_30;
    PRM->xy.h.y = win->field_30.h.field_32 + obj->field_30.h.field_32;
    PRM->uv.word = obj->field_5E;
    *(u32 *)&PRM->extent = obj->field_3C.word;
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;

        i = rec->field_32 + rec->field_36;
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
            func_80042188(PRM, CTX, arg1, arg, EXT);
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        PRM->xy.h.x = win->field_30.h.field_30 + 0x61;
        PRM->cxcy.h.cy = 0xF8;
        PRM->xy.h.y = win->field_30.h.field_32 + 0xAB;
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf2[i] * 6 + 0x10;
            func_80042188(PRM, CTX, arg1, arg, EXT);
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
                func_80042188(PRM, CTX, arg1, arg, EXT);
                PRM->xy.h.x = PRM->xy.h.x - 9;
                i++;
            } while (i < (s32)rec->field_3A);
        }
    } else {
        func_80042188(PRM, CTX, arg1, arg, EXT);
    }

    PRM->xy.h.x = win->field_30.h.field_30 + 0x6E;
    *(u32 *)&PRM->extent = 0x00100010;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xD;
    tile = rec->field_3B << 4;
    PRM->uv.b.lo = tile;
    PRM->uv.b.hi = PRM->uv.b.hi & 0x80;
    PRM->cxcy.h.cy = 0xFF;
    PRM->cxcy.h.cx = win->field_40.h.field_40 + (u8)tile;
    func_80042188(PRM, CTX, arg1, arg, EXT);
}
