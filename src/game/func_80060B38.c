#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

extern u8 D_80090DD8[];
extern u8 D_801A8000[];
extern s32 gDuel_adwCardStats[];

extern void Text_EncodeDecimalDigits(s32, s32, u8 *);

/* Draws a deck box's six type-count digits and its seven card stat rows
   as 16x16 and 8x8 sprites into the ordering table. The glyph table
   D_80090DD8 gives the u/v and page bits of each type icon; the selected
   icon (byte +0x69) is drawn bright. For each card entry of the box
   record (D_801A8000, 24 bytes per box) that names a monster, the attack
   and defence from gDuel_adwCardStats are rendered as four decimal digits
   each (through Text_EncodeDecimalDigits), dimmed when the entry's high
   bit is set. */
void func_80060B38(u8 *obj, GsOT *ot) {
    u8 atk[8];
    u8 def[8];
    GsSPRITE *sp;
    u8 *tbl;
    u8 *rec;
    u16 *entries;
    s32 pri;
    s32 i;
    s32 j;
    s32 id;
    u8 d;

    tbl = D_80090DD8;
    sp = (GsSPRITE *)0x1F800320;
    rec = D_801A8000 + obj[0x6A] * 24;
    pri = *(s16 *)(obj + 0x14);
    sp->attribute = *(u32 *)(obj + 4);
    sp->tpage = 0xB;
    *(u32 *)&sp->r = 0x808080;
    *(u32 *)&sp->w = 0x100010;
    sp->x = *(u16 *)(obj + 0x30) + 0x22;
    sp->y = *(u16 *)(obj + 0x32) - 0x18;
    sp->cy = 0xFB;
    i = 0;
    do {
        *(u32 *)&sp->r = 0x202020;
        if (i == obj[0x69]) {
            *(u32 *)&sp->r = 0x808080;
        }
        sp->u = (tbl[0] & 0xF) * 8 - 0x80;
        sp->v = tbl[0] & 0xF0;
        sp->cx = (tbl[1] & 0xF0) | 0x200;
        GsSortFastSprite(sp, ot, pri);
        sp->x += 0x14;
        i++;
        tbl += 2;
    } while (i < 6);

    if (*(u16 *)(rec + 8) != 0) {
        sp->attribute = *(u32 *)(obj + 4);
        sp->x = *(u16 *)(obj + 0x30) + 0x24;
        *(u32 *)&sp->w = 0x80008;
        entries = (u16 *)(rec + 0xA);
        sp->y = *(u16 *)(obj + 0x32);
        sp->tpage = 0xB;
        sp->cx = 0x290;
        sp->cy = 0xFA;
        sp->u = 0x80;
        sp->v = 0x70;
        i = 0;
        do {
            id = *entries & 0xFFF;
            if (id != 0) {
                if (((gDuel_adwCardStats[id - 1] >> 26) & 0x1F) < 0x14) {
                    sp->x = *(u16 *)(obj + 0x30) + 0x24;
                    *(u32 *)&sp->r = 0x808080;
                    if (*entries & 0x8000) {
                        *(u32 *)&sp->r = 0x404040;
                    }
                    Text_EncodeDecimalDigits((gDuel_adwCardStats[id - 1] & 0x1FF) * 10, 4, atk);
                    Text_EncodeDecimalDigits(((gDuel_adwCardStats[id - 1] >> 9) & 0x1FF) * 10, 4, def);
                    for (j = 3; j >= 0; j--) {
                        d = atk[j];
                        if (d < 10) {
                            sp->u = d * 8 - 0x80;
                            GsSortFastSprite(sp, ot, pri);
                        }
                        if (def[j] < 10) {
                            sp->y += 8;
                            sp->u = def[j] * 8 - 0x80;
                            GsSortFastSprite(sp, ot, pri);
                            sp->y -= 8;
                        }
                        sp->x += 8;
                    }
                }
            }
            i++;
            sp->y += 0x16;
            entries++;
        } while (i < 7);
    }
}
