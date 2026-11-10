/* Text commands: the stream state command, the secondary duel-effect
 * command table entries, the campaign duel start, the sound, fade and object
 * commands, the primary control-byte handlers and text-box layout helpers,
 * TextBox_BuildStep, the text-box runtime, the display-object fade helpers
 * and callbacks, entry processing and the menu record reset. In every
 * executable these are one contiguous run of text (0x80037D6C-0x80039F1C in
 * the US build). */
#include "../types.h"

#include "text_stream_commands.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "dialog_choice.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
volatile s32 gText_CommandDisposition;

void Text_SetStateFromStream(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object->playback_state = value;
    gText_CommandDisposition = 1;
}

#include "card_constants.h"
#include "duel_effect_entries.h"
#include "duel_card.h"
#include "text_constants.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "duel_effect.h"
#include "text_encode_decimal_digits.h"
#include "text_stream_read_le.h"
#include "duel_effect_command.h"
#include "../unmatched.h"

/* Zero-initialized storage this unit owns at 0x800EAFF8-0x800EB010 in the
 * US build. */
u16 gText_DecimalDigitGlyphMap[12];

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u16 gText_StagedStringId;
u8 gText_GuardianGlyphCode;
s16 gDuel_wSelectedCardID;
u8 gText_SelectedCardColorIndex;

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

/* Entries 0 through 12 of the secondary text-command table gText_SecondaryCommandHandlers,
   the handlers the F8 escape reaches, together with Text_EmitSymbolGlyph, the
   helper two of them share -- which is not itself a table entry, so this is
   fourteen definitions rather than thirteen entries. Each entry takes the
   text channel and reads its operands from the channel's live stream. Entry
   13, Text_StartCampaignDuel, is next in both the table and the image. */

#ifdef VERSION_JAPAN
/* The Japanese card-text command: no field_62 (the Japanese channel record
   ends at 0x60), a +0x100 rather than +0xD100 id for op bit 6, the stars
   without the US type and 0x17 tests, an unconditional glyph for the plain
   case, and the two-bank string lookup of the Japanese Text_LookupString. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_index;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    n = id;
    if (id & TEXT_GLOBAL_STRING_ID_BASE) {
        text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                      gText_GlobalStringOffsets[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                      gText_PrimaryStringOffsets[n]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = text;
    return;
plain:
    object->flags_34 |= 0x80;
    DuelEffect_AppendEntry(object, id);
    object->flags_34 &= 0xFF7F;
    object->pen_x += 0x10;
}
#elif defined(VERSION_EUROPE)
/* The European card-text command: the Japanese shape (no field_62 reset,
   +0x100 for op bit 6, the stars without the US type and 0x17 tests), a
   fourth selector (the card type with bit 7 set), the European string banks
   and the plain glyph flagged 0x100 rather than 0x80. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    u8 *text;
    u8 *current;

    text = (u8 *)(s32)object->stream_index;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0x100;
    } else {
        id = 0;
        switch (op & 0xF) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_1_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) & CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            break;
        case 3:
            id = ((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                   CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) | 0x80;
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    if (id <= 0x7FFF) {
        if (id >= 0x500) {
            s32 n = id - 0x100;

            text = (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                          gText_PrimaryStringOffsets[n]);
        } else {
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                          gText_PrimaryStringOffsets[id]);
        }
    } else {
        text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                      gText_GlobalStringOffsets[id - 0x8000]);
    }
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = text;
    return;
plain:
    object->flags_34 |= 0x100;
    DuelEffect_AppendEntry(object, id);
    object->flags_34 &= 0xFEFF;
    object->pen_x += 0x10;
}
#else
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object)
{
    s32 op;
    s32 id;
    s32 n;
    s32 kind;
    s32 stats;
    s32 type;
    u8 *text;
    u8 *current;
    u8 **slot;

    text = (u8 *)(s32)object->stream_index;
    object->field_62 = 0;
    text = (u8 *)((u32)text * 4);
    {
        u8 *stream = (u8 *)object;

        stream += (u32)text;
        text = stream;
        current = *(u8 **)text;
        op = *current++;
        *(u8 **)text = current;
    }
    n = 0;
    if (op & 0x10) {
        object->color_index_54 = gText_SelectedCardColorIndex;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0xD100;
    } else {
        kind = op & 0xF;
        id = 0;
        switch (kind) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            stats = gDuel_adwCardStats[gDuel_wSelectedCardID - 1];
            id = (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            type = (stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            id += 0x17;
            if ((u32)(type - CARD_TYPE_MAGIC) < CARD_NON_MONSTER_TYPE_COUNT) {
                object->field_62 = type;
            }
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            if (id == 0x17) {
                n = 1;
            }
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object->stream_index++;
    n = id;
    if (id > 0xCFFF) {
        text = (u8 *)((u32)gText_PrimaryStringOffsets & 0xFFFF0000) +
               gText_PrimaryStringOffsets[id - 0xD000];
    } else if (id > 0x7FFF) {
        text = (u8 *)((u32)gText_GlobalStringOffsets & 0xFFFF0000) +
               gText_GlobalStringOffsets[id - 0x8000];
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)((u32)gText_PrimaryStringAddressAnchor & 0xFFFF0000) + gText_PrimaryStringOffsets[n];
    }
store:
    slot = &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    *slot = text;
    return;
plain:
    object->flags_34 |= 0x80;
    if ((u8)n == 0) {
        DuelEffect_AppendEntry(object, id);
    }
    object->flags_34 &= 0xFF7F;
    object->pen_x += 0x10;
}
#endif

/* The channel flag bit Text_EmitSymbolGlyph holds across its call; 0x100 in the
 * European build, whose wrapper defines it. */
#ifndef DUEL_EFFECT_SELECTOR_FLAG
#define DUEL_EFFECT_SELECTOR_FLAG 0x80
#endif

void Text_EmitSymbolGlyph(DuelEffectChannel *object, s32 value)
{
    *(u8 *)&object->flags_34 = *(u8 *)&object->flags_34;
    object->flags_34 |= DUEL_EFFECT_SELECTOR_FLAG;
    DuelEffect_AppendEntry(object, value);
    object->flags_34 &= 0xFFFF ^ DUEL_EFFECT_SELECTOR_FLAG;
    object->pen_x += 0x10;
}

void Text_EmitGuardianStarGlyph(DuelEffectChannel *object)
{
    Text_EmitSymbolGlyph(object, gText_GuardianGlyphCode);
}

void Text_EmitNextSymbolGlyph(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];

    Text_EmitSymbolGlyph(object, *(*stream)++);
}

void Text_ResetPenXAndAdvanceY(DuelEffectChannel *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    object->pen_x = 0;
    stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    object->pen_y += (s8)value;
}

void Text_AdvancePenX(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    object->pen_x += value;
}

void Text_PushFormattedDecimal(DuelEffectChannel *object)
{
    u8 buf[8];
    u8 *e;
    u8 *bp;
    s32 r;
    s32 c;
    s32 t;
    s32 k;
    s32 i;
    s32 h;
    s32 w;

    r = TextStream_ReadU32LE(TEXT_STREAM_OWNER(object));
    t = *TEXT_STREAM_OWNER(object)->streams[object->stream_index]++;
    c = t;
    Text_EncodeDecimalDigits(*(s32 *)r, c & 0xF, buf);

    h = 0;

    if ((c & 0x80) != 0) {
        if ((c & 0x40) == 0) {
            goto skip;
        }
        h = *(u16 *)&gText_DecimalDigitGlyphMap[0];
        e = object->decimal_buffer;
        goto write;
    }

    if (c < 2) {
        e = object->decimal_buffer;
        goto write;
    }

    bp = buf;
    k = c - 1;
    while (1) {
        if (bp[k] < TEXT_DECIMAL_RADIX) {
            break;
        }
        c = k;
        if (k < 2) {
            break;
        }
        k = c - 1;
    }

skip:
    e = object->decimal_buffer;

write:
    i = (c & 0xF) - 1;
    do {
        w = h;
        if (buf[i] < TEXT_DECIMAL_RADIX) {
            w = *(u16 *)&gText_DecimalDigitGlyphMap[buf[i]];
        }
        if (w >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            *e = (w >> 8) - 0x10;
            e[1] = w;
            e += 2;
        } else {
            *e = w;
            e += 1;
        }
        i--;
    } while (i >= 0);

    *e = TEXT_STRING_TERMINATOR;
    object->stream_index++;
    TEXT_STREAM_OWNER(object)->streams[object->stream_index] = object->decimal_buffer;
}

