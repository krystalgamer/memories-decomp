#include "../types.h"
#define G_SDVALUE_IN_DATA
#define FUNC_80049F50_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "sound.h"

#define STATE_BYTES ((u8 *)g_SDValue)

void func_80046294(void)
{
    s32 i;
    s32 offset;
    s32 next_offset;
    s32 command;
    u8 *base;

    base = STATE_BYTES;
    /* Flattening this initialization changes GCC 2.8.1's loop-register
     * allocation; the table and both byte offsets must remain independent. */
    do {
        i = 0;
    } while (0);
    if (*(s16 *)(base + 0x4C) > 0) {
        while (1) {
            offset = i * SD_COMMAND_RECORD_SIZE;
            next_offset = (i + 1) * SD_COMMAND_RECORD_SIZE;
            if (*(s16 *)(base + 0x4C) == 0) {
                goto after;
            }
            command = (base + offset)[SD_COMMAND_QUEUE_BYTE_OFFSET];
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
                if (*(s32 *)(STATE_BYTES + offset + 0x90) != 0x20) {
                    goto test;
                }
                *(SDCommand *)(STATE_BYTES + offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
                    *(SDCommand *)(STATE_BYTES + next_offset + SD_COMMAND_QUEUE_BYTE_OFFSET);
decrement:
                *(u16 *)(STATE_BYTES + 0x4C) =
                    *(u16 *)(STATE_BYTES + 0x4C) - 1;
                goto test;
            default:
                i++;
            }
test:
            base = STATE_BYTES;
            if (i >= *(s16 *)(base + 0x4C)) {
                break;
            }
            if (i < 0) {
                break;
            }
        }
    }
after:
    switch (STATE_BYTES[0x7C]) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        STATE_BYTES[0x7C] = 0;
        STATE_BYTES[0x7D] = 0;
        break;
    }

    if ((*(u16 *)(STATE_BYTES + 0x40) & 0x80) != 0) {
        if (*(s16 *)(STATE_BYTES + 0x157E) != -1) {
            if (func_80049F50() == 1) {
                func_80049C40(*(s16 *)(STATE_BYTES + 0x157E));
            }
            *(u16 *)(STATE_BYTES + 0x40) =
                *(u16 *)(STATE_BYTES + 0x40) & 0xFF7F;
        }
    }
}
