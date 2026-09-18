/*
 * Model tint queue pass. Reclassified from matching_c (#3859), where the
 * source was byte-exact under gcc_2_8_1_g8_split only by pinning 16 variables
 * to hard registers and building the request table base in an inline helper
 * that was itself inline asm. Current best with no hard register pin and no
 * inline asm: 288 of 288 instructions, and an empty opcode census -- the
 * multiset of encoded opcodes is the target's, so the addu this entry used to
 * record as missing is gone.
 *
 * The table base is read directly from D_800F2B50, the slot is indexed as
 * &D_800F2C40[side], func_80059AA8 gets a literal zero, and the three colour
 * channels are plain interpolations. The second declaration of D_800F2C40 is
 * gone.
 *
 * Three values carry a second name and the row address is named off the table
 * pointer the function already holds: keep2 for the saved sprite id the second
 * loop restores, v2 for the shifted id the first loop writes, and rowbase for
 * (u8 *)table + 1, assigned at the top of the iteration and added as
 * off + rowbase so the addu takes the offset first. Each name is assigned once
 * from a live value with no write to its source in between, so the pass does
 * what it did; what changes is that gcc gives each short live range a register
 * of its own instead of carrying one value across the whole iteration.
 *
 * The shift that produces the sprite id is written as two statements against
 * the one name, v = v >> 3; v = v & 0x1F;. One name across two statements is
 * what stops the pair being combined, and it puts the second srl where retail
 * has it: without it that instruction sits one position later and is the only
 * difference in the function that is not a register name.
 *
 * Residual: 15 blocks, and every one of them is a register name alone. No
 * opcode, no instruction count and no instruction's position differs. Two
 * separate values wear the wrong register, not one: the shifted sprite id, and
 * a halfword the pass holds on the stack across the redraw and hands back as
 * the second loop's argument.
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
    s32 keep2;
    s32 rowbase;
    u32 v2;
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
        rowbase = (s32)((u8 *)table + 1);
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
        v = v >> 3;
        v = v & 0x1F;
        slot = &D_800F2C40[side];
        sav06 = slot->field_E06;
        v2 = v;
        keep = slot->field_BF5;
        old = func_80059AA8(side, 0);
        save = *(ModelTintColor *)slot->field_DC0;
        col.b3 = e->start.b3;
        col.b0 = e->start.b0 * (hi - lo) / hi + e->end.b0 * lo / hi;
        col.b1 = e->start.b1 * (hi - lo) / hi + e->end.b1 * lo / hi;
        col.b2 = e->start.b2 * (hi - lo) / hi + e->end.b2 * lo / hi;
        *(ModelTintColor *)slot->field_DC0 = col;
        keep2 = keep;

        aa = a;
        for (j = 0; j < slot->field_E1B; j++) {
            s32 t = slot->field_BF5;
            if (v2 != 0) {
                slot->field_1E0[j]->sid = v2;
                t = v2;
            }
            func_8004DC38(slot, j, t, aa);
        }
        if (v2 != 0) {
            slot->field_BF5 = v2;
        }

        D_8009AF9C = off + rowbase;
        D_8009AF9B = (e->flags >> 2) & 1;
        func_800540B4(side);
        D_8009AF9C = 0;
        D_8009AF9B = 0;

        sv = sav06;
        for (k = 0; k < slot->field_E1B; k++) {
            s32 t = slot->field_BF5;
            if (keep2 != 0) {
                slot->field_1E0[k]->sid = keep2;
                t = keep2;
            }
            func_8004DC38(slot, k, t, sv);
        }
        if (keep2 != 0) {
            slot->field_BF5 = keep2;
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
