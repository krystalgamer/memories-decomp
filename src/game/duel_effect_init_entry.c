#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_init_entry.h"

#ifdef VERSION_EUROPE
/* The European channel is 100 bytes. The low two flag bits pick one of
   four text sizes, the same four as the European func_800382A8 (8 and 16
   by default), and the halfword at 0x62 is cleared. */
DuelEffectChannel *DuelEffect_InitEntry(int index, int value, int flags)
{
    DuelEffectChannel *e;
    unsigned short *range;
    s32 narrow;
    s32 wide;
    s32 size;

    e = (DuelEffectChannel *)((u8 *)D_800EB0F8 + index * 100);
    e->flags_34 = flags | DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    e->field_36 = value;
    narrow = 8;
    wide = 16;
    size = flags & 3;
    e->index_57 = index;
    e->field_54 = 0;
    e->field_38 = 0;
    e->field_3A = 0;
    e->field_5A = narrow;
    e->field_5B = wide;
    switch (size) {
    case 1:
        e->field_5A = narrow;
        e->field_5B = narrow;
        break;
    case 2:
        e->field_5A = 12;
        e->field_5B = wide;
        break;
    case 3:
        e->field_5A = wide;
        e->field_5B = wide;
        break;
    }
    e->field_53 = 1;
    range = (unsigned short *)((unsigned char *)gDuelEffect_awEntryRangeBoundaries + (index << 1));
    e->field_59 = 0;
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
#endif