/* Inlining keeps the stream value and channel in independent live ranges. */
static __inline__ u32 read_operand(DuelEffectChannel *object)
{
    u8 **stream =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    u8 *cursor = *stream;
    u32 value = *cursor++;

    *stream = cursor;
    return value;
}

void Text_SetFontMode(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    u8 value;
#else
    u32 value;
#endif

#ifdef VERSION_EUROPE
    object->flags_34 &= 0xFFFC;
#else
    object->flags_34 &= 0xFEFF;
#endif
    value = read_operand(object);
#ifdef VERSION_JAPAN
    object->line_advance_y = value << 3;
    object->glyph_advance_x = value << 3;
#elif defined(VERSION_EUROPE)
    /* The European release has four text sizes and keeps the size in the
       channel's two low flag bits. */
    switch (value) {
    case 0:
        object->glyph_advance_x = 8;
        object->line_advance_y = 16;
        break;
    case 1:
        object->glyph_advance_x = 8;
        object->line_advance_y = 8;
        object->flags_34 |= 1;
        break;
    case 2:
        object->glyph_advance_x = 12;
        object->line_advance_y = 16;
        object->flags_34 |= 2;
        break;
    case 3:
        object->glyph_advance_x = 16;
        object->line_advance_y = 16;
        object->flags_34 |= 3;
        break;
    }
#else
    switch (value) {
    case 1:
        object->glyph_advance_x = 8;
        object->line_advance_y = 8;
        break;
    case 2:
        object->glyph_advance_x = 8;
        object->line_advance_y = 12;
        break;
    }
#endif
#ifndef VERSION_EUROPE
    if (value == 1)
        object->flags_34 |= 0x100;
#endif
}

void Text_SetGlyphCellSize(DuelEffectChannel *object)
{
    /* Separate lifetimes preserve allocation across the two stream reads. */
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->glyph_advance_x = value;
    }
    {
        u8 **stream =
            &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
        u8 *current = *stream;
        u8 value = *current++;

        *stream = current;
        object->line_advance_y = value;
    }
}

void Text_SetPenX(DuelEffectChannel *object)
{
    object->pen_x = TextStream_ReadU16LE(object);
}

#ifdef VERSION_EUROPE
/* A European-only command, next to Text_SetPenX in the image: the same
   read into field_3A. */
void Text_SetPenY(DuelEffectChannel *object)
{
    object->pen_y = TextStream_ReadU16LE(object);
}
#endif

void Text_SetOutputLimit(DuelEffectChannel *object)
{
#ifdef VERSION_EUROPE
    /* The European build clears the pixel counter and sets the limit
       (halfwords at 0x60 and 0x62) from one operand byte, times eight. */
    *(u16 *)&object->output_count = 0;
    *(u16 *)&object->field_62 =
        read_operand(object) << 3;
#else
    object->output_count = 0;
    object->output_limit = TextStream_ReadU16LE(object);
#endif
}

