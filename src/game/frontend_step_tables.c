#include "../types.h"
#include "frontend_step_tables.h"
#include "func_80030FA0.h"
#include "func_80030FD0.h"
#include "async_state_poll.h"
#include "frontend_scene_states.h"
#include "func_80030998.h"
#include "func_80031354.h"
#include "../unmatched.h"

/* Initialized data at 0x80090D7C: the two step tables func_80031084
 * dispatches through.
 *
 * It picks between them on D_8009B2F0 and indexes the winner with
 * `D_8009B2EB & FRONTEND_STEP_INDEX_MASK`. Both are written here rather than
 * resolved out of the
 * blob at 0x80090BA8 because every entry is a function this tree already
 * names. All but one of the twenty-three are matching C: func_80030998 is
 * generated assembly, while func_80030D5C is owned by
 * frontend_scene_state_80030d5c.c.
 *
 * The two are adjacent and that is load bearing. D_80090D7C holds only two
 * entries while the index mask permits thirty-two, so an index above 1 runs
 * straight into D_80090D84 -- whose first entry is the same func_80031078
 * that D_80090D7C starts with. Whether that is deliberate reuse or simply an
 * index the game never produces is not established here, so the two arrays
 * are defined separately, in this order, exactly as the image has them.
 *
 * What this does NOT claim: the tables are attributed to the frontend
 * because their entries live in frontend_scene_states.c, func_80030E30.c,
 * func_80030FA0.c and
 * their neighbours in the 0x80030000 range, not because anything in the tree
 * names them. The state byte they index, D_8009B2EB, is also written by the
 * memory card paths, so this is not evidence that the tables are
 * frontend-only. */

void (*D_80090D7C[])(void) = {
    func_80031078,
    func_80031000,
};

void (*D_80090D84[])(void) = {
    func_80031078,
    func_80030FA0,
    func_80030998,
    func_80030F40,
    func_80030FA0,
    func_800307B8,
    func_80031354,
    func_80030FA0,
    func_80030F80,
    func_80030FA0,
    func_80030CB0,
    func_80030FA0,
    func_80030FA0,
    func_80030D5C,
    func_80030E30,
    func_80030E7C,
    func_80030EC8,
    func_80030FA0,
    func_80030FA0,
    func_80030FA0,
    func_80030FD0,
};
