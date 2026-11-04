#include "../types.h"
#include "model_record_tables.h"
#include "display_object_stream_read_next_command.h"
#include "display_object_stream_state.h"

/* Initialized data this unit owns at 0x80090FEC, previously a generated blob
   (#2602).

   tent_DisplayObjectStreamCommandTable is the display-object stream handler table.
   DisplayObjectStream_ReadNextCommand dispatches table[op ^ 0xFF] for opcodes
   from 0xF0 up. All seven callbacks receive the shared DisplayObjectStreamState
   and operand cursor, including the reset and no-op entries that do not need
   both arguments. This keeps the interpreter boundary checked instead of
   hiding five prototype differences behind table casts.

   The background records and shadow fan steps that follow it in this image
   belong to model_background_records.c: Japan links the movie-stream and
   model-effect data between the two. */
s32 (*tent_DisplayObjectStreamCommandTable[])(DisplayObjectStreamState *, const u8 *) = {
    DisplayObjectStream_Stop,
    DisplayObjectStream_ResetOffset,
    DisplayObjectStream_Noop,
    DisplayObjectStream_JumpToOffset,
    DisplayObjectStream_ToggleFlagAndJumpToOffset,
    DisplayObjectStream_JumpToRandomOffset,
    DisplayObjectStream_ConfigureRotation,
};

void DisplayObjectStream_ReadNextCommand(DisplayObjectStreamState *object)
{
    u8 *p;
    s32 op;
    s32 (**table)(DisplayObjectStreamState *, const u8 *);
    s32 value;

    p = object->current + (u16)object->command_offset;
    op = *p;
    p++;

    if (op >= 0xF0) {
        table = tent_DisplayObjectStreamCommandTable;
        do {
            if (table[op ^ 0xFF](object, p) == -1) {
                return;
            }
            p = object->current + (u16)object->command_offset;
            op = *p;
            p++;
        } while (op >= 0xF0);
    }

    object->frame_delay = op;
    value = (p[1] << 8) | p[0];
    object->frame_data = object->base + value;
    object->command_offset += 3;
}
