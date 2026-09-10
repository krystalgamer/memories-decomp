#ifndef MEMORIES_DECOMP_DUEL_CARD_SELECTION_H
#define MEMORIES_DECOMP_DUEL_CARD_SELECTION_H

#include "../types.h"

/* The two private records the trailing selectors in ai_turn_action.c reach
 * their candidate cards through.
 *
 * Neither record view leaves the merged translation unit. The immediately
 * following unmatched AI routine calls both selectors through opaque
 * storage, so these shapes remain constrained solely by the reads here. */

/* A card's display object as this file uses it: only the byte at 0x6A is
 * named, and it is the value both functions return. This is NOT the whole
 * display record -- other translation units describe the same memory with
 * their own wider views. */
typedef struct {
    char pad[0x6A];
    u8 index;
} DuelSelectionObject;

/* The caller's one-pointer argument block. It is passed by address and only
 * its first word is read, so this names that word rather than claiming the
 * caller's record ends here. */
typedef struct {
    DuelSelectionObject *ptr;
} DuelSelectionSource;

/* Both scan the field row for an occupied card and return the index byte of
 * the best match, but they spell their argument differently and that is left
 * alone. func_8002778C takes the block as DuelSelectionSource *, while
 * func_800278A0 takes it as void * and casts at the access:
 * `*(DuelSelectionObject **)arg0`. The two describe the same one-pointer
 * block; giving func_800278A0 the struct type would be retyping a parameter
 * on the strength of its neighbour rather than of its own body, which is a
 * separate question from moving the types out of the .c. */
int func_8002778C(DuelSelectionSource *source);
s32 func_800278A0(void *arg0);

#endif