u8 **Text_PushStagedString(DuelEffectChannel *a0) {
    DuelEffectChannel *a3 = a0;
#ifdef VERSION_JAPAN
    s32 a2 = (s16)gText_StagedStringId;
#else
    s32 a2 = gText_StagedStringId;
#endif
    u32 v1;
    u8 counter;
    u8 **slot;

#ifdef VERSION_JAPAN
    if (a2 & TEXT_GLOBAL_STRING_ID_BASE) {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
             gText_GlobalStringOffsets[a2 & (TEXT_GLOBAL_STRING_ID_BASE - 1)];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[a2];
    }
#elif defined(VERSION_EUROPE)
    /* The European banks, as in TextBox_BuildStep. */
    if (a2 <= 0x7FFF) {
        if (a2 >= 0x500) {
            s32 n = a2 - 0x100;

            v1 = ((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[n];
        } else {
            v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[a2];
        }
    } else {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_GlobalStringOffsets[a2 - 0x8000];
    }
#else
    if (a2 > 0xCFFF) {
        v1 = ((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_PrimaryStringOffsets[a2 - 0xD000];
    } else if (a2 > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
        v1 = ((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
             gText_GlobalStringOffsets[a2 - TEXT_GLOBAL_STRING_ID_BASE];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) + gText_PrimaryStringOffsets[a2];
    }
#endif

    counter = *(u8 *)&a3->stream_index + 1;
    *(u8 *)&a3->stream_index = counter;
    slot = &TEXT_STREAM_OWNER(a3)->streams[(s8)counter];
    *slot = (u8 *)v1;
    return slot;
}

void Text_SetColorIndex(DuelEffectChannel *object)
{
    u8 **slot =
        &TEXT_STREAM_OWNER(object)->streams[object->stream_index];
    u8 *q = *slot;
    s32 v = *q;
    s32 w;

    *slot = q + 1;
    w = v;
    if (v & 0x80) {
        w = gText_abColorSlots[v & 0xF];
    }
    object->color_index_54 = w;
}

void Text_SetFastPrintMode(DuelEffectChannel *object)
{
    register DuelEffectChannel *obj;
    register u8 **stream;
    register u8 *current;
    register u32 value;

    obj = object;
    obj->flags_34 &= 0xEFFF;
    stream = &TEXT_STREAM_OWNER(obj)->streams[obj->stream_index];
    current = *stream;
    value = *current;
    current++;
    *stream = current;
    if (value)
        obj->flags_34 |= 0x1000;
}

#include "text_start_campaign_duel.h"
#include "duel_effect.h"
#include "text_stream_read_le.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "../unmatched.h"
#include "duel_session_controls.h"

#include "ai_opponent_data.h"
#include "duel_terrain_boost.h"
#include "main_mode_state.h"

#define TEXT_STREAM_OWNER_VIEW(object) ((TextStreamOwner *)(object))

/*
 * Each byte read owns its slot pointer, advancing cursor and unsigned value.
 * Those separate lifetimes recover all 352 bytes under uniform G0 without
 * register bindings. Sharing the three locals instead changes the index
 * register in all four reads (twelve words).
 *
 * The opponent is reloaded unsigned after the opaque word reader, then
 * explicitly narrowed for the two signed-byte equality tests.
 */
void Text_StartCampaignDuel(DuelEffectChannel *o)
{
    s32 x;
    gDuel_bSide0ControllerSelector[0] = -1;
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bOpponentID = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_awCampaignSceneByOutcome[0] = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_awCampaignSceneByOutcome[1] = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bTerrain = v;
    }
    gDuel_wBgmId = TextStream_ReadU16LE(o);
    gDuel_wAnimatedBattleBgmCommand = 0x72A0;
    x = (u8)gDuel_bOpponentID;
    if ((u32)(x - 9) < 8) gDuel_wAnimatedBattleBgmCommand = 0x7280;
    if ((s8)x == 0x11) gDuel_wAnimatedBattleBgmCommand = 0x7290;
    if ((s8)x == 0x26) gDuel_wAnimatedBattleBgmCommand = 0x72B0;
    gDuel_bPostDuelMainMode = 2;
    gDuel_bResumeFromAnimatedBattle = 0;
    gMain_bActiveModeState = 3;
}

/* Seven contiguous duel-effect command handlers from DuelEffect_PlayBgmCommand
 * (0x80038690) through Text_OpenCampaignMap. The four sound handlers read their
 * operands from the channel's selected stream and drive SD_BGMPlay,
 * SD_SEPlayFull or the BGM fade; then come the two empty table entries and
 * the handler that reads one byte into gCampaignMap_bEntryLocation and moves the scene mode
 * to 5. */

#include "secondary_scene_commands.h"
#include "duel_effect_sound_commands.h"
#include "text_stream_read_le.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_play_sound_command.h"
#include "duel_effect_fade_out_bgm_command.h"
#include "main_mode_state.h"
#include "../unmatched.h"
#include "sound_bgm_commands.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s16 gText_CdSoundPollBudget;

void DuelEffect_PlayBgmCommand(DuelEffectChannel *object)
{
    SD_BGMPlay((u16)TextStream_ReadU16LE(object));
}

void DuelEffect_ProcessBgmCommand(DuelEffectChannel *object)
{
    u8 **slot =
        &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *stream = *slot;
    s32 command = *stream;
    s32 op;

    *slot = stream + 1;
    op = command;
    if (op & 0x3F) {
        SD_BGMPlay(TextStream_ReadU16LE(object) & 0xFFFF);
    } else {
        if (op & 1) {
            SD_BGMPlay(gSD_dwSavedBgmCommand);
        }
        if (op & 2) {
            gSD_dwSavedBgmCommand = TextStream_ReadU16LE(object) & 0xFFFF;
        }
        if (op & 4) {
            gSD_dwSavedBgmCommand = gSD_dwCurrentBgmCommand;
        }
    }
    if (op & 0x80) {
        object->playback_state = 0xC;
        gText_CommandDisposition = 1;
    }
}

void DuelEffect_PlaySoundCommand(DuelEffectChannel *object) {
    u32 value = TextStream_ReadU16LE(object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        SD_CdSoundPlay((u16)value);
        gText_CdSoundPollBudget = TextStream_ReadU16LE(object);
        object->playback_state = 0x11;
        gText_CommandDisposition = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}

void DuelEffect_FadeOutBgmCommand(DuelEffectCommand *command)
{
    u8 **slot;
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    u32 opcode;
    u32 high_bit;

    slot = &((TextStreamOwner *)command)->streams[command->cursor_slot];
    cursor = *slot;
    raw = *cursor++;
    saved_raw = raw;
    opcode = saved_raw & 0x7F;
    *slot = cursor;

    if (opcode == 0) {
        SD_BGMFadeOut();
        goto shared_test;
    } else {
        SD_BGMFadeOutWithStep(opcode);
    }

shared_test:
    high_bit = saved_raw & 0x80;
    if (high_bit != 0) {
        command->command_state = 13;
        gText_CommandDisposition = 1;
    }
}

void Text_NoOpSecondary12(void)
{
}

void Text_NoOpSecondary13(void)
{
}

void Text_OpenCampaignMap(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 value = *(*stream)++;
    gMain_bActiveModeState = 5;
    gCampaignMap_bEntryLocation = value;
}

#include "fade.h"
#include "fade_overlay_controls.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "duel_effect_sound_commands.h"

void Text_ApplyFadeCommand(DuelEffectChannel *object)
{
    u8 **cursor = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *stream = *cursor;
    s32 command = *stream;
    s32 opcode;

    *cursor = stream + 1;
    opcode = command;
    if (opcode & 0x40) {
        gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] + 9;
    }
    if (opcode & 0x20) {
        gFade_bKeepOverlayDepth = 4;
    }
    if (opcode & 0x10) {
        if (opcode & 1) {
            Fade_InitOutColor(0xFFFFFF);
        } else {
            Fade_InitInColor(0xFFFFFF);
        }
        gFade_State.step = 4;
    } else if (opcode & 1) {
        Fade_StartOutKeepOverlay();
    } else {
        Fade_StartInKeepOverlay();
    }
    if (opcode & 0x80) {
        Fade_Wait();
    }
}

#include "text_stream_read_le.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_object_commands.h"
/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gText_BranchSelectorB;
u8 gText_BranchSelectorA;

#ifdef VERSION_EUROPE
#include "save_data.h"
#include "player_name_glyphs.h"

#endif

/* Regional value: the channel flag Text_DisableCompletionWait clears (8 -> 0x10). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CLEARED_CHANNEL_FLAG
#define DUEL_EFFECT_CLEARED_CHANNEL_FLAG 8
#endif

void Text_DisableCompletionWait(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~DUEL_EFFECT_CLEARED_CHANNEL_FLAG;
}

#ifndef DUEL_EFFECT_STREAM_HIGH_MASK
#define DUEL_EFFECT_STREAM_HIGH_MASK 0xFFFF0000
#endif

void Text_JumpBySelectorA(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_index] += gText_BranchSelectorA * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_index] =
        (u8 *)(((u32)owner->streams[object->stream_index] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

void Text_JumpBySelectorB(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_index] += gText_BranchSelectorB * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_index] =
        (u8 *)(((u32)owner->streams[object->stream_index] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

void Text_UnlockDuelist(DuelEffectChannel *object)
{
    s16 duelist_id;
    u32 value;
    u8 **stream;
    u8 *cursor;

    stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    cursor = *stream;
    value = *cursor++;
    duelist_id = value;
    *stream = cursor;
    if (duelist_id > 0) {
        Library_UpdateCardUsedFlag(
            duelist_id + CAMPAIGN_FLAG_DUELIST_DEFEATED_BASE);
        Library_UpdateCardUsedFlag(duelist_id + FREE_DUEL_UNLOCK_FLAG_BASE);
    }
}

void Text_CloseChoice(DuelEffectChannel *object)
{
    DisplayObject_ReleaseIfPresent(object->prompt_object);
    object->prompt_object = 0;
    object->playback_state = 2;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
    gText_CommandDisposition = 1;
}

#ifdef VERSION_EUROPE
/* European only: pushes the player name's glyph codes, gText_aCurrentPlayerNameGlyphs, as the
   next text stream. */
void Text_PushPlayerNameStream(DuelEffectChannel *object)
{
    ((TextStreamOwner *)object)->streams[object->stream_index + 1] = gText_aCurrentPlayerNameGlyphs;
    object->stream_index++;
}
#endif

/* The text stream's primary control-byte handlers and the two text-box
   layout helpers after them, contiguous from Text_DispatchSecondaryCommand
   (0x80038B4C) through TextBox_RefreshDisplayObjects.

   The nine handlers are the ones TextBox_BuildStep reaches through
   gText_apfnPrimaryCommands for bytes F6 and F8 through FF: the F8 escape into the
   secondary table, the cursor set, the choice command, the page wait and
   campaign-flag command, the stream push, the new line and end of stream,
   and the display-effect command. See notes/text-control-bytecode.md. The
   other two primary handlers are Text_ExtendGlyphCode, in
   src/game/text_extend_glyph_code.c, and Text_SetStateFromStream, at the
   start of this file.

   The layout helpers are TextBox_UpdateOutline, which TextBox_SetPos calls to lay
   the box's second object out as an outline around the first, and
   TextBox_RefreshDisplayObjects, which TextBox_BuildStep calls to create or refresh the
   box's frame object. */

#include "duel_effect_command_table.h"
#include "duel_effect.h"
#include "text_stream_read_le.h"
#include "text_stream_commands.h"
#include "duel_effect_entry_occupancy.h"
#include "dialog_choice.h"
#include "campaign_flags.h"
#include "text_box_wrap_line.h"
#include "display_effect_lifecycle.h"

#include "menu_record.h"
#include "display_menu_records.h"
#include "campaign_scene_package.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "color_constants.h"
#include "text_box_draw_entries.h"
#include "text_box_runtime.h"
#include "text_control_commands.h"
#include "../unmatched.h"
#include "dialog_choice_control.h"

/* Zero-initialized storage this unit owns at 0x800EB010-0x800EB0F8 in the
 * US build. */
MenuRecord gDisplayEffect_aMenuRecords[3];

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
MenuRecord *gText_DisplayEffectRecord;
void (*gText_ChoiceLayoutCallback)(volatile DuelEffectChannel *object);
u8 gDialog_bInputState;
u8 gDialog_bChoiceEnabled;
u8 gDialog_bChoiceControl;
s8 gDialog_bChoiceCount;
s8 gDialog_bChoice;

#ifndef TEXT_STREAM_CURSOR_HIGH_MASK
#define TEXT_STREAM_CURSOR_HIGH_MASK 0xFFFF0000
#endif

void Text_DispatchSecondaryCommand(DuelEffectChannel *object)
{
    u8 **pp = &TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index];
    u8 *p = *pp;
    s32 op = *p;

    *pp = p + 1;
    gText_SecondaryCommandHandlers[op](object);
}

  void Text_SetCursorOffset(DuelEffectChannel *o){int v; u8 **p;v=TextStream_ReadU16LE(o);p=&TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_index];*p=(u8 *)(((u32)*p&TEXT_STREAM_CURSOR_HIGH_MASK)|(v&0xFFFF));}

void Text_HandleChoiceCommand(DuelEffectChannel *object)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    gText_CommandDisposition = 1;
    t = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index]++;
    c = t;
    d = TEXT_CHOICE_OPTIONS_ENABLED_MASK;
    if (c & TEXT_CHOICE_COMMAND_HAS_OPTIONS) {
        u = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index]++;
        t = u;
        d = t;
    }
    if (c & TEXT_CHOICE_COMMAND_BRANCH) {
        TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_index] +=
            gDialog_bChoice * 2;
        Text_SetCursorOffset(object);
    } else {
        gDialog_bChoiceCount = TEXT_CHOICE_COMMAND_COUNT_MASK;
        gDialog_bChoiceCount = c & gDialog_bChoiceCount;
        gDialog_bChoiceControl = c & DIALOG_CHOICE_CONTROL_MASK;
        gDialog_bChoiceEnabled = d & TEXT_CHOICE_OPTIONS_ENABLED_MASK;
        w = d & TEXT_CHOICE_OPTIONS_INPUT_FLAG;
        gDialog_bChoice = 0;
        gDialog_bInputState = 0;
        if (w != 0) {
            gDialog_bInputState = 1;
        }
#ifndef VERSION_EUROPE
        /* The European build no longer clears the channel's occupancy value
           here, as in TextBox_Destroy. */
        DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
        DuelEffect_ClearMatchingMarker(object->channel_index);
        v = object->flags_34;
        object->explicit_newline_count = 0;
        gText_ChoiceLayoutCallback = Text_TryCompleteChoiceLayout;
        object->flags_34 = v | 0x1000;
    }
}

