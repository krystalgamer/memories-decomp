#ifndef MEMORIES_DECOMP_FUNC_80039794_H
#define MEMORIES_DECOMP_FUNC_80039794_H

#include "../types.h"

/* The second cursor the body parks on a record's 0x30 pair, so the choice
   object and its flags are reached at displacements 0 and 4. Kept as its own
   view because folding it into the record base costs a register and four
   instructions. */
typedef struct {
    void *obj;
    u16 flags;
} ChoiceView;

/* One entry of the halfword pair table at D_801D9000. text_constants.h
 * declares the same table as u32 for its two word readers. */
typedef struct {
    u16 lo;
    u16 hi;
} TblEnt;

void func_80039794(void);

#endif
