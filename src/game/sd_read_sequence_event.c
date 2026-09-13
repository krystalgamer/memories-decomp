#include "../types.h"
#include "sd_read_sequence_event.h"
#include "sound_sequence_parser.h"
#include "sound.h"
#include "sound_sequence_reader.h"

/* The event byte is kept twice: `cmd` as read, and `ev` as a u8 copy. GCC
 * 2.8.1 assigns a u8 local with a plain move and zero-extends at each use,
 * which is retail's `$a1` to `$a0` copy followed by `andi 0xFF` at the meta
 * test and in the tail. `byte2` is declared before `type` so the two
 * equal-priority callee-saved pseudos keep retail's $s2/$s3 order. */
s32 SD_ReadSequenceEvent(SDSequenceTrack *arg0)
{
    s32 cmd;
    u8 ev;
    u32 status;
    u8 byte2;
    s32 type;
    u8 byte3;

    byte3 = 0;
    cmd = SD_ReadSequenceByte(arg0);
    ev = cmd;
    if (!(cmd & SD_SEQUENCE_STATUS_BIT)) {
        status = arg0->running_status;
        arg0->running_status_held = 1;
    } else {
        if (ev != SD_SEQUENCE_META_EVENT) {
            arg0->running_status = cmd;
        }
        status = cmd;
        arg0->running_status_held = 0;
    }
    type = D_80011484[(status >> SD_SEQUENCE_STATUS_SHIFT) &
                      SD_SEQUENCE_STATUS_INDEX_MASK];
    if (type != 0) {
        if (arg0->running_status_held == 0) {
            byte2 = SD_ReadSequenceByte(arg0);
        } else {
            byte2 = ev;
        }
        if (type == 2) {
            byte3 = SD_ReadSequenceByte(arg0);
        }
        SD_DispatchSequenceChannelEvent(arg0, status & 0xFF, byte2, byte3);
        return 0;
    }
    switch (ev) {
    case SD_SEQUENCE_META_EVENT:
        SD_HandleSequenceMetaEvent(arg0, SD_ReadSequenceByte(arg0) & 0xFF);
        return 0;
    case SD_SEQUENCE_SYSEX_EVENT:
        SD_SkipSequenceSysEx(arg0);
        return 0;
    default:
        SD_IgnoreSequenceEvent(ev);
        return 0;
    }
}