void Text_StartPageWait(DuelEffectChannel *value)
{
    value->playback_state = 4;
    gText_CommandDisposition = 1;
}

void Text_HandleCampaignFlagCommand(DuelEffectChannel *object)
{
    s32 flag = TextStream_ReadU16LE(object);

    flag &= CAMPAIGN_FLAG_COMMAND_WORD_MASK;
    if (flag & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(flag & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
        return;
    }

    {
        s32 target = TextStream_ReadU16LE(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor = (s32 *)(
                (u32)object + (u32)&((u8 **)0)[object->stream_index]);

            *cursor = (*cursor & TEXT_STREAM_CURSOR_HIGH_MASK) | target;
        }
    }
}

void Text_PushStreamOffset(DuelEffectChannel *arg0)
{
    TextStreamOwner *owner = TEXT_STREAM_OWNER_VIEW(arg0);
    s32 c;
    s32 v;

    v = TextStream_ReadU16LE(arg0);
    c = arg0->stream_index;
    owner->streams[c + 1] =
        (u8 *)(((u32)owner->streams[c] & TEXT_STREAM_CURSOR_HIGH_MASK) |
               (v & 0xFFFF));
    arg0->stream_index++;
}

void Text_NewLine(DuelEffectChannel *record)
{
    record->explicit_newline_count++;
    record->pen_x = 0x1000;
#ifdef VERSION_EUROPE
    /* The European build moves to state 4 only from the fourth line on. */
    if (TextBox_WrapLineIfNeeded(record) && record->explicit_newline_count >= 4) {
#else
    if (TextBox_WrapLineIfNeeded(record)) {
#endif
        record->playback_state = 4;
    }
    gText_CommandDisposition = 1;
    if (gText_ChoiceLayoutCallback) {
        gText_ChoiceLayoutCallback(record);
    }
}

void Text_EndStream(DuelEffectChannel *record)
{
    u16 flags;

    record->stream_index--;
    if (record->stream_index < 0) {
        flags = record->flags_34;
        gText_CommandDisposition = 1;
        flags |= 0x2000;
        record->flags_34 = flags;
    }
}

/* Effect-script command handler: reads a command id and a flag byte from the
   object's current script stream, finds the display effect record for the id
   in gDisplayEffect_aMenuRecords (ids below 0x41 live in the first two records, others in
   the third), and depending on the flags either stops it (bit 7), adjusts a
   running one (bits 5 and 6) or starts it in the slot given by bit 0, with
   the object's state byte set to the matching wait state. */
void Text_HandleDisplayEffectCommand(EffectObject *o) {
    MenuRecord *e;
    s32 id;
    s32 flags;
    s32 slot;
    s32 a;
    s32 b;

    gText_CommandDisposition = 1;
    a = *o->streams[o->depth]++;
    b = *o->streams[o->depth]++;
    id = a;
    flags = b;

    e = gDisplayEffect_aMenuRecords;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e += 2;
    } else if (e->effect_id != id) {
        e++;
        if (e->effect_id != id) {
            e = 0;
        }
    }

    if (flags & 0x80) {
        if (e == 0) {
            return;
        }
        gText_DisplayEffectRecord = e;
        if (flags & 2) {
            DisplayEffect_ReleaseRecord(e);
            return;
        }
        if (flags & 1) {
            o->state = 0xE;
            return;
        }
        if (e->slot_index != 0) {
            e->field_40 = 0x178;
        } else {
            e->field_40 = -0x38;
        }
        e->tween_target_y = 0xB2;
        e->field_44 = 0x10;
        o->state = 7;
        e->display_effect_step = 3;
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            e->display_effect_step = 5;
            e->field_40 = 1;
        }
        return;
    }
    if (flags & 0x60) {
        if (e == 0) {
            return;
        }
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            return;
        }
        gText_DisplayEffectRecord = e;
        if (flags & 0x40) {
            e->resource_entry_index = flags & 3;
            o->state = 9;
            return;
        } else if (flags & 0x20) {
            e->variant_update_flags &= 0xEF;
            if (flags & 1) {
                e->variant_update_flags |= 0x10;
            }
            return;
        }
    }

    slot = flags & 1;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        slot = 2;
    }
    e = &gDisplayEffect_aMenuRecords[slot];
    DisplayEffect_ResetRecordState((DisplayEffectState *)e);
    e->effect_id = id;
    e->slot_index = slot;
    if (slot != 0) {
        *(s16 *)&e->x = 0x178;
    } else {
        *(s16 *)&e->x = -0x38;
    }
    o->state = 6;
    gText_DisplayEffectRecord = e;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e->display_effect_step = 5;
        e->slot_index = 2;
        *(s16 *)&e->x = 0xF0;
        e->field_40 = 0;
        *(s16 *)&e->y = 0x60;
        return;
    }
    e->display_effect_step = 2;
    e->field_40 = 3;
    if (flags & 8) {
        *(s16 *)&e->x = 0x400;
        e->field_40 = 7;
    }
    if (flags & 0x10) {
        e->resource_entry_index = (flags >> 1) & 3;
    }
}

