/*
 * Model tint queue pass. Reclassified from matching_c (#3859), where the
 * source was byte-exact under gcc_2_8_1_g8_split only by pinning 16 variables
 * to hard registers and building the request table base in an inline helper
 * that was itself inline asm. Current best with no hard register pin and no
 * inline asm: 287/288 instructions and opcode distance 1 (one missing addu).
 *
 * The table base is read directly from D_800F2B50, the slot is indexed as
 * &D_800F2C40[side], func_80059AA8 gets a literal zero, and the three colour
 * channels are plain interpolations. The second declaration of D_800F2C40 is
 * gone.
 *
 * Residual: retail copies the table base into $s5 in the prologue, loads the
 * zero argument in an earlier branch delay slot, and keeps the sprite id in
 * $s2 with its shift and mask split around the func_80059AA8 call.
 */
#include "../types.h"
#include "../game/model_view_adjustments.h"
#include "../game/model_has_insufficient_buffer_space.h"
#include "../game/func_8004E7B0.h"
#include "../game/func_80059AA8.h"
#include "../game/model_effect_coefficients.h"
#include "../game/func_8004DC38.h"
#include "../game/func_80058E1C.h"
#include "../game/func_800528AC.h"
#include "../game/func_80052D2C.h"
#include "../game/model_record_tables.h"
#include "../game/model_scene_setup.h"
#include "../game/model_transfer_flags.h"
#include "../game/model_state_setters.h"
#include "../game/model_slot_setup.h"
#include "../game/model_update_view_metrics.h"
#include "../game/model_graphics_state.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "../game/func_80058434.h"
#include "../game/camera_view.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/memory.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h; camera_view.h above
   supplies libgs.h. */
#include "../psyq/libhmd.h"
#include "../game/model.h"
#include "../game/model_scene_states.h"
#include "../game/model_transfer_state.h"
#include "../unmatched.h"

/* The contiguous model-scene runtime: camera/view correction, tint request
   processing, camera-move setup, and scene reset/configuration. The seven
   functions share D_800F56F0, D_800F2B20, D_800F2B50, model slots, and the
   D_8009AFxx scene-control state. */

/* Per-frame tint pass over the ten requests at D_800F2B50. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the requested part id, then restores the slot and advances the request. */
void func_800528AC(void)
{
    ModelTintColor save;
    ModelTintColor col;
    s32 i;
    s32 j;
    s32 k;
    u32 v;
    s32 keep;
    s32 old;
    u8 side;
    u16 sav06;
    s32 off;
    u16 lo;
    u16 hi;
    u16 a;
    s32 aa;
    s32 sv;
    ModelSlot *slot;
    ModelTintRequest *e;
    ModelTintRequest *table;

    table = (ModelTintRequest *)D_800F2B50;
    for (i = 0, off = 0; i < MODEL_TINT_REQUEST_COUNT; off += 0x18, i++) {
        e = &table[i];
        if ((*(u8 *)e & 1) == 0) {
            continue;
        }
        if (D_800F2C40[(e->flags >> 1) & 1].field_E1F == 0) {
            continue;
        }
        if (Model_HasInsufficientBufferSpace((e->flags >> 1) & 1, e->start.b3)) {
            goto tail;
        }
        v = e->flags;
        a = e->field_0A;
        lo = e->elapsed;
        hi = e->duration;
        side = (v >> 1) & 1;
        v = (v >> 3) & 0x1F;
        slot = &D_800F2C40[side];
        sav06 = slot->field_E06;
        keep = slot->field_BF5;
        old = func_80059AA8(side, 0);
        save = *(ModelTintColor *)slot->field_DC0;
        col.b3 = e->start.b3;
        col.b0 = e->start.b0 * (hi - lo) / hi + e->end.b0 * lo / hi;
        col.b1 = e->start.b1 * (hi - lo) / hi + e->end.b1 * lo / hi;
        col.b2 = e->start.b2 * (hi - lo) / hi + e->end.b2 * lo / hi;
        *(ModelTintColor *)slot->field_DC0 = col;

        aa = a;
        for (j = 0; j < slot->field_E1B; j++) {
            s32 t = slot->field_BF5;
            if (v != 0) {
                slot->field_1E0[j]->sid = v;
                t = v;
            }
            func_8004DC38(slot, j, t, aa);
        }
        if (v != 0) {
            slot->field_BF5 = v;
        }

        D_8009AF9C = (s32)((u8 *)D_800F2B50 + 1) + off;
        D_8009AF9B = (e->flags >> 2) & 1;
        func_800540B4(side);
        D_8009AF9C = 0;
        D_8009AF9B = 0;

        sv = sav06;
        for (k = 0; k < slot->field_E1B; k++) {
            s32 t = slot->field_BF5;
            if (keep != 0) {
                slot->field_1E0[k]->sid = keep;
                t = keep;
            }
            func_8004DC38(slot, k, t, sv);
        }
        if (keep != 0) {
            slot->field_BF5 = keep;
        }

        func_80059AA8(side, old);
        *(ModelTintColor *)slot->field_DC0 = save;
tail:
        e->elapsed += func_80058E1C();
        if (e->elapsed >= e->duration) {
            e->flags &= 0xFFFE;
        }
    }
}

#include "../psyq/stdarg.h"
#include "../game/model.h"
