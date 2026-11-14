#include "../../types.h"
#include "apv2.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

extern s32 D_801694D0;
extern Apv2Message D_8016947C[];

void func_8016903C(Apv2Rect *rect, u8 *bits, u16 color, u32 range)
{
    Apv2GlyphPrim prim;
    s32 x;
    s32 y;
    s32 row;
    s32 i;
    s32 b;
    u16 lo;
    u16 hi;
    u16 *p;
    u16 zero;

    lo = range;
    hi = range >> 16;
    x = rect->x;
    y = rect->y;
    prim.tag = (((rect->w + 1) >> 1) + 3) << 24;
    prim.code = 0xA0000000;
    prim.wh = *(u32 *)&rect->w;
    for (row = 0; row < 15; row++) {
        p = prim.data;
        i = 0;
        if (D_8016947C[D_801694D0].count != 0) {
            do {
                for (b = lo; b >= hi; b--) {
                    zero = *bits >> 8;
                    *p++ = (((*bits >> b) & 1) == 0) ? zero : color;
                }
                i++;
                bits++;
            } while (i < D_8016947C[D_801694D0].count);
        }
        for (i = 0; i < 2; i++) {
            prim.xy = *(u32 *)rect;
            DrawPrim(&prim);
            rect->x++;
        }
        rect->x = x;
        rect->y++;
    }
    rect->y = y;
}
