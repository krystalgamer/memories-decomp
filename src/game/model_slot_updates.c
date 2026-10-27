#include "../types.h"
#include "model_slot_updates.h"

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_update_coordinate_unit_matrix.h"

#define MODEL_UNIT_OFFSET(member) ((u32)&(((GsCOORDUNIT *)0)->member))

/* The slot data entries at ModelSlot.entries, the record root_coordinate points at
 * and the block func_800580D4 builds on the stack are all one type: libhmd's
 * GsCOORDUNIT. These pin every offset the game still reaches by hand. */
extern char ModelSlotDataEntry_must_be_a_GsCOORDUNIT[
    sizeof(GsCOORDUNIT) == MODEL_SLOT_DATA_ENTRY_SIZE ? 1 : -1
];
extern char GsCOORDUNIT_matrix_offset_must_be_0x4[
    MODEL_UNIT_OFFSET(matrix) == 0x4 ? 1 : -1
];
extern char GsCOORDUNIT_matrix_t_offset_must_be_0x18[
    MODEL_UNIT_OFFSET(matrix.t) == 0x18 ? 1 : -1
];
extern char GsCOORDUNIT_workm_offset_must_be_0x24[
    MODEL_UNIT_OFFSET(workm) == 0x24 ? 1 : -1
];
extern char GsCOORDUNIT_rot_offset_must_be_0x44[
    MODEL_UNIT_OFFSET(rot) == 0x44 ? 1 : -1
];
extern char GsCOORDUNIT_super_offset_must_be_0x4C[
    MODEL_UNIT_OFFSET(super) == 0x4C ? 1 : -1
];

#undef MODEL_UNIT_OFFSET

/* ModelSlotPart is model.h's mirror of libhmd's GsSEQ, kept there so that
 * header does not need the libhmd chain. Every field agrees. */
#define MODEL_SEQ_AGREES(member) \
    ((u32)&(((ModelSlotPart *)0)->member) == (u32)&(((GsSEQ *)0)->member))
extern char ModelSlotPart_must_be_a_GsSEQ[
    sizeof(ModelSlotPart) == sizeof(GsSEQ) ? 1 : -1
];
extern char ModelSlotPart_seq_fields_must_agree[
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

void Model_SetSlotPartSpeeds(s32 slot_index, s32 speed_request)
{
    ModelSlot *slot_state = &gModel_aSlots[slot_index];
    ModelSlotPart *G32 *part_sequences;
    s32 part_index;

    part_sequences = slot_state->animation_sequences;
    part_index = 0;
    if (slot_state->animation_sequence_count != 0) {
        do {
            (*part_sequences)->speed = speed_request;
            part_sequences++;
            part_index++;
        } while (part_index < slot_state->animation_sequence_count);
    }
}

/* Places the slot's own coordinate unit: translation into the local matrix's
 * t vector and a single yaw into its Euler angles, then rebuilds the matrix.
 * The unit pointer is reloaded per store, as retail does. */
void Model_SetSlotRootTransform(ModelSlot *slot_state, int translation_x, int translation_y, int translation_z, int yaw_request)
{
    if (slot_state->root_coordinate) {
        slot_state->root_coordinate->rot.vx = 0;
        slot_state->root_coordinate->rot.vy = yaw_request;
        slot_state->root_coordinate->rot.vz = 0;
        slot_state->root_coordinate->matrix.t[0] = translation_x;
        slot_state->root_coordinate->matrix.t[1] = translation_y;
        slot_state->root_coordinate->matrix.t[2] = translation_z;
    }
    Model_UpdateCoordinateUnitMatrix(slot_state->root_coordinate, 0);
}
