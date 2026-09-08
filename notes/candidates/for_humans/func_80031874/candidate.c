#include "../../../../src/types.h"
#include "../../../../src/psyq/libgte.h"
#include "../../../../src/psyq/libgpu.h"
#include "../../../../src/psyq/libgs.h"

extern u8 *D_8009B2FC;
extern u8 D_80090DD8[];
extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));

extern void func_800316F0(u8 *, s32, u8 *, s32);
extern void func_80031784(u8 *, s32, u8 *, s32);
extern void Text_EncodeDecimalDigits(s32, s32, u8 *);
extern void Text_EncodeDecimalNoPadding(s32, s32, u8 *);

/* Draws the eight rows of a card list into the ordering table. It builds two
 * sprites in the scratchpad - the row sprite at 0x1F800020 and the header tag
 * at 0x1F800060 - and encodes every number through the shared text buffer at
 * 0x1F800000.
 *
 * The list's record block is D_8009B2FC + kind * 0x2D4C + 4, where the kind
 * comes from the object's +0x67; its +0x2D3C is the scroll offset, which
 * selects the first of the sixteen-byte rows, and its +0x2D45 is passed to
 * func_80031784 for the header. Each row draws its number (only when the kind
 * is non-zero), its card id, and, for kind zero, the owned and deck counts
 * from the three byte tables at +0x606A, +0x5D97 and +0x5AC4, with the deck
 * count turning red when it is at least three or when the id is one of the
 * five in 0x11 to 0x15 and the count is non-zero. */
void func_80031874(u8 *obj, s32 ot)
{
    u8 *text = (u8 *)0x1F800000;
    u8 *spr = (u8 *)0x1F800020;
    register u8 *tag asm("$10") = (u8 *)0x1F800060;
    u8 *rec;
    u8 *row;
    s32 idx;
    register s32 x asm("$23");
    s32 x0;
    register s32 vy asm("$6");
    s32 y;
    s32 i;
    s32 ry;
    s32 value;
    s32 n;
    s32 v;
    register u32 tex asm("$5");

    x0 = *(s16 *)(obj + 0x30);
    tex = *(u32 *)(obj + 4);
    y = *(s16 *)(obj + 0x32);
    *(s16 *)(tag + 0xC) = 0xB;
    *(s16 *)(spr + 0xC) = 0xB;
    *(u32 *)(spr + 0x14) = 0x808080;
    *(u32 *)(spr + 8) = 0x80008;
    *(u32 *)(tag + 8) = 0x100010;
    vy = gGraphics_sViewportY;
    x = x0 - gGraphics_sViewportX;
    *(s16 *)(spr + 0x10) = 0x290;
    *(s16 *)(spr + 0x12) = 0xFA;
    y = y - vy;
    *(u32 *)(tag + 0) = tex;
    *(u32 *)(spr + 0) = tex;
    idx = obj[0x67];
    rec = D_8009B2FC + (idx * 0x2D4C + 4);
    row = rec + *(s16 *)(rec + 0x2D3C) * 16;
    if (idx == 0) {
        *(s16 *)(tag + 4) = x + 0x88;
        *(s16 *)(tag + 6) = y + 0xF;
        func_80031784(tag, ot, D_80090DD8, rec[0x2D45]);
    } else {
        *(s16 *)(tag + 4) = x + 0x6A;
        *(s16 *)(tag + 6) = y + 0xF;
        func_80031784(tag, ot, &D_80090DD8[idx * 16], rec[0x2D45]);
    }
    i = 0;
    ry = 0x2B;
    row += 8;
    do {
        *(s16 *)(spr + 4) = x + 4;
        *(s16 *)(spr + 6) = ry;
        if (row[5] != 0) {
            *(u32 *)(spr + 0x14) = 0x808080;
            value = *(s16 *)(row - 4);
            if (row[5] & 0x80) {
                *(u32 *)(spr + 0x14) = 0x404040;
            }
            if (idx != 0) {
                *(s16 *)(spr + 4) = x + 0x11;
                Text_EncodeDecimalNoPadding(
                    *(s16 *)(rec + 0x2D3C) + i + 1, 2, text
                );
                func_800316F0(spr, ot, text, 2);
                *(u16 *)(spr + 4) = *(u16 *)(spr + 4) + 4;
            } else if (*(D_8009B2FC + value + 0x606A) != 0) {
                spr[0xF] = 0x68;
                *(s16 *)(spr + 8) = 0x18;
                spr[0xE] = 0xE8;
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) + 8;
                GsSortFastSprite((GsSPRITE *)spr, (GsOT *)ot, 0);
                *(s16 *)(spr + 8) = 8;
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) - 8;
            }
            Text_EncodeDecimalNoPadding(value, 3, text);
            func_800316F0(spr, ot, text, 3);
            *(u16 *)(spr + 4) = *(u16 *)(spr + 4) + 0x88;
            if (row[2] < 0x14) {
                *(s16 *)(spr + 0xE) = 0x58D0;
                GsSortFastSprite((GsSPRITE *)spr, (GsOT *)ot, 0);
                *(u16 *)(spr + 4) = *(u16 *)(spr + 4) + 8;
                Text_EncodeDecimalDigits(*(s16 *)(row - 2), 4, text);
                func_800316F0(spr, ot, text, 4);
                *(s16 *)(spr + 0xE) = 0x58D8;
                *(u16 *)(spr + 4) = *(u16 *)(spr + 4) - 0x28;
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) + 8;
                GsSortFastSprite((GsSPRITE *)spr, (GsOT *)ot, 0);
                *(u16 *)(spr + 4) = *(u16 *)(spr + 4) + 8;
                Text_EncodeDecimalDigits(*(s16 *)row, 4, text);
                func_800316F0(spr, ot, text, 4);
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) - 8;
            }
            if (idx == 0) {
                n = 3;
                *(s16 *)(spr + 4) = x + 0x107;
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) + 8;
                Text_EncodeDecimalDigits(
                    *(D_8009B2FC + value + 0x5D97), n, text
                );
                func_800316F0(spr, ot, text, n);
                v = *(D_8009B2FC + value + 0x5AC4);
                if (v < 3) {
                    if ((u32)(value - 0x11) < 5) {
                        if (v == 0) {
                            goto emit;
                        }
                    } else {
                        goto emit;
                    }
                }
                *(u32 *)(spr + 0x14) = 0x2020FF;
emit:
                n = 2;
                *(s16 *)(spr + 4) = x + 0x122;
                Text_EncodeDecimalDigits(v, n, text);
                func_800316F0(spr, ot, text, n);
                *(u32 *)(spr + 0x14) = 0x808080;
                *(u16 *)(spr + 6) = *(u16 *)(spr + 6) - 8;
            }
        }
        row += 0x10;
        i++;
        ry += 0x16;
    } while (i < 8);
}
