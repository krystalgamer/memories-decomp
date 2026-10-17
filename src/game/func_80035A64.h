#ifndef MEMORIES_DECOMP_FUNC_80035A64_H
#define MEMORIES_DECOMP_FUNC_80035A64_H

/* Clears flags_34 and fields 0x28-0x30 on all four D_800EB0F8 effect
   channels, then the occupancy table (not in the European build) and the
   entry markers. */
void DuelEffect_ResetState(void);

#endif
