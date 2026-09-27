#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_control.h"

#ifdef VERSION_JAPAN
#include "duel_effect_entry_occupancy.h"

#define DUEL_EFFECT_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_ENTRIES ((JapaneseDuelEffectEntry *)tent_DuelEffectEntries)
#define DUEL_EFFECT_RANGE_START(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->range_start_5C)
#define DUEL_EFFECT_RANGE_COUNT(channel) \
    (((JapaneseDuelEffectEntryRange *)(channel))->range_count_5D)
#elif defined(VERSION_EUROPE)
#include "duel_effect_entry_occupancy.h"

/* The European entries are 0x16 bytes, with field_13 and field_15 at 0x11
   and 0x13. */
#define DUEL_EFFECT_ENTRY_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_ENTRIES ((EuropeanDuelEffectEntry *)tent_DuelEffectEntries)
#define DUEL_EFFECT_RANGE_START(channel) ((channel)->range_start_5C)
#define DUEL_EFFECT_RANGE_COUNT(channel) ((channel)->range_count_5E)
#define DUEL_EFFECT_FIELD_13_OFFSET 0x11
#define DUEL_EFFECT_FIELD_15_OFFSET 0x13
#else
#define DUEL_EFFECT_ENTRY_TYPE DuelEffectEntry
#define DUEL_EFFECT_ENTRIES tent_DuelEffectEntries
#define DUEL_EFFECT_RANGE_START(channel) ((channel)->range_start_5C)
#define DUEL_EFFECT_RANGE_COUNT(channel) ((channel)->range_count_5E)
#endif

#ifndef DUEL_EFFECT_FIELD_13_OFFSET
#define DUEL_EFFECT_FIELD_13_OFFSET 0x13
#endif
#ifndef DUEL_EFFECT_FIELD_15_OFFSET
#define DUEL_EFFECT_FIELD_15_OFFSET 0x15
#endif

#define DUEL_EFFECT_ENTRY_FROM_FIELD_13(field) \
    ((DUEL_EFFECT_ENTRY_TYPE *)((field) - DUEL_EFFECT_FIELD_13_OFFSET))
#define DUEL_EFFECT_ENTRY_FROM_FIELD_15(field) \
    ((DUEL_EFFECT_ENTRY_TYPE *)((field) - DUEL_EFFECT_FIELD_15_OFFSET))
#define DUEL_EFFECT_ENTRY_BYTES(entry) ((u8 *)(entry))

/* Starting from the record's entry range, scans its active entries;
   returns 1 on the first entry with DUEL_EFFECT_ENTRY_FLAG_ACTIVE set and
   field_13 nonzero, 0
   if that flag clears, the count runs out, or the range is empty. */
#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_DUEL_EFFECT_HAS_ACTIVE_ENTRY)
int DuelEffect_HasActiveEntry(DuelEffectChannel *a0) {
    int v0;
    int count;
    u8 *v1;
    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    v1 = DUEL_EFFECT_ENTRY_BYTES(&DUEL_EFFECT_ENTRIES[v0]);
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = v1 + DUEL_EFFECT_FIELD_13_OFFSET;
loop:
    v0 = DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->flags_11 &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
    if (v0 == 0) {
        return v0;
    }
    if (DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->field_13 != 0) {
        return 1;
    }
    count = count - 1;
    v1 = v1 + sizeof(DUEL_EFFECT_ENTRY_TYPE);
    if (count != 0) {
        goto loop;
    }
ret_zero_a:
    return 0;
}
#endif

/* Starting from the record's entry range, walks its active entries;
   for each active entry, writes a1 to field_13 and a2 to field_15,
   stopping at the first entry with that flag clear or when the count runs
   out. */
#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800373C8)
void func_800373C8(DuelEffectChannel *a0, u8 a1, u8 a2) {
    int v0;
    int count;
    u8 *v1;

    v0 = DUEL_EFFECT_RANGE_START(a0);
    count = DUEL_EFFECT_RANGE_COUNT(a0);
    v1 = DUEL_EFFECT_ENTRY_BYTES(&DUEL_EFFECT_ENTRIES[v0]);
    if (count == 0) {
        return;
    }
    v1 = v1 + DUEL_EFFECT_FIELD_15_OFFSET;
loop:
    if ((DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->flags_11 &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
        return;
    }
    count = count - 1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->field_13 = a1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->field_15 = a2;
    v1 = v1 + sizeof(DUEL_EFFECT_ENTRY_TYPE);
    if (count != 0) {
        goto loop;
    }
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_TEXT_COMPLETE_PAGE_ADVANCE)
void Text_CompletePageAdvance(DuelEffectChannel *object)
{
    u8 state = object->state_51;
    DUEL_EFFECT_ENTRY_TYPE *entry;

    if ((state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->state_51 = state | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        func_800373C8(object, 2, 0);
        return;
    }

    entry = &DUEL_EFFECT_ENTRIES[DUEL_EFFECT_RANGE_START(object)];
    if (entry->flags_11 & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        return;
    }

    object->field_56 = 0;
    object->field_38 = 0;
    object->field_3A = 0;
    object->state_51 = 0;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
}
#endif
