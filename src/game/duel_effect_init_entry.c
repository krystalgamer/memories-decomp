#include "../types.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_init_entry.h"

#ifdef VERSION_EUROPE
/* The European channel is 100 bytes. The low two flag bits pick one of
   four text sizes, the same four as the European Text_SetFontMode (8 and 16
   by default), and the halfword at 0x62 is cleared. */
DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    DuelEffectChannel *e;
    unsigned short *range;
    s32 narrow;
    s32 wide;
    s32 size;

    e = (DuelEffectChannel *)((u8 *)gText_aChannels + index * 100);
    e->flags_34 = flags | DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    e->initial_string_id = value;
    narrow = 8;
    wide = 16;
    size = flags & 3;
    e->channel_index = index;
    e->color_index_54 = 0;
    e->pen_x = 0;
    e->pen_y = 0;
    e->glyph_advance_x = narrow;
    e->line_advance_y = wide;
    switch (size) {
    case 1:
        e->glyph_advance_x = narrow;
        e->line_advance_y = narrow;
        break;
    case 2:
        e->glyph_advance_x = 12;
        e->line_advance_y = wide;
        break;
    case 3:
        e->glyph_advance_x = wide;
        e->line_advance_y = wide;
        break;
    }
    e->delay_reload_53 = 1;
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + (index << 1));
    e->depth_offset = 0;
    *(u16 *)((u8 *)e + 0x62) = 0;
    e->range_start_5C = range[0];
    e->range_count_5E = range[1] - range[0];
    return e;
}
#else
DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    register int offset = index << 1;
#ifdef VERSION_JAPAN
    JapaneseDuelEffectChannel *e;
    unsigned char *range;
    e = &((JapaneseDuelEffectChannel *)gText_aChannels)[index];
#else
    DuelEffectChannel *e;
    unsigned short *range;
    e = &gText_aChannels[index];
#endif
    flags |= DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
#ifdef VERSION_JAPAN
    e->line_advance_y = 16;
    e->glyph_advance_x = 16;
#else
    e->glyph_advance_x = 8;
    e->line_advance_y = 12;
#endif
    e->delay_reload_53 = 2;
#ifdef VERSION_JAPAN
    /* The 16-bit boundaries are stored modulo 256 in the compact channel. */
    range = (unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset;
#else
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset);
#endif
    e->channel_index = index;
    e->initial_string_id = value;
    e->color_index_54 = 0;
    e->flags_34 = flags;
    e->pen_x = 0;
    e->pen_y = 0;
    e->depth_offset = 0;
#ifdef VERSION_JAPAN
    e->field_5F = 0;
    e->range_start_5C = range[0];
    e->range_count_5D = range[2] - range[0];
#else
    e->field_61 = 0;
    e->range_start_5C = range[0];
    e->range_count_5E = range[1] - range[0];
#endif
    return (DuelEffectChannel *)e;
}
#endif
