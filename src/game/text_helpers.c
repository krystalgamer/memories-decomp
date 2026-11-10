#include "../types.h"
#include "../ygo_types.h"
#include "text_constants.h"
#include "text_decimal_key_views.h"
#include "european_text_bank_views.h"
#include "text_string_bank_views.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_init_entry.h"
#include "input.h"
#include "text_helpers.h"

/* The text and dialog helpers between the menu-record display effects and the
 * Free Duel package stage, one translation unit from Text_InitDecimalDigitGlyphMap
 * (US 0x8003B5C8) through TextStream_ReadByte (0x8003B7E0): the decimal-digit
 * glyph map the number printers index, the duel-effect entry range layout and
 * default-flag entry setup, the pad-1 confirm test the dialogs poll, and the
 * string-bank lookup and stream byte reader the text boxes run on. */

/* Copies the ten two-byte Shift-JIS digit keys from gText_DecimalDigitSjisKeys
   into a local buffer and walks them as big-endian u16 keys. Each key is
   looked up in the 4-byte-stride table at gText_aGlyphSearchEntries (key in the low halfword,
   a zero word terminating) and the 1-based match index is written to
   gText_DecimalDigitGlyphMap. The lookup is skipped entirely when the table is empty.

   gText_aGlyphSearchEntries is const so that the guard read hoists out of the loop: GCC treats
   a load from unchanging memory as loop-invariant regardless of the stores in
   the body, which is what leaves one shared %hi in the preheader feeding both
   the guard load and the per-iteration table address. */
#ifdef VERSION_EUROPE
/* European only: the 1-based index of key in the glyph lookup table (the
   entries from four bytes into gText_aGlyphLookup, code in the low
   halfword), or 0 when the zero terminator comes first. */
s32 Text_FindGlyphLookupIndex(s32 key)
{
    s32 n = 0;
    TextGlyphLookupEntry *e = gText_aGlyphLookup;
    s32 code;

    do {
        e += 1;
        code = e->halves.key;
        n++;
        if (code == 0) {
            return 0;
        }
    } while (code != key);
    return n;
}

/* The European map stores the ten digit codes themselves, read from the
   byte string at gText_DecimalDigitGlyphString: a byte from 0xF0 up starts a two-byte code
   (its low nibble is the high byte), and 0xFF ends the string early. */
void Text_InitDecimalDigitGlyphMap(void)
{
    u8 *p = gText_DecimalDigitGlyphString;
    s32 i = 0;
    s32 c = *p;

    if (c != 0xFF) {
        do {
            if (c >= 0xF0) {
                c &= 0xF;
                p++;
                c <<= 8;
                c |= *p;
            }
            p++;
            gText_DecimalDigitGlyphMap[i] = c;
            i++;
            if (i >= 10) {
                break;
            }
            c = *p;
        } while (c != 0xFF);
    }
}
#else
void Text_InitDecimalDigitGlyphMap(void) {
    TextDecimalDigitKeyBlock buf;
    u16 *out;
    u16 *q;
    const TextGlyphLookupEntry *e;
    u8 *p;
    u8 *end;
    s32 i;
    s32 n;
    s32 key;

    out = gText_DecimalDigitGlyphMap;
    i = 1;
    p = buf.bytes;
    end = buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = gText_DecimalDigitSjisKeys;

    /* Pointer ordering emits sltu; retain the retail signed address view. */
    do {
        e = gText_aGlyphSearchEntries;
        n = 1;
        key = (*p << 8) | buf.bytes[i];
        if (gText_aGlyphSearchEntries[0].packed != 0) {
            q = out;
        search:
            if (key == e->halves.key) {
                *q = n;
                goto found;
            }
            e++;
            n++;
            if (e->packed != 0) {
                goto search;
            }
        found:
            ;
        }
        out++;
        p += 2;
        i += 2;
    } while ((s32)p < (s32)end);
}
#endif

/* Only the North American build has the entry-range layout helper; the
 * Japanese and European images go straight from the digit map to
 * DuelEffect_InitEntryDefaultFlags. */
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
void DuelEffect_SetEntryRangeLayout(s32 range, s32 layout)
{
    u16 *t = gDuelEffect_awEntryRangeBoundaries;
    s32 next = range + 1;
    u16 *lo = &t[range];
    u16 *hi = &t[next];
    s32 i = *lo;

    if (i < *hi) {
        DuelEffectEntry *base = gDuelEffect_aEntries;
        DuelEffectEntry *p = &base[i];
        do {
            p->layout_selector = layout;
            p++;
            i++;
        } while (i < *hi);
    }
}
#endif

void DuelEffect_InitEntryDefaultFlags(s32 index, s32 value)
{
    DuelEffect_InitEntry(index, value, 0);
}

unsigned int Input_IsPad1ConfirmPressed(void)
{
#ifdef VERSION_JAPAN
    /* The Japanese build confirms with Circle or Square. */
    return gInput_awPadPressed[0] &
           (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE);
#else
    return gInput_awPadPressed[0] & PAD_BUTTON_CONFIRM_MASK;
#endif
}

#ifdef VERSION_EUROPE
u8 *Text_LookupString(s32 unused_word, s32 string_id)
{
    /* The European banks, as in TextBox_BuildStep. */
    if (string_id <= 0x7FFF) {
        if (string_id >= 0x500) {
            string_id -= 0x100;
            return (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) |
                gText_PrimaryStringOffsets[string_id]);
        }
        return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[string_id]);
    }
    return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
        gText_GlobalStringOffsets[string_id - 0x8000]);
}
#else
u8 *Text_LookupString(s32 unused_word, s32 string_id)
{
    s32 index = string_id;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    if (index & TEXT_GLOBAL_STRING_ID_BASE)
        return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_GlobalStringOffsets[index & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
#else
    if (index > 0xCFFF)
        return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_PrimaryStringOffsets[index - 0xD000]);
    if (index > (TEXT_GLOBAL_STRING_ID_BASE - 1))
        return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_GlobalStringOffsets[index - TEXT_GLOBAL_STRING_ID_BASE]);
#endif
    if (index >= 0x500)
        index -= 0x100;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[index]);
#else
    return (u8 *)(((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[index]);
#endif
}
#endif

u8 TextStream_ReadByte(TextStreamOwner *object)
{
    return *object->streams[object->stream_index]++;
}