/* Lays the text box's second object out as a six-point outline around the
 * first, eight pixels outside its top, sides, and bottom. */
void TextBox_UpdateOutline(struct DuelEffectChannel *record) {
    DisplayObject *a = record->render_object, *b = record->aux_object;
    u16 v;
    b->field_18.center.x = a->field_18.center.x;
    b->field_18.center.y = a->field_18.center.y;
    v = a->xy.h.x;
    b->field_48.vertex.x = v;
    b->position.h.field_28 = v;
    b->field_38.h.field_38 = v - 8;
    v = a->xy.h.x + a->field_3C.size.w;
    b->field_50.h.field_50 = v;
    b->xy.h.x = v;
    b->field_40.vertex.x = v + 8;
    v = a->xy.h.y - 8;
    b->xy.h.y = v;
    b->position.h.field_2A = v;
    v = a->xy.h.y + a->field_48.pivot.y;
    b->field_40.vertex.y = v;
    b->field_38.h.field_3A = v;
    v = a->xy.h.y + a->field_3C.size.h + 8;
    b->field_50.h.field_52 = v;
    b->field_48.vertex.y = v;
}

/* Regional value: the flags_34 bit under which the box rebuilds its gouraud
 * quad (0x20 -> 0x40). The European build (src/game/european/) defines its
 * own. */
#ifndef TEXT_BOX_LAYOUT_QUAD_FLAG
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x20
#endif

#ifndef TEXT_BOX_GLYPH_CLUT_X
#define TEXT_BOX_GLYPH_CLUT_X 0x280
#endif

#ifndef TEXT_BOX_GLYPH_CLUT_Y
#define TEXT_BOX_GLYPH_CLUT_Y 0xE8
#endif

void TextBox_RefreshDisplayObjects(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 v;
    s32 b;
    s32 f;

    e = p->render_object;
    if (e == (DisplayObject *)0) {
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 6);
        b = p->channel_index;
        e->field_40.clut.x = TEXT_BOX_GLYPH_CLUT_X;
        e->field_40.clut.y = TEXT_BOX_GLYPH_CLUT_Y;
        e->texture_page = 0xA;
        f = e->flags;
        e->field_4C = (s32)TextBox_DrawEntries;
        e->field_67 = b;
        e->flags = f | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->render_object = e;
    }

    DisplayObject_SelectOrderingTable1(e);
    DisplayObject_SetDepthOffset(e, (s8)p->depth_offset);

    e->xy.h.x = (u16)p->rect_x;
    e->xy.h.y = (u16)p->rect_y;
    e->field_3C.size.w = (u16)p->rect_width;
    e->field_3C.size.h = (u16)p->rect_height;
    v = p->rect_width / 2;
    e->field_18.center.x = v;
    e->field_48.pivot.x = v;
    v = p->rect_height / 2;
    e->field_18.center.y = v;
    e->field_48.pivot.y = v;

    if ((p->flags_34 & TEXT_BOX_LAYOUT_QUAD_FLAG) != 0) {
        if (p->aux_object != 0) {
            DisplayObject_ReleaseIfPresent(p->aux_object);
        }
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
        DisplayObject_InitializeGouraudQuad(e, 1);
        p->aux_object = e;
        e->attribute = e->attribute | (GsALON | GsATWO);
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(p->depth_offset - 1));
        *(s32 *)&e->field_54 = 0xA0A0A0;
        e->field_4C = 0xA0A0A0;
        e->field_34.word = 0xA0A0A0;
        e->field_2C.word = 0xA0A0A0;
        e->field_44.word = COLOR_RGB24_NEUTRAL_GREY;
        e->field_3C.word = COLOR_RGB24_NEUTRAL_GREY;
        TextBox_UpdateOutline(p);
    }
}

#include "text_box_wrap_line.h"
#include "input.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_state_callbacks.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_entry_occupancy.h"
#include "text_constants.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "display_object_core.h"
#include "text_box_runtime.h"
#include "duel_effect_entries.h"
#include "menu_record_reset.h"
#include "script_command_table.h"
#include "script_state.h"
#include "text_box_state_callbacks.h"
#include "text_stream_commands.h"
#include "dialog_choice_state.h"
#include "../unmatched.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s16 gText_PumpedGlyphIndex;
u8 gText_ScriptDispatchGate;

/* Initialized data this unit owns at 0x80090E64: the text-box state callback table.
 *
 * TextBox_BuildStep indexes this table with the low five bits of state byte
 * 0x51. The image contains exactly eighteen entries; the masked index can
 * exceed that bound, so the public declaration remains unsized.
 */

TextBoxStateCallback gTextBox_apfnStateCallbacks[] = {
    Dialog_UpdateChoice,
    Dialog_UpdateChoice,
    Text_CompletePageAdvance,
    Text_BeginPageClear,
    Text_WaitForPageAdvance,
    Text_StageScriptImage,
    Text_ContinueDisplayEffect,
    Text_WaitForDisplayEffectAndRelease,
    Text_WaitForDisplayEffect,
    Text_RequestDisplayEffectCrossfade,
    TextBox_WaitForScriptCompletion,
    Text_StartViewportTween,
    DuelEffect_WaitForFileTransfer,
    Text_WaitForBgmFade,
    Text_RequestDisplayEffectFadeOut,
    Text_WaitForDelay,
    Text_WaitWithViewportShake,
    Text_WaitForCdSoundTransfer,
};

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   opcode store below a single %gp_rel instruction whose load-delay slot needs
   the retail nop. The address comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here.  volatile is what keeps the
   read-back after the store, which retail issues at every use. */
volatile u16 gText_CurrentStreamCode;

/* The glyph counter and its limit, and the buttons that skip ahead. PAL
   views both as halfwords and the Japanese release confirms with Circle
   rather than Cross, so a regional build supplies its own. */
#ifndef TEXT_BOX_GLYPH_COUNT
#define TEXT_BOX_GLYPH_COUNT(o) ((o)->output_count)
#define TEXT_BOX_GLYPH_LIMIT(o) ((o)->output_limit)
#endif
#ifndef TEXT_BOX_ENTRY_TYPE
#define TEXT_BOX_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef TEXT_BOX_ADVANCE_MASK
#define TEXT_BOX_ADVANCE_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* The flag that keeps the box's previous contents: 2 in the US build, 4 in
   the European one, whose wrapper defines its own. */
#ifndef TEXT_BOX_BUILD_KEEP_FLAG
#define TEXT_BOX_BUILD_KEEP_FLAG 2
#endif
/* The flag that keeps the channel's entries: 0x40, 0x80 in the European
   build. */
#ifndef TEXT_BOX_BUILD_OWN_ENTRIES_FLAG
#define TEXT_BOX_BUILD_OWN_ENTRIES_FLAG 0x40
#endif

