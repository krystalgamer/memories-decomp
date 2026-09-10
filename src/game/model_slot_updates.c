#include "../types.h"
#include "model_slot_updates.h"

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "func_8005922C.h"

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

void func_8005A468(s32 arg0, s32 arg1)
{
    ModelSlot *rec = &D_800F2C40[arg0];
    u8 **p;
    s32 i;

    p = (u8 **)rec->field_1E0;
    i = 0;
    if (rec->field_E1B != 0) {
        do {
            (*p)[0xD] = arg1;
            p++;
            i++;
        } while (i < rec->field_E1B);
    }
}

/* Places the slot's own coordinate unit: translation into the local matrix's
 * t vector and a single yaw into its Euler angles, then rebuilds the matrix.
 * The unit pointer is reloaded per store, as retail does. */
void func_8005A4C4(unsigned char *e, int a, int b, int c, int d)
{
    if (*(GsCOORDUNIT **)(e + 0xD18)) {
        (*(GsCOORDUNIT **)(e + 0xD18))->rot.vx = 0;
        (*(GsCOORDUNIT **)(e + 0xD18))->rot.vy = d;
        (*(GsCOORDUNIT **)(e + 0xD18))->rot.vz = 0;
        (*(GsCOORDUNIT **)(e + 0xD18))->matrix.t[0] = a;
        (*(GsCOORDUNIT **)(e + 0xD18))->matrix.t[1] = b;
        (*(GsCOORDUNIT **)(e + 0xD18))->matrix.t[2] = c;
    }
    func_8005922C(*(GsCOORDUNIT **)(e + 0xD18), 0);
}
