#include "../types.h"
#include "../game/text_encode_decimal_digits.h"
#include "../game/display_object_projection.h"
#include "../game/duel_effect_resource_record.h"
#include "../psyq/libgs.h"
#include "../game/display_object.h"
#include "../game/sprite_primitive.h"
#include "../game/card_constants.h"

typedef struct {
    u8 field_0;
    u8 field_1;
    u8 field_2;
    u8 field_3;
    u8 field_4;
    u8 field_5;
    u8 field_6;
    u8 field_7;
    u16 field_8;
    u8 field_A;
    u8 field_B;
} Ctx;

typedef struct {
    u32 field_0;
    u32 field_4;
} Extra;

extern void func_80042188(SpritePrim *, Ctx *, s32, s32, Extra *);

/*
 * Current best under gcc_2_8_1_g8_split: 384/384 instructions with 172
 * differing positions. Scratchpad records must be locals below the entry
 * guards; the parameter word widths, field_67/field_68 roles, live 0xFFFF
 * addend, shared clamp/loop local, and delayed field_0 update are structural.
 * Residual: two rematerialized 0xF8 values replace target nops, and the two
 * later scratchpad bases remain exchanged between $s6 and $s7.
 */
void func_80028B08(DisplayObject *obj, s32 arg1) {
    u8 buf1[5];
    u8 buf2[5];
    Extra *EXT;
    SpritePrim *PRM;
    Ctx *CTX;
    DisplayObject *win;
    DuelEffectResourceRecord *rec;
    s32 arg;
    s32 i;
    u16 flags;
    u32 f4;
    u32 tile;
    s32 white;
    s32 wrap;

    white = 0xF8;
    wrap = 0xFFFF;
    win = (DisplayObject *)obj->field_54;
    if ((obj->attribute & GsDOFF) != 0) {
        return;
    }
    flags = win->flags;
    if ((flags & 0x40) == 0) {
        return;
    }
    CTX = (Ctx *)0x1F800344;
    PRM = (SpritePrim *)0x1F800320;
    EXT = (Extra *)0x1F800398;
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
        CTX->field_3 = 9;
        arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0xF0000;
        *(u32 *)&CTX->field_4 = win->field_0C;
        CTX->field_7 = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->field_30.h.field_30 + 0x13;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->xy.h.y = win->field_30.h.field_32 + 0x32;
    PRM->rgb = win->field_0C;
    *(u32 *)&PRM->cxcy = obj->field_40.word;
    PRM->uv.word = obj->field_5C;
    PRM->tpage = obj->field_66;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 = CTX->field_7 | 2;
    PRM->xy.h.x = win->field_30.h.field_30 + 0xC;
    PRM->extent.wh.w.word = 0x60;
    PRM->extent.wh.h = 0xE;
    PRM->cxcy.h.cy = white;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    PRM->attribute = (PRM->attribute & 0xFEFFFFFF) | 0x60000000;
    PRM->cxcy.h.cx = 0x1E0;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xE;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->field_30.h.field_30 + obj->field_30.h.field_30;
    PRM->xy.h.y = win->field_30.h.field_32 + obj->field_30.h.field_32;
    *(u32 *)&PRM->extent = obj->field_3C.word;
    PRM->uv.word = obj->field_5E;
    rec = &D_800EA0E8[obj->field_67];
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = white;
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = white;

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
        PRM->cxcy.h.cy = white;
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
        PRM->cxcy.h.cx = 0x1C0;
        *(u32 *)&PRM->extent = 0x00090009;
        PRM->uv.b.lo = 0;
        PRM->cxcy.h.cy = white;
        PRM->xy.h.y = win->field_30.h.field_32 + 0x20;
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