void TextBox_BuildStep(DuelEffectChannel *object)
{
    u16 flags;
    s32 id;
    u8 *text;
    u8 **slot;
    u8 *script;
    DuelEffectEntry *entry;
    s32 op;
    void (**handlers)(u8 *);

    flags = object->flags_34;
    if ((flags & TEXT_BOX_FLAG_BUILD_ACTIVE) == 0) {
        flags |= TEXT_BOX_FLAG_BUILD_ACTIVE;
        object->flags_34 = flags;
        if ((flags & TEXT_BOX_BUILD_KEEP_FLAG) == 0) {
            DisplayEffect_ResetMenuRecords();
        }
#ifndef VERSION_EUROPE
        /* The European build makes neither field store. */
        if ((object->flags_34 & 0x100) != 0) {
            object->line_advance_y = 8;
            object->glyph_advance_x = 8;
        }
#endif
        id = object->initial_string_id;
        gText_ScriptDispatchGate = 0;
        gText_ChoiceLayoutCallback = 0;
        object->delay_countdown = 1;
        TEXT_BOX_GLYPH_COUNT(object) = 0;
        object->stream_index = 0;
#ifdef VERSION_JAPAN
        /* The Japanese two-bank layout, as in its Text_LookupString. */
        if (id & TEXT_GLOBAL_STRING_ID_BASE) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_GlobalStringOffsets[id & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
                gText_PrimaryStringOffsets[id]);
        }
#elif defined(VERSION_EUROPE)
        /* The European banks: global strings from 0x8000, ids below 0x500
           in the first bank and the rest in the second, all through the
           tables four bytes into each bank. */
        if (id <= 0x7FFF) {
            if (id >= 0x500) {
                s32 n = id - 0x100;

                text = (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[n]);
            } else {
                text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                    gText_PrimaryStringOffsets[id]);
            }
        } else {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - 0x8000]);
        }
#else
        if (id > 0xCFFF) {
            text = (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id - 0xD000]);
        } else if (id > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
            text = (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) +
                gText_GlobalStringOffsets[id - TEXT_GLOBAL_STRING_ID_BASE]);
        } else {
            if (id >= 0x500) {
                id -= 0x100;
            }
            text = (u8 *)(((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) +
                gText_PrimaryStringOffsets[id]);
        }
#endif
        object->root_stream = text;
        object->explicit_newline_count = 0;
        object->playback_state = 0;
        DisplayObject_ReleaseIfPresent(object->prompt_object);
        DisplayObject_ReleaseIfPresent(object->aux_object);
        object->prompt_object = (void *)0;
        object->aux_object = 0;
        TextBox_RefreshDisplayObjects(object);
        if ((object->flags_34 & TEXT_BOX_BUILD_OWN_ENTRIES_FLAG) == 0) {
            entry = (DuelEffectEntry *)&((TEXT_BOX_ENTRY_TYPE *)gDuelEffect_aEntries)[
                object->entry_range_start];
            object->entry_begin = entry;
            object->entry_write_cursor = entry;
#ifndef VERSION_EUROPE
            /* The European build no longer clears the channel's occupancy
               value, as in TextBox_Destroy. */
            DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
            DuelEffect_ClearMatchingMarker(object->channel_index);
        }
        return;
    }

    if (gText_ScriptDispatchGate != 0) {
        gScript_apfnCommandHandlers[*(u8 *)&gScript_wCommandWord]();
        if (gScript_wCommandWord == 0) {
            gText_ScriptDispatchGate = 0;
        }
    }
    if (object->playback_state != 0) {
        gTextBox_apfnStateCallbacks[object->playback_state & DUEL_EFFECT_STATE_INDEX_MASK](object);
        object->flags_34 = object->flags_34 & 0xFBFF;
        return;
    }
    if ((object->flags_34 & 0x1C00) == 0) {
        if ((gInput_awPadHeld[0] & PAD_BUTTON_SQUARE) ||
            (gInput_awPadPressed[0] & TEXT_BOX_ADVANCE_MASK)) {
            DuelEffect_SetEntryControlBytes(object, 0, 0);
            object->delay_countdown = 1;
            object->flags_34 = object->flags_34 | 0x400;
        }
        object->delay_countdown = object->delay_countdown - 1;
        if (object->delay_countdown != 0) {
            return;
        }
    }
    handlers = gText_apfnPrimaryCommands;
    object->delay_countdown = object->glyph_delay;
next_opcode:
    slot = &((TextStreamOwner *)object)->streams[object->stream_index];
    script = *slot;
    gText_CurrentStreamCode = script[0];
    op = (s16)gText_CurrentStreamCode;
    *slot = script + 1;
    if (op >= 0xF0) {
        gText_CommandDisposition = 0;
        handlers[(s16)gText_CurrentStreamCode - 0xF0]((u8 *)object);
        if (gText_CommandDisposition >= 0) {
            if (gText_CommandDisposition == 1) {
                return;
            }
            goto next_opcode;
        }
    }
    if (TextBox_WrapLineIfNeeded(object) != 0) {
        object->playback_state = 4;
        return;
    }
    gText_PumpedGlyphIndex = gText_CurrentStreamCode;
#ifdef VERSION_EUROPE
    /* The European glyph call returns a signed adjustment to the nominal
       horizontal advance. The box counts pixels (the wrapper makes the
       counter and its limit halfwords at 0x60 and 0x62) rather than
       glyphs. */
    {
        s32 w;
        s32 n;
        s32 limit;

        w = object->glyph_advance_x +
            DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
        limit = (s16)TEXT_BOX_GLYPH_LIMIT(object);
        object->pen_x = object->pen_x + w;
        n = TEXT_BOX_GLYPH_COUNT(object) + w;
        TEXT_BOX_GLYPH_COUNT(object) = n;
        if (limit != 0) {
            w = (s16)n + object->glyph_advance_x;
            if (limit < w) {
                object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
            }
        }
    }
#else
    DuelEffect_AppendEntry(object, gText_aGlyphLookup[(s16)gText_CurrentStreamCode].packed & 0x8FF0FFFF);
    TEXT_BOX_GLYPH_COUNT(object) = TEXT_BOX_GLYPH_COUNT(object) + 1;
    if (TEXT_BOX_GLYPH_LIMIT(object) != 0 &&
        TEXT_BOX_GLYPH_COUNT(object) >= TEXT_BOX_GLYPH_LIMIT(object)) {
        object->flags_34 = object->flags_34 | TEXT_BOX_FLAG_DONE;
    }
    object->pen_x = object->pen_x + object->glyph_advance_x;
#endif
}

#include "graphics_frame.h"
#include "input.h"
#include "display_effect_process_menu_records.h"
#include "duel_effect.h"

#include "text_channel_state.h"
#include "duel_effect_process_entries.h"
#include "sound.h"
#include "display_object.h"
#include "display_object_core.h"
#include "text_box_update_channels.h"
#include "text_box_runtime.h"
#include "text_box_blocking_build.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice_state.h"
#include "text_constants.h"

/* Zero-initialized storage this unit owns (0x800EB0F8-0x800EB288 in North
 * America, 0x800EAFD8-0x800EB158 in Japan). */
DuelEffectChannel gText_aChannels[DUEL_EFFECT_CHANNEL_COUNT];

/* Small global another unit owns, declared here as a common symbol the
 * way the original read it through $gp. */
u8 gText_PreviousGlyphKind;

/* Regional value: the choice.flags bit that opens and closes the choice (0x8
 * -> 0x10). The European build (src/game/european/) defines its own. */
#ifndef FUNC_80039794_CHOICE_FLAG
#define FUNC_80039794_CHOICE_FLAG 8
#endif

#ifndef DISPLAY_EFFECT_MENU_CONFIRM_MASK
#define DISPLAY_EFFECT_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

/* One record pointer walks the four channels beside a down-counter. Under
 * strength reduction GCC folds the record's prompt_object and flags_34
 * accesses into a second induction register parked at offset 0x30, which is
 * the retail shape. */
