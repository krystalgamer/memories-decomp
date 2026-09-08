#include "../types.h"
#include "model.h"

typedef struct {
    u16 flags;
    u8 pad_02[8];
    u16 field_0A;
    u16 field_0C;
    u16 field_0E;
    u8 field_10;
    u8 field_11;
    u8 field_12;
    u8 field_13;
    u8 field_14;
    u8 field_15;
    u8 field_16;
    u8 pad_17;
} Record;

typedef struct {
    u8 b[4];
} Quad;

extern Record D_800F2B50[10];
extern ModelSlot D_800F2C40[];
extern u8 D_8009AF9B;
extern s32 D_8009AF9C;

extern s32 Model_HasInsufficientBufferSpace(s32, s32);
extern s32 func_80059AA8(s32, s32);
extern void func_8004DC38(ModelSlot *, s32, s32, s32);
extern void func_800540B4(s32);
extern s32 func_80058E1C(void);

/* Preserve zero-valued inputs without introducing materialized constants. */
register const u32 hard_zero asm("$0");
/* Keep the body lookup distinct from the earlier slot-active lookup. */
extern ModelSlot D_800F2C40_alias[] asm("D_800F2C40");

/* A partial-width value boundary keeps the loop GIV's table-base move alive. */
static __inline__ Record *make_table_base(void)
{
    register union { u64 d; struct { u32 lo; u32 hi; } w; } wide asm("$2");
    register volatile u32 addr asm("$2");
    addr = (u32)D_800F2B50;
    wide.w.lo = addr;
    return (Record *)(u32)wide.d;
}

/* Per-frame tint pass over the ten requests at D_800F2B50. A live request
 * interpolates its start colour (+0x10..0x12) towards its end colour
 * (+0x14..0x16) by elapsed/duration, drops the result into the model slot's
 * field_DC0, redraws the slot through func_800540B4 with the request's part id
 * pushed into every part record, then restores the colour, the part records
 * and the slot's field_BF5 and advances the request's clock. */
void func_800528AC(void)
{
    Quad save;
    Quad col;
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
    Record *e;
    Record *table;

    table = make_table_base();
    for (i = 0, off = 0; i < 10; off += 0x18, i++) {
        e = &table[i];
        if ((*(u8 *)e & 1) == 0) {
            continue;
        }
        if (D_800F2C40[(e->flags >> 1) & 1].field_E1F == 0) {
            continue;
        }
        if (Model_HasInsufficientBufferSpace((e->flags >> 1) & 1, e->field_13)) {
            goto tail;
        }
        v = e->flags;
        a = e->field_0A;
        lo = e->field_0C;
        hi = e->field_0E;
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
        save = *(Quad *)slot->field_DC0;
        col.b[3] = e->field_13;
        col.b[0] = ({ register s32 p asm("$3"); p = e->field_10 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->field_14 * lo; p / hi; });
        col.b[1] = ({ register s32 p asm("$3"); p = e->field_11 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->field_15 * lo; p / hi; });
        {
            register s32 p asm("$3");
            register s32 out asm("$4");
            register s32 q asm("$16");
            p = e->field_12 * (hi - lo);
            out = p / hi;
            p = e->field_16 * lo;
            q = p / hi;
            out += q;
            col.b[2] = out;
        }
        *(Quad *)slot->field_DC0 = col;

        aa = a;
        for (j = 0; j < slot->field_E1B; j++) {
            s32 t = slot->field_BF5;
            if (v != 0) {
                *((u8 *)slot->field_1E0[j] + 0xC) = v;
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
                register s32 global_base asm("$9") = (s32)((u8 *)D_800F2B50 + 1);
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
                *((u8 *)slot->field_1E0[k] + 0xC) = keep;
                t = keep;
            }
            func_8004DC38(slot, k, t, sv);
        }
        if (keep != 0) {
            slot->field_BF5 = keep;
        }

        func_80059AA8(side, old);
        *(Quad *)slot->field_DC0 = save;
tail:
        e->field_0C += func_80058E1C();
        if (e->field_0C >= e->field_0E) {
            e->flags &= 0xFFFE;
        }
    }
}
