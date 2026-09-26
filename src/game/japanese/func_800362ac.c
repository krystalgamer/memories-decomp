#include "../../types.h"
#include "../../psyq/libapi.h"

#define VERSION_JAPAN
#define D_800EAF08 gJapanese_DuelEffectOccupancy
#include "../func_80036C14.h"

/* Rasterises one 16x16 glyph into a 4bpp bitmap for the Japanese text
 * boxes. The glyph comes from the game's own text records when the value
 * is negative and from the BIOS kanji ROM otherwise; it has fifteen rows of
 * sixteen bits. Each nibble of a row selects one of sixteen pixel patterns,
 * masked to the channel's colour, and a copy shifted one pixel is ORed into
 * the row below as a shadow. */
void func_800362AC(DuelEffectChannel *channel, s32 tagged_value, s32 slot, s16 *bitmap)
{
    u16 *glyph;
    u16 *lut;
    u16 *p;
    u16 *row;
    u32 w;
    s32 v;
    s32 i;

    D_800EAF08[slot] = channel->index_57 + 1;
    if (tagged_value < 0) {
        glyph = (u16 *)Text_FindRecordById(tagged_value & 0xFFFF);
    } else {
        glyph = (u16 *)Krom2RawAdd2(tagged_value & 0xFFFF);
    }
    lut = (u16 *)0x1F800000;
    w = D_80090E48[channel->field_54];
    p = lut;
    for (i = 0; i < 16; i++) {
        *p++ = D_80090E28[i] & w;
    }
    p = lut;
    row = (u16 *)0x1F800040;
    lut = (u16 *)0x1F800040;
    *(u32 *)&row[0] = 0;
    *(u32 *)&row[2] = 0;
    for (i = 15; i != 0; i--) {
        w = *glyph++;
        v = p[(w >> 4) & 0xF];
        row[0] |= v;
        row[4] = v & ~0x1111;
        row[1] |= row[4] >> 12;
        v = p[w & 0xF];
        row[1] |= v;
        row[5] = v & ~0x1111;
        row[2] |= row[5] >> 12;
        v = p[w >> 12];
        row[2] |= v;
        row[6] = v & ~0x1111;
        row[3] |= row[6] >> 12;
        v = p[(w >> 8) & 0xF];
        row[3] |= v;
        row[7] = v & ~0x1111;
        row += 4;
    }
    row = lut;
    for (i = 16; i != 0; i--) {
        ((u32 *)bitmap)[0] = ((u32 *)row)[0];
        ((u32 *)bitmap)[1] = ((u32 *)row)[1];
        row += 4;
        bitmap += 4;
    }
}