void TextBox_UpdateChannels(void)
{
    DuelEffectChannel *p;
    s32 reset_value;
    TextGlyphLookupEntry *table;
    s32 n;
    s32 cnt;
    s32 idx;
    s32 kind;
    s32 arg;
    u16 f;

    p = gText_aChannels;
    n = 4;
    reset_value = -1;
    table = gText_aGlyphLookup;
    do {
        if (p->flags_34 & 0x8000) {
            gText_PumpedGlyphIndex = 0;
            if ((p->flags_34 & 0x2000) == 0) {
                gText_PumpedGlyphIndex = reset_value;
                cnt = -1;
                for (;;) {
                    TextBox_BuildStep(p);
                    cnt++;
                    f = p->flags_34;
                    if (f & 0x2000) {
                        if (f & FUNC_80039794_CHOICE_FLAG) {
                            p->prompt_object = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if ((f & 0x1C00) == 0) {
                        if (cnt >= gGraphics_bExtraFrameStep) {
                            break;
                        }
                    } else {
                        gText_PumpedGlyphIndex = reset_value;
                    }
                }
            } else {
                if (p->flags_34 & FUNC_80039794_CHOICE_FLAG) {
                    if (gInput_awPadPressed[0] &
                        DISPLAY_EFFECT_MENU_CONFIRM_MASK) {
                        p->flags_34 &= 0xFFFF ^ FUNC_80039794_CHOICE_FLAG;
                        DisplayObject_ReleaseIfPresent(p->prompt_object);
                        p->prompt_object = 0;
                        SD_SEPlayFull(0xB);
                    }
                }
            }
            arg = -1;
            idx = gText_PumpedGlyphIndex;
            if (idx >= 0) {
                kind = table[idx].halves.upper & 7;
                if (kind == 4) {
                    arg = gText_PreviousGlyphKind;
                } else {
                    gText_PreviousGlyphKind = kind;
                    arg = kind;
                }
            }
            DisplayEffect_ProcessMenuRecords(arg);
            DuelEffect_ProcessEntries(p);
        }
        p++;
        n--;
    } while (n != 0);
}

/* These three reach the record through the parameter directly. What used to
 * stand at each use was a cast of the pointer to its own type, left from when
 * the parameter was a byte pointer; removing them is byte-identical, and
 * measured.
 *
 * What does change the code is a typed local. Hoisting the record into one
 * changes the prologue register saves under GCC 2.8.1, which is the
 * constraint the old note here was about. */
void TextBox_SetPos(DuelEffectChannel *record, s32 x, s32 y)
{
    DisplayObject *object;

    object = record->render_object;
    record->rect_x = x;
    record->rect_y = y;
    if (object != 0) {
        object->xy.h.x = x;
        object->xy.h.y = y;
    }
    object = record->aux_object;
    if (object != 0) {
        if (object->list_key == 4)
            TextBox_UpdateOutline(record);
        else {
            object->xy.h.x = x;
            object->xy.h.y = y;
        }
    }
    object = record->prompt_object;
    if (object != 0) {
        if (object->list_key == 4)
            DuelEffect_UpdateObjectLayout(record);
        else {
            object->xy.h.x = record->rect_width + x - 16;
            object->xy.h.y = record->rect_height + y - 16;
        }
    }
}

void TextBox_BuildBlocking(DuelEffectChannel *object)
{
    object->flags_34 |= TEXT_BOX_FLAG_BUILD_REQUESTED;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}

void TextBox_BuildBlockingWithGlyphScratch(DuelEffectChannel *object)
{
    object->flags_34 |= 0xA00;
    do {
        TextBox_BuildStep(object);
    } while (!(object->flags_34 & TEXT_BOX_FLAG_DONE));
}

#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "display_object_fade.h"
/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s32 gDuelEffect_nCompactionRequested;

#ifdef VERSION_EUROPE
#include "duel_effect_entry_occupancy.h"

#endif

/* The two helpers the display-object fade callbacks below share: the first-frame latch on the
   channel's byte 0x13, and the release that frees the channel's entry slot
   and requests an entry-list rebuild. */

#ifdef VERSION_EUROPE
/* The European channel is laid out as a EuropeanDuelEffectEntry: the latch
   byte sits at 0x11 and the occupied byte at 0xF. */
#define FADE_LATCH(object) (((EuropeanDuelEffectEntry *)(object))->handler_state)
#define FADE_OCCUPIED(object) (((EuropeanDuelEffectEntry *)(object))->entry_flags)
#else
#define FADE_LATCH(object) ((object)->handler_state)
#define FADE_OCCUPIED(object) ((object)->entry_flags)
#endif

s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *object)
{
    u8 flags = FADE_LATCH(object);
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        FADE_LATCH(object) = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}

void DisplayObjectFade_ReleaseChannel(DuelEffectChannel *object)
{
#ifndef VERSION_EUROPE
    /* The European build does not clear the occupancy value, as in
       TextBox_Destroy. */
    gDuelEffect_aOccupancyValues[object->glyph_index] = 0;
#endif
    FADE_OCCUPIED(object) = 0;
    gDuelEffect_nCompactionRequested = 1;
}

#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "duel_effect.h"
#include "graphics_frame.h"
#include "display_object_fade.h"
#include "display_object_fade_callbacks.h"
#include "duel_effect_entry_occupancy.h"
#include "graphics_frame_controls.h"
#ifdef VERSION_EUROPE
#include "european_glyph_fade_callbacks.h"
#endif

#define DUEL_EFFECT_FIELD_04_WORD(type, object) \
    (*(type *)&(object)->field_04)

/* The European build's entries are 0x16 bytes (EuropeanDuelEffectEntry):
   its callbacks reach their fields through this view. */
#define EU_ENTRY(record) ((EuropeanDuelEffectEntry *)(record))
#define EU_ENTRY_FADE_VALUE(record) (((u8 *)(record))[0x15])

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    /* The European entry (0x16 bytes, two-byte aligned) clears field_04 as
       two halfwords, and the fade runs at half the US steps. */
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 2;
        *(u16 *)&EU_ENTRY(record)->field_04 = 0;
        *(u16 *)&EU_ENTRY(record)->field_06 = 0;
        EU_ENTRY(record)->field_14 = 16;
    }
    EU_ENTRY(record)->field_14 =
        EU_ENTRY(record)->field_14 - *(u8 *)&gGraphics_nBaseFrameStep;
    if ((EU_ENTRY(record)->handler_state &
         DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        EU_ENTRY(record)->field_04 =
            EU_ENTRY(record)->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_04 < 0) {
            EU_ENTRY(record)->field_04 = 128;
            EU_ENTRY(record)->field_14 = 8;
            EU_ENTRY(record)->handler_state |=
                DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        EU_ENTRY(record)->field_06 = EU_ENTRY(record)->field_04;
    } else {
        EU_ENTRY(record)->field_05 =
            EU_ENTRY(record)->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_05 < 0) {
            EU_ENTRY(record)->field_14 = 0;
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
        }
        EU_ENTRY(record)->field_07 = EU_ENTRY(record)->field_05;
    }
}
#else
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        record->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(s32, record) = 0;
        record->field_14 = 32;
    }
    record->field_14 = record->field_14 - gGraphics_nBaseFrameStep * 2;
    if ((record->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        record->field_04 = record->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_04 < 0) {
            record->field_04 = 128;
            record->field_14 = 16;
            record->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        record->field_06 = record->field_04;
    } else {
        record->field_05 = record->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_05 < 0) {
            record->field_14 = 0;
            record->render_mode = 0;
            record->handler_state = 0;
        }
        record->field_07 = record->field_05;
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    /* The European entry sets field_04 as two halfwords, and the fade
       steps by gGraphics_nBaseFrameStep * 24 where the US build steps by * 16. */
    if (!DisplayObjectFade_MarkInitialized(p)) {
        EU_ENTRY(p)->render_mode = 2;
        *(u16 *)&EU_ENTRY(p)->field_04 = 0x8080;
        *(u16 *)&EU_ENTRY(p)->field_06 = 0x8080;
        EU_ENTRY(p)->field_14 = 0;
    }
    if (!(EU_ENTRY(p)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = EU_ENTRY(p)->field_04;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            EU_ENTRY(p)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        EU_ENTRY(p)->field_04 = v;
        EU_ENTRY(p)->field_05 = v;
    } else {
        v = EU_ENTRY(p)->field_06;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        EU_ENTRY(p)->field_06 = v;
        EU_ENTRY(p)->field_07 = v;
    }
}
#else
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    if (!DisplayObjectFade_MarkInitialized(p)) {
        p->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(u32, p) = 0x80808080;
        p->field_14 = 0;
    }
    if (!(p->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = p->field_04 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            p->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        p->field_04 = v;
        p->field_05 = v;
    } else {
        v = p->field_06 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        p->field_06 = v;
        p->field_07 = v;
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        EU_ENTRY(record)->render_mode = 1;
        a = EU_ENTRY(record)->local_x;
        b = EU_ENTRY(record)->local_y;
        EU_ENTRY(record)->field_08 = 0;
        EU_ENTRY(record)->field_09 = 0;
        EU_ENTRY(record)->field_0A = 0;
        EU_ENTRY(record)->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(EU_ENTRY(record)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = EU_ENTRY(record)->field_04 - 1;

        EU_ENTRY(record)->field_04 = v;

        if ((u8)v == 0) {
            EU_ENTRY(record)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = EU_ENTRY(record)->field_08 + 4;

        EU_ENTRY(record)->field_0A = v;
        EU_ENTRY(record)->field_09 = v;
        EU_ENTRY(record)->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(record);
        }
    }
}
#else
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        record->render_mode = 1;
        a = record->local_x;
        b = record->local_y;
        record->field_08 = 0;
        record->field_09 = 0;
        record->field_0A = 0;
        record->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(record->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = record->field_04 - 1;

        record->field_04 = v;

        if ((u8)v == 0) {
            record->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = record->field_08 + 4;

        record->field_0A = v;
        record->field_09 = v;
        record->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(record);
        }
    }
}
#endif

#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context)
{
    s32 radius;
    s32 value;
    s32 next_angle;
    u32 flags;
    u16 *origin_x;
    u16 *origin_y;

    origin_x = (u16 *)&EU_ENTRY(record)->field_04;
    origin_y = (u16 *)&EU_ENTRY(record)->field_06;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY_FADE_VALUE(record) = 0;
        EU_ENTRY(record)->render_mode = 3;
        if ((rand() & 0xFF) >= 0x80) {
            EU_ENTRY(record)->field_14 |= 0x40;
        }
        *origin_x = EU_ENTRY(record)->local_x;
        *origin_y = EU_ENTRY(record)->local_y;
        *(u16 *)&EU_ENTRY(record)->field_08 = rand() & 0xFFF;
        *(u16 *)&EU_ENTRY(record)->field_0A = 0x100;
        if (EU_ENTRY(record)->field_14 & 0x80) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
            *(u16 *)&EU_ENTRY(record)->field_0A = 0;
        }
    }

    flags = EU_ENTRY(record)->field_14;
    if (flags & 0x80) {
        radius = *(u16 *)&EU_ENTRY(record)->field_0A + 10;
        if (radius < 0x140) {
            flags &= 0x7F;
            goto assign_value;
        }
        DisplayObjectFade_ReleaseChannel(record);
        return;
    }

    radius = *(u16 *)&EU_ENTRY(record)->field_0A - 10;
    value = radius;
    if (radius > 0) {
        goto clamp_value;
    }
    EU_ENTRY(record)->local_x = *origin_x;
    EU_ENTRY(record)->local_y = *origin_y;
    EU_ENTRY(record)->render_mode = 0;
    EU_ENTRY(record)->handler_state = 0;
    return;

