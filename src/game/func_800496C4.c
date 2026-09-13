/*
 * func_800496C4: transfer initialization without register pins
 *
 * The 152-byte function at `0x800496C4` matches under the existing uniform
 * `gcc_2_8_1_g0` profile using `SDSecondaryState` and `SDSecondaryTransfer` from
 * `sound.h`. The shared `s32(u8 *, s16, s32)` contract is unchanged. No new
 * declaration view, compiler profile, inline assembly, or literal global address
 * is needed.
 *
 * The zero value is initialized in a single-iteration scope, then the existing
 * signed-halfword argument is captured into a word local before the first
 * `D_8009B458` load. This replaces both hard-register bindings. Flattening the
 * zero initialization loses one instruction; moving the argument capture before
 * that scope exchanges three prologue words; initializing the state pointer at
 * declaration moves its absolute load too early. The inherited eight-byte local
 * frame remains: retail adjusts `$sp` on entry and exit but never accesses it.
 *
 * The first state's consumed-byte count is cleared before the conditional
 * inactive-state rejection. The function then reloads the state pointer, clears
 * the transfer status, and populates the input pointer, header-derived byte
 * counts, two control bytes, and the final pointer-valued field in retail order.
 * The input remains a byte pointer because this match does not establish a new
 * shared input-header type.
 *
 * All 38 target instructions agree with retail. The original canonical match
 * and six-entry inline-refinement series ending in deferral remain intact; a new
 * `post_terminal_resolution` row records the initialization-order discriminator.
 */
#include "../types.h"
#include "sound.h"
#include "sound_transfer_lifecycle.h"

s32 func_800496C4(u8 *input, s16 expected, s32 value)
{
    /* Retail reserves eight stack bytes without accessing them. */
    volatile s32 pad[2];
    s32 zero;
    s32 requested;
    SDSecondaryState *initial;
    SDSecondaryState *state;
    SDSecondaryTransfer *entry;

    do {
        zero = 0;
    } while (0);
    requested = expected;
    initial = D_8009B458;
    initial->bytes_consumed = 0;
    if (requested == SD_TRANSFER_STATE_INACTIVE &&
        initial->transfer.field_0000 != requested) {
        return SD_TRANSFER_ERROR;
    }
    state = D_8009B458;
    state->transfer.field_0000 = zero;
    entry = &state->transfer;
    entry->field_0004 = input;
    entry->field_0008 = (*(u16 *)(input + 0x12) << 9) + 0xA20;
    entry->field_0010 = *(s32 *)(input + 0x0C) - entry->field_0008;
    entry->field_0018 = input[0x18];
    entry->field_001B = input[0x19];
    entry->field_0014 = (u8 *)value;
    return 0;
}
