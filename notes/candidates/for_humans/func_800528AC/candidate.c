#include "../../../../src/types.h"
#include "../../../../src/game/model.h"

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
    Record *e;
    Record *table;
    u8 *base1;

    table = D_800F2B50;
    base1 = (u8 *)table + 1;
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
        slot = &D_800F2C40[side];
        sav06 = slot->field_E06;
        keep = slot->field_BF5;
        old = func_80059AA8(side, 0);
        save = *(Quad *)slot->field_DC0;
        col.b[3] = e->field_13;
        col.b[0] = e->field_10 * (hi - lo) / hi + e->field_14 * lo / hi;
        col.b[1] = e->field_11 * (hi - lo) / hi + e->field_15 * lo / hi;
        col.b[2] = e->field_12 * (hi - lo) / hi + e->field_16 * lo / hi;
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

        D_8009AF9C = (s32)(base1 + off);
        D_8009AF9B = (e->flags >> 2) & 1;
        func_800540B4(side);
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