assign_value:
    value = radius;

clamp_value:
    *(u16 *)&EU_ENTRY(record)->field_0A = radius;
    if (radius >= 0xA1) {
        value = 0xA0;
    }
    EU_ENTRY_FADE_VALUE(record) = (0xA0 - value) * 0x80 / 0xA0;

    if (flags & 0x40) {
        flags &= 0x3F;
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 - flags;
    } else {
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 + flags;
    }
    value = next_angle & 0xFFF;
    *(u16 *)&EU_ENTRY(record)->field_08 = value;
    EU_ENTRY(record)->local_x =
        *origin_x + rcos(value) * radius / 0x1000;
    EU_ENTRY(record)->local_y =
        *origin_y + rsin(value) * radius / 0x1000;
}

void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 8;
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 32;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}

void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 8) | -0x80;
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 32) | -0x80;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}

void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record)
{
    s32 value;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 3;
        value = EU_ENTRY(record)->field_14;
        EU_ENTRY_FADE_VALUE(record) = 0;
        if (value != 0) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
        }
    }

    value = EU_ENTRY_FADE_VALUE(record);
    if (EU_ENTRY(record)->field_14 != 0) {
        value -= 5;
        if (value <= 0) {
            DisplayObjectFade_ReleaseChannel(record);
            return;
        }
    } else {
        value += 5;
        if (value >= 0x80) {
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
            return;
        }
    }
    EU_ENTRY_FADE_VALUE(record) = value;
}

void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0;
    DuelEffect_UpdateGlyphLinearFade(record);
}

void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0x80;
    DuelEffect_UpdateGlyphLinearFade(record);
}
#endif

#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "main_modes.h"
#include "duel_effect_process_entries.h"

/* Zero-initialized storage this unit owns at 0x800EB288-0x800EF668 in the
 * US build; the European builds place the battle models right after
 * gText_aChannels instead. A regional wrapper selects its own entry record
 * and count through DUEL_EFFECT_ENTRY_STORAGE_TYPE/COUNT. */
DUEL_EFFECT_ENTRY_STORAGE_TYPE gDuelEffect_aEntries[DUEL_EFFECT_ENTRY_STORAGE_COUNT];
AnimatedBattleModelProperties gMain_aAnimatedBattleModels[2];

#ifndef DUEL_EFFECT_PROCESS_ENTRY_TYPE
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE DuelEffectEntry
#endif

void DuelEffect_ProcessEntries(DuelEffectChannel *channel)
{
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *p;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *q;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *s;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *e;

    p = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_begin;
    gDuelEffect_nCompactionRequested = 0;

    while (p->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        if (p->handler_state != 0) {
            gDuelEffect_EntryHandlers[p->handler_state & DUEL_EFFECT_ENTRY_HANDLER_INDEX_MASK](
                (DuelEffectEntry *)p, channel);
        }
        p++;
    }

    if (gDuelEffect_nCompactionRequested != 0) {
        q = &((DUEL_EFFECT_PROCESS_ENTRY_TYPE *)gDuelEffect_aEntries)[
            channel->entry_range_start];
        e = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_write_cursor;
        s = q;
        while (s != e) {
            if (s->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
                *q = *s;
                q++;
            }
            s++;
        }
        channel->entry_write_cursor = (DuelEffectEntry *)q;
        q->entry_flags = 0;
    }
}

#include "display_effect_update_resource_acquisition.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "menu_record_reset.h"
#ifdef VERSION_JAPAN
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#endif

s8 *DisplayEffect_ResetMenuRecords(void)
{
    s32 i;
    s32 column;
#ifdef VERSION_JAPAN
    s32 has_more;
#else
    s32 empty;
    s8 *entry;
#endif

    for (i = MENU_RECORD_COUNT - 1; i >= 0; i--) {
        MenuRecord *record = &gDisplayEffect_aMenuRecords[i];

        record->effect_id = -1;
        record->variant_update_flags = 0;
        record->requested_variant = 0;
        record->variant_update_counter = 0;
        for (column = 2; column >= 0; column--) {
            record->grid[0][column] = 0;
            record->grid[1][column] = 0;
            record->grid[2][column] = 0;
            record->grid[3][column] = 0;
        }
    }

#ifdef VERSION_JAPAN
    i = 0;
    do {
        ((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + i] = -1;
        i++;
        has_more = i < DISPLAY_EFFECT_VRAM_SLOT_COUNT;
    } while (has_more);
    /* The final false loop predicate is the Japanese null return. */
    return (s8 *)has_more;
#else
    empty = -1;
    i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1;
    entry = gDisplayEffect_CachedResourceIds + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
#endif
}
