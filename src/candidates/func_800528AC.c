/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 16 variables to hard registers and an inline helper that is itself inline asm, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/model_scene_setup.c.
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

extern ModelSlot D_800F2C40_alias[] asm("D_800F2C40");

/* The contiguous model-scene runtime: camera/view correction, tint request
   processing, camera-move setup, and scene reset/configuration. The seven
   functions share D_800F56F0, D_800F2B20, D_800F2B50, model slots, and the
   D_8009AFxx scene-control state. */

static __inline__ ModelTintRequest *make_table_base(void)
{
    register Pair64 wide asm("$2");
    register volatile u32 addr asm("$2");
    addr = (u32)D_800F2B50;
    wide.w.lo = addr;
    return (ModelTintRequest *)(u32)wide.d;
}

/* Per-frame tint pass over the ten requests at D_800F2B50. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the requested part id, then restores the slot and advances the request. */
void func_800528AC(void)
{
    /* File scope changes the later varargs function and grows it by 0xC. */
    register const u32 hard_zero asm("$0");
    ModelTintColor save;
    ModelTintColor col;
    s32 i;
    s32 j;
    s32 k;
    register u32 v asm("$18");
    s32 keep;
    s32 old;
    u8 side;
    u16 sav06;
    s32 off;
    u16 lo;
    u16 hi;
    u16 a;
    register s32 aa asm("$19");
    s32 sv;
    ModelSlot *slot;
    ModelTintRequest *e;
    ModelTintRequest *table;

    table = make_table_base();
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
        {
            register s32 zero_arg asm("$5") = hard_zero;
            {
                register u32 model_base asm("$9") = (u32)D_800F2C40_alias;
                slot = (ModelSlot *)((u32)side * sizeof(ModelSlot) + model_base);
            }
            sav06 = slot->field_E06;
            keep = slot->field_BF5;
            old = func_80059AA8(side, zero_arg);
        }
        save = *(ModelTintColor *)slot->field_DC0;
        col.b3 = e->start.b3;
        col.b0 = ({ register s32 p asm("$3"); p = e->start.b0 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->end.b0 * lo; p / hi; });
        col.b1 = ({ register s32 p asm("$3"); p = e->start.b1 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->end.b1 * lo; p / hi; });
        {
            register s32 p asm("$3");
            register s32 out asm("$4");
            register s32 q asm("$16");
            p = e->start.b2 * (hi - lo);
            out = p / hi;
            p = e->end.b2 * lo;
            q = p / hi;
            out += q;
            col.b2 = out;
        }
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

        {
            register s32 render_side asm("$4");
            render_side = side;
            {
                register s32 global_ptr asm("$3");
                register u32 global_flag asm("$2");
                register s32 global_base asm("$9") =
                    (s32)((u8 *)D_800F2B50 + 1);
                global_ptr = off + global_base;
                global_flag = (e->flags >> 2) & 1;
                D_8009AF9C = global_ptr;
                D_8009AF9B = global_flag;
                func_800540B4(render_side);
            }
        }
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
