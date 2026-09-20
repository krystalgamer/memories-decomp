#include "../types.h"
#include "model_view_adjustments.h"
#include "model_has_insufficient_buffer_space.h"
#include "func_8004E7B0.h"
#include "func_80059AA8.h"
#include "model_effect_coefficients.h"
#include "func_8004DC38.h"
#include "func_80058E1C.h"
#include "func_800528AC.h"
#include "model_record_tables.h"
#include "model_scene_setup.h"
#include "model_transfer_flags.h"
#include "model_state_setters.h"
#include "model_slot_setup.h"
#include "model_update_view_metrics.h"
#include "model_graphics_state.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "func_80058434.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/memory.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h; camera_view.h above
   supplies libgs.h. */
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_scene_states.h"
#include "model_transfer_state.h"
#include "../unmatched.h"
#include "../game/func_800540B4.h"

#define MODEL_TINT_COLOR_VIEW(color) ((ModelTintColor *)(color))

/* The start of the contiguous model-scene runtime: the two camera/view
   correction passes, which work on D_800F56F0, the camera move at D_800F2B20
   and the D_8009B478/D_8009B47C view values. The tint request pass that
   follows them, func_800528AC, closes this file; the camera-move setup and
   the scene reset/configuration after it are in func_80052D2C.c. */

void func_80052528(void) {
    GsRVIEW2 *view;
    s32 x;
    s32 f;
    s32 v;
    s32 lo;
    s32 c;
    s32 d;

    if (*(s16 *)&D_8009B47C >= 0x801) {
        view = &D_800F56F0;
        x = view->vry;
        f = 0;
        if (x >= -0xC7 && view->vpy < x) {
            if (x < -0x64) {
                f = (-200 - x) * MODEL_FIXED_ONE / -100;
            } else {
                f = MODEL_FIXED_ONE;
            }
        }

        v = (f * 3755 + (MODEL_FIXED_ONE - f) * 3968) /
            MODEL_FIXED_ONE;
        lo = v - 0x10;

        do {
            c = *(s16 *)&D_8009B47C;
            d = v + 0x10;
        } while (0);

        if (d < c) {
            d = d - c;
            if (d < -1) {
                d = -1;
            }
            func_80058434(1, 0, d, 0, 0);
        } else if (c < 0xEAB) {
            func_80058434(1, 0, 0xEAB - c, 0, 0);
        } else if (c < lo) {
            d = lo - c;
            if (d >= 5) {
                d = 4;
            }
            func_80058434(1, 0, d, 0, 0);
        }
    } else {
        func_80058434(1, 0, -4, 0, 0);
    }
}

void func_80052694(s32 arg0) {
    ModelCameraMove *e;
    ModelCameraMove *g;
    s32 n;
    s32 lo;
    s32 hi;
    s32 z;
    s32 d;
    s32 t;

    n = D_800F2B4A[0];
    lo = 0x2BC;
    hi = 0x3E8;
    z = 0;

    if (n >= 0) {
        lo = func_8005F1B8(n, lo);
        if (lo >= hi) {
            hi = func_8005F1B8(n, hi);
        }
    }

    if (func_80051350(1, 0x96, 0) != 0) {
        e = &D_800F2B20;
        if (e->field_04 < 0x10) {
            e->field_04 = e->field_04 + 1;
        }
        e->flags = e->flags & 0xFE;
    } else {
        g = &D_800F2B20;
        if (g->field_06 < g->field_04) {
            g->field_04 = g->field_04 - 1;
        }
    }

    func_80052528();

    t = *(s16 *)&D_8009B478;
    if (t < 0x12C) {
        if (t < 0x96) {
            d = 0x96 - t;
            func_80058434(1, z, 0, d, 0);
        } else {
            d = 0x12C - t;
            if (d >= 0xB) {
                d = 0xA;
            }
            func_80058434(1, z, 0, d, 0);
        }
    } else if (t < lo) {
        d = lo - t;
        if (d >= 6) {
            d = 5;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    if (arg0 != 0) {
        d = *(s16 *)&D_8009B478;
        if (hi < d) {
            goto last;
        }
        if (lo < d) {
            d = lo - d;
            if (d < -0xA) {
                d = -0xA;
            }
            func_80058434(1, 0, 0, d, 0);
        }
    }

    if (hi < *(s16 *)&D_8009B478) {
last:
        d = hi - *(s16 *)&D_8009B478;
        if (d < -0x14) {
            d = -0x14;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    func_80052528();
}

#include "../psyq/stdarg.h"
#include "model.h"

/* Per-frame tint pass over the ten requests at D_800F2B50. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the requested part id, then restores the slot and advances the request.
 *
 * What the allocation rests on: the shifted flag word `v` is masked into its
 * own name, `part`, ahead of the Model_SetSlotShadowEnabled call, and `part`
 * is copied to `sid` after it. `part` therefore lives and dies inside the one
 * basic block while crossing the call, so local-alloc gives the flag word,
 * shift and mask $s2 and pushes field_0A to $s3; `sid` then inherits $s2
 * through its
 * copy preference. `v` itself never crosses the call, which is what holds the
 * srl and andi ahead of it where the scheduler would otherwise sink them to
 * the end of the block. The saved field_BF5 is one name, `keep`, across both
 * loops. */
void func_800528AC(void)
{
    ModelTintColor save;
    ModelTintColor col;
    s32 i;
    s32 j;
    s32 k;
    u32 v;
    s32 keep;
    s32 rowbase;
    u32 part;
    u32 sid;
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
        slot = &D_800F2C40[side];
        sav06 = slot->field_E06;
        part = v & 0x1F;
        keep = slot->field_BF5;
        old = Model_SetSlotShadowEnabled(side, 0);
        sid = part;
        save = *MODEL_TINT_COLOR_VIEW(slot->field_DC0);
        col.b3 = e->start.b3;
        col.b0 = e->start.b0 * (hi - lo) / hi + e->end.b0 * lo / hi;
        col.b1 = e->start.b1 * (hi - lo) / hi + e->end.b1 * lo / hi;
        col.b2 = e->start.b2 * (hi - lo) / hi + e->end.b2 * lo / hi;
        *MODEL_TINT_COLOR_VIEW(slot->field_DC0) = col;

        aa = a;
        for (j = 0; j < slot->field_E1B; j++) {
            s32 t = slot->field_BF5;
            if (sid != 0) {
                slot->field_1E0[j]->sid = sid;
                t = sid;
            }
            func_8004DC38(slot, j, t, aa);
        }
        if (sid != 0) {
            slot->field_BF5 = sid;
        }

        D_8009AF9C = off + rowbase;
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

        Model_SetSlotShadowEnabled(side, old);
        *MODEL_TINT_COLOR_VIEW(slot->field_DC0) = save;
tail:
        e->elapsed += Model_GetFrameStep();
        if (e->elapsed >= e->duration) {
            e->flags &= 0xFFFE;
        }
    }
}
