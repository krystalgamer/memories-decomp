#include "../../types.h"
#include "../model_record_tables.h"
#include "../display_object_stream_read_next_command.h"
#include "../display_object_stream_state.h"

/* Japan's copy of the stream handler table, at 0x80090E9C; see the North
   American unit for its contract. */
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
