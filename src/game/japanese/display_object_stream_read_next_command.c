#define D_80090FEC gJapanese_DisplayObjectStreamCommandTable
#include "../../types.h"
#include "../model_record_tables.h"
#include "../display_object_stream_read_next_command.h"

void DisplayObjectStream_ReadNextCommand(DisplayObjectStreamState *object)
{
    u8 *p;
    s32 op;
    s32 (**table)(DisplayObjectStreamState *, const u8 *);
    s32 value;

    p = object->current + (u16)object->field_58;
    op = *p;
    p++;

    if (op >= 0xF0) {
        table = gJapanese_DisplayObjectStreamCommandTable;
        do {
            if (table[op ^ 0xFF](object, p) == -1) {
                return;
            }
            p = object->current + (u16)object->field_58;
            op = *p;
            p++;
        } while (op >= 0xF0);
    }

    object->field_5A = op;
    value = (p[1] << 8) | p[0];
    object->field_4C = object->base + value;
    object->field_58 += 3;
}
