/*
 * Semantic absolute value and keyframe bounds: `func_8005E808`
 *
 * The 1004-byte keyframe routine matches all 251 instruction words under the
 * existing uniform `gcc_2_8_1_g8_split` profile. Its parameter uses the
 * canonical 0x28-byte `Key` layout shared with the evaluator. The duration
 * field at +0x22 is consumed as a denominator by `model_effect_state.c`; the
 * retired candidate's "audible radius" description was not the established
 * contract.
 *
 * Two `__builtin_abs` calls are load-bearing. They use the same semantic
 * intrinsic as matching `duel_draw_status_numbers.c` and `func_80058624.c`.
 * The compiler emits its `abs` pseudo-instruction, rather than a hand-written
 * conditional whose scheduled delay slot duplicates a multiply. Both operands
 * come from signed halfwords promoted to s32, including -32768, so their
 * magnitudes fit the result type.
 *
 * Case-local distance temporaries and per-case bound checks preserve the
 * multiply/result registers and let the bound loads fill their original
 * slots. Pose traversal is indexed from `D_800F5768`, and the path counter is
 * initialized to one before its byte offset is reset. With those source
 * dependencies recovered, ordinary G8 remains five address words away, while
 * the existing split profile materializes the common pose base exactly.
 *
 * The global active-key guard remains separate from the input key, and the
 * initial threshold still tests the input magnitude against 0x4000, not a
 * replacement `min(scale, 0x4000)` expression. All six canonical attempts are
 * preserved; one post-terminal resolution records this result. No register
 * pins, source-level inline assembly, symbol aliases, or new profile is used.
 */
#include "../types.h"
#include "camera_view.h"
#include "model_copy_slot_u16_values.h"
#include "func_80058DD8.h"
#include "func_8005EBF4.h"
#include "model_transfer_state.h"
#include "model_transfer_flags.h"

void func_8005E808(Key *state)
{
    /* The copier writes four halfwords; only the first three are coordinates. */
    s16 pos[4];
    s16 buf[10][3];
    Coeff *e;
    SVECTOR *g;
    s32 i;
    s32 j;
    s32 a;
    s32 k;
    s32 sum;
    u32 d;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot;
    u16 v;

    {
        s32 n;
        s32 t;
        n = state->magnitude;
        a = __builtin_abs(n);
        t = a;
        if (a <= 0) {
            t = 1;
        }
        k = t * 2;
    }
    func_8005FB30(state);
    state->progress = 0;
    if (D_8009B074->ready != 0) {
        return;
    }
    state->ready = 1;
    if (a >= 0x4000) {
        state->duration = 0x4000;
    } else {
        state->duration = k;
    }
    if (state->magnitude >= 0) {
        return;
    }

    /* Coeff preserves the aligned halfword loads of these packed endpoints. */
    e = (Coeff *)state;
    for (i = 0; i < 2; i++, e++) {
        g = &D_800F5768[i];
        switch ((s16)e->w) {
        case 0x80:
        case 0x81:
        {
            s32 dx, dy, dz;
            slot = (s16)(e->w & 0xFF7F);
            if (func_80058DD8(slot) != 1) {
                continue;
            }
            Model_CopySlotU16Values(slot, (u16 *)pos);
            pos[0] = pos[0] + (u16)e->x;
            pos[1] = pos[1] + (u16)e->y;
            pos[2] = pos[2] + (u16)e->z;
            dx = pos[0] - g->vx;
            dy = pos[1] - g->vy;
            dz = pos[2] - g->vz;
            d = k * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        }
        case 1:
            sum = 0;
            for (j = 0; j < 10; j++) {
                func_8005EBF4((Key *)state, i, j, 0x1E, buf[j]);
            }
            for (j = 1; j < 10; j++) {
                dx = buf[j - 1][0] - buf[j][0];
                dy = buf[j - 1][1] - buf[j][1];
                dz = buf[j - 1][2] - buf[j][2];
                sum += SquareRoot0(dx * dx + dy * dy + dz * dz);
            }
            d = k * sum / 1000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        case 4:
        {
            s32 radius_scaled;
            s32 factor;
            v = (u16)D_800F5768[2].vx;
            if (D_800F5768[2].vx < e->x) {
                v = (u16)e->x;
            }
            radius_scaled = (s16)v * 6284 / 1000;
            d = k * radius_scaled;
            factor = __builtin_abs(e->y);
            d = (u32)(d * factor) / 4096000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        }
        default:
            continue;
        }
    }
}
