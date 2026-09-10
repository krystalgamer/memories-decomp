#include "../types.h"
#include "model_slot_updates.h"

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

#define MODEL_UNIT_OFFSET(member) ((u32)&(((GsCOORDUNIT *)0)->member))

/* The slot data entries at ModelSlot.entries, the record field_D18 points at
 * and the block func_800580D4 builds on the stack are all one type: libhmd's
 * GsCOORDUNIT. These pin every offset the game still reaches by hand. */
typedef char ModelSlotDataEntry_must_be_a_GsCOORDUNIT[
    sizeof(GsCOORDUNIT) == MODEL_SLOT_DATA_ENTRY_SIZE ? 1 : -1
];
typedef char GsCOORDUNIT_matrix_offset_must_be_0x4[
    MODEL_UNIT_OFFSET(matrix) == 0x4 ? 1 : -1
];
typedef char GsCOORDUNIT_matrix_t_offset_must_be_0x18[
    MODEL_UNIT_OFFSET(matrix.t) == 0x18 ? 1 : -1
];
typedef char GsCOORDUNIT_workm_offset_must_be_0x24[
    MODEL_UNIT_OFFSET(workm) == 0x24 ? 1 : -1
];
typedef char GsCOORDUNIT_rot_offset_must_be_0x44[
    MODEL_UNIT_OFFSET(rot) == 0x44 ? 1 : -1
];
typedef char GsCOORDUNIT_super_offset_must_be_0x4C[
    MODEL_UNIT_OFFSET(super) == 0x4C ? 1 : -1
];

#undef MODEL_UNIT_OFFSET

/* ModelSlotPart is model.h's mirror of libhmd's GsSEQ, kept there so that
 * header does not need the libhmd chain. Every field agrees. */
#define MODEL_SEQ_AGREES(member) \
    ((u32)&(((ModelSlotPart *)0)->member) == (u32)&(((GsSEQ *)0)->member))
typedef char ModelSlotPart_must_be_a_GsSEQ[
    sizeof(ModelSlotPart) == sizeof(GsSEQ) ? 1 : -1
];
typedef char ModelSlotPart_seq_fields_must_agree[
    MODEL_SEQ_AGREES(rewrite_idx) && MODEL_SEQ_AGREES(size) &&
    MODEL_SEQ_AGREES(num) && MODEL_SEQ_AGREES(ii) &&
    MODEL_SEQ_AGREES(aframe) && MODEL_SEQ_AGREES(sid) &&
    MODEL_SEQ_AGREES(speed) && MODEL_SEQ_AGREES(srcii) &&
    MODEL_SEQ_AGREES(rframe) && MODEL_SEQ_AGREES(tframe) &&
    MODEL_SEQ_AGREES(ci) && MODEL_SEQ_AGREES(ti) &&
    MODEL_SEQ_AGREES(start) && MODEL_SEQ_AGREES(start_sid) &&
    MODEL_SEQ_AGREES(traveling) ? 1 : -1
];
#undef MODEL_SEQ_AGREES

void func_8005A468(s32 arg0, s32 arg1)
{
    ModelSlot *rec = &D_800F2C40[arg0];
    ModelSlotPart **p;
    s32 i;

    p = rec->field_1E0;
    i = 0;
    if (rec->field_E1B != 0) {
        do {
            (*p)->speed = arg1;
            p++;
            i++;
        } while (i < rec->field_E1B);
    }
}

/* Places the slot's own coordinate unit: translation into the local matrix's
 * t vector and a single yaw into its Euler angles, then rebuilds the matrix.
 * The unit pointer is reloaded per store, as retail does. */
void func_8005A4C4(ModelSlot *e, int a, int b, int c, int d)
{
    if (e->field_D18) {
        e->field_D18->rot.vx = 0;
        e->field_D18->rot.vy = d;
        e->field_D18->rot.vz = 0;
        e->field_D18->matrix.t[0] = a;
        e->field_D18->matrix.t[1] = b;
        e->field_D18->matrix.t[2] = c;
    }
    func_8005922C(e->field_D18, 0);
}
