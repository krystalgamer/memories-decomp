#include "../types.h"
#include "sd_read_sequence_event.h"
#include "sound_sequence_parser.h"
#include "sound.h"
#include "sound_sequence_reader.h"

extern s32 D_80011484[];

s32 SD_ReadSequenceEvent(SDSequenceTrack *arg0)
{
    register s32 cmd asm("a1");
    u32 status;
    register s32 tableVal asm("s3");
    s32 masked;
    s32 *table;
    s32 byte3 = 0;

    cmd = SD_ReadSequenceByte(arg0);
    {
        register s32 a0copy asm("a0") = cmd;

        if (!(cmd & SD_SEQUENCE_STATUS_BIT)) {
            status = arg0->running_status;
            arg0->running_status_held = 1;
        } else {
            status = cmd;
            if ((a0copy & 0xFF) != SD_SEQUENCE_META_EVENT) {
                arg0->running_status = cmd;
            }
            arg0->running_status_held = 0;
        }

        table = D_80011484;
        tableVal = table[(status >> SD_SEQUENCE_STATUS_SHIFT) &
                         SD_SEQUENCE_STATUS_INDEX_MASK];
        if (tableVal != 0) {
            register s32 byte2 asm("s2") = a0copy;

            if (arg0->running_status_held == 0) {
                byte2 = SD_ReadSequenceByte(arg0);
            }
            if (tableVal == 2) {
                byte3 = SD_ReadSequenceByte(arg0);
            }
            SD_DispatchSequenceChannelEvent(arg0, status & 0xFF, byte2 & 0xFF, byte3 & 0xFF);
            return 0;
        }

        masked = a0copy & 0xFF;
        if (masked == SD_SEQUENCE_SYSEX_EVENT) {
            goto case_f0;
        }
        if (masked != SD_SEQUENCE_META_EVENT) {
            goto case_default;
        }
        SD_HandleSequenceMetaEvent(arg0, SD_ReadSequenceByte(arg0) & 0xFF);
        return 0;
    }

case_f0:
    SD_SkipSequenceSysEx(arg0);
    return 0;

case_default:
    SD_IgnoreSequenceEvent();
    return 0;
}
