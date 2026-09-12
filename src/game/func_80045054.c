/*
 * func_80045054: decoded output measurement without register pins
 *
 * The 192-byte routine at `0x80045054` matches all 48 instructions under the
 * existing uniform `gcc_2_8_1_g0` profile. It uses the existing
 * `G_SDVALUE_VOLATILE` pointer view and `SDValue` decoded-buffer, pointer-table,
 * and flag members. The unnamed selector and accumulator fields remain local
 * byte-offset accesses into that same state; no new shared layout or API is
 * introduced.
 *
 * Three source properties replace the historical eight register bindings and
 * compiler barrier. The volatile pointer view retains the snapshot-to-state
 * handoff; removing it loses one instruction. Returning the level directly on
 * the unmuted path gives the required return-register allocation. Finally, the
 * single-iteration level-read scope keeps the signed high-halfword read before
 * the flag read; flattening it exchanges the two words at `+0x90` and `+0x94`.
 * No additional volatile field reads are needed.
 *
 * The routine preserves the selected CD half, all 256 signed sample squares,
 * the unsigned eight-bit shift of each square, both accumulator initializations
 * and final publications, and the low-two-bit output gate. The signed-high-word
 * reads and existing `s32(void)` contract remain unchanged.
 *
 * The historical canonical match and six-entry refinement series ending in
 * deferral are retained. A post-terminal record identifies the pointer view,
 * return path, and load-order scope as the new discriminator.
 */
#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"

s32 func_80045054(void)
{
    s32 select = SpuReadDecodedData(
        (SpuDecodedData *)g_SDValue->buffer_053C, SPU_CDONLY
    );
    SDValue *choice_state = g_SDValue;
    s16 *values;
    s32 i;
    SDValue *loaded;
    SDValue *state;

    *(s32 *)((u8 *)choice_state + 0x538) = select;
    if (select == SPU_DECODED_FIRSTHALF) {
        values = (s16 *)choice_state->buffer_ptrs_153C[0];
    } else {
        values = (s16 *)choice_state->buffer_ptrs_153C[1];
    }
    loaded = g_SDValue;
    i = 0;
    state = loaded;
    *(s32 *)((u8 *)state + 0x154C) = 0;
    *(s32 *)((u8 *)state + 0x1550) = 0;
    do {
        s32 value = *values;
        u32 square = value * value;
        *(u32 *)((u8 *)state + 0x154C) += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        s32 result;
        s32 flags;
        s32 other;
        state = g_SDValue;
        do {
            result = *(s16 *)((u8 *)state + 0x154E);
        } while (0);
        flags = state->flags_0040;
        other = *(s16 *)((u8 *)state + 0x1552);
        flags &= 3;
        *(s32 *)((u8 *)state + 0x154C) = result;
        *(s32 *)((u8 *)state + 0x1550) = other;
        if (!flags) {
            return result;
        }
        return 0;
    }
}
