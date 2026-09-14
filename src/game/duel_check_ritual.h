#ifndef MEMORIES_DECOMP_DUEL_CHECK_RITUAL_H
#define MEMORIES_DECOMP_DUEL_CHECK_RITUAL_H

#include "../types.h"
#include "card_constants.h"

/* Optional ritual-match output: the three matched tribute display objects,
 * followed by a word the function clears. */
typedef struct DuelRitualResult {
    void *tribute_objects[DUEL_RITUAL_TRIBUTE_COUNT];
    s32 field_0C;
} DuelRitualResult;

typedef char DuelRitualResult_size_must_be_0x10[
    sizeof(DuelRitualResult) == 0x10 ? 1 : -1
];
typedef char DuelRitualResult_clear_word_offset_must_be_0x0C[
    (u32)&((DuelRitualResult *)0)->field_0C == 0x0C ? 1 : -1
];

/* The recipe table that Duel_CheckRitual walks.
 * Duel_LoadPackageStage's case 4 fills it, handing the address to a transfer
 * as both value_08 and value_0C with mode 0x800, the same way its case 3 fills
 * gDuel_aFusionTable. Duel_CheckRitual then scans it as fixed-size records --
 * id first, stepping by DUEL_RITUAL_RECIPE_HALFWORD_COUNT -- and stops on a
 * zero id, so the loaded data is self-terminating.
 *
 * The same memory is also borrowed as scratch once the duel is over:
 * DuelScene_UpdateResultOutro casts it to DuelResultSpriteSlot * for the outro, which
 * duel_result_outro.h documents as "the gDuel_awRitualData scratch". That reuse is
 * why the array stays unsized here -- the two consumers disagree about the
 * element type, and only the halfword view belongs to this header.
 *
 * DuelScene_UpdateResultRewards reuses the same backing for a complete
 * 0x40-byte result display record after recipe processing has ended. Its guarded view
 * below describes that bounded record, not a new allocation. */
#ifdef DUEL_RITUAL_DATA_RESULT_VIEW
#include "duel_result_display.h"
extern DuelResultDisplayState gDuel_awRitualData;
#else
extern u16 gDuel_awRitualData[];
#endif

s32 Duel_CheckRitual(DuelRitualResult *out, s32 ritual_id);

#endif
