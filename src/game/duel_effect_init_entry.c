#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_init_entry.h"

DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    register int offset = index << 1;
#ifdef VERSION_JAPAN
    JapaneseDuelEffectChannel *e;
    unsigned char *range;
    e = &((JapaneseDuelEffectChannel *)D_800EB0F8)[index];
#else
    DuelEffectChannel *e;
    unsigned short *range;
    e = &D_800EB0F8[index];
#endif
    flags |= DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
#ifdef VERSION_JAPAN
    e->field_5B = 16;
    e->field_5A = 16;
#else
    e->field_5A = 8;
    e->field_5B = 12;
#endif
    e->field_53 = 2;
#ifdef VERSION_JAPAN
    /* The 16-bit boundaries are stored modulo 256 in the compact channel. */
    range = (unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset;
#else
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + offset);
#endif
    e->index_57 = index;
    e->field_36 = value;
    e->field_54 = 0;
    e->flags_34 = flags;
    e->field_38 = 0;
    e->field_3A = 0;
    e->field_59 = 0;
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
