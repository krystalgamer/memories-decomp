#include "../types.h"
#define G_SDVALUE_IN_DATA
#define SD_POLL_SEQUENCE_STATE_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "sound.h"

#define STATE_BYTES ((u8 *)g_SDValue)

void func_80046294(void)
{
    s32 i;
    s32 offset;
    s32 next_offset;
    s32 command;
    SDValue *base;

    base = g_SDValue;
    /* Flattening this initialization changes GCC 2.8.1's loop-register
     * allocation; the table and both byte offsets must remain independent. */
    do {
        i = 0;
    } while (0);
    if (base->command_count > 0) {
        while (1) {
            offset = i * SD_COMMAND_RECORD_SIZE;
            next_offset = (i + 1) * SD_COMMAND_RECORD_SIZE;
            if (base->command_count == 0) {
                goto after;
            }
            command = base->commands.b[offset];
            switch (command) {
            case 0x42:
            case 0x43:
            case 0x45:
            case 0x46:
            case 0x48:
                *(SDCommand *)(STATE_BYTES + offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
                    *(SDCommand *)(STATE_BYTES + next_offset + SD_COMMAND_QUEUE_BYTE_OFFSET);
                goto decrement;
            case 0x20:
                /* Keep the byte-cursor sum: direct member access reschedules
                   the queue-base add and the following load. */
                if (*(s32 *)(STATE_BYTES + offset +
                        SD_COMMAND_QUEUE_BYTE_OFFSET +
                        (u32)&((SDCommand *)0)->field_0010) != 0x20) {
                    goto test;
                }
                *(SDCommand *)(STATE_BYTES + offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
                    *(SDCommand *)(STATE_BYTES + next_offset + SD_COMMAND_QUEUE_BYTE_OFFSET);
decrement:
                g_SDValue->command_count = g_SDValue->command_count - 1;
                goto test;
            default:
                i++;
            }
test:
            base = g_SDValue;
            if (i >= base->command_count) {
                break;
            }
            if (i < 0) {
                break;
            }
        }
    }
after:
    switch (g_SDValue->field_007C) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        g_SDValue->field_007C = 0;
        g_SDValue->field_007D = 0;
        break;
    }

    if ((g_SDValue->flags_0040 & 0x80) != 0) {
        if (g_SDValue->field_157E != -1) {
            if (SD_PollSequenceState() == 1) {
                SD_StopSequence(g_SDValue->field_157E);
            }
            g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFF7F;
        }
    }
}
