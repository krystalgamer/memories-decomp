#include "../types.h"

/* Deliberately NOT func_8002C604.h. The definition takes an s32 that it
 * stores as the request id at +0x18, and this caller passes nothing, so the
 * id is whatever happens to be in $a0. That is what the retail image does:
 * declaring the real prototype here makes the call a constraint violation,
 * and passing an argument to satisfy it adds the instruction that sets $a0
 * and breaks the match. The wrong declaration is load-bearing. */
extern void *func_8002C604(void);
/* Declared here rather than through duel_effect_request.h, which this TU
 * cannot include for the reason above. */
extern u8 D_8009B260;

void *func_8002C68C(void)
{
    void *value = func_8002C604();

    if (value != 0) {
        D_8009B260 |= 0x80;
    }
    return value;
}
