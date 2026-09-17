#define G_SDVALUE_IN_DATA
#include "../types.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output.h"

#define SD_VALUE_LINK_BLOCK_SIZE 0x800

void SD_QueueValueLinkTransfer(s32 index)
{
    SDCommand req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 step;
    s32 off;
    SDValueLink *entry;

    a = g_SDValue;
    step = (s32)(a->field_0002 << 3);
    step = (step + SD_VALUE_LINK_BLOCK_SIZE - 1) /
           SD_VALUE_LINK_BLOCK_SIZE;
    step = step + 1;
    off = (index & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)a->field_0448);
    func_800471D0(a->field_0438, 0x801E6800,
                  step + *(u16 *)entry, entry->field_0004, 0x800, 0x10);
    b = g_SDValue;
    req.command = 0x51;
    req.field_0004 = b->field_0438;
    req.field_000C = 0x801E6800;
    SD_EnqueueCommand(&req);
    c = g_SDValue;
    off += (s32)c->field_0448;
    entry = (SDValueLink *)off;
    c->field_0438 = c->field_0438 + entry->field_0004;
}
